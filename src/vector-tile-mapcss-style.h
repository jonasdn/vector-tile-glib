#ifndef __VECTOR_TILE_MAPCSS_STYLE_H__
#define __VECTOR_TILE_MAPCSS_STYLE_H__

#include <glib.h>

typedef struct {
  GHashTable *properties;
} VTileMapCSSStyle;

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
