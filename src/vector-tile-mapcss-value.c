#include <string.h>

#include "vector-tile-mapcss-private.h"

VTileMapCSSValue *
vtile_mapcss_value_new ()
{
  VTileMapCSSValue *value = g_new0 (VTileMapCSSValue, 1);

  value->str = NULL;

  return value;
}

VTileMapCSSValue *
vtile_mapcss_value_copy (VTileMapCSSValue *src)
{
  VTileMapCSSValue *dest = g_new0 (VTileMapCSSValue, 1);

  memcpy (dest, src, sizeof (VTileMapCSSValue));
  if (src->str)
    dest->str = src->str;


  return dest;
}

void
vtile_mapcss_value_free (VTileMapCSSValue *value)
{
  if (value->type == VTILE_MAPCSS_VALUE_TYPE_STRING)
    g_free (value->str);

  g_free (value);
}
