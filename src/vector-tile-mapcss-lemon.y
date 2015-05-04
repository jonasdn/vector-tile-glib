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

%include {
  #include <glib.h>
  #include <assert.h>

  #include "vector-tile-mapcss.h"
  #include "vector-tile-mapcss-private.h"
  #include "vector-tile-mapcss-flex.h"

  static VTileMapCSSSelectorType
  get_selector_type (VTileMapCSSValue *value, GList **tests)
  {
    VTileMapCSSSelectorType type;

    if (value->enum_value == VTILE_MAPCSS_SELECTOR_TYPE_AREA) {
      VTileMapCSSTest *test = vtile_mapcss_test_new ();

      type = VTILE_MAPCSS_SELECTOR_TYPE_WAY;
      test->tag = g_strdup ("area");
      test->operator = VTILE_MAPCSS_TEST_TAG_EQUALS;
      test->value = g_strdup ("yes");
      *tests = g_list_prepend (*tests, test);
    } else if (value->enum_value == VTILE_MAPCSS_SELECTOR_TYPE_LINE) {
      VTileMapCSSTest *test = vtile_mapcss_test_new ();

      type = VTILE_MAPCSS_SELECTOR_TYPE_WAY;
      test->tag = g_strdup ("area");
      test->operator = VTILE_MAPCSS_TEST_TAG_EQUALS;
      test->value = g_strdup ("no");
      *tests = g_list_prepend (*tests, test);
    } else {
      type = value->enum_value;
    }

    return type;
  }
 }

%token_type { VTileMapCSSToken }
%default_type { VTileMapCSSToken }

%extra_argument { VTileMapCSS *parser }
%syntax_error {
  gint i;
  gint n;
  GString *string;

  string = g_string_new (NULL);

  n = sizeof(yyTokenName) / sizeof(yyTokenName[0]);
  for (i = 0; i < n; ++i) {
    int a;

    a = yy_find_shift_action(yypParser, (YYCODETYPE)i);
    if (a < YYNSTATE + YYNRULE) {
      if (string->len == 0)
        g_string_append (string, yyTokenName[i]);
      else
        g_string_append_printf (string, ", %s", yyTokenName[i]);
    }
  }

  vtile_mapcss_set_syntax_error (parser, g_string_free (string, FALSE));
}

stylesheet ::= rule_list .
rule_list ::= rule_list rule .
rule_list ::= .

rule ::= selector_list(A) LBRACE declaration_list(B) RBRACE . {
  GList *l = NULL;

  for (l = A.list; l != NULL; l = l->next) {
    VTileMapCSSSelector *selector = (VTileMapCSSSelector *) l->data;

    vtile_mapcss_selector_add_declarations (selector, B.table);
    vtile_mapcss_add_selector (parser, selector);
  }

  if (B.table)
    g_hash_table_unref (B.table);
  g_list_free (A.list);
}

selector_list(A) ::= selector_list(B) COMMA selector(C) . {
  A.list = B.list;
  A.list = g_list_append (A.list, C.selector);
}

selector_list(A) ::= selector(B) . {
  A.list = NULL;
  A.list = g_list_append (A.list, B.selector);
}

selector(A) ::= selector_type(B) zoom_levels(C) tag_tests(D) . {
  VTileMapCSSSelectorType type = get_selector_type (B.value, &D.list);
  A.selector = vtile_mapcss_selector_new (type, D.list, C.range);
  vtile_mapcss_value_free (B.value);
}

zoom_levels(A) ::= PIPE ZL(B) . {
  A.range = g_malloc (sizeof (guint) * 2);
  A.range[0] = B.value->num;
  A.range[1] = B.value->num;

  vtile_mapcss_value_free (B.value);
}

zoom_levels(A) ::= PIPE ZL(B) DASH NUM(C) . {
  A.range = g_malloc (sizeof (guint) * 2);

  A.range[0] = B.value->num;
  A.range[1] = C.value->num;

  vtile_mapcss_value_free (B.value);
  vtile_mapcss_value_free (C.value);
}

zoom_levels(A) ::= . {
  A.range = NULL;
}

tag_tests(A) ::= tag_tests(B) tag_test(C) . {
  A.list = B.list;
  A.list = g_list_prepend (A.list, C.test);
}

tag_tests(A) ::= . {
  A.list = NULL;
}

tag_test(A) ::= LBRACKET test(B) RBRACKET . {
  A.test = B.test;
}

test(A) ::= IDENT(B) . {
  A.test = vtile_mapcss_test_new ();
  A.test->tag = B.str;
  A.test->operator = VTILE_MAPCSS_TEST_TAG_IS_SET;
}

test(A) ::= NOT IDENT(B) . {
  A.test = vtile_mapcss_test_new ();
  A.test->tag = B.str;
  A.test->operator = VTILE_MAPCSS_TEST_TAG_IS_NOT_SET;
}

test(A) ::= IDENT(B) EQUAL IDENT(C) . {
  A.test = vtile_mapcss_test_new ();
  A.test->tag = B.str;
  A.test->value = C.str;
  A.test->operator = VTILE_MAPCSS_TEST_TAG_EQUALS;
}

test(A) ::= IDENT(B) NOT EQUAL IDENT(C) . {
  A.test = vtile_mapcss_test_new ();
  A.test->tag = B.str;
  A.test->value = C.str;
  A.test->operator = VTILE_MAPCSS_TEST_TAG_NOT_EQUALS;
}

selector_type(A) ::= AREA . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_SELECTOR_TYPE_AREA;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

selector_type(A) ::= WAY . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_SELECTOR_TYPE_WAY;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

selector_type(A) ::= CANVAS . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_SELECTOR_TYPE_CANVAS;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

selector_type(A) ::= LINE . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_SELECTOR_TYPE_LINE;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

selector_type(A) ::= NODE . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_SELECTOR_TYPE_NODE;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

declaration_list(A) ::= declaration_list(B) declaration(C) . {
  A.table = B.table;
  g_hash_table_insert (A.table, C.str, C.value);
}

declaration_list(A) ::= declaration(B) . {
  A.table = g_hash_table_new_full (g_str_hash,
                                   g_str_equal,
                                   g_free,
                                   (GDestroyNotify) vtile_mapcss_value_free);
  g_hash_table_insert (A.table, B.str, B.value);
}

declaration(A) ::= property(B) COLON value(C) SEMICOLON . {
  A.str = B.str;
  A.value = C.value;
}

property(A) ::= IDENT(B) . {
  A.str = B.str;
}

value(A) ::= IDENT(B) . {
  A.value = vtile_mapcss_value_new ();
  A.value->str = B.str;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_STRING;
}

value(A) ::= NUM(B) . {
  A.value = B.value;
}

value(A) ::= HEXCOLOR(B) . {
  A.value = B.value;
}

value(A) ::= dash(B) . {
  A.value = B.value;
}

value(A) ::= enum_value(B) . {
  A.value = B.value;
}

enum_value(A) ::= LINE . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_VALUE_LINE;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

enum_value(A) ::= CENTER . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_VALUE_CENTER;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

enum_value(A) ::= NONE . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_VALUE_NONE;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

enum_value(A) ::= UPPERCASE . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_VALUE_UPPERCASE;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

enum_value(A) ::= LOWERCASE . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_VALUE_LOWERCASE;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

enum_value(A) ::= CAPITALIZE . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_VALUE_CAPITALIZE;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

enum_value(A) ::= UNDERLINE . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_VALUE_UNDERLINE;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

enum_value(A) ::= NORMAL . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_VALUE_NORMAL;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

enum_value(A) ::= SMALL_CAPS . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_VALUE_SMALL_CAPS;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

enum_value(A) ::= ITALIC . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_VALUE_ITALIC;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

enum_value(A) ::= BOLD . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_VALUE_BOLD;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

enum_value(A) ::= SQUARE . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_VALUE_SQUARE;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

enum_value(A) ::= MITER . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_VALUE_MITER;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

enum_value(A) ::= BEVEL . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_VALUE_BEVEL;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

enum_value(A) ::= ROUND . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_VALUE_ROUND;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

dash(A) ::= NUM_LIST(B) . {
  A.value = B.value;
}
