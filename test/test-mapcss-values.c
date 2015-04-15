#include <glib.h>
#include <locale.h>

#include "vector-tile-mapcss.h"
#include "vector-tile-mapcss-private.h"

VTileMapCSS *stylesheet;

static gboolean
mapcss_new_and_load (const char *filename)
{
  gboolean status;
  GError *error = NULL;

  stylesheet = vtile_mapcss_new ();

  status = vtile_mapcss_load (stylesheet, filename, &error);
  if (!status) {
    g_print ("%s\n", error->message);
    g_error_free (error);

    return FALSE;
  }

  return TRUE;
}

static void
test_merge (void)
{
  char *filename = "merge.mapcss";
  GHashTable *tags;
  VTileMapCSSStyle *style;
  VTileMapCSSValue *value;

  g_assert (mapcss_new_and_load (filename));
  g_assert (stylesheet != NULL);

  g_assert_cmpint (vtile_mapcss_get_num_styles (stylesheet), ==, 5);

  style = vtile_mapcss_get_style (stylesheet, "area", NULL, 1);
  g_assert (style != NULL);
  value = vtile_mapcss_style_get (style, "width");
  g_assert_cmpfloat (value->num, ==, 5.0);
  vtile_mapcss_style_free (style);

  tags = g_hash_table_new (g_str_hash, g_str_equal);
  g_hash_table_insert (tags, "highway", "primary");

  style = vtile_mapcss_get_style (stylesheet, "way", tags, 1);
  g_assert (style != NULL);
  value = vtile_mapcss_style_get (style, "width");
  g_assert_cmpfloat (value->num, ==, 6.0);
  vtile_mapcss_style_free (style);

  g_hash_table_destroy (tags);
  g_object_unref (stylesheet);
}

static void
test_values (void)
{
  VTileMapCSSStyle *style;
  VTileMapCSSValue *value;
  char *filename = "basic.mapcss";

  g_assert (mapcss_new_and_load (filename));
  g_assert (stylesheet != NULL);

  style = vtile_mapcss_get_style (stylesheet, "way", NULL, 1);
  g_assert (style != NULL);

  value = vtile_mapcss_style_get (style, "casing-width");
  g_assert (value != NULL);
  g_assert (value->type == VTILE_MAPCSS_VALUE_TYPE_NUMBER);
  g_assert_cmpfloat (value->num, ==, 0.95);

  value = vtile_mapcss_style_get (style, "width");
  g_assert (value != NULL);
  g_assert (value->type == VTILE_MAPCSS_VALUE_TYPE_NUMBER);
  g_assert_cmpfloat (value->num, ==, 1.0);

  value = vtile_mapcss_style_get (style, "color");
  g_assert (value != NULL);
  g_assert (value->type == VTILE_MAPCSS_VALUE_TYPE_COLOR);
  g_assert_cmpfloat (value->color.r, ==, 1.0);
  g_assert_cmpfloat (value->color.g, ==, 0.0);
  g_assert_cmpfloat (value->color.b, ==, 0.0);

  value = vtile_mapcss_style_get (style, "casing-color");
  g_assert (value != NULL);
  g_assert (value->type == VTILE_MAPCSS_VALUE_TYPE_COLOR);
  g_assert_cmpfloat (value->color.r, ==, 0.0);
  g_assert_cmpfloat (value->color.g, ==, 0.0);
  g_assert_cmpfloat (value->color.b, ==, 1.0);

  value = vtile_mapcss_style_get (style, "linecap");
  g_assert (value != NULL);
  g_assert (value->type == VTILE_MAPCSS_VALUE_TYPE_LINE_CAP);
  g_assert_cmpint (value->line_cap, ==, VTILE_MAPCSS_LINE_CAP_ROUND);

  value = vtile_mapcss_style_get (style, "dashes");
  g_assert (value != NULL);
  g_assert (value->type == VTILE_MAPCSS_VALUE_TYPE_DASH);
  g_assert_cmpint (value->dash.num_dashes, ==, 2);
  g_assert_cmpfloat (value->dash.dashes[0], ==, 2);
  g_assert_cmpfloat (value->dash.dashes[1], ==, 4);

  vtile_mapcss_style_free (style);
  g_object_unref (stylesheet);
}

int
main (int argc, char *argv[])
{
  setlocale (LC_ALL, "");

  g_test_init (&argc, &argv, NULL);
  g_test_bug_base ("http://bugzilla.gnome.org/show_bug.cgi?id=");

  g_test_add_func ("/test/values", test_values);
  g_test_add_func ("/test/merge", test_merge);

  return g_test_run ();
}
