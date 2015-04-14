%{
  #include <glib.h>

  #include "vector-tile-mapcss-lemon.h"
  #include "vector-tile-mapcss-private.h"

  gint lineno;
  gint column;

  #define YY_USER_ACTION                                                \
    do {                                                                \
      yyset_column (column, yyscanner);                                 \
      column = yyget_column (yyscanner) + yyget_leng (yyscanner);       \
    } while (0);


  #define YY_USER_INIT                                                  \
    do {                                                                \
      lineno = 1;                                                       \
      column = 0;                                                       \
    } while (0);
%}

%x COMMENT

%option noyywrap
%option reentrant
%option yylineno

ident          [_a-z-]*
num            [0-9]+|[0-9]*"."[0-9]+
hexcolor       #([A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})
zoomlevel      "z"([0-9]|[0-9]{2})
%%

"/*" {
  BEGIN(COMMENT);
}

<COMMENT>[^*\n]* {
  /* eat anything that's not a '*' */
 }
<COMMENT>"*"+[^*/\n]* {
  /* eat up '*'s not followed by '/'s */
 }

<COMMENT>\n {
  lineno++;
 }

<COMMENT>"*"+"/" {
  BEGIN(INITIAL);
}

"node" {
  yylval.str = g_strdup (yytext);

  return NODE_SELECTOR;
}

"way" {
  yylval.str = g_strdup (yytext);

  return WAY_SELECTOR;
}

"canvas" {
  yylval.str = g_strdup (yytext);

  return CANVAS_SELECTOR;
}

"area" {
  yylval.str = g_strdup (yytext);

  return AREA_SELECTOR;
}

"none" {
  return NONE;
}

"round" {
  return ROUND;
}

"square" {
  return SQUARE;
}

"bevel" {
  return BEVEL;
}

"miter" {
  return MITER;
}

{num} {
  yylval.value = vtile_mapcss_value_new ();
  yylval.value->num = g_ascii_strtod (yytext, NULL);
  yylval.value->type = VTILE_MAPCSS_VALUE_TYPE_NUMBER;

  return NUM;
}

{hexcolor} {
  long val;

  val = strtol (yytext + 1, NULL, 16);

  yylval.value = vtile_mapcss_value_new ();
  yylval.value->color.r = ((val >> 16) & 0xFF) / 255.0;
  yylval.value->color.g = ((val >> 8)  & 0xFF) / 255.0;
  yylval.value->color.b = (val & 0xFF) / 255.0;
  yylval.value->type = VTILE_MAPCSS_VALUE_TYPE_COLOR;

  return HEXCOLOR;
}

"{" {
  return LBRACE;
}

"}" {
  return RBRACE;
}

"[" {
  return LBRACKET;
}

"]" {
  return RBRACKET;
}

";" {
  return SEMICOLON;
}

":" {
  return COLON;
}

"!" {
  return NOT;
}

"=" {
  return EQUAL;
}

"," {
  return COMMA;
}

"-" {
  return DASH;
}

"|" {
  return PIPE;
}

{zoomlevel} {
  yylval.value = vtile_mapcss_value_new ();
  yylval.value->num = atoi (yytext + 1);

  return ZL;
}

{ident} {
  yylval.str = g_strdup (yytext);

  return IDENT;
 }

[\n\r] {
  yyset_lineno (++lineno, yyscanner);
  column = 0;
}

[ \t] {
  /* ignore whitespace */
}

. {
  return yytext[0];
}


%%
