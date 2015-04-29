#include <glib.h>
#include <locale.h>

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
test_selector (void)
{
  char *filename = "selector.mapcss";
  VTileMapCSSStyle *style;
  GHashTable *tags;
  gdouble num;


  tags = g_hash_table_new (g_str_hash, g_str_equal);
  g_hash_table_insert (tags, "area", "yes");
  g_assert (mapcss_new_and_load (filename));

  style = vtile_mapcss_get_style (stylesheet,
                                  VTILE_MAPCSS_SELECTOR_TYPE_CANVAS,
                                  NULL, 1);
  g_assert (style != NULL);
  num = vtile_mapcss_style_get_num (style, "width");
  g_assert_cmpfloat (num, ==, 2.0);
  vtile_mapcss_style_free (style);

  style = vtile_mapcss_get_style (stylesheet,
                                  VTILE_MAPCSS_SELECTOR_TYPE_WAY,
                                  NULL, 1);
  g_assert (style != NULL);
  num = vtile_mapcss_style_get_num (style, "width");
  g_assert_cmpfloat (num, ==, 3.0);
  vtile_mapcss_style_free (style);

  style = vtile_mapcss_get_style (stylesheet,
                                  VTILE_MAPCSS_SELECTOR_TYPE_WAY,
                                  tags, 1);
  g_assert (style != NULL);
  num = vtile_mapcss_style_get_num (style, "width");
  g_assert_cmpfloat (num, ==, 4.0);
  vtile_mapcss_style_free (style);

  style = vtile_mapcss_get_style (stylesheet,
                                  VTILE_MAPCSS_SELECTOR_TYPE_NODE,
                                  NULL, 1);
  g_assert (style != NULL);
  num = vtile_mapcss_style_get_num (style, "width");
  g_assert_cmpfloat (num, ==, 5.0);
  vtile_mapcss_style_free (style);

  g_object_unref (stylesheet);
}

static void
test_selector_list (void)
{
  char *filename = "selector_list.mapcss";
  gdouble num;
  VTileMapCSSStyle *style;
  GHashTable *tags;

  tags = g_hash_table_new (g_str_hash, g_str_equal);
  g_hash_table_insert (tags, "area", "yes");

  g_assert (mapcss_new_and_load (filename));

  style = vtile_mapcss_get_style (stylesheet,
                                  VTILE_MAPCSS_SELECTOR_TYPE_WAY,
                                  NULL, 1);
  g_assert (style != NULL);
  num = vtile_mapcss_style_get_num (style, "width");
  g_assert_cmpfloat (num, ==, 2.0);
  vtile_mapcss_style_free (style);

  style = vtile_mapcss_get_style (stylesheet,
                                  VTILE_MAPCSS_SELECTOR_TYPE_WAY,
                                  tags, 1);
  g_assert (style != NULL);
  num = vtile_mapcss_style_get_num (style, "width");
  g_assert_cmpfloat (num, ==, 2.0);
  vtile_mapcss_style_free (style);

  g_object_unref (stylesheet);
}

static void
test_selector_test (void)
{
  char *filename = "selector_test.mapcss";
  GHashTable *tags;
  gdouble num;
  VTileMapCSSStyle *style;

  tags = g_hash_table_new (g_str_hash, g_str_equal);

  g_hash_table_insert (tags, "highway", "primary");
  g_hash_table_insert (tags, "building", "restaurant");

  g_assert (mapcss_new_and_load (filename));

  style = vtile_mapcss_get_style (stylesheet,
                                  VTILE_MAPCSS_SELECTOR_TYPE_WAY,
                                  tags, 1);
  g_assert (style != NULL);
  num = vtile_mapcss_style_get_num (style, "width");
  g_assert_cmpfloat (num, ==, 4.0);
  vtile_mapcss_style_free (style);

  g_hash_table_insert (tags, "area", "yes");
  style = vtile_mapcss_get_style (stylesheet,
                                  VTILE_MAPCSS_SELECTOR_TYPE_WAY,
                                  tags, 1);
  g_assert (style != NULL);
  num = vtile_mapcss_style_get_num (style, "width");
  g_assert_cmpfloat (num, ==, 7.0);
  vtile_mapcss_style_free (style);

  g_hash_table_remove_all (tags);
  g_hash_table_insert (tags, "highway", "footway");

  style = vtile_mapcss_get_style (stylesheet,
                                  VTILE_MAPCSS_SELECTOR_TYPE_WAY,
                                  tags, 1);
  g_assert (style != NULL);
  num = vtile_mapcss_style_get_num (style, "width");
  g_assert_cmpfloat (num, ==, 5.0);
  vtile_mapcss_style_free (style);

  g_hash_table_insert (tags, "area", "yes");
  style = vtile_mapcss_get_style (stylesheet,
                                  VTILE_MAPCSS_SELECTOR_TYPE_WAY,
                                  tags, 1);
  g_assert (style != NULL);
  num = vtile_mapcss_style_get_num (style, "width");
  g_assert_cmpfloat (num, ==, 9.0);
  vtile_mapcss_style_free (style);

  style = vtile_mapcss_get_style (stylesheet,
                                  VTILE_MAPCSS_SELECTOR_TYPE_WAY,
                                  NULL, 1);
  g_assert (style != NULL);
  num = vtile_mapcss_style_get_num (style, "width");
  g_assert_cmpfloat (num, ==, 1.0);
  vtile_mapcss_style_free (style);

  g_hash_table_destroy (tags);
  g_object_unref (stylesheet);
}


static void
test_selector_zoom (void)
{
  char *filename = "selector_zoom.mapcss";
  gdouble num;
  VTileMapCSSStyle *style;
  GHashTable *tags;
  gint i;

  tags = g_hash_table_new (g_str_hash, g_str_equal);

  g_hash_table_insert (tags, "highway", "primary");

  g_assert (mapcss_new_and_load (filename));

  for (i = 0; i < 20; i++) {
    VTileMapCSSStyle *style;

    g_hash_table_insert (tags, "area", "no");
    style = vtile_mapcss_get_style (stylesheet,
                                    VTILE_MAPCSS_SELECTOR_TYPE_WAY,
                                    tags, i);
    g_assert (style != NULL);
    num = vtile_mapcss_style_get_num (style, "width");
    if (i < 10)
      g_assert_cmpfloat (num, ==, 4.0);
    else if (i == 10)
      g_assert_cmpfloat (num, ==, 2.0);
    else
      g_assert_cmpfloat (num, ==, 1.0);
    vtile_mapcss_style_free (style);

    g_hash_table_insert (tags, "area", "yes");
    style = vtile_mapcss_get_style (stylesheet,
                                    VTILE_MAPCSS_SELECTOR_TYPE_WAY,
                                    tags, i);
    g_assert (style != NULL);
    num = vtile_mapcss_style_get_num (style, "width");
    if (i == 5)
      g_assert_cmpfloat (num, ==, 3.0);
    else
      g_assert_cmpfloat (num, !=, 3.0);
  }

  g_hash_table_remove_all (tags);
  g_object_unref (stylesheet);
}

static void
test_all (void)
{
  char *filename = "all.mapcss";

  g_assert (mapcss_new_and_load (filename));
  g_assert (stylesheet != NULL);
  g_object_unref (stylesheet);
}

static void
assert_error_where (const char *filename,
                    guint ex_lineno, guint ex_column)
{
  guint lineno, column;

  g_assert (mapcss_new_and_load (filename) != TRUE);
  g_object_get (stylesheet,
                "lineno", &lineno,
                "column", &column,
                NULL);
  g_assert_cmpint (lineno, ==, ex_lineno);
  g_assert_cmpint (column, ==, ex_column);
  g_object_unref (stylesheet);
}

static void
test_errors_where (void)
{
  guint lineno, column;
  char *filename = "all.mapcss";

  assert_error_where ("error-selector-1.mapcss", 1, 0);
  assert_error_where ("error-selector-2.mapcss", 3, 0);
  assert_error_where ("error-selector-3.mapcss", 3, 3);
  assert_error_where ("error-declaration-1.mapcss", 3, 10);
  assert_error_where ("error-declaration-2.mapcss", 4, 0);
  assert_error_where ("error-declaration-3.mapcss", 7, 4);
}

int
main (int argc, char *argv[])
{
  setlocale (LC_ALL, "");

  g_test_init (&argc, &argv, NULL);
  g_test_bug_base ("http://bugzilla.gnome.org/show_bug.cgi?id=");

  g_test_add_func ("/parse/selector", test_selector);
  g_test_add_func ("/parse/selector_list", test_selector_list);
  g_test_add_func ("/parse/selector_test", test_selector_test);
  g_test_add_func ("/parse/selector_zoom", test_selector_zoom);
  g_test_add_func ("/parse/all", test_all);
  g_test_add_func ("/parse/errors", test_errors_where);

  return g_test_run ();
}
