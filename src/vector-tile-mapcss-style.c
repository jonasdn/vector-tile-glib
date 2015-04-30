/*
 * Copyright 2015 Jonas Danielsson <jonas@threetimestwo.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with vector-tile-glib; if not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>

#include "vector-tile-mapcss-private.h"
#include "vector-tile-mapcss-style.h"

typedef struct {
  VTileMapCSSValue value;
  char *name;
} VTileMapCSSProperty;

static VTileMapCSSProperty vtile_mapcss_style_default_properties[] = {
  { .name = "width",
    .value = {
      .type = VTILE_MAPCSS_VALUE_TYPE_NUMBER,
      .num = 1.0
    }
  },
  { .name = "z-index",
    .value = {
      .type = VTILE_MAPCSS_VALUE_TYPE_NUMBER,
      .num = 0.0
    }
  },
  { .name = "opacity",
    .value = {
      .type = VTILE_MAPCSS_VALUE_TYPE_NUMBER,
      .num = 1.0
    }
  },
  { .name = "fill-opacity",
    .value = {
      .type = VTILE_MAPCSS_VALUE_TYPE_NUMBER,
      .num = 1.0
    }
  },
  { .name = "casing-opacity",
    .value = {
      .type = VTILE_MAPCSS_VALUE_TYPE_NUMBER,
      .num = 1.0
    }
  },
  { .name = "casing-width",
    .value = {
      .type = VTILE_MAPCSS_VALUE_TYPE_NUMBER,
      .num = 0.0
    }
  },
  { .name = "fill-color",
    .value = {
      .type = VTILE_MAPCSS_VALUE_TYPE_COLOR,
      .color = {
        .r = 0.5, .b = 0.5, .g = 0.5
      }
    }
  },
  { .name = "color",
    .value = {
      .type = VTILE_MAPCSS_VALUE_TYPE_COLOR,
      .color = {
        .r = 0.0, .b = 0.0, .g = 0.0
      }
    }
  },
  { .name = "casing-color",
    .value = {
      .type = VTILE_MAPCSS_VALUE_TYPE_COLOR,
      .color = {
        .r = 0.0, .b = 0.0, .g = 0.0
      }
    }
  },
  { .name = "dashes",
    .value = {
      .type = VTILE_MAPCSS_VALUE_TYPE_DASH,
      .dash = {
        .num_dashes = 0, .dashes = { 0 }
      }
    }
  },
  { .name = "linecap",
    .value = {
      .type = VTILE_MAPCSS_VALUE_TYPE_ENUM,
      .enum_value = VTILE_MAPCSS_LINE_CAP_NONE
    }
  },
  { .name = "linejoin",
    .value = {
      .type = VTILE_MAPCSS_VALUE_TYPE_ENUM,
      .enum_value = VTILE_MAPCSS_LINE_JOIN_ROUND
    }
  },
  { .name = "font-family",
    .value = {
      .type = VTILE_MAPCSS_VALUE_TYPE_STRING,
      .str = "DejaVu"
    }
  },
  { .name = "font-size",
    .value = {
      .type = VTILE_MAPCSS_VALUE_TYPE_NUMBER,
      .num = 12,
    }
  },
  { .name = "font-weight",
    .value = {
      .type = VTILE_MAPCSS_VALUE_TYPE_ENUM,
      .enum_value = VTILE_MAPCSS_FONT_WEIGHT_NORMAL
    }
  },
  { .name = "font-style",
    .value = {
      .type = VTILE_MAPCSS_VALUE_TYPE_ENUM,
      .enum_value = VTILE_MAPCSS_FONT_STYLE_NORMAL
    }
  },
  { .name = "font-variant",
    .value = {
      .type = VTILE_MAPCSS_VALUE_TYPE_ENUM,
      .enum_value = VTILE_MAPCSS_FONT_VARIANT_NORMAL
    },
    .name = "test-decoration",
    .value = {
      .type = VTILE_MAPCSS_VALUE_TYPE_ENUM,
      .enum_value = VTILE_MAPCSS_TEXT_DECORATION_NONE
    }
  },
  { .name = "test-transform",
    .value = {
      .type = VTILE_MAPCSS_VALUE_TYPE_ENUM,
      .enum_value = VTILE_MAPCSS_TEXT_TRANSFORM_NONE
    },
    .name = "test-decoration",
    .value = {
      .type = VTILE_MAPCSS_VALUE_TYPE_ENUM,
      .enum_value = VTILE_MAPCSS_TEXT_DECORATION_NONE
    }
  },
  { .name = "test-position",
    .value = {
      .type = VTILE_MAPCSS_VALUE_TYPE_ENUM,
      .enum_value = VTILE_MAPCSS_TEXT_POSITION_CENTER
    }
  },
  { .name = "text-color",
    .value = {
      .type = VTILE_MAPCSS_VALUE_TYPE_COLOR,
      .color = {
        .r = 0.0, .b = 0.0, .g = 0.0
      }
    }
  },
  { .name = "text-halo-color",
    .value = {
      .type = VTILE_MAPCSS_VALUE_TYPE_COLOR,
      .color = {
        .r = 0.0, .b = 0.0, .g = 0.0
      }
    }
  },
  { .name = "text-opacity",
    .value = {
      .type = VTILE_MAPCSS_VALUE_TYPE_NUMBER,
      .num = 1.0
    }
  },
  { .name = "text-offset",
    .value = {
      .type = VTILE_MAPCSS_VALUE_TYPE_NUMBER,
      .num = 0.0
    }
  },
  { .name = "text-halo-radius",
    .value = {
      .type = VTILE_MAPCSS_VALUE_TYPE_NUMBER,
      .num = 0.0
    }
  }
};

static void
vtile_mapcss_style_add_num (VTileMapCSSStyle *style,
                            const char *name,
                            gdouble num)
{
  VTileMapCSSValue *value = vtile_mapcss_value_new ();

  value->type = VTILE_MAPCSS_VALUE_TYPE_NUMBER,
  value->num = num;
  g_hash_table_insert (style->properties, g_strdup (name), value);
}

gdouble
vtile_mapcss_style_get_num (VTileMapCSSStyle *style,
                            const char *name)
{
  VTileMapCSSValue *value;

  value = g_hash_table_lookup (style->properties, name);
  if (value)
    return value->num;

  return -G_MAXDOUBLE;
}

VTileMapCSSColor *
vtile_mapcss_style_get_color (VTileMapCSSStyle *style,
                              const char *name)
{
  VTileMapCSSValue *value;

  value = g_hash_table_lookup (style->properties, name);
  if (value)
    return &value->color;

  return NULL;
}

VTileMapCSSDash *
vtile_mapcss_style_get_dash (VTileMapCSSStyle *style,
                             const char *name)
{
  VTileMapCSSValue *value;

  value = g_hash_table_lookup (style->properties, name);
  if (value)
    return &value->dash;

  return NULL;
}

gint
vtile_mapcss_style_get_enum (VTileMapCSSStyle *style,
                             const char *name)
{
  VTileMapCSSValue *value;

  value = g_hash_table_lookup (style->properties, name);
  if (value)
      return value->enum_value;

  return -1;
}

char *
vtile_mapcss_style_get_str (VTileMapCSSStyle *style,
                            const char *name)
{
  VTileMapCSSValue *value;

  value = g_hash_table_lookup (style->properties, name);
  if (value)
      return value->str;

  return NULL;

}

/**
 * vtile_mapcss_style_new:
 *
 * Returns: A #VTileMapCSSStyle object, free with vtile_mapcss_style_free()
 */
VTileMapCSSStyle *
vtile_mapcss_style_new ()
{
  VTileMapCSSStyle *style = g_new0 (VTileMapCSSStyle, 1);
  gint i;

  style->properties = g_hash_table_new (g_str_hash, g_str_equal);
  for (i = 0; i < G_N_ELEMENTS (vtile_mapcss_style_default_properties); i++) {
    g_hash_table_insert (style->properties,
                         vtile_mapcss_style_default_properties[i].name,
                         &vtile_mapcss_style_default_properties[i].value);
  }

  return style;
}

void
vtile_mapcss_style_free (VTileMapCSSStyle *style)
{
  g_hash_table_destroy (style->properties);
  g_free (style);
}
