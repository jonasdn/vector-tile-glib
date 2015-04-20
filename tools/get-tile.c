#include <stdlib.h>
#include <math.h>
#include <gio/gio.h>
#include <geocode-glib/geocode-glib.h>
#include <libsoup/soup.h>

#define BASE_URI "http://vector.mapzen.com/osm/all/"

static char *search;
static gdouble longitude = -G_MAXDOUBLE;
static gdouble latitude = -G_MAXDOUBLE;
static gint zoom = -1;
static guint x;
static guint y;

static GOptionEntry entries[] =
  {
    { "latitude", 0, 0, G_OPTION_ARG_DOUBLE, &latitude,
      "Latitude to get tile y for", NULL },
    { "longitude", 0, 0, G_OPTION_ARG_DOUBLE, &longitude,
      "Longitude to get tile y for", NULL },
    { "zoom", 'z', 0, G_OPTION_ARG_INT, &zoom,
      "zoom level", NULL },
    { "search", 's', 0, G_OPTION_ARG_STRING, &search,
        "Location to get tile for", NULL },
    { NULL },
  };

static guint
tile_get_x ()
{
  return (int) (floor ((longitude + 180.0) / 360.0 * pow (2.0, zoom)));
}

static guint
tile_get_y ()
{
  return (int) (floor ((1.0 - log (tan (latitude * M_PI/180.0) + 1.0 / cos (latitude * M_PI/180.0)) / M_PI) / 2.0 * pow (2.0, zoom)));
}


static void
tile_download (char *uri)
{
  SoupSession *session;
  SoupMessage *msg;
  GOutputStream *stream;
  gboolean success;
  GError *error = NULL;
  char *filename;
  GFile *file;
  guint status;
  gsize written;

  session = soup_session_new ();
  msg = soup_message_new ("GET", uri);

  g_print ("Downloading...\n");
  status = soup_session_send_message (session, msg);
  if (status != SOUP_STATUS_OK) {
    g_print ("Failed to download: %s\n",
             msg->reason_phrase ? msg->reason_phrase : "error");
    exit (1);
  }

  filename = g_strdup_printf ("z%d-x%d-y%d.mapbox",
                              zoom, x, y);
  file = g_file_new_for_path (filename);

  stream = (GOutputStream *) g_file_replace (file, NULL, FALSE, 0,
                                             NULL, NULL);
  success = g_output_stream_write_all (stream, msg->response_body->data,
                                       msg->response_body->length, &written,
                                       NULL, &error);
  if (!success) {
    g_print ("Writing file failed: %s\n", error->message);
    exit (1);
  }

  g_print ("Downloaded file %s\n", filename);
  g_free (filename);
}

int
main (int argc, char **argv)
{
  GOptionContext *context;
  GFile *file;
  char *uri;
  GError *error = NULL;

  context = g_option_context_new ("- download mapbox tile");
  g_option_context_add_main_entries (context, entries, NULL);
  if (!g_option_context_parse (context, &argc, &argv, &error)) {
    g_print ("option parsing failed: %s\n", error->message);
    exit (1);
  }

  if (zoom < 0) {
      g_print ("No zoom level specified!\n");
      g_print ("%s\n", g_option_context_get_help (context, FALSE, NULL));
      exit (1);
  }

  if ((latitude == -G_MAXDOUBLE  || longitude == -G_MAXDOUBLE) && !search)
    {
      g_print ("No coordinates specified!\n");
      g_print ("%s\n", g_option_context_get_help (context, FALSE, NULL));
      exit (1);
   }

  if (search) {
      GeocodeForward *forward;
      GeocodeLocation *location;
      GList *res;

      forward = geocode_forward_new_for_string (search);
      geocode_forward_set_answer_count (forward, 1);
      g_print ("Searching...\n");
      res = geocode_forward_search (forward, &error);
      if (res == NULL) {
        g_print ("Geocoding failed: %s\n", error->message);
        exit (1);
      }
      g_object_unref (forward);

      location = geocode_place_get_location (res->data);
      latitude = geocode_location_get_latitude (location);
      longitude = geocode_location_get_longitude (location);
  }

  if (latitude < -90 || latitude > 90 || longitude < -180 || longitude > 180) {
    g_print ("Invalid latitude/longitude\n");
    exit (1);
  }

  x = tile_get_x ();
  y = tile_get_y ();
  uri = g_strdup_printf ("%s%d/%d/%d.mapbox",
                         BASE_URI, zoom, x, y);

  g_print ("Found tile: %s\n", uri);
  tile_download (uri);

  g_free (uri);
  return 0;
}
