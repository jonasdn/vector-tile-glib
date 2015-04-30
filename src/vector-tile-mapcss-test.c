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
