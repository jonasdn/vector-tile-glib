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

#ifndef __VECTOR_TILE_MAPCSS_BOXED_H__
#define __VECTOR_TILE_MAPCSS_BOXED_H__

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * VTileMapCSSColor:
 * @r: red component
 * @g: green component
 * @b: blue component
 *
 * All values are between 0.0 and 1.0.
 */
typedef struct {
  gdouble r;
  gdouble g;
  gdouble b;
} VTileMapCSSColor;

/**
 * VTileMapCSSDash:
 * @dashes: a pattern of on and off, example { 2, 4, 2, 4 }
 * @num_dashes: The number of dash patterns supplied,
 *
 * Represents a dash patterh for a line.
 */
typedef struct {
  gdouble dashes[4];
  gint num_dashes;
} VTileMapCSSDash;

GType vtile_mapcss_color_get_type (void);
#define VTILE_TYPE_MAPCSS_COLOR (vtile_mapcss_color_get_type);

GType vtile_mapcss_dash_get_type (void);
#define VTILE_TYPE_MAPCSS_DASH (vtile_mapcss_dash_get_type);

GType vtile_mapcss_style_get_type (void);
#define VTILE_TYPE_MAPCSS_STYLE (vtile_mapcss_style_get_type);

G_END_DECLS

#endif
