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
#include "vector-tile-mapcss-selector.h"

struct _VTileMapCSSSelectorPrivate {
  char *name;
  GList *tests;
  GList *declarations;
  gint *zoom_levels;
};

G_DEFINE_TYPE_WITH_PRIVATE (VTileMapCSSSelector, vtile_mapcss_selector, G_TYPE_OBJECT)

static void
vtile_mapcss_selector_finalize (GObject *vselector)
{
  VTileMapCSSSelector *selector = (VTileMapCSSSelector *) vselector;

  if (selector->priv->tests)
    g_list_free_full (selector->priv->tests,
                      (GDestroyNotify) vtile_mapcss_test_free);

  if (selector->priv->declarations)
    g_list_free_full (selector->priv->declarations, g_object_unref);

  if (selector->priv->zoom_levels)
    g_free (selector->priv->zoom_levels);

  g_free (selector->priv->name);

  G_OBJECT_CLASS (vtile_mapcss_selector_parent_class)->finalize (vselector);
}

static void
vtile_mapcss_selector_class_init (VTileMapCSSSelectorClass *klass)
{
  GObjectClass *selector_class = G_OBJECT_CLASS (klass);

  selector_class->finalize = vtile_mapcss_selector_finalize;
}

static void
vtile_mapcss_selector_init (VTileMapCSSSelector *selector)
{
  selector->priv = vtile_mapcss_selector_get_instance_private (selector);
  selector->priv->declarations = NULL;
  selector->priv->zoom_levels = NULL;
}


VTileMapCSSSelector *
vtile_mapcss_selector_new (char *name, GList *tests, gint *zoom_levels)
{
  VTileMapCSSSelector *selector;
  VTileMapCSSSelectorPrivate *priv;

  selector = g_object_new (VTILE_TYPE_MAPCSS_SELECTOR, NULL);

  selector->priv->tests = tests;
  selector->priv->name = name;
  selector->priv->zoom_levels = zoom_levels;

  return selector;
}

void
vtile_mapcss_selector_add_declarations (VTileMapCSSSelector *selector,
                                        GList *declarations)
{
  selector->priv->declarations = declarations;
}

GList *
vtile_mapcss_selector_get_declarations (VTileMapCSSSelector *selector)
{
  return selector->priv->declarations;
}

GList *
vtile_mapcss_selector_get_tests (VTileMapCSSSelector *selector)
{
  return selector->priv->tests;
}

char *
vtile_mapcss_selector_get_name (VTileMapCSSSelector *selector)
{
  return selector->priv->name;
}

guint *
vtile_mapcss_selector_get_zoom_levels (VTileMapCSSSelector *selector)
{
  return selector->priv->zoom_levels;
}
