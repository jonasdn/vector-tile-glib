#include <glib.h>
#include <stdlib.h>

#include "vector-tile-mapcss.h"

int main (int argc, char **argv)
{
  VTileMapCSS *style;
  GError *error = NULL;

  if (argc < 2)
    exit (1);

  style = vtile_mapcss_new ();
  if (!vtile_mapcss_load (style, argv[1], &error)) {
    g_printerr ("%s\n", error->message);
    g_error_free (error);
  }

  return 0;
}
