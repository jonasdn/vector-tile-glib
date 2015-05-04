#include <glib.h>
#include <locale.h>

#include "vector-tile-mapcss.h"
#include "vector-tile-mapcss-style.h"
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
test_values (void)
{
  VTileMapCSSStyle *style;
  gdouble num;
  gint line_enum;
  VTileMapCSSColor *color;
  VTileMapCSSDash *dash;
  char *filename = "basic.mapcss";

  g_assert (mapcss_new_and_load (filename));
  g_assert (stylesheet != NULL);

  style = vtile_mapcss_get_style (stylesheet,
                                  VTILE_MAPCSS_SELECTOR_TYPE_WAY,
                                  NULL, 1);
  g_assert (style != NULL);

  num = vtile_mapcss_style_get_num (style, "casing-width");
  g_assert_cmpfloat (num, ==, 0.95);

  num = vtile_mapcss_style_get_num (style, "width");
  g_assert_cmpfloat (num, ==, 1.0);

  color = vtile_mapcss_style_get_color (style, "color");
  g_assert_cmpfloat (color->r, ==, 1.0);
  g_assert_cmpfloat (color->g, ==, 0.0);
  g_assert_cmpfloat (color->b, ==, 0.0);

  color = vtile_mapcss_style_get_color (style, "casing-color");
  g_assert_cmpfloat (color->r, ==, 0.0);
  g_assert_cmpfloat (color->g, ==, 0.0);
  g_assert_cmpfloat (color->b, ==, 1.0);

  color = vtile_mapcss_style_get_color (style, "fill-color");
  g_assert_cmpfloat (color->r, ==, 0.0);
  g_assert_cmpfloat (color->g, ==, 1.0);
  g_assert_cmpfloat (color->b, ==, 0.0);

  line_enum = vtile_mapcss_style_get_enum (style, "linecap");
  g_assert_cmpint (line_enum, ==, VTILE_MAPCSS_LINE_CAP_ROUND);

  dash = vtile_mapcss_style_get_dash (style, "dashes");
  g_assert_cmpint (dash->num_dashes, ==, 2);
  g_assert_cmpfloat (dash->dashes[0], ==, 2);
  g_assert_cmpfloat (dash->dashes[1], ==, 4);

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

  return g_test_run ();
}
