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

#include <cairo.h>
#include <pango/pango.h>

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

typedef struct {
  gint offset_x;
  gint offset_y;
  gint width;
  gint height;
  cairo_surface_t *surface;
  char *uid;
} VTileMapboxText;

#define VTILE_MAPBOX_ERROR (vtile_mapbox_error_quark ())

/**
 * VTileMapboxError:
 * @VTILE_MAPBOX:ERROR_LOAD: An error occured loading the tile.
 *
 * Error codes returned by vtile_mapbox functions.
 */
typedef enum {
  VTILE_MAPBOX_ERROR_LOAD
} VTileMapboxError;

VTileMapbox *vtile_mapbox_new (guint tile_size,
                               guint zoom_level);

gboolean
vtile_mapbox_load (VTileMapbox *mapbox,
                   guint8 *data,
                   gsize size,
                   GError **error);
gboolean
vtile_mapbox_load_from_file (VTileMapbox *mapbox,
                             const char *filename,
                             GError **error);

void vtile_mapbox_set_stylesheet (VTileMapbox *mapbox,
                                  VTileMapCSS *stylesheet);

gboolean vtile_mapbox_render (VTileMapbox *mapbox,
                              cairo_t *cr,
                              GError **error);

void vtile_mapbox_render_async (VTileMapbox *mapbox,
                                cairo_t *cr,
                                GAsyncReadyCallback callback,
                                gpointer user_data);

gboolean vtile_mapbox_render_finish (VTileMapbox *mapbox,
                                     GAsyncResult *result,
                                     GError **error);

GList *vtile_mapbox_get_texts (VTileMapbox *mapbox);

GQuark vtile_mapbox_error_quark (void);

G_END_DECLS

#endif /* VECTOR_TILE_MAPBOX */
