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
  VTileMapCSSSelectorType type;
  GList *tests;
  GHashTable *declarations;
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
    g_hash_table_unref (selector->priv->declarations);

  if (selector->priv->zoom_levels)
    g_free (selector->priv->zoom_levels);

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
vtile_mapcss_selector_new (VTileMapCSSSelectorType type,
                           GList *tests,
                           gint *zoom_levels)
{
  VTileMapCSSSelector *selector;

  selector = g_object_new (VTILE_TYPE_MAPCSS_SELECTOR, NULL);

  selector->priv->tests = tests;
  selector->priv->type = type;
  selector->priv->zoom_levels = zoom_levels;

  return selector;
}


gboolean
test_in_list (VTileMapCSSTest *test, GList *tests)
{
  GList *l = NULL;

  for (l = tests; l != NULL; l = l->next) {
    VTileMapCSSTest *t = (VTileMapCSSTest *) l->data;

    if (test->operator != t->operator)
      continue;

    if (g_strcmp0 (test->tag, t->tag))
      continue;

    if (g_strcmp0 (test->value, t->value))
      continue;

    return TRUE;
  }

  return FALSE;
}

gboolean
vtile_mapcss_selector_equals (VTileMapCSSSelector *a,
                              VTileMapCSSSelector *b)
{
  GList *l = NULL;

  if (a->priv->type != b->priv->type)
    return FALSE;

  if (a->priv->zoom_levels && b->priv->zoom_levels) {
    if (a->priv->zoom_levels[0] != b->priv->zoom_levels[0] ||
        a->priv->zoom_levels[1] != b->priv->zoom_levels[1])
      return FALSE;
  } else {
    if (a->priv->zoom_levels || b->priv->zoom_levels)
      return FALSE;
  }

  if (a->priv->tests && b->priv->tests) {
    if (g_list_length (a->priv->tests) != g_list_length (b->priv->tests))
      return FALSE;

    for (l = a->priv->tests; l != NULL; l = l->next) {
      VTileMapCSSTest *test = (VTileMapCSSTest *) l->data;

      if (!test_in_list (test, b->priv->tests))
        return FALSE;
    }
  } else {
    if (a->priv->tests || b->priv->tests)
      return FALSE;
  }

  return TRUE;
}

void
vtile_mapcss_selector_merge (VTileMapCSSSelector *a,
                             VTileMapCSSSelector *b)
{
  GHashTableIter iter;
  gpointer key, value;

  g_hash_table_iter_init (&iter, b->priv->declarations);
  while (g_hash_table_iter_next (&iter, &key, &value))
    g_hash_table_insert (a->priv->declarations, key, value);
}

void
vtile_mapcss_selector_add_declarations (VTileMapCSSSelector *selector,
                                        GHashTable *declarations)
{
  GList *l = NULL;

  if (declarations)
    selector->priv->declarations = g_hash_table_ref (declarations);
}

GHashTable *
vtile_mapcss_selector_get_declarations (VTileMapCSSSelector *selector)
{
  return selector->priv->declarations;
}

GList *
vtile_mapcss_selector_get_tests (VTileMapCSSSelector *selector)
{
  return selector->priv->tests;
}

VTileMapCSSSelectorType
vtile_mapcss_selector_get_selector_type (VTileMapCSSSelector *selector)
{
  return selector->priv->type;
}

guint *
vtile_mapcss_selector_get_zoom_levels (VTileMapCSSSelector *selector)
{
  return selector->priv->zoom_levels;
}
