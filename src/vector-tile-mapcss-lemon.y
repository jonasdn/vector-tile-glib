%include {
  #include <glib.h>
  #include <assert.h>

  #include "vector-tile-mapcss.h"
  #include "vector-tile-mapcss-private.h"
  #include "vector-tile-mapcss-flex.h"
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

    vtile_mapcss_selector_add_declarations (selector, B.list);
    vtile_mapcss_add_selector (parser, selector);
  }

  g_list_free_full (B.list, g_object_unref);
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
  A.selector = vtile_mapcss_selector_new (B.str, D.list, C.range);
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
  A.list = g_list_append (A.list, C.test);
}

tag_tests(A) ::= tag_test(B) . {
  A.list = NULL;
  A.list = g_list_append (A.list, B.test);
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

selector_type(A) ::= NODE_SELECTOR(B) . {
  A.str = B.str;
}

selector_type(A) ::= WAY_SELECTOR(B) . {
  A.str = B.str;
}

selector_type(A) ::= AREA_SELECTOR(B) . {
  A.str = B.str;
}

selector_type(A) ::= CANVAS_SELECTOR(B) . {
  A.str = B.str;
}

declaration_list(A) ::= declaration_list(B) declaration(C) . {
  A.list = B.list;
  A.list = g_list_append (A.list, C.declaration);
}

declaration_list(A) ::= declaration(B) . {
  A.list = NULL;
  A.list = g_list_append (A.list, B.declaration);
}

declaration_list ::= .

declaration(A) ::= property(B) COLON value(C) SEMICOLON . {
  A.declaration = vtile_mapcss_declaration_new (B.str, C.value);
}

declaration ::= .

property(A) ::= IDENT(B) . {
  A.str = B.str;
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

value(A) ::= line_cap(B) . {
  A.value = B.value;
}

value(A) ::= line_join(B) . {
  A.value = B.value;
}

value(A) ::= font_weight(B) . {
  A.value = B.value;
}

value(A) ::= font_style(B) . {
  A.value = B.value;
}

value(A) ::= font_variant(B) . {
  A.value = B.value;
}

value(A) ::= text_decoration(B) . {
  A.value = B.value;
}

value(A) ::= text_transform(B) . {
  A.value = B.value;
}

value(A) ::= text_position(B) . {
  A.value = B.value;
}

text_position(A) ::= LINE . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_TEXT_POSITION_LINE;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

text_position(A) ::= CENTER . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_TEXT_POSITION_CENTER;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

text_transform(A) ::= NONE . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_TEXT_TRANSFORM_NONE;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

text_transform(A) ::= UPPERCASE . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_TEXT_TRANSFORM_UPPERCASE;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

text_transform(A) ::= LOWERCASE . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_TEXT_TRANSFORM_LOWERCASE;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

text_transform(A) ::= CAPITALIZE . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_TEXT_TRANSFORM_CAPITALIZE;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

text_decoration(A) ::= NONE . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_TEXT_DECORATION_NONE;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

text_decoration(A) ::= UNDERLINE . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_TEXT_DECORATION_UNDERLINE;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

font_variant(A) ::= NORMAL . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_FONT_VARIANT_NORMAL;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

font_variant(A) ::= SMALL_CAPS . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_FONT_VARIANT_SMALL_CAPS;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

font_style(A) ::= ITALIC . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_FONT_STYLE_ITALIC;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

font_style(A) ::= NORMAL . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_FONT_STYLE_NORMAL;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

font_weight(A) ::= BOLD . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_FONT_WEIGHT_BOLD;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

font_weight(A) ::= NORMAL . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_FONT_WEIGHT_BOLD;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

line_cap(A) ::= NONE . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_LINE_CAP_NONE;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

line_cap(A) ::= SQUARE . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_LINE_CAP_SQUARE;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

line_cap(A) ::= ROUND . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_LINE_CAP_ROUND;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

line_join(A) ::= MITER . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_LINE_JOIN_MITER;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

line_join(A) ::= BEVEL . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_LINE_JOIN_BEVEL;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

line_join(A) ::= ROUND . {
  A.value = vtile_mapcss_value_new ();
  A.value->enum_value = VTILE_MAPCSS_LINE_JOIN_ROUND;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_ENUM;
}

dash(A) ::= num_list(B) . {
  A.value = B.value;
}

num_list(A) ::= num_list(B) COMMA NUM(C) . {
  A.value = B.value;
  A.value->dash.dashes[A.value->dash.num_dashes] = C.value->num;
  A.value->dash.num_dashes++;

  vtile_mapcss_value_free (C.value);
}

num_list(A) ::= NUM(B) . {
  A.value = B.value;
  A.value->dash.num_dashes = 1;
  A.value->dash.dashes[0] = B.value->num;
  A.value->type = VTILE_MAPCSS_VALUE_TYPE_DASH;
}
