#include <stdlib.h>
#include <gio/gio.h>
#include <cairo.h>

#include "vector-tile-mapbox.h"

static char *output;
static char **input = NULL;
static gint tile_size;

static GOptionEntry entries[] =
  {
    { "size", 's', 0, G_OPTION_ARG_INT, &tile_size,
      "The size of the tile, default: 256", NULL },
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
  VectorTileMapbox *mapbox;
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
    g_print ("Failed to read file\n");
    exit (1);
  }

  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
                                        tile_size, tile_size);
  cr = cairo_create (surface);

  mapbox = vector_tile_mapbox_new (tile_buffer, size, tile_size);
  vector_tile_mapbox_render_to_cairo (mapbox, cr, NULL);

  cairo_surface_write_to_png (surface, output);
  cairo_destroy (cr);
  cairo_surface_destroy (surface);

  return 0;
}
