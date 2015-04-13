#ifndef __VECTOR_TILE_MAPCSS_STYLE_H__
#define __VECTOR_TILE_MAPCSS_STYLE_H__

#include <glib.h>

typedef struct {
  GHashTable *properties;
  GHashTable *tags;
} VTileMapCSSStyle;

typedef enum {
  VTILE_MAPCSS_VALUE_TYPE_COLOR,
  VTILE_MAPCSS_VALUE_TYPE_NUMBER,
  VTILE_MAPCSS_VALUE_TYPE_STRING,
  VTILE_MAPCSS_VALUE_TYPE_DASH,
  VTILE_MAPCSS_VALUE_TYPE_LINE_CAP,
  VTILE_MAPCSS_VALUE_TYPE_LINE_JOIN,
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
    VTileMapCSSLineCap line_cap;
    VTileMapCSSLineJoin line_join;
    gdouble num;
    char *str;
  };
} VTileMapCSSValue;

VTileMapCSSStyle *vtile_mapcss_style_new ();
void vtile_mapcss_style_free (VTileMapCSSStyle *style);
VTileMapCSSValue *vtile_mapcss_style_get (VTileMapCSSStyle *style,
                                          const char *name);


#endif /* __VECTOR_TILE_MAPCSS_STYLE_H__ */
