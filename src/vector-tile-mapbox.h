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

#ifndef __VECTOR_TILE_MAPBOX_H__
#define __VECTOR_TILE_MAPBOX_H__

G_BEGIN_DECLS

GType vector_tile_mapbox_get_type (void) G_GNUC_CONST;

#define VECTOR_TILE_TYPE_MAPBOX                  (vector_tile_mapbox_get_type ())
#define VECTOR_TILE_MAPBOX(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), VECTOR_TILE_TYPE_MAPBOX, VectorTileMapbox))
#define VECTOR_TILE_IS_MAPBOX(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VECTOR_TILE_TYPE_MAPBOX))
#define VECTOR_TILE_MAPBOX_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), VECTOR_TILE_TYPE_MAPBOX, VectorTileMapboxClass))
#define VECTOR_TILE_IS_MAPBOX_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), VECTOR_TILE_TYPE_MAPBOX))
#define VECTOR_TILE_MAPBOX_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), VECTOR_TILE_TYPE_MAPBOX, VectorTileMapboxClass))

typedef struct _VectorTileMapbox        VectorTileMapbox;
typedef struct _VectorTileMapboxClass   VectorTileMapboxClass;
typedef struct _VectorTileMapboxPrivate VectorTileMapboxPrivate;

struct _VectorTileMapbox {
  /* <private> */
  GObject parent_instance;
  VectorTileMapboxPrivate *priv;
};

struct _VectorTileMapboxClass {
  /* <private> */
  GObjectClass parent_class;
};

VectorTileMapbox *vector_tile_mapbox_new (guint8 *data, gssize length, gint tile_size);
gboolean vector_tile_mapbox_render_to_cairo (VectorTileMapbox *mapbox,
                                             cairo_t *cr, GError **error);

G_END_DECLS

#endif /* VECTOR_TILE_MAPBOX */
