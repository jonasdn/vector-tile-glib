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

#include <glib-object.h>
#include <string.h>

#include <vector-tile-boxed.h>
#include <vector-tile-mapbox.h>

static VTileMapCSSStyle *
vtile_mapcss_style_copy (const VTileMapCSSStyle *src)
{
  VTileMapCSSStyle *dest = g_new (VTileMapCSSStyle, 1);
  dest->properties = g_hash_table_ref (src->properties);

  return dest;
}

static VTileMapboxText *
vtile_mapbox_text_copy (const VTileMapboxText *src)
{
  VTileMapboxText *dest = g_new (VTileMapboxText, 1);

  memcpy (dest, src, sizeof (VTileMapboxText));
  return dest;
}

static VTileMapCSSColor *
vtile_mapcss_color_copy (const VTileMapCSSColor *src)
{
  VTileMapCSSColor *dest = g_new (VTileMapCSSColor, 1);

  memcpy (dest, src, sizeof (VTileMapCSSColor));
  return dest;
}

static VTileMapCSSDash *
vtile_mapcss_dash_copy (const VTileMapCSSDash *src)
{
  VTileMapCSSDash *dest = g_new (VTileMapCSSDash, 1);

  memcpy (dest, src, sizeof (VTileMapCSSDash));
  return dest;
}

G_DEFINE_BOXED_TYPE (VTileMapCSSStyle, vtile_mapcss_style, vtile_mapcss_style_copy, vtile_mapcss_style_free)
G_DEFINE_BOXED_TYPE (VTileMapCSSColor, vtile_mapcss_color, vtile_mapcss_color_copy, g_free)
G_DEFINE_BOXED_TYPE (VTileMapCSSDash, vtile_mapcss_dash, vtile_mapcss_dash_copy, g_free)
G_DEFINE_BOXED_TYPE (VTileMapboxText, vtile_mapbox_text, vtile_mapbox_text_copy, g_free)
