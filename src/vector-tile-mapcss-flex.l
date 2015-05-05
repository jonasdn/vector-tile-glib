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

  static gint
  string_to_hexcolor (const char *str)
  {
    long val;
    char hexcolor[7];
    gint i;

    /* Check for 3 letter hexcolor */
    if (strlen (str + 1) == 3) {
      for (i = 0; i < 3; i++) {
        hexcolor[i * 2] = str[i + 1];
        hexcolor[(i * 2) + 1] = str[i + 1];
      }
      hexcolor[6] = '\0';
    } else {
      strncpy (hexcolor, str + 1, 7);
    }

    val = strtol (hexcolor, NULL, 16);
    yylval.value = vtile_mapcss_value_new ();
    yylval.value->color.r = ((val >> 16) & 0xFF) / 255.0;
    yylval.value->color.g = ((val >> 8)  & 0xFF) / 255.0;
    yylval.value->color.b = (val & 0xFF) / 255.0;
    yylval.value->type = VTILE_MAPCSS_VALUE_TYPE_COLOR;

    return HEXCOLOR;
  }
%}

%x COMMENT

%option noyywrap
%option reentrant
%option yylineno

ident          [_a-z-]*
num            [0-9]+|[0-9]*"."[0-9]+
numlist        {num}","[ \t]*{num}(","[ \t]*{num})*
hexcolor       #([A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})
zoomlevel      "z"([0-9]|[0-9]{2})
%%

"/*"                  { BEGIN(COMMENT);                        }
<COMMENT>[^*\n]*      { /* eat anything that's not a '*' */    }
<COMMENT>"*"+[^*/\n]* { /* eat up '*'s not followed by '/'s */ }
<COMMENT>\n           { lineno++;                              }
<COMMENT>"*"+"/"      { BEGIN(INITIAL);                        }

 /* Selector types */
"area"   { return AREA;   }
"node"   { return NODE;   }
"way"    { return WAY;    }
"line"   { return LINE;   }
"canvas" { return CANVAS; }

 /* Property values */
"none"       { return NONE;       }
"round"      { return ROUND;      }
"square"     { return SQUARE;     }
"bevel"      { return BEVEL;      }
"miter"      { return MITER;      }
"normal"     { return NORMAL;     }
"bold"       { return BOLD;       }
"italic"     { return ITALIC;     }
"underline"  { return UNDERLINE;  }
"uppercase"  { return UPPERCASE;  }
"lowercase"  { return LOWERCASE;  }
"capitalize" { return CAPITALIZE; }
"center"     { return CENTER;     }
"small-caps" { return SMALL_CAPS; }

{hexcolor} { return string_to_hexcolor (yytext);    }
"aqua"     { return string_to_hexcolor ("#00FFFF"); }
"black"    { return string_to_hexcolor ("#000000"); }
"blue"     { return string_to_hexcolor ("#0000FF"); }
"fuchsia"  { return string_to_hexcolor ("#FF00FF"); }
"gray"     { return string_to_hexcolor ("#808080"); }
"geen"     { return string_to_hexcolor ("#008000"); }
"lime"     { return string_to_hexcolor ("#00FF00"); }
"maroon"   { return string_to_hexcolor ("#800000"); }
"navy"     { return string_to_hexcolor ("#000080"); }
"olive"    { return string_to_hexcolor ("#808000"); }
"orange"   { return string_to_hexcolor ("#FFA500"); }
"purple"   { return string_to_hexcolor ("#800080"); }
"red"      { return string_to_hexcolor ("#FF0000"); }
"silver"   { return string_to_hexcolor ("#C0C0C0"); }
"teal"     { return string_to_hexcolor ("#008080"); }
"white"    { return string_to_hexcolor ("#FFFFFF"); }
"yellow"   { return string_to_hexcolor ("#FFFF00"); }

{numlist} {
  char **nums;
  char **iter;
  gint i = 0;

  yylval.value = vtile_mapcss_value_new ();
  yylval.value->type = VTILE_MAPCSS_VALUE_TYPE_DASH;

  nums = g_strsplit (yytext, ",", -1);
  for (iter = nums; *iter != NULL; iter++)
    yylval.value->dash.dashes[i++] = g_ascii_strtod (*iter, NULL);
  yylval.value->dash.num_dashes = i;

  g_strfreev (nums);

  return NUM_LIST;
}

{num} {
  yylval.value = vtile_mapcss_value_new ();
  yylval.value->num = g_ascii_strtod (yytext, NULL);
  yylval.value->type = VTILE_MAPCSS_VALUE_TYPE_NUMBER;

  return NUM;
}

 /* Language constructs */
[ \t]"[" { return yytext[0]; };
[ \t]"|" { return yytext[0]; };

"{" { return LBRACE;    }
"}" { return RBRACE;    }
"[" { return LBRACKET;  }
"]" { return RBRACKET;  }
";" { return SEMICOLON; }
":" { return COLON;     }
"!" { return NOT;       }
"=" { return EQUAL;     }
"-" { return DASH;      }
"|" { return PIPE;      }
"," { return COMMA;     }

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

[ \t] { /* ignore whitespace */ }

. { return yytext[0]; }


%%
