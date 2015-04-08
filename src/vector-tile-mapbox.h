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

#include "vector-tile-mapcss.h"

G_BEGIN_DECLS

GType vtile_mapbox_get_type (void) G_GNUC_CONST;

#define VTILE_TYPE_MAPBOX                  (vtile_mapbox_get_type ())
#define VTILE_MAPBOX(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), VTILE_TYPE_MAPBOX, VTileMapbox))
#define VTILE_IS_MAPBOX(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VTILE_TYPE_MAPBOX))
#define VTILE_MAPBOX_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), VTILE_TYPE_MAPBOX, VTileMapboxClass))
#define VTILE_IS_MAPBOX_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), VTILE_TYPE_MAPBOX))
#define VTILE_MAPBOX_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), VTILE_TYPE_MAPBOX, VTileMapboxClass))

typedef struct _VTileMapbox        VTileMapbox;
typedef struct _VTileMapboxClass   VTileMapboxClass;
typedef struct _VTileMapboxPrivate VTileMapboxPrivate;

struct _VTileMapbox {
  /* <private> */
  GObject parent_instance;
  VTileMapboxPrivate *priv;
};

struct _VTileMapboxClass {
  /* <private> */
  GObjectClass parent_class;
};

VTileMapbox *vtile_mapbox_new (guint8 *data, gssize length, gint tile_size);
void vtile_mapbox_set_stylesheet (VTileMapbox *mapbox,
                                  VTileMapCSS *stylesheet);
gboolean vtile_mapbox_render_to_cairo (VTileMapbox *mapbox,
                                       cairo_t *cr, GError **error);

G_END_DECLS

#endif /* VECTOR_TILE_MAPBOX */
