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

VTileMapCSSValue *
vtile_mapcss_value_new ()
{
  VTileMapCSSValue *value = g_new0 (VTileMapCSSValue, 1);

  value->str = NULL;

  return value;
}

VTileMapCSSValue *
vtile_mapcss_value_copy (VTileMapCSSValue *src)
{
  VTileMapCSSValue *dest = g_new0 (VTileMapCSSValue, 1);

  memcpy (dest, src, sizeof (VTileMapCSSValue));
  if (src->type == VTILE_MAPCSS_VALUE_TYPE_STRING)
    dest->str = g_strdup (src->str);

  return dest;
}

void
vtile_mapcss_value_free (VTileMapCSSValue *value)
{
  if (value->type == VTILE_MAPCSS_VALUE_TYPE_STRING)
    g_free (value->str);

  g_free (value);
}
