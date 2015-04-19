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

#include "vector-tile-mapcss-private.h"
#include "vector-tile-mapcss-style.h"
#include "vector-tile-mapbox.h"
#include "vector_tile.pb-c.h"

#define ZIGZAG_DECODE(val) (((val) >> 1) ^ (-((val) & 1)))

enum {
  MAPBOX_CMD_MOVE_TO = 1,
  MAPBOX_CMD_LINE_TO = 2,
  MAPBOX_CMD_CLOSE_PATH = 7
};

enum {
  MAPBOX_RENDER_LAYER_EARTH,
  MAPBOX_RENDER_LAYER_LANDUSE,
  MAPBOX_RENDER_LAYER_WATER,
  MAPBOX_RENDER_LAYER_PLACES,
  MAPBOX_RENDER_LAYER_ROADS,
  MAPBOX_RENDER_LAYER_BUILDINGS,
  MAPBOX_RENDER_LAYER_BRIDGE_TUNNEL,
  MAPBOX_RENDER_LAYER_POI,
  NUM_RENDER_LAYERS
};

typedef struct {
  char *primary_tag;
  guint index;
} MapboxLayerData;

typedef struct {
  VectorTile__Tile__Feature *feature;
  VTileMapCSSStyle *style;
  MapboxLayerData *layer_data;

  guint z_index;
  guint extent;
  guint tile_size;
} MapboxFeatureData;


struct _VTileMapboxPrivate {
  guint8 *data;
  gssize size;
  guint tile_size;
  guint zoom_level;

  GList *render_layers[NUM_RENDER_LAYERS];
  VTileMapCSS *stylesheet;
};

G_DEFINE_TYPE_WITH_PRIVATE (VTileMapbox, vtile_mapbox, G_TYPE_OBJECT)

static void
vtile_mapbox_finalize (GObject *vmapbox)
{
  G_OBJECT_CLASS (vtile_mapbox_parent_class)->finalize (vmapbox);
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
  mapbox->priv = vtile_mapbox_get_instance_private (mapbox);
}

VTileMapbox *
vtile_mapbox_new (guint8 *data,
                  gssize size,
                  guint tile_size,
                  guint zom_level)
{
  VTileMapbox *mapbox;

  mapbox = g_object_new (VTILE_TYPE_MAPBOX, NULL);

  mapbox->priv->data = data;
  mapbox->priv->size = size;
  mapbox->priv->tile_size = tile_size;
  mapbox->priv->zoom_level;

  return mapbox;
}

void
vtile_mapbox_set_stylesheet (VTileMapbox *mapbox, VTileMapCSS *stylesheet)
{
  mapbox->priv->stylesheet = stylesheet;
}

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
      style = vtile_mapcss_get_style (mapbox->priv->stylesheet, "area",
                                      tags, mapbox->priv->zoom_level);
      break;
    case VECTOR_TILE__TILE__GEOM_TYPE__LINESTRING:
      style = vtile_mapcss_get_style (mapbox->priv->stylesheet, "way",
                                      tags, mapbox->priv->zoom_level);
      break;
    case VECTOR_TILE__TILE__GEOM_TYPE__POINT:
      style = vtile_mapcss_get_style (mapbox->priv->stylesheet, "node",
                                      tags, mapbox->priv->zoom_level);
      break;
    default:
      style = vtile_mapcss_get_style (mapbox->priv->stylesheet, "node",
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
mapbox_render_geometry (MapboxFeatureData *data,
                        cairo_t *cr)
{
  gint n;
  gdouble scale;
  gint p_geom = 0;

  /* We need this since we are using relative move_to and line_to */
  cairo_move_to (cr, 0, 0);

  cairo_save (cr);
  /* layer->extent: The bounding box for the tile spans from 0..4095 units */
  scale = (gdouble) data->tile_size / data->extent;
  cairo_scale (cr, scale, scale);

  do {
    gint cmd;
    gint length;

    cmd = data->feature->geometry[p_geom] & 7L;
    length = data->feature->geometry[p_geom] >> 3;

    if (cmd == MAPBOX_CMD_MOVE_TO || cmd == MAPBOX_CMD_LINE_TO) {
      gint32 from;
      gint32 to;

      for (n = 0; n < length; n++) {
        guint32 parameter;

        parameter = data->feature->geometry[++p_geom];
        from = ZIGZAG_DECODE (parameter);

        parameter = data->feature->geometry[++p_geom];
        to = ZIGZAG_DECODE (parameter);

        if (cmd == MAPBOX_CMD_MOVE_TO)
          cairo_rel_move_to (cr, from, to);
        else
          cairo_rel_line_to (cr, from, to);
      }
    } else {
      /* MAPBOX_CMD_CLOSE_PATH */
      cairo_close_path (cr);
    }

    p_geom += 1;
  } while(p_geom < data->feature->n_geometry);

  cairo_restore (cr);
}

static void
mapbox_process_feature (VTileMapbox *mapbox,
                        VectorTile__Tile__Feature *feature,
                        VectorTile__Tile__Layer *layer,
                        char *primary_tag,
                        guint layer_index)
{
  MapboxFeatureData *data;
  VTileMapCSSValue *value;
  GHashTable *tags;
  char *tag_value;
  GList *render_layer;

  tags = mapbox_get_tags (feature, layer, primary_tag);

  data = g_new (MapboxFeatureData, 1);
  data->style = mapbox_feature_get_style (mapbox, tags, feature, layer);
  value = vtile_mapcss_style_get (data->style, "z-index");
  data->z_index = value->num;
  data->extent = layer->extent;
  data->tile_size = mapbox->priv->tile_size;
  data->feature = feature;

  if (layer_index == MAPBOX_RENDER_LAYER_ROADS) {
    tag_value = g_hash_table_lookup (tags, "is_tunnel");
    if (tag_value && !g_strcmp0 (tag_value, "yes"))
        layer_index = MAPBOX_RENDER_LAYER_BRIDGE_TUNNEL;
    else {
      tag_value = g_hash_table_lookup (tags, "is_bridge");
      if (tag_value &&  !g_strcmp0 (tag_value, "yes"))
          layer_index = MAPBOX_RENDER_LAYER_BRIDGE_TUNNEL;
    }
  }

  mapbox->priv->render_layers[layer_index] =
    g_list_prepend (mapbox->priv->render_layers[layer_index], data);
  g_hash_table_destroy (tags);
}

static void
mapbox_render_feature (MapboxFeatureData *data,
                       cairo_t *cr)
{
  VTileMapCSSValue *value;
  gdouble line_width;

  /* Get line width */
  value = vtile_mapcss_style_get (data->style, "width");
  line_width = value->num;

  value = vtile_mapcss_style_get (data->style, "casing-width");
  if (value->num > 0 &&
      data->feature->type == VECTOR_TILE__TILE__GEOM_TYPE__LINESTRING && value->num > 0) {

    /* Set casing color */
    value = vtile_mapcss_style_get (data->style, "casing-color");
    cairo_set_source_rgb (cr,
                          value->color.r,
                          value->color.g,
                          value->color.b);

    /* Set casing width */
    value = vtile_mapcss_style_get (data->style, "casing-width");
    cairo_set_line_width (cr, line_width + (2 * value->num));

    /* Set linecap */
    value = vtile_mapcss_style_get (data->style, "linecap");
    cairo_set_line_cap (cr, value->line_cap);
   /* Set linejoin */
    value = vtile_mapcss_style_get (data->style, "linejoin");
    cairo_set_line_cap (cr, value->line_join);

    /* Set dashes */
    value = vtile_mapcss_style_get (data->style, "dashes");
    cairo_set_dash (cr, value->dash.dashes, value->dash.num_dashes, 0);


    mapbox_render_geometry (data, cr);
    cairo_stroke (cr);
  }

  /* Set line color */
  value = vtile_mapcss_style_get (data->style, "color");
  cairo_set_source_rgb (cr,
                        value->color.r,
                        value->color.g,
                        value->color.b);

  /* Set line width */
  value = vtile_mapcss_style_get (data->style, "width");
  cairo_set_line_width (cr, value->num);

  /* Set linecap */
  value = vtile_mapcss_style_get (data->style, "linecap");
  cairo_set_line_cap (cr, value->line_cap);

  /* Set linejoin */
  value = vtile_mapcss_style_get (data->style, "linejoin");
  cairo_set_line_cap (cr, value->line_join);

  /* Set dashes */
  value = vtile_mapcss_style_get (data->style, "dashes");

  mapbox_render_geometry (data, cr);

  if (data->feature->type == VECTOR_TILE__TILE__GEOM_TYPE__POLYGON) {
    cairo_stroke_preserve (cr);

    /* Set the fill color */
    value = vtile_mapcss_style_get (data->style, "fill-color");
    cairo_set_source_rgb (cr,
                          value->color.r,
                          value->color.g,
                          value->color.b);
    cairo_fill (cr);
  } else {
    cairo_stroke (cr);
  }

  vtile_mapcss_style_free (data->style);
  g_free (data);
}

static void
mapbox_set_canvas_style (VTileMapbox *mapbox,
                         cairo_t *cr)
{
  VTileMapCSSStyle *style;
  VTileMapCSSValue *value;

  style = vtile_mapcss_get_style (mapbox->priv->stylesheet, "canvas",
                                  NULL, mapbox->priv->zoom_level);

  value = vtile_mapcss_style_get (style, "fill-color");
  cairo_set_source_rgb (cr,
                        value->color.r,
                        value->color.g,
                        value->color.b);
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

    mapbox_get_layer_data (layer->name, &primary_tag, &layer_index);

    for (f = 0; f < layer->n_features; f++) {
      VectorTile__Tile__Feature *feature = layer->features[f];

      mapbox_process_feature (mapbox, feature, layer, primary_tag, layer_index);
    }
  }

  for (l = 0; l < NUM_RENDER_LAYERS; l++) {
    GList *layer = mapbox->priv->render_layers[l];

    if (layer) {
      layer = g_list_sort (layer, (GCompareFunc) mapbox_compare_z_index);
      g_list_foreach (layer, (GFunc) mapbox_render_feature, cr);
      g_list_free (layer);
    }
  }

  return TRUE;
}

gboolean
vtile_mapbox_render (VTileMapbox *mapbox, cairo_t *cr,
                     GError **error)
{
  VectorTile__Tile *tile;
  gint n;
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

void vtile_mapbox_render_async (VTileMapbox *mapbox,
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

gboolean
vtile_mapbox_render_finish (VTileMapbox *mapbox,
                            GAsyncResult *result,
                            GError **error)
{
  g_return_val_if_fail (g_task_is_valid (result, mapbox), FALSE);

  return g_task_propagate_boolean (G_TASK (result), error);
}

void
vtile_mapbox_dump_info (VTileMapbox *mapbox)
{
  gint l, f;
  VectorTile__Tile *tile;

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
      g_print("\n");
    }
  }
  vector_tile__tile__free_unpacked (tile, NULL);
}
