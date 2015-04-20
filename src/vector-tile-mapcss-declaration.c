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

#include <string.h>

#include "vector-tile-mapcss-private.h"
#include "vector-tile-mapcss-declaration.h"

struct _VTileMapCSSDeclarationPrivate {
  char *property;
  VTileMapCSSValue *value;
};

G_DEFINE_TYPE_WITH_PRIVATE (VTileMapCSSDeclaration, vtile_mapcss_declaration, G_TYPE_OBJECT)

static void
vtile_mapcss_declaration_finalize (GObject *vdeclaration)
{
  VTileMapCSSDeclaration *declaration = (VTileMapCSSDeclaration *) vdeclaration;

  if (declaration->priv->property)
    g_free (declaration->priv->property);

  vtile_mapcss_value_free (declaration->priv->value);

  G_OBJECT_CLASS (vtile_mapcss_declaration_parent_class)->finalize (vdeclaration);
}

static void
vtile_mapcss_declaration_class_init (VTileMapCSSDeclarationClass *klass)
{
  GObjectClass *declaration_class = G_OBJECT_CLASS (klass);

  declaration_class->finalize = vtile_mapcss_declaration_finalize;
}

static void
vtile_mapcss_declaration_init (VTileMapCSSDeclaration *declaration)
{
  declaration->priv = vtile_mapcss_declaration_get_instance_private (declaration);
}


VTileMapCSSDeclaration *
vtile_mapcss_declaration_new (char *property, VTileMapCSSValue *value)
{
  VTileMapCSSDeclaration *declaration;

  declaration = g_object_new (VTILE_TYPE_MAPCSS_DECLARATION, NULL);

  declaration->priv->property = property;
  declaration->priv->value = value;

  return declaration;
}

VTileMapCSSValue *
vtile_mapcss_declaration_get_value (VTileMapCSSDeclaration *declaration)
{
  return declaration->priv->value;
}

char *
vtile_mapcss_declaration_get_property (VTileMapCSSDeclaration *declaration)
{
  return declaration->priv->property;
}
