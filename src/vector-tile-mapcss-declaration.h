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

#ifndef __VECTOR_TILE_MAPCSS_DECLARATION_H__
#define __VECTOR_TILE_MAPCSS_DECLARATION_H__

#include <gio/gio.h>

#include "vector-tile-mapcss.h"

G_BEGIN_DECLS

GType vtile_mapcss_declaration_get_type (void) G_GNUC_CONST;

#define VTILE_TYPE_MAPCSS_DECLARATION                  (vtile_mapcss_declaration_get_type ())
#define VTILE_MAPCSS_DECLARATION(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), VTILE_TYPE_MAPCSS_DECLARATION, VTileMapCSSDeclaration))
#define VTILE_IS_MAPCSS_DECLARATION(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VTILE_TYPE_MAPCSS_DECLARATION))
#define VTILE_MAPCSS_DECLARATION_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), VTILE_TYPE_MAPCSS_DECLARATION, VTileMapCSSDeclarationClass))
#define VTILE_IS_MAPCSS_DECLARATION_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), VTILE_TYPE_MAPCSS_DECLARATION))
#define VTILE_MAPCSS_DECLARATION_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), VTILE_TYPE_MAPCSS_DECLARATION, VTileMapCSSDeclarationClass))

typedef struct _VTileMapCSSDeclaration        VTileMapCSSDeclaration;
typedef struct _VTileMapCSSDeclarationClass   VTileMapCSSDeclarationClass;
typedef struct _VTileMapCSSDeclarationPrivate VTileMapCSSDeclarationPrivate;

struct _VTileMapCSSDeclaration {
  /* <private> */
  GObject parent_instance;
  VTileMapCSSDeclarationPrivate *priv;
};

struct _VTileMapCSSDeclarationClass {
  /* <private> */
  GObjectClass parent_class;
};


VTileMapCSSDeclaration *vtile_mapcss_declaration_new (char *property,
						      VTileMapCSSValue *value);
VTileMapCSSValue *vtile_mapcss_declaration_get_value (VTileMapCSSDeclaration *declaration);
char * vtile_mapcss_declaration_get_property (VTileMapCSSDeclaration *declaration);

G_END_DECLS

#endif /* VECTOR_TILE_MAPCSS_DECLARATION */
