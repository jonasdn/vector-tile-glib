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

#ifndef __VECTOR_TILE_MAPCSS_PRIVATE_H__
#define __VECTOR_TILE_MAPCSS_PRIVATE_H__

#include <glib.h>

#include "vector-tile-mapcss-value.h"
#include "vector-tile-mapcss-selector.h"

G_BEGIN_DECLS

typedef enum {
  VTILE_MAPCSS_TEST_TAG_IS_SET,
  VTILE_MAPCSS_TEST_TAG_IS_NOT_SET,
  VTILE_MAPCSS_TEST_TAG_EQUALS,
  VTILE_MAPCSS_TEST_TAG_NOT_EQUALS
} VTileMapCSSTestOperator;

typedef struct {
  VTileMapCSSTestOperator operator;
  char *tag;
  char *value;
} VTileMapCSSTest;

struct _VTileMapCSSStyle {
  GHashTable *properties;
};

#ifndef YYSTYPE
typedef struct {
  char *str;
  guint *range;
  GList *list;
  GHashTable *table;
  VTileMapCSSSelector *selector;
  VTileMapCSSTest *test;
  VTileMapCSSValue *value;
} VTileMapCSSToken;
#define YYSTYPE VTileMapCSSToken
#define YYSTYPE_IS_TRIVIAL 1
#endif

YYSTYPE yylval;

VTileMapCSSTest *vtile_mapcss_test_new ();
void vtile_mapcss_test_free (VTileMapCSSTest *test);

VTileMapCSSValue *vtile_mapcss_value_new ();
void vtile_mapcss_value_free (VTileMapCSSValue *value);
G_END_DECLS

#endif /* VECTOR_TILE_MAPCSS_PRIVATE */
