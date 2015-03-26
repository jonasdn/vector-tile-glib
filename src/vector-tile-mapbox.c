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

#include "vector-tile-mapbox.h"
#include "vector_tile.pb-c.h"

#define ZIGZAG_DECODE(val) (((val) >> 1) ^ (-((val) & 1)))

#define GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), VECTOR_TILE_TYPE_MAPBOX, VectorTileMapboxPrivate))

enum {
  MAPBOX_CMD_MOVE_TO = 1,
  MAPBOX_CMD_LINE_TO = 2,
  MAPBOX_CMD_CLOSE_PATH = 7
};

struct _VectorTileMapboxPrivate {
  guint8 *data;
  gssize size;
  gint tile_size;
};

typedef struct {
  gdouble r;
  gdouble g;
  gdouble b;
  gdouble alpha;
} VectorTileMapboxColor;

G_DEFINE_TYPE (VectorTileMapbox, vector_tile_mapbox, G_TYPE_OBJECT)

static void
vector_tile_mapbox_finalize (GObject *vmapbox)
{
  VectorTileMapbox *mapbox = (VectorTileMapbox *) vmapbox;

  G_OBJECT_CLASS (vector_tile_mapbox_parent_class)->finalize (vmapbox);
}

static void
vector_tile_mapbox_class_init (VectorTileMapboxClass *klass)
{
  GObjectClass *mapbox_class = G_OBJECT_CLASS (klass);

  mapbox_class->finalize = vector_tile_mapbox_finalize;

  g_type_class_add_private (klass, sizeof (VectorTileMapboxPrivate));
}

static void
vector_tile_mapbox_init (VectorTileMapbox *mapbox)
{
  VectorTileMapboxPrivate *priv = GET_PRIVATE (mapbox);

  mapbox->priv = priv;
  priv->data = NULL;
  priv->size = 0;
}

VectorTileMapbox *
vector_tile_mapbox_new (guint8 *data, gssize size, gint tile_size)
{
  VectorTileMapbox *mapbox;
  VectorTileMapboxPrivate *priv;

  mapbox = g_object_new (VECTOR_TILE_TYPE_MAPBOX, NULL);
  priv = GET_PRIVATE (mapbox);

  priv->data = data;
  priv->size = size;
  priv->tile_size = tile_size;

  return mapbox;
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
mapbox_render_feature (VectorTile__Tile__Feature *feature,
                       VectorTile__Tile__Layer *layer, cairo_t *cr)
{
  gint p_geom = 0;
  gint n;
  VectorTileMapboxColor fill_color;

  /*
   * Tags of the feature are encoded as repeated pairs of
   * integers. Even indexed values (n, beginning with 0) are
   * themselves indexes into the layer's keys list. Odd indexed
   * values (n+1) are indexes into the layer's values list.
   * The first (n=0) tag of a feature, therefore, has a key of
   * layer.keys[feature.tags[0]] and a value of
   * layer.values[feature.tags[1]].
   */
  for (n = 0; n < feature->n_tags; n += 2) {
    const char *key = layer->keys[feature->tags[n]];

    if (!strncmp (key, "land", 4)) {
        fill_color.r = 0.961;
        fill_color.g = 0.992;
        fill_color.b = 0.941;
        break;
    } else {
        fill_color.r = 0.752;
        fill_color.g = 0.847;
        fill_color.b = 1.0;
    }
  }

  switch (feature->type) {
  case VECTOR_TILE__TILE__GEOM_TYPE__POINT:
  case VECTOR_TILE__TILE__GEOM_TYPE__LINESTRING:
  case VECTOR_TILE__TILE__GEOM_TYPE__UNKNOWN:
    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
    cairo_set_line_width (cr, 1.0);
    break;

  case VECTOR_TILE__TILE__GEOM_TYPE__POLYGON:
    cairo_set_source_rgb (cr, fill_color.r, fill_color.g, fill_color.b);
    break;
  }

  do {
    gint cmd;
    gint length;

    cmd = feature->geometry[p_geom] & 7L;
    length = feature->geometry[p_geom] >> 3;

    if (cmd == MAPBOX_CMD_MOVE_TO || cmd == MAPBOX_CMD_LINE_TO) {
      gint32 from;
      gint32 to;

      for (n = 0; n < length; n++) {
        guint32 parameter;

        parameter = feature->geometry[++p_geom];
        from = ZIGZAG_DECODE (parameter);

        parameter = feature->geometry[++p_geom];
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
  } while(p_geom < feature->n_geometry);

  if (feature->type == VECTOR_TILE__TILE__GEOM_TYPE__POLYGON) {
    cairo_stroke_preserve (cr);
    cairo_fill (cr);
  } else {
    cairo_stroke_preserve (cr);
  }
}

static gboolean
mapbox_render_tile (VectorTileMapbox *mapbox, VectorTile__Tile *tile,
                    cairo_t *cr)
{
  VectorTileMapboxPrivate *priv = GET_PRIVATE (mapbox);
  gint l;

  cairo_set_source_rgb (cr, 0.961, 0.992, 0.941);
  cairo_rectangle (cr, 0.0, 0.0, priv->tile_size, priv->tile_size);
  cairo_fill (cr);

  /* We need this since we are using relative move_to and line_to */
  cairo_move_to (cr, 0, 0);

  for (l = 0; l < tile->n_layers; l++) {
    VectorTile__Tile__Layer *layer = tile->layers[l];
    gint f;

    /* layer->extent: The bounding box for the tile spans from 0..4095 units */
    gdouble scale = (gdouble) priv->tile_size / layer->extent;
    cairo_scale (cr, scale, scale);

    for (f = 0; f < layer->n_features; f++) {
      VectorTile__Tile__Feature *feature = layer->features[f];

      mapbox_render_feature (feature, layer, cr);
    }
  }

  return TRUE;
}

gboolean
vector_tile_mapbox_render_to_cairo (VectorTileMapbox *mapbox, cairo_t *cr,
                                    GError **error)
{
  VectorTileMapboxPrivate *priv = GET_PRIVATE (mapbox);
  VectorTile__Tile *tile;
  gint n;

  g_return_val_if_fail (mapbox != NULL, FALSE);
  g_return_val_if_fail (cr != NULL, FALSE);

  tile = vector_tile__tile__unpack (NULL,
                                    priv->size,
                                    priv->data);
  if (!tile) {
    return FALSE;
  }

  return mapbox_render_tile (mapbox, tile, cr);
}
