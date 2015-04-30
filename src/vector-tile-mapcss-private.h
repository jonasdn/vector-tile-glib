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

#include "vector-tile-mapcss.h"
#include "vector-tile-mapcss-selector.h"
#include "vector-tile-mapcss-style.h"

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

VTileMapCSSStyle *vtile_mapcss_get_style (VTileMapCSS *mapcss,
                                          VTileMapCSSSelectorType type,
                                          GHashTable *tags,
                                          guint zoom);

gboolean
vile_mapcss_add_selector (VTileMapCSS *mapcss,
                          VTileMapCSSSelector *selector);

void
vtile_mapcss_set_syntax_error (VTileMapCSS *mapcss,
                               char *valid_tokens);
void
vtile_mapcss_set_type_error (VTileMapCSS *mapcss);


VTileMapCSSTest *vtile_mapcss_test_new ();
void vtile_mapcss_test_free (VTileMapCSSTest *test);

VTileMapCSSValue *vtile_mapcss_value_new ();
void vtile_mapcss_value_free (VTileMapCSSValue *value);

gint vtile_mapcss_get_num_styles (VTileMapCSS *mapcss);
G_END_DECLS

#endif /* VECTOR_TILE_MAPCSS_PRIVATE */
