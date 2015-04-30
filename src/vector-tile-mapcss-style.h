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

typedef struct _VTileMapCSSStyle VTileMapCSSStyle;

struct _VTileMapCSSStyle {
  GHashTable *properties;
};

typedef enum {
  VTILE_MAPCSS_VALUE_TYPE_COLOR,
  VTILE_MAPCSS_VALUE_TYPE_NUMBER,
  VTILE_MAPCSS_VALUE_TYPE_STRING,
  VTILE_MAPCSS_VALUE_TYPE_DASH,
  VTILE_MAPCSS_VALUE_TYPE_ENUM,
} VTileMapCSSValueType;

typedef enum {
  VTILE_MAPCSS_LINE_CAP_NONE,
  VTILE_MAPCSS_LINE_CAP_ROUND,
  VTILE_MAPCSS_LINE_CAP_SQUARE
} VTileMapCSSLineCap;

typedef enum {
  VTILE_MAPCSS_LINE_JOIN_MITER,
  VTILE_MAPCSS_LINE_JOIN_ROUND,
  VTILE_MAPCSS_LINE_JOIN_BEVEL
} VTileMapCSSLineJoin;

typedef enum {
  VTILE_MAPCSS_TEXT_POSITION_LINE,
  VTILE_MAPCSS_TEXT_POSITION_CENTER
} VTileMapCSSTextPosition;

typedef enum {
  VTILE_MAPCSS_TEXT_TRANSFORM_NONE,
  VTILE_MAPCSS_TEXT_TRANSFORM_UPPERCASE,
  VTILE_MAPCSS_TEXT_TRANSFORM_LOWERCASE,
  VTILE_MAPCSS_TEXT_TRANSFORM_CAPITALIZE
} VTileMapCSSTextTransform;

typedef enum {
  VTILE_MAPCSS_TEXT_DECORATION_NONE,
  VTILE_MAPCSS_TEXT_DECORATION_UNDERLINE
} VTileMapCSSTextDecoration;

typedef enum {
  VTILE_MAPCSS_FONT_STYLE_NORMAL,
  VTILE_MAPCSS_FONT_STYLE_ITALIC
} VTileMapCSSFontStyle;

typedef enum {
  VTILE_MAPCSS_FONT_WEIGHT_NORMAL,
  VTILE_MAPCSS_FONT_WEIGHT_BOLD
} VTileMapCSSFontWeight;

typedef enum {
  VTILE_MAPCSS_FONT_VARIANT_NORMAL,
  VTILE_MAPCSS_FONT_VARIANT_SMALL_CAPS
} VTileMapCSSFontVariant;

typedef struct {
  gdouble r;
  gdouble g;
  gdouble b;
} VTileMapCSSColor;

typedef struct {
  gdouble dashes[4];
  gint num_dashes;
} VTileMapCSSDash;

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


GType vtile_mapcss_color_get_type (void);
#define VTILE_TYPE_MAPCSS_COLOR (vtile_mapcss_color_get_type);

GType vtile_mapcss_dash_get_type (void);
#define VTILE_TYPE_MAPCSS_DASH (vtile_mapcss_dash_get_type);

GType vtile_mapcss_style_get_type (void);
#define VTILE_TYPE_MAPCSS_STYLE (vtile_mapcss_style_get_type);

VTileMapCSSStyle *vtile_mapcss_style_new ();
void vtile_mapcss_style_free (VTileMapCSSStyle *style);

gdouble vtile_mapcss_style_get_num (VTileMapCSSStyle *style,
                                    const char *name);
VTileMapCSSColor *vtile_mapcss_style_get_color (VTileMapCSSStyle *style,
                                                const char *name);
VTileMapCSSDash *vtile_mapcss_style_get_dash (VTileMapCSSStyle *style,
                                              const char *name);
gint vtile_mapcss_style_get_enum (VTileMapCSSStyle *style,
                                  const char *name);

char *vtile_mapcss_style_get_str (VTileMapCSSStyle *style,
                                  const char *name);

#endif /* __VECTOR_TILE_MAPCSS_STYLE_H__ */
