#include <string.h>

#include "vector-tile-mapcss-private.h"
#include "vector-tile-mapcss-style.h"

static void
vtile_mapcss_style_add_num (VTileMapCSSStyle *style,
                            const char *name,
                            gdouble num)
{
  VTileMapCSSValue *value = vtile_mapcss_value_new ();

  value->type = VTILE_MAPCSS_VALUE_TYPE_NUMBER,
  value->num = num;
  g_hash_table_insert (style->properties, g_strdup (name), value);
}

static void
vtile_mapcss_style_add_color (VTileMapCSSStyle *style,
                              const char *name,
                              VTileMapCSSColor color)
{
  VTileMapCSSValue *value = vtile_mapcss_value_new ();

  value->type = VTILE_MAPCSS_VALUE_TYPE_COLOR,
  value->color = color;
  g_hash_table_insert (style->properties, g_strdup (name), value);
}

static void
vtile_mapcss_style_add_dashes (VTileMapCSSStyle *style,
                               const char *name,
                               VTileMapCSSDash dash)
{
  VTileMapCSSValue *value = vtile_mapcss_value_new ();

  value->type = VTILE_MAPCSS_VALUE_TYPE_DASH,
  value->dash = dash;
  g_hash_table_insert (style->properties, g_strdup (name), value);
}

static void
vtile_mapcss_style_add_enum (VTileMapCSSStyle *style,
                             const char *name,
                             gint enum_value)
{
  VTileMapCSSValue *value = vtile_mapcss_value_new ();

  value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM,
  value->enum_value = enum_value;
  g_hash_table_insert (style->properties, g_strdup (name), value);
}

static void
vtile_mapcss_style_add_str (VTileMapCSSStyle *style,
                            const char *name,
                            char *str)
{
  VTileMapCSSValue *value = vtile_mapcss_value_new ();

  value->type = VTILE_MAPCSS_VALUE_TYPE_STRING;
  value->str = g_strdup (str);
  g_hash_table_insert (style->properties, g_strdup (name), value);
}

static VTileMapCSSValue *
vtile_mapcss_style_get (VTileMapCSSStyle *style, const char *name)
{
  return g_hash_table_lookup (style->properties, name);
}

gdouble
vtile_mapcss_style_get_num (VTileMapCSSStyle *style,
                            const char *name)
{
  VTileMapCSSValue *value;

  value = vtile_mapcss_style_get (style, name);
  if (value)
    return value->num;

  return -G_MAXDOUBLE;
}

VTileMapCSSColor *
vtile_mapcss_style_get_color (VTileMapCSSStyle *style,
                              const char *name)
{
  VTileMapCSSValue *value;

  value = vtile_mapcss_style_get (style, name);
  if (value)
    return &value->color;

  return NULL;
}

VTileMapCSSDash *
vtile_mapcss_style_get_dash (VTileMapCSSStyle *style,
                             const char *name)
{
  VTileMapCSSValue *value;

  value = vtile_mapcss_style_get (style, name);
  if (value)
    return &value->dash;

  return NULL;
}

gint
vtile_mapcss_style_get_enum (VTileMapCSSStyle *style,
                             const char *name)
{
  VTileMapCSSValue *value;

  value = vtile_mapcss_style_get (style, name);
  if (value)
      return value->enum_value;

  return -1;
}

char *
vtile_mapcss_style_get_str (VTileMapCSSStyle *style,
                            const char *name)
{
  VTileMapCSSValue *value;

  value = vtile_mapcss_style_get (style, name);
  if (value)
      return value->str;

  return NULL;

}

VTileMapCSSStyle *
vtile_mapcss_style_new ()
{
  VTileMapCSSStyle *style = g_new0 (VTileMapCSSStyle, 1);
  VTileMapCSSColor color;

  style->properties = g_hash_table_new_full (g_str_hash, g_str_equal,
                                             g_free,
                                             (GDestroyNotify) vtile_mapcss_value_free);

  vtile_mapcss_style_add_num (style, "width", 1.0);
  vtile_mapcss_style_add_num (style, "z-index", 0.0);
  vtile_mapcss_style_add_num (style, "opacity", 1.0);
  vtile_mapcss_style_add_num (style, "fill-opacity", 1.0);
  vtile_mapcss_style_add_num (style, "casing-opacity", 1.0);
  vtile_mapcss_style_add_num (style, "casing-width", 0.0);

  vtile_mapcss_style_add_color (style, "fill-color",
                                (VTileMapCSSColor) {
                                  .r = 0.5,
                                  .g = 0.5,
                                  .b = 0.5
                                 });
  vtile_mapcss_style_add_color (style, "color",
                                (VTileMapCSSColor) {
                                  .r = 0.0,
                                  .g = 0.0,
                                  .b = 0.0
                                 });
  vtile_mapcss_style_add_color (style, "casing-color",
                                (VTileMapCSSColor) {
                                  .r = 0.0,
                                  .g = 0.0,
                                  .b = 0.0
                                 });
  vtile_mapcss_style_add_dashes (style, "dashes",
                                 (VTileMapCSSDash) {
                                   .num_dashes = 0,
                                     .dashes = { 0 }
                                   });
  vtile_mapcss_style_add_enum (style, "linecap",
                               VTILE_MAPCSS_LINE_CAP_NONE);

  vtile_mapcss_style_add_enum (style, "linejoin",
                               VTILE_MAPCSS_LINE_JOIN_ROUND);

  vtile_mapcss_style_add_str (style, "font-family", "DejaVu");
  vtile_mapcss_style_add_num (style, "font-size", 12);
  vtile_mapcss_style_add_enum (style, "font-weight",
                               VTILE_MAPCSS_FONT_WEIGHT_NORMAL);
  vtile_mapcss_style_add_enum (style, "font-style",
                               VTILE_MAPCSS_FONT_STYLE_NORMAL);
  vtile_mapcss_style_add_enum (style, "font-variant",
                               VTILE_MAPCSS_FONT_VARIANT_NORMAL);
  vtile_mapcss_style_add_enum (style, "text-decoration",
                               VTILE_MAPCSS_TEXT_DECORATION_NONE);
  vtile_mapcss_style_add_enum (style, "text-transform",
                               VTILE_MAPCSS_TEXT_TRANSFORM_NONE);
  vtile_mapcss_style_add_enum (style, "text-position",
                               VTILE_MAPCSS_TEXT_POSITION_CENTER);
  vtile_mapcss_style_add_color (style, "text-color",
                                (VTileMapCSSColor) {
                                  .r = 0.0,
                                  .g = 0.0,
                                  .b = 0.0
                                });
  vtile_mapcss_style_add_color (style, "text-halo-color",
                                (VTileMapCSSColor) {
                                  .r = 0.0,
                                  .g = 0.0,
                                  .b = 0.0
                                });
  vtile_mapcss_style_add_num (style, "text-opacity", 1.0);
  vtile_mapcss_style_add_num (style, "text-offset", 0.0);
  vtile_mapcss_style_add_num (style, "text-halo-radius", 0.0);

  return style;
}

void
vtile_mapcss_style_free (VTileMapCSSStyle *style)
{
  g_hash_table_destroy (style->properties);
  g_free (style);
}
