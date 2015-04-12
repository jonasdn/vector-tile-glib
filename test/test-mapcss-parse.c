#include <glib.h>
#include <locale.h>

#include "vector-tile-mapcss.h"

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
test_basic (void)
{
  char *filename = "basic.mapcss";

  g_assert (mapcss_new_and_load (filename));

  g_assert (vtile_mapcss_get_style (stylesheet, "canvas", NULL) != NULL);
  g_assert (vtile_mapcss_get_style (stylesheet, "way", NULL) != NULL);
  g_assert (vtile_mapcss_get_style (stylesheet, "area", NULL) != NULL);
  g_assert (vtile_mapcss_get_style (stylesheet, "node", NULL) != NULL);

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
test_errors (void)
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

  g_test_add_func ("/parse/basic", test_basic);
  g_test_add_func ("/parse/all", test_all);
  g_test_add_func ("/parse/errors", test_errors);

  return g_test_run ();
}
