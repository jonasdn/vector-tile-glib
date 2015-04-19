#include <stdlib.h>
#include <math.h>
#include <gio/gio.h>
#include <geocode-glib/geocode-glib.h>

#define BASE_URI "http://vector.mapzen.com/osm/all/"

static char *search;
static gdouble longitude = -G_MAXDOUBLE;
static gdouble latitude = -G_MAXDOUBLE;
static gint zoom = -1;

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

int tile_get_x ()
{
  return (int) (floor ((longitude + 180.0) / 360.0 * pow (2.0, zoom)));
}

int tile_get_y ()
{
  return (int) (floor ((1.0 - log (tan (latitude * M_PI/180.0) + 1.0 / cos (latitude * M_PI/180.0)) / M_PI) / 2.0 * pow (2.0, zoom)));
}

int
main (int argc, char **argv)
{
  GOptionContext *context;
  gint x, y;
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
  g_free (uri);

  return 0;
}
