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

#ifndef __VECTOR_TILE_MAPCSS_H__
#define __VECTOR_TILE_MAPCSS_H__

#include <gio/gio.h>

#include "vector-tile-mapcss-style.h"

G_BEGIN_DECLS

GType vtile_mapcss_get_type (void) G_GNUC_CONST;

#define VTILE_TYPE_MAPCSS                  (vtile_mapcss_get_type ())
#define VTILE_MAPCSS(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), VTILE_TYPE_MAPCSS, VTileMapCSS))
#define VTILE_IS_MAPCSS(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VTILE_TYPE_MAPCSS))
#define VTILE_MAPCSS_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), VTILE_TYPE_MAPCSS, VTileMapCSSClass))
#define VTILE_IS_MAPCSS_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), VTILE_TYPE_MAPCSS))
#define VTILE_MAPCSS_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), VTILE_TYPE_MAPCSS, VTileMapCSSClass))

typedef struct _VTileMapCSS        VTileMapCSS;
typedef struct _VTileMapCSSClass   VTileMapCSSClass;
typedef struct _VTileMapCSSPrivate VTileMapCSSPrivate;

struct _VTileMapCSS {
  /* <private> */
  GObject parent_instance;
  VTileMapCSSPrivate *priv;
};

struct _VTileMapCSSClass {
  /* <private> */
  GObjectClass parent_class;
};

VTileMapCSS *vtile_mapcss_new (void);
gboolean vector_tile_mapcss_load (VTileMapCSS *mapcss,
                                  const char *filename,
                                  GError **error);
VTileMapCSSStyle *vtile_mapcss_get_style (VTileMapCSS *mapcss,
                                          const char *slector,
                                          GHashTable *tags,
                                          guint zoom);

G_END_DECLS

#endif /* VECTOR_TILE_MAPCSS */
