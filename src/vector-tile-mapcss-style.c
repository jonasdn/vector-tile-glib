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
vtile_mapcss_style_add_line_cap (VTileMapCSSStyle *style,
                                 const char *name,
                                 VTileMapCSSLineCap line_cap)
{
  VTileMapCSSValue *value = vtile_mapcss_value_new ();

  value->type = VTILE_MAPCSS_VALUE_TYPE_LINECAP,
  value->line_cap = line_cap;
  g_hash_table_insert (style->properties, g_strdup (name), value);
}


VTileMapCSSValue *
vtile_mapcss_style_get (VTileMapCSSStyle *style, const char *name)
{
  return g_hash_table_lookup (style->properties, name);
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
  vtile_mapcss_style_add_line_cap (style, "linecap",
                                   VTILE_MAPCSS_LINE_CAP_NONE);
  return style;
}

void
vtile_mapcss_style_free (VTileMapCSSStyle *style)
{
  g_hash_table_destroy (style->properties);
  g_free (style);
}
