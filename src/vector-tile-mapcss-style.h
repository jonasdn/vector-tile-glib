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

#ifndef __VECTOR_TILE_MAPCSS_STYLE_H__
#define __VECTOR_TILE_MAPCSS_STYLE_H__

#include <glib.h>

#include "vector-tile-mapcss-private.h"
#include "vector-tile-boxed.h"
G_BEGIN_DECLS


/**
 * VTileMapCSSStyle
 */
typedef struct _VTileMapCSSStyle VTileMapCSSStyle;

/**
 * VTileMapCSSEnumValue:
 */
typedef enum {
  VTILE_MAPCSS_VALUE_NONE,
  VTILE_MAPCSS_VALUE_ROUND,
  VTILE_MAPCSS_VALUE_SQUARE,
  VTILE_MAPCSS_VALUE_MITER,
  VTILE_MAPCSS_VALUE_BEVEL,
  VTILE_MAPCSS_VALUE_LINE,
  VTILE_MAPCSS_VALUE_CENTER,
  VTILE_MAPCSS_VALUE_UPPERCASE,
  VTILE_MAPCSS_VALUE_LOWERCASE,
  VTILE_MAPCSS_VALUE_CAPITALIZE,
  VTILE_MAPCSS_VALUE_UNDERLINE,
  VTILE_MAPCSS_VALUE_NORMAL,
  VTILE_MAPCSS_VALUE_ITALIC,
  VTILE_MAPCSS_VALUE_BOLD,
  VTILE_MAPCSS_VALUE_SMALL_CAPS
} VTileMapCSSEnumValue;

VTileMapCSSStyle *vtile_mapcss_style_new ();
void vtile_mapcss_style_free (VTileMapCSSStyle *style);

gdouble vtile_mapcss_style_get_num (VTileMapCSSStyle *style,
                                    const char *name);
VTileMapCSSColor *vtile_mapcss_style_get_color (VTileMapCSSStyle *style,
                                                const char *name);
VTileMapCSSDash *vtile_mapcss_style_get_dash (VTileMapCSSStyle *style,
                                              const char *name);
VTileMapCSSEnumValue vtile_mapcss_style_get_enum (VTileMapCSSStyle *style,
                                                  const char *name);

char *vtile_mapcss_style_get_str (VTileMapCSSStyle *style,
                                  const char *name);

G_END_DECLS
#endif /* __VECTOR_TILE_MAPCSS_STYLE_H__ */
