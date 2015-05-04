/*
 * Copyright 2015 Jonas Danielsson <jonas@threetimestwo.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with vector-tile-glib; if not, see <http://www.gnu.org/licenses/>.
 */

#include <gio/gio.h>
#include <cairo.h>
#include <pango/pango.h>
#include <pango/pangocairo.h>
#include <math.h>

#include "vector-tile-mapcss-private.h"
#include "vector-tile-mapcss-style.h"
#include "vector-tile-mapbox.h"
#include "vector_tile.pb-c.h"

/*
 * ZigZag encoding maps signed integers to unsigned integers so that numbers
 * with a small absolute value (for instance, -1) have a small varint encoded
 * value too. It does this in a way that "zig-zags" back and forth through the
 * positive and negative integers, so that -1 is encoded as 1, 1 is encoded as
 * 2, -2 is encoded as 3, and so on.
 *
 * This is used by Google Protocol Buffers, used to encode mapbox vector tiles.
 *
 */
#define ZIGZAG_DECODE(val) (((val) >> 1) ^ (-((val) & 1)))

enum {
  MAPBOX_CMD_MOVE_TO = 1,
  MAPBOX_CMD_LINE_TO = 2,
  MAPBOX_CMD_CLOSE_PATH = 7
};

/* This is the rendering layers and order we currently use */
enum {
  MAPBOX_RENDER_LAYER_EARTH,
  MAPBOX_RENDER_LAYER_LANDUSE,
  MAPBOX_RENDER_LAYER_WATER,
  MAPBOX_RENDER_LAYER_LANDUSE_NATURE,
  MAPBOX_RENDER_LAYER_PLACES,
  MAPBOX_RENDER_LAYER_ROADS,
  MAPBOX_RENDER_LAYER_BUILDINGS,
  MAPBOX_RENDER_LAYER_BRIDGE_TUNNEL,
  MAPBOX_RENDER_LAYER_POI,
  NUM_RENDER_LAYERS
};

/*
 * The index represents the order of the layer, and the primary tag
 * tells us what key value to use for the "kind" value.
 */
typedef struct {
  char *primary_tag;
  guint index;
} MapboxLayerData;


/*
 * This represents all we need to know to render a feature. It is collected
 * during the first pass where we determine which layer a feature belongs to.
 */
typedef struct {
  VectorTile__Tile__Feature *feature;
  VTileMapCSSStyle *style;
  guint layer_index;
  cairo_t *layer_cr;
  GHashTable *tags;
  VTileMapbox *mapbox;

  guint z_index;
  guint extent;
  guint tile_size;
} MapboxFeatureData;

typedef struct {
  GList *strokes;
  GList *casings;
} MapboxRenderLayer;


struct _VTileMapboxPrivate {
  guint8 *data;
  gssize size;
  guint tile_size;
  guint zoom_level;

  GList *texts;
  MapboxRenderLayer *render_layers[NUM_RENDER_LAYERS];
  VTileMapCSS *stylesheet;
};

G_DEFINE_TYPE_WITH_PRIVATE (VTileMapbox, vtile_mapbox, G_TYPE_OBJECT)

static void
free_mapbox_text (VTileMapboxText *text) {
  cairo_surface_destroy (text->surface);
  g_free (text->uid);
  g_free (text);
}

static void
vtile_mapbox_finalize (GObject *object)
{
  VTileMapbox *mapbox = (VTileMapbox *) object;
  gint i;

  g_list_free_full (mapbox->priv->texts, (GDestroyNotify) free_mapbox_text);
  for (i = 0; i < NUM_RENDER_LAYERS; i++)
    g_free (mapbox->priv->render_layers[i]);

  G_OBJECT_CLASS (vtile_mapbox_parent_class)->finalize (object);
}

static void
vtile_mapbox_class_init (VTileMapboxClass *klass)
{
  GObjectClass *mapbox_class = G_OBJECT_CLASS (klass);

  mapbox_class->finalize = vtile_mapbox_finalize;
}

static void
vtile_mapbox_init (VTileMapbox *mapbox)
{
  gint i;

  mapbox->priv = vtile_mapbox_get_instance_private (mapbox);
  mapbox->priv->texts = NULL;
  for (i = 0; i < NUM_RENDER_LAYERS; i++)
    mapbox->priv->render_layers[i] = g_new0 (MapboxRenderLayer, 1);
}

/**
 * vtile_mapbox_new:
 * @data: data of the tile to render.
 * @size: the size of the @data.
 * @tile_size: the size (width/height) of the tile to render.
 * @zoom_level: the zoom level of the tile.
 *
 * Create a new #VtileMapbox object, used to render a Mapbox vector tile.
 *
 * Returns: a new #VTileMapbox object. Use g_object_unref() when done.
 */
VTileMapbox *
vtile_mapbox_new (guint8 *data,
                  gssize size,
                  guint tile_size,
                  guint zoom_level)
{
  VTileMapbox *mapbox;

  mapbox = g_object_new (VTILE_TYPE_MAPBOX, NULL);

  mapbox->priv->data = data;
  mapbox->priv->size = size;
  mapbox->priv->tile_size = tile_size;
  mapbox->priv->zoom_level = zoom_level;

  return mapbox;
}

/**
 * vtile_mapbox_set_stylesheet:
 * @mapbox: a #VTileMapbox object.
 * @stylesheet: a #VTileMapCSS object.
 *
 * Set @stylesheet to the @mapbox object. This determines how we will
 * draw the tile.
 */
void
vtile_mapbox_set_stylesheet (VTileMapbox *mapbox,
                             VTileMapCSS *stylesheet)
{
  mapbox->priv->stylesheet = stylesheet;
}

/* Debug function */
static void
mapbox_print_tags (GHashTable *tags)
{
  char **keys;
  gint n, i;

  keys = (char **) g_hash_table_get_keys_as_array (tags, &n);
  for (i = 0; i < n; i++) {
    g_print ("%s = %s\n", keys[i], g_hash_table_lookup (tags, keys[i]));
  }
  g_free (keys);
}

/* Determine which tags to use from a feature */
static GHashTable *
mapbox_get_tags (VectorTile__Tile__Feature *feature,
                 VectorTile__Tile__Layer *layer,
                 char *primary_tag)
{
  gint n;
  gint added_kind = 0;
  GHashTable *tags;

  tags = g_hash_table_new (g_str_hash, g_str_equal);

  for (n = 0; n < feature->n_tags; n += 2) {
    char *key = layer->keys[feature->tags[n]];
    VectorTile__Tile__Value *value = layer->values[feature->tags[n + 1]];

    if (value->string_value) {
      if (!g_strcmp0 (key, "kind")) {
        g_hash_table_insert (tags, primary_tag,
                             value->string_value);
      } else
        g_hash_table_insert (tags, key, value->string_value);
    }
  }

  if (!added_kind) {
    if (!g_hash_table_lookup (tags, primary_tag))
      g_hash_table_insert (tags, primary_tag, "");
  }

  if (feature->type == VECTOR_TILE__TILE__GEOM_TYPE__POLYGON)
    g_hash_table_insert (tags, "area", "yes");
  else if (feature->type == VECTOR_TILE__TILE__GEOM_TYPE__LINESTRING)
    g_hash_table_insert (tags, "area", "no");

  return tags;
}

static VTileMapCSSStyle *
mapbox_feature_get_style (VTileMapbox *mapbox,
                          GHashTable *tags,
                          VectorTile__Tile__Feature *feature,
                          VectorTile__Tile__Layer *layer)
{
  VTileMapCSSStyle *style;

  switch (feature->type)
    {
    case VECTOR_TILE__TILE__GEOM_TYPE__POLYGON:
      style = vtile_mapcss_get_style (mapbox->priv->stylesheet,
                                      VTILE_MAPCSS_SELECTOR_TYPE_WAY,
                                      tags, mapbox->priv->zoom_level);
      break;
    case VECTOR_TILE__TILE__GEOM_TYPE__LINESTRING:
      style = vtile_mapcss_get_style (mapbox->priv->stylesheet,
                                      VTILE_MAPCSS_SELECTOR_TYPE_WAY,
                                      tags, mapbox->priv->zoom_level);
      break;
    case VECTOR_TILE__TILE__GEOM_TYPE__POINT:
      style = vtile_mapcss_get_style (mapbox->priv->stylesheet,
                                      VTILE_MAPCSS_SELECTOR_TYPE_NODE,
                                      tags, mapbox->priv->zoom_level);
      break;
    default:
      style = vtile_mapcss_get_style (mapbox->priv->stylesheet,
                                      VTILE_MAPCSS_SELECTOR_TYPE_NODE,
                                      tags, mapbox->priv->zoom_level);
      break;
    }

  return style;
}

/*
  The geometry of a feature contains a stream of commands and parameters
  (vertices). The repeat count is shifted to the left by 3 bits. This means
  that the command has 3 bits (0-7). The repeat count
  indicates how often this command is to be repeated. Defined
  commands are:
  - MoveTo:    1   (2 parameters follow)
  - LineTo:    2   (2 parameters follow)
  - ClosePath: 7   (no parameters follow)

  Commands are encoded as uint32 varints. Vertex parameters
  are encoded as deltas to the previous position and, as they
  may be negative, are further "zigzag" encoded as unsigned
  32-bit ints:

  n = (n << 1) ^ (n >> 31)

  Ex.: MoveTo(3, 6), LineTo(8, 12), LineTo(20, 34), ClosePath
  Encoded as: [ 9 6 12 18 10 12 24 44 15 ]
  |       |              `> [00001 111] command type 7 (ClosePath), length 1
  |       |       ===== relative LineTo(+12, +22) == LineTo(20, 34)
  |       | ===== relative LineTo(+5, +6) == LineTo(8, 12)
  |       `> [00010 010] = command type 2 (LineTo), length 2
  | ==== relative MoveTo(+3, +6)
  `> [00001 001] = command type 1 (MoveTo), length 1

  The original position is (0,0).
*/
static void
mapbox_draw_path (MapboxFeatureData *data, cairo_t *cr)
{
 gint n;
  gdouble scale;
  gint p_geom = 0;

  /* We need this since we are using relative move_to and line_to */
  cairo_move_to (cr, 0, 0);

  cairo_save (cr);
  scale = (gdouble) data->tile_size / data->extent;
  cairo_scale (cr, scale, scale);

  do {
    gint cmd;
    gint length;

    cmd = data->feature->geometry[p_geom] & 7L;
    length = data->feature->geometry[p_geom] >> 3;

    if (cmd == MAPBOX_CMD_MOVE_TO || cmd == MAPBOX_CMD_LINE_TO) {
      gint32 dx;
      gint32 dy;

      for (n = 0; n < length; n++) {
        guint32 parameter;

        parameter = data->feature->geometry[++p_geom];
        dx = ZIGZAG_DECODE (parameter);

        parameter = data->feature->geometry[++p_geom];
        dy = ZIGZAG_DECODE (parameter);

        if (cmd == MAPBOX_CMD_MOVE_TO)
          cairo_rel_move_to (cr, dx, dy);
        else
          cairo_rel_line_to (cr, dx, dy);
      }
    } else {
      /* MAPBOX_CMD_CLOSE_PATH */
      cairo_close_path (cr);
    }

    p_geom += 1;
  } while(p_geom < data->feature->n_geometry);

  cairo_restore (cr);
}


/* Draw the geometry of a feature, this will be a line or a polygon */
static cairo_path_t *
mapbox_render_geometry (MapboxFeatureData *data,
                        cairo_t *cr)
{
  cairo_path_t *path;

  mapbox_draw_path (data, cr);
  path = cairo_copy_path (cr);

  if (data->feature->type == VECTOR_TILE__TILE__GEOM_TYPE__POLYGON) {
    VTileMapCSSColor *color;
    gdouble opacity;

    cairo_stroke_preserve (cr);

    cairo_save (cr);
    cairo_clip (cr);
    opacity = vtile_mapcss_style_get_num (data->style, "fill-opacity");
    color = vtile_mapcss_style_get_color (data->style, "fill-color");
    cairo_set_source_rgba (cr,
                           color->r,
                           color->g,
                           color->b,
                           opacity);
    cairo_paint (cr);
    cairo_restore (cr);
  } else {
    cairo_stroke (cr);
  }

  return path;
}

/* Render the casings for a line, this is done by drawing a thicker version
 * of the line before we draw the actual line. So the width of the casing
 * will be: line_width + (2 * casing_width).
*/
static void
mapbox_render_casings (MapboxFeatureData *data,
                       cairo_t *cr)
{
  VTileMapCSSColor *color;
  VTileMapCSSDash *c_dash, *dash;
  VTileMapCSSEnumValue c_line_cap, line_cap;
  VTileMapCSSEnumValue c_line_join, line_join;
  gdouble c_width, width;
  gdouble opacity;

  c_width = vtile_mapcss_style_get_num (data->style, "casing-width");
  if (!c_width)
    return;

  width = vtile_mapcss_style_get_num (data->style, "width");
  dash = vtile_mapcss_style_get_dash (data->style, "dashes");
  line_cap = vtile_mapcss_style_get_enum (data->style, "linecap");
  line_join = vtile_mapcss_style_get_enum (data->style, "linejoin");

  opacity = vtile_mapcss_style_get_num (data->style, "casing-opacity");
  color = vtile_mapcss_style_get_color (data->style, "casing-color");
  cairo_set_source_rgba (cr,
                        color->r,
                        color->g,
                        color->b,
                        opacity);
  c_width = width + (2 * c_width);
  cairo_set_line_width (cr, c_width);

  c_line_cap = vtile_mapcss_style_get_enum (data->style, "casing-linecap");
  if (c_line_cap > 0) {
    switch (c_line_cap) {
    case VTILE_MAPCSS_VALUE_NONE:
      cairo_set_line_cap (cr, CAIRO_LINE_CAP_BUTT);
      break;
    case VTILE_MAPCSS_VALUE_ROUND:
      cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);
      break;
    case VTILE_MAPCSS_VALUE_SQUARE:
      cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE);
      break;
    }
  } else {
    cairo_set_line_cap (cr, line_cap);
  }

  c_line_join  = vtile_mapcss_style_get_enum (data->style, "casing-linejoin");
  if (c_line_join > 0) {
    switch (c_line_join) {
    case VTILE_MAPCSS_VALUE_ROUND:
      cairo_set_line_join (cr, CAIRO_LINE_JOIN_ROUND);
      break;
    case VTILE_MAPCSS_VALUE_MITER:
      cairo_set_line_join (cr, CAIRO_LINE_JOIN_MITER);
      break;
    case VTILE_MAPCSS_VALUE_BEVEL:
      cairo_set_line_join (cr, CAIRO_LINE_JOIN_BEVEL);
      break;
    }
  } else {
    cairo_set_line_join (cr, line_join);
  }

  c_dash = vtile_mapcss_style_get_dash (data->style, "casing-dashes");
  if (c_dash)
    cairo_set_dash (cr, c_dash->dashes, c_dash->num_dashes, 0);

  cairo_path_destroy (mapbox_render_geometry (data, cr));
}

/* Render all lines, fetch the style data and draw the geometry */
static cairo_path_t *
mapbox_render_lines (MapboxFeatureData *data,
                     cairo_t *cr)
{
  VTileMapCSSColor *color;
  VTileMapCSSDash *dash;
  VTileMapCSSEnumValue line_cap;
  VTileMapCSSEnumValue line_join;
  gdouble opacity;
  gdouble width;

  width = vtile_mapcss_style_get_num (data->style, "width");
  dash = vtile_mapcss_style_get_dash (data->style, "dashes");
  line_cap = vtile_mapcss_style_get_enum (data->style, "linecap");
  line_join = vtile_mapcss_style_get_enum (data->style, "linejoin");

  opacity = vtile_mapcss_style_get_num (data->style, "opacity");
  color = vtile_mapcss_style_get_color (data->style, "color");
  cairo_set_source_rgba (cr,
                         color->r,
                         color->g,
                         color->b,
                         opacity);

  cairo_set_line_width (cr, width);
  switch (line_cap) {
  case VTILE_MAPCSS_VALUE_NONE:
    cairo_set_line_cap (cr, CAIRO_LINE_CAP_BUTT);
    break;
  case VTILE_MAPCSS_VALUE_ROUND:
    cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);
    break;
  case VTILE_MAPCSS_VALUE_SQUARE:
    cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE);
    break;
  }

  switch (line_join) {
  case VTILE_MAPCSS_VALUE_ROUND:
    cairo_set_line_join (cr, CAIRO_LINE_JOIN_ROUND);
    break;
  case VTILE_MAPCSS_VALUE_MITER:
    cairo_set_line_join (cr, CAIRO_LINE_JOIN_MITER);
    break;
  case VTILE_MAPCSS_VALUE_BEVEL:
    cairo_set_line_join (cr, CAIRO_LINE_JOIN_BEVEL);
    break;
  }

  cairo_set_dash (cr, dash->dashes, dash->num_dashes, 0);

  return mapbox_render_geometry (data, cr);
}

static PangoAttrList *
mapbox_get_text_attributes (MapboxFeatureData *data)
{
  PangoAttrList *attr_list;
  PangoFontDescription *desc;
  PangoStyle style;
  PangoVariant variant;
  PangoWeight weight;
  gdouble size;
  gint enum_value;
  char *family;
  VTileMapCSSColor *color;

  attr_list = pango_attr_list_new ();

  desc = pango_font_description_new ();
  family = vtile_mapcss_style_get_str (data->style, "font-family");
  pango_font_description_set_family (desc, family);

  enum_value = vtile_mapcss_style_get_enum (data->style, "font-style");
  if (enum_value == VTILE_MAPCSS_VALUE_NORMAL)
    style = PANGO_STYLE_NORMAL;
  else
    style = PANGO_STYLE_ITALIC;
  pango_font_description_set_style (desc, style);

  enum_value = vtile_mapcss_style_get_enum (data->style, "font-variant");
  if (enum_value == VTILE_MAPCSS_VALUE_NORMAL)
    style = PANGO_VARIANT_NORMAL;
  else
    style = PANGO_VARIANT_SMALL_CAPS;
  pango_font_description_set_variant (desc, variant);

  enum_value = vtile_mapcss_style_get_enum (data->style, "font-weight");
  if (enum_value == VTILE_MAPCSS_VALUE_NORMAL)
    weight = PANGO_WEIGHT_NORMAL;
  else
    style = PANGO_WEIGHT_BOLD;
  pango_font_description_set_weight (desc, weight);

  size = vtile_mapcss_style_get_num (data->style, "font-size");
  pango_font_description_set_size (desc, (gint) size * PANGO_SCALE);

  pango_attr_list_insert (attr_list, pango_attr_font_desc_new (desc));

  enum_value = vtile_mapcss_style_get_enum (data->style, "text-decoration");
  if (enum_value == VTILE_MAPCSS_VALUE_UNDERLINE)
    pango_attr_list_insert (attr_list,
                            pango_attr_underline_new (PANGO_UNDERLINE_SINGLE));

  return attr_list;
}

static void
mapbox_find_text_pos (MapboxFeatureData *data,
                      cairo_path_t *path,
                      guint *x_out,
                      guint *y_out,
                      gdouble *angle,
                      guint *length)
{
  cairo_path_data_t *path_data;
  gint i;
  gint x, y;
  gint lowest_x, lowest_y, highest_x, highest_y;
  gint longest_x, longest_y;
  gint longest = 0;
  gint l_x, l_y, r_x, r_y;

  x = y = 0;
  highest_x = highest_y = 0;
  lowest_x = lowest_y = 4096;

  for (i = 0; i < path->num_data; i += path->data[i].header.length) {
    gint old_x, old_y;

    path_data = &path->data[i];

    switch (path_data->header.type) {
    case CAIRO_PATH_MOVE_TO:
      x = path_data[1].point.x;
      y = path_data[1].point.y;
      break;
    case CAIRO_PATH_LINE_TO:
      old_x = x;
      old_y = y;

      x = path_data[1].point.x;
      y = path_data[1].point.y;

      if (data->feature->type == VECTOR_TILE__TILE__GEOM_TYPE__POLYGON) {
        if (x <= lowest_x)
          lowest_x = x;
        if (x >= highest_x)
          highest_x = x;
        if (y <= lowest_y)
          lowest_y = y;
        if (y >= highest_y)
          highest_y = y;
      }

      if (data->feature->type == VECTOR_TILE__TILE__GEOM_TYPE__LINESTRING) {
        gint d;

        if (old_x < x) {
          l_x = old_x;
          l_y = old_y;
          r_x = x;
          r_y = y;
        } else {
          l_x = x;
          l_y = y;
          r_x = old_x;
          r_y = old_y;
        }

        d = sqrt (pow (r_x - l_x, 2) + pow (r_y - l_y, 2));
        if (d > longest)
          longest = d;
      }
      break;
    }
  }

  if (data->feature->type == VECTOR_TILE__TILE__GEOM_TYPE__POLYGON) {
    gint width = highest_x - lowest_x;
    gint height = highest_y - lowest_y;

    *x_out = lowest_x + (width / 2);
    *y_out = lowest_y + (height / 2);
  } else if (data->feature->type == VECTOR_TILE__TILE__GEOM_TYPE__LINESTRING) {
    guint line_width = vtile_mapcss_style_get_num (data->style, "width");

    *x_out = l_x;
    *y_out = l_y - line_width;
    *angle = atan2 (r_y - l_y, r_x - l_x);
    *length = longest;
  } else {
    *x_out = x;
    *y_out = y;
  }
}

static void
find_new_bounding_box (cairo_matrix_t matrix,
                       gint width,
                       gint height,
                       gint *min_x,
                       gint *max_x,
                       gint *min_y,
                       gint *max_y)
{
  gint ax[4];
  gint ay[4];
  gint i;

  *min_x = *min_y = 256;
  *max_x = *max_y = -256;

  /*
   * To find the new bounding box after the transformations
   * from the matrix are applied we look for the new min/max values.
   * of the transformed corners.
   *
   * Example:
   *                         0'
   *                        / \
   *                       /   \
   *    0--------3        1'    3'
   *    |        |   ==>   \    /
   *    |        |          \  /
   *    1--------2           2'
   *
   * min_x = 1', max_x = 3', min_y = 0', max_y = 2'
   *
   *  x_new = xx * x + xy * y + x0;
   *  y_new = yx * x + yy * y + y0;
   *
   */
  ax[0] = matrix.x0;
  ax[1] = matrix.xy * height + matrix.x0;
  ax[2] = matrix.xx * width + matrix.xy * height + matrix.x0;
  ax[3] = matrix.xx * width + matrix.x0;

  ay[0] = matrix.y0;
  ay[1] = matrix.yy * height + matrix.y0;
  ay[2] = matrix.yx * width + matrix.yy * height + matrix.y0;
  ay[3] = matrix.yx * width + matrix.y0;

  for (i = 0; i < 4; i++) {
    if (ax[i] > *max_x)
      *max_x = ax[i];
    if (ax[i] < *min_x)
      *min_x = ax[i];
    if (ay[i] > *max_y)
      *max_y = ay[i];
    if (ay[i] < *min_y)
      *min_y = ay[i];
  }
}

static void
mapbox_add_text (MapboxFeatureData *data,
                 cairo_t *cr,
                 cairo_path_t *path,
                 char *text)
{
  PangoAttrList *attr_list;
  PangoLayout *layout;
  VTileMapCSSColor *color;
  gint halo_width;
  cairo_surface_t *target;
  cairo_matrix_t matrix;
  cairo_t *text_cr;
  VTileMapboxText *m_text = g_new0 (VTileMapboxText, 1);
  gint width, height;
  gint32 x;
  gint32 y;
  gdouble angle = 0.0;
  guint length;

  mapbox_find_text_pos (data, path, &x, &y, &angle, &length);
  attr_list = mapbox_get_text_attributes (data);

  layout = pango_cairo_create_layout (cr);
  pango_layout_set_text (layout, text, -1);
  pango_layout_set_attributes (layout, attr_list);
  pango_attr_list_unref (attr_list);
  pango_layout_get_pixel_size (layout, &width, &height);

  if (width > length) {
    g_object_unref (layout);
    return;
  }

  target = cairo_get_target (cr);
  m_text->offset_x = x;
  m_text->offset_y = y;
  m_text->uid = g_strdup (g_hash_table_lookup (data->tags, "uid"));

  if (angle != 0.0) {
    gint min_x;
    gint max_x;
    gint min_y;
    gint max_y;

    /* Translate to center point and rotate with angle */
    cairo_save (cr);
    cairo_identity_matrix (cr);
    cairo_translate (cr, width / 2, height / 2);
    cairo_rotate (cr, angle);
    cairo_translate (cr, -width / 2, -height / 2);
    cairo_get_matrix (cr, &matrix);
    cairo_restore (cr);

    /* Find the bounding box of the text after rotation */
    find_new_bounding_box (matrix, width, height,
                           &min_x, &max_x, &min_y, &max_y);

    /* Get the new origo of the bounding box */
    m_text->surface_offset_x = min_x;
    m_text->surface_offset_y = min_y;

    m_text->width = max_x - min_x;
    m_text->height = max_y - min_y;

    /* Get the new position of the old origo */
    m_text->offset_x -= matrix.x0 - min_x;
    m_text->offset_y -= matrix.y0 - min_y;
  } else {
    m_text->width = width;
    m_text->height = height;
    m_text->offset_x -= (width / 2);
    m_text->offset_y -= (height / 2);
    m_text->surface_offset_x = 0;
    m_text->surface_offset_y = 0;

    cairo_get_matrix (cr, &matrix);
  }
  m_text->surface = cairo_surface_create_similar (target,
                                                  CAIRO_CONTENT_COLOR_ALPHA,
                                                  m_text->width,
                                                  m_text->height);

  /* Make sure we draw to the correct place */
  cairo_surface_set_device_offset (m_text->surface,
                                   -m_text->surface_offset_x,
                                   -m_text->surface_offset_y);

  text_cr = cairo_create (m_text->surface);
  cairo_set_matrix (text_cr, &matrix);
  pango_cairo_update_layout (text_cr, layout);
  pango_cairo_layout_path (text_cr, layout);

  color = vtile_mapcss_style_get_color (data->style, "text-color");
  halo_width = vtile_mapcss_style_get_num (data->style, "text-halo-radius");
  if (halo_width > 0) {
    VTileMapCSSColor *halo_color;

    cairo_set_line_width (text_cr, halo_width);
    halo_color = vtile_mapcss_style_get_color (data->style, "text-halo-color");
    cairo_set_source_rgb (text_cr, halo_color->r, halo_color->g, halo_color->b);
  } else {
    cairo_set_source_rgb (text_cr, color->r, color->g, color->b);
  }
  cairo_stroke_preserve (text_cr);
  cairo_set_source_rgb (text_cr, color->r, color->g, color->b);
  cairo_fill (text_cr);

  data->mapbox->priv->texts = g_list_prepend (data->mapbox->priv->texts,
                                              m_text);
  g_object_unref (layout);
}


static void
mapbox_render_feature (MapboxFeatureData *data,
                       cairo_t *cr)
{
  char *text_tag;
  char *text;
  cairo_path_t *path = NULL;

  if (data->feature->type == VECTOR_TILE__TILE__GEOM_TYPE__POLYGON ||
      data->feature->type == VECTOR_TILE__TILE__GEOM_TYPE__LINESTRING) {
    path = mapbox_render_lines (data, cr);
  } else {
    mapbox_draw_path (data, cr);
    path = cairo_copy_path (cr);
  }

  if (path) {
    text_tag = vtile_mapcss_style_get_str (data->style, "text");
    if (text_tag && (text = g_hash_table_lookup (data->tags, text_tag))) {
        mapbox_add_text (data, cr, path, text);
    }

    cairo_path_destroy (path);
  }

  vtile_mapcss_style_free (data->style);
  g_hash_table_destroy (data->tags);
  g_free (data);
}

static gboolean
mapbox_move_feature_if (GHashTable *tags,
                        const char *tag,
                        const char *value)
{
  char *tag_value;

  tag_value = g_hash_table_lookup (tags, tag);
  return tag_value && !g_strcmp0 (tag_value, value);
}

static void
mapbox_process_feature (VTileMapbox *mapbox,
                        VectorTile__Tile__Feature *feature,
                        VectorTile__Tile__Layer *layer,
                        char *primary_tag,
                        guint layer_index)
{
  MapboxFeatureData *data;
  GHashTable *tags;

  tags = mapbox_get_tags (feature, layer, primary_tag);

  data = g_new (MapboxFeatureData, 1);
  data->style = mapbox_feature_get_style (mapbox, tags, feature, layer);
  data->z_index = vtile_mapcss_style_get_num (data->style, "z-index");
  data->layer_index = layer_index;
  data->extent = layer->extent;
  data->tile_size = mapbox->priv->tile_size;
  data->feature = feature;
  data->tags = tags;
  data->mapbox = mapbox;

  if (layer_index == MAPBOX_RENDER_LAYER_ROADS) {
    if (mapbox_move_feature_if (tags, "is_tunnel", "yes"))
      layer_index = MAPBOX_RENDER_LAYER_BRIDGE_TUNNEL;
    else if (mapbox_move_feature_if (tags, "is_bridge", "yes"))
      layer_index = MAPBOX_RENDER_LAYER_BRIDGE_TUNNEL;
  } else if (layer_index == MAPBOX_RENDER_LAYER_LANDUSE) {
    if (mapbox_move_feature_if (tags, "landuse", "wood"))
      layer_index = MAPBOX_RENDER_LAYER_LANDUSE_NATURE;
    else if (mapbox_move_feature_if (tags, "landuse", "scrub"))
      layer_index = MAPBOX_RENDER_LAYER_LANDUSE_NATURE;
    else if (mapbox_move_feature_if (tags, "landuse", "rock"))
      layer_index = MAPBOX_RENDER_LAYER_LANDUSE_NATURE;
  }

  if (vtile_mapcss_style_get_num (data->style, "casing-width") > 0) {
    mapbox->priv->render_layers[layer_index]->casings =
      g_list_prepend (mapbox->priv->render_layers[layer_index]->casings, data);
  }

  mapbox->priv->render_layers[layer_index]->strokes =
    g_list_prepend (mapbox->priv->render_layers[layer_index]->strokes, data);
}

static void
mapbox_set_canvas_style (VTileMapbox *mapbox,
                         cairo_t *cr)
{
  VTileMapCSSStyle *style;
  VTileMapCSSColor *color;
  gdouble opacity;

  style = vtile_mapcss_get_style (mapbox->priv->stylesheet,
                                  VTILE_MAPCSS_SELECTOR_TYPE_CANVAS,
                                  NULL, mapbox->priv->zoom_level);

  opacity = vtile_mapcss_style_get_num (style, "fill-opacity");
  color = vtile_mapcss_style_get_color (style, "fill-color");
  cairo_set_source_rgba (cr,
                         color->r,
                         color->g,
                         color->b,
                         opacity);
  vtile_mapcss_style_free (style);

  cairo_rectangle (cr, 0, 0,
                   mapbox->priv->tile_size,
                   mapbox->priv->tile_size);
  cairo_fill (cr);
}

static void
mapbox_get_layer_data (const char *name,
                       char **primary_tag,
                       guint *layer_index)
{
  if (!g_strcmp0 (name, "water")) {
    *layer_index = MAPBOX_RENDER_LAYER_WATER;
    *primary_tag = "water";
  } else if (!g_strcmp0 (name, "earth")) {
    *layer_index = MAPBOX_RENDER_LAYER_EARTH;
    *primary_tag = "earth";
  } else if (!g_strcmp0 (name, "places")) {
    *layer_index = MAPBOX_RENDER_LAYER_PLACES;
    *primary_tag = "place";
  } else if (!g_strcmp0 (name, "landuse")) {
    *layer_index = MAPBOX_RENDER_LAYER_LANDUSE;
    *primary_tag = "landuse";
  } else if (!g_strcmp0 (name, "roads")) {
    *layer_index = MAPBOX_RENDER_LAYER_ROADS;
    *primary_tag = "road";
  } else if (!g_strcmp0 (name, "buildings")) {
    *layer_index = MAPBOX_RENDER_LAYER_BUILDINGS;
    *primary_tag = "building";
  } else {
    *layer_index = MAPBOX_RENDER_LAYER_POI;
    *primary_tag = "poi";
  }
}

static gint
mapbox_compare_z_index (const MapboxFeatureData *a,
                        const MapboxFeatureData *b)
{
  if (a->z_index < b->z_index)
    return -1;
  else if (a->z_index > b->z_index)
    return 1;

  return 0;
}

static void
mapbox_render_layer (VTileMapbox *mapbox,
                     guint layer_index,
                     cairo_t *cr)
{
  MapboxRenderLayer *layer = mapbox->priv->render_layers[layer_index];

  if (!layer->strokes)
      return;

    if (layer->casings) {
      layer->casings = g_list_sort (layer->casings,
                                    (GCompareFunc) mapbox_compare_z_index);

      g_list_foreach (layer->casings, (GFunc) mapbox_render_casings, cr);
      g_list_free (layer->casings);
      mapbox->priv->render_layers[layer_index]->casings = NULL;
    }

    layer->strokes = g_list_sort (layer->strokes,
                                  (GCompareFunc) mapbox_compare_z_index);

    g_list_foreach (layer->strokes, (GFunc) mapbox_render_feature, cr);
    g_list_free (layer->strokes);
    mapbox->priv->render_layers[layer_index]->strokes = NULL;
}

static gboolean
mapbox_render_tile (VTileMapbox *mapbox,
                    VectorTile__Tile *tile,
                    cairo_t *cr)

{
  gint l, f;

  for (l = 0; l < tile->n_layers; l++) {
    char *primary_tag;
    guint layer_index;
    VectorTile__Tile__Layer *layer = tile->layers[l];
    cairo_t *layer_cr;

    mapbox_get_layer_data (layer->name, &primary_tag, &layer_index);

    for (f = 0; f < layer->n_features; f++) {
      VectorTile__Tile__Feature *feature = layer->features[f];

      mapbox_process_feature (mapbox, feature, layer,
                              primary_tag, layer_index);
    }
  }

  for (l = 0; l < NUM_RENDER_LAYERS; l++)
    mapbox_render_layer (mapbox, l, cr);

  return TRUE;
}

/**
 * vtile_mapbox_render:
 * @mapbox: a #VTileMapbox object.
 * @cr: the cairo context to render to.
 * @error: a #GError, or %NULL.
 *
 * Returns: %TRUE on success, %FALSE on error.
 */
gboolean
vtile_mapbox_render (VTileMapbox *mapbox,
                     cairo_t *cr,
                     GError **error)
{
  VectorTile__Tile *tile;
  gboolean status;

  g_return_val_if_fail (mapbox != NULL, FALSE);
  g_return_val_if_fail (cr != NULL, FALSE);

  tile = vector_tile__tile__unpack (NULL,
                                    mapbox->priv->size,
                                    mapbox->priv->data);
  if (!tile) {
    return FALSE;
  }

  status = mapbox_render_tile (mapbox, tile, cr);
  vector_tile__tile__free_unpacked (tile, NULL);


  return status;
}

static void
vtile_mapbox_render_thread (GTask *task,
                            VTileMapbox *mapbox,
                            cairo_t *cr,
                            GCancellable *cancellable)
{
  GError *error = NULL;
  gboolean success;

  success = vtile_mapbox_render (mapbox, cr, &error);
  if (success)
    g_task_return_boolean (task, success);
  else
    g_task_return_error (task, error);
}

/**
 * vtile_mapbox_render_async:
 * @mapbox: a #VTileMapbox object.
 * @cr: the cairo context to render to.
 * @callback: a #GAsyncReadyCallbac to call when the request is satisfied.
 * @user_data: the data to pass to callback function.
 */
void
vtile_mapbox_render_async (VTileMapbox *mapbox,
                           cairo_t *cr,
                           GAsyncReadyCallback callback,
                           gpointer user_data)
{
  GTask *task;

  task = g_task_new (mapbox, NULL, callback, user_data);
  g_task_set_task_data (task, cr, NULL);
  g_task_run_in_thread (task, (GTaskThreadFunc) vtile_mapbox_render_thread);
  g_object_unref (task);
}

/**
 * vtile_mapbox_render_finish:
 * @mapbox: a #VTileMapbox object
 * @result: a #GAsyncResult
 * @error: a #GError, or %NULL
 *
 * Returns: %TRUE on success, %FALSE on error.
 */
gboolean
vtile_mapbox_render_finish (VTileMapbox *mapbox,
                            GAsyncResult *result,
                            GError **error)
{
  g_return_val_if_fail (g_task_is_valid (result, mapbox), FALSE);

  return g_task_propagate_boolean (G_TASK (result), error);
}

/**
 * vtile_mapbox_dump_info: (skip)
 */
void
vtile_mapbox_dump_info (VTileMapbox *mapbox)
{
  gint l, f;
  VectorTile__Tile *tile;
  GHashTable *tags;

  tile = vector_tile__tile__unpack (NULL,
                                    mapbox->priv->size,
                                    mapbox->priv->data);
  if (!tile)
    return;

  for (l = 0; l < tile->n_layers; l++) {
    char *primary_tag;
    guint layer_index;
    VectorTile__Tile__Layer *layer = tile->layers[l];

    g_print ("New layer: %s\n", layer->name);

    mapbox_get_layer_data (layer->name, &primary_tag, &layer_index);
    for (f = 0; f < layer->n_features; f++) {
      VectorTile__Tile__Feature *feature = layer->features[f];
      gint n;

      g_print ("New feature: %d\n", feature->id);
      for (n = 0; n < feature->n_tags; n += 2) {
        char *key = layer->keys[feature->tags[n]];
        VectorTile__Tile__Value *value = layer->values[feature->tags[n + 1]];

        g_print ("key/value from tile:\n");
        g_print ("%s = ", key);

        if (value->string_value)
          g_print ("%s (string)\n", value->string_value);
        else if (value->has_float_value)
          g_print ("%f (float)\n", value->float_value);
        else if (value->has_double_value)
          g_print ("%f (double)\n", value->double_value);
        else if (value->has_int_value)
          g_print ("%d (int)\n", value->int_value);
        else if (value->has_uint_value)
          g_print ("%u (uint)\n", value->uint_value);
        else if (value->has_sint_value)
          g_print ("%d (sint)\n", value->sint_value);
        else if (value->has_bool_value)
          g_print ("%s (boolean)\n", value->bool_value ? "true" : "false");
      }
      g_print ("\nstylable tags:\n");
      tags = mapbox_get_tags (feature, layer, primary_tag);
      mapbox_print_tags (tags);
      g_print("\n");
    }
  }
  vector_tile__tile__free_unpacked (tile, NULL);
}

/**
 * vtile_mapbox_get_texts:
 * @mapbox: A #VTileMapbox object.
 *
 * Returns all labels found while rendering the tile,
 * or %NULL if none was found.
 *
 * Returns: (element-type VTileMapboxText) (transfer none): List of #VTileMapboxText
 */
GList *
vtile_mapbox_get_texts (VTileMapbox *mapbox)
{
  return mapbox->priv->texts;
}
