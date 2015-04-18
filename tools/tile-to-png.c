#include <stdlib.h>
#include <gio/gio.h>
#include <cairo.h>

#include "vector-tile-mapbox.h"
#include "vector-tile-mapcss.h"
#include "vector-tile-mapcss-style.h"

static char *output;
static char **input = NULL;
static guint tile_size;
static guint zoom_level;

static GOptionEntry entries[] =
  {
    { "size", 's', 0, G_OPTION_ARG_INT, &tile_size,
      "The size of the tile, default: 256", NULL },
    { "zoom", 'z', 0, G_OPTION_ARG_INT, &zoom_level,
      "The zoom-level of the tile, default: 0", NULL },
    { "output", 'o', 0, G_OPTION_ARG_FILENAME, &output,
      "Output PNG filename, default: 'image.png'", NULL },
    { G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_FILENAME_ARRAY, &input,
      "The tile to render", NULL },
    { NULL },
  };

int
main (int argc, char **argv)
{
  GFile *file;
  GFileInfo *info;
  GFileInputStream *stream;
  goffset size;
  gssize bytes_read;
  gboolean status;
  guint8 *tile_buffer;
  cairo_surface_t *surface;
  cairo_t *cr;
  VTileMapbox *mapbox;
  VTileMapCSS *stylesheet;
  GError *error = NULL;
  GOptionContext *context;

  context = g_option_context_new ("- test rendering to png image");
  g_option_context_add_main_entries (context, entries, NULL);
  if (!g_option_context_parse (context, &argc, &argv, &error)) {
    g_print ("option parsing failed: %s\n", error->message);
    exit (1);
  }

  if (!input) {
    g_print ("You need to specify a tile to render!\n\n");
    g_print ("%s\n", g_option_context_get_help (context, FALSE, NULL));
    exit (1);
  }

  g_option_context_free (context);

  if (!output)
    output = "image.png";

  if (!tile_size)
    tile_size = 256;

  file = g_file_new_for_path (input[0]);
  info = g_file_query_info (file,
                            G_FILE_ATTRIBUTE_STANDARD_SIZE,
                            G_FILE_QUERY_INFO_NONE,
                            NULL,
                            NULL);
  if (!info) {
    g_object_unref (file);
    g_print ("Failed to query info\n");
    exit (1);
  }

  size = g_file_info_get_size (info);
  tile_buffer = g_malloc (size);

  stream = g_file_read (file, NULL, NULL);
  status = g_input_stream_read_all ((GInputStream *) stream,
                                    tile_buffer,
                                    size,
                                    &bytes_read,
                                    NULL,
                                    NULL);
  if (!status) {
    g_free (tile_buffer);
    g_object_unref (file);
    g_print ("Failed to read file\n");
    exit (1);
  }

  g_object_unref (file);
  g_object_unref (info);
  g_object_unref (stream);

  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
                                        tile_size, tile_size);
  cr = cairo_create (surface);

  stylesheet = vtile_mapcss_new ();
  if (!vtile_mapcss_load (stylesheet, "sample.mss", &error)) {
    g_printerr ("%s\n", error->message);
    g_error_free (error);

    return 0;
  }

  mapbox = vtile_mapbox_new (tile_buffer, size, tile_size, zoom_level);
  vtile_mapbox_set_stylesheet (mapbox, stylesheet);

  if (!vtile_mapbox_render (mapbox, cr, NULL)) {
    g_print ("Failed to render!\n");
  } else {
    cairo_surface_write_to_png (surface, output);
  }
  cairo_destroy (cr);
  cairo_surface_destroy (surface);

  g_free (tile_buffer);
  g_object_unref (stylesheet);
  g_object_unref (mapbox);

  return 0;
}
