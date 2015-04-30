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

#ifndef __VECTOR_TILE_MAPCSS_VALUE_H__
#define __VECTOR_TILE_MAPCSS_VALUE_H__

#include "vector-tile-boxed.h"

typedef enum {
  VTILE_MAPCSS_VALUE_TYPE_COLOR,
  VTILE_MAPCSS_VALUE_TYPE_NUMBER,
  VTILE_MAPCSS_VALUE_TYPE_STRING,
  VTILE_MAPCSS_VALUE_TYPE_DASH,
  VTILE_MAPCSS_VALUE_TYPE_ENUM,
} VTileMapCSSValueType;

typedef struct {
  VTileMapCSSValueType type;
  union {
    VTileMapCSSColor color;
    VTileMapCSSDash dash;
    gint enum_value;
    gdouble num;
    char *str;
  };
} VTileMapCSSValue;

#endif
