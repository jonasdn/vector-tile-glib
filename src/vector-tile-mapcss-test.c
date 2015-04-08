#include <string.h>

#include "vector-tile-mapcss-private.h"

VTileMapCSSTest *
vtile_mapcss_test_new ()
{
  VTileMapCSSTest *test = g_new0 (VTileMapCSSTest, 1);

  test->tag = NULL;
  test->value = NULL;

  return test;
}

void
vtile_mapcss_test_free (VTileMapCSSTest *test)
{
  if (test->tag)
    g_free (test->tag);

  if (test->value)
    g_free (test->value);

  g_free (test);
}
