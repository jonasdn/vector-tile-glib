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

#include <gio/gio.h>

#include "vector-tile-mapcss.h"
#include "vector-tile-mapcss-style.h"
#include "vector-tile-mapcss-private.h"
#include "vector-tile-mapcss-lemon.h"
#include "vector-tile-mapcss-flex.h"

#define VTILE_MAPCSS_ERROR vtile_mapcss_error_quark ()

enum {
  VTILE_MAPCSS_ERROR_PARSE
};

enum {
  PROP_0,

  PROP_LINENO,
  PROP_COLUMN
};

G_DEFINE_TYPE_WITH_PRIVATE (VTileMapCSS, vtile_mapcss, G_TYPE_OBJECT)

void *ParseAlloc(void *(*mallocProc)(size_t));

GQuark
vtile_mapcss_error_quark (void)
{
  return g_quark_from_static_string ("vtile-mapcss-error");
}

static void
vtile_mapcss_finalize (GObject *vmapcss)
{
  VTileMapCSS *mapcss = VTILE_MAPCSS (vmapcss);

  if (mapcss->priv->parse_error)
    g_free (mapcss->priv->parse_error);

  g_hash_table_destroy (mapcss->priv->selectors);

  G_OBJECT_CLASS (vtile_mapcss_parent_class)->finalize (vmapcss);
}

static void
vtile_mapcss_get_property (GObject *object,
                           guint property_id,
                           GValue *value,
                           GParamSpec *pspec)
{
  VTileMapCSS *mapcss = VTILE_MAPCSS (object);

  switch (property_id) {
  case PROP_LINENO:
    g_value_set_uint (value, mapcss->priv->lineno);
    break;

  case PROP_COLUMN:
    g_value_set_uint (value, mapcss->priv->column);
    break;

  default:
    /* We don't have any other property... */
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    break;
  }
}

static void
vtile_mapcss_class_init (VTileMapCSSClass *klass)
{
  GObjectClass *mapcss_class = G_OBJECT_CLASS (klass);
  GParamSpec *pspec;

  mapcss_class->finalize = vtile_mapcss_finalize;
  mapcss_class->get_property = vtile_mapcss_get_property;

  /**
   * VTileMapCSS:lineno:
   *
   * The line currently being parsed
   */
  pspec = g_param_spec_uint ("lineno",
                             "Line number",
                             "The line currently being parsed",
                             0,
                             G_MAXUINT,
                             0,
                             G_PARAM_READABLE |
                             G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (mapcss_class, PROP_LINENO, pspec);

  /**
   * VTileMapCSS:column:
   *
   * The line column being parsed
   */
  pspec = g_param_spec_uint ("column",
                             "Column",
                             "The column currently being parsed",
                             0,
                             G_MAXUINT,
                             0,
                             G_PARAM_READABLE |
                             G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (mapcss_class, PROP_COLUMN, pspec);
}

static void
vtile_mapcss_free_selectors (gpointer queue)
{
  g_queue_free_full ((GQueue *) queue, g_object_unref);
}

static void
vtile_mapcss_init (VTileMapCSS *mapcss)
{
  mapcss->priv = vtile_mapcss_get_instance_private (mapcss);

  mapcss->priv->lineno = 0;
  mapcss->priv->column = 0;
  mapcss->priv->text = NULL;
  mapcss->priv->parse_error = NULL;

  mapcss->priv->selectors = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                   NULL,
                                                   vtile_mapcss_free_selectors);
}

/**
 * vtile_mapcss_new:
 *
 * Returns: a new #GeocodeForward. Use g_object_unref() when done.
 **/
VTileMapCSS *
vtile_mapcss_new (void)
{
  VTileMapCSS *mapcss;

  mapcss = g_object_new (VTILE_TYPE_MAPCSS, NULL);

  return mapcss;
}

static gboolean
vtile_mapcss_parse (VTileMapCSS *mapcss, guint8 *data, gssize size,
                    GError **error)
{
  yyscan_t scanner;
  YY_BUFFER_STATE buffer_state;
  void *lemon_mapcss;
  gint lex_code;
  gboolean ret = TRUE;

  yylex_init(&scanner);

  buffer_state = yy_scan_buffer (data, size, scanner);
  lemon_mapcss = ParseAlloc (malloc);

  yyset_lineno (1, scanner);
  yyset_column (0, scanner);
  do {
    gdouble val;

    lex_code = yylex (scanner);
    mapcss->priv->text = yyget_text (scanner);
    mapcss->priv->lineno = yyget_lineno (scanner);
    mapcss->priv->column = yyget_column (scanner);

    Parse (lemon_mapcss, lex_code, yylval, mapcss);
  } while (lex_code > 0 && !mapcss->priv->parse_error);

  if (mapcss->priv->parse_error) {
    g_set_error_literal (error,
                         VTILE_MAPCSS_ERROR,
                         VTILE_MAPCSS_ERROR_PARSE,
                         mapcss->priv->parse_error);
    ret = FALSE;
  }

  yy_delete_buffer(buffer_state, scanner);
  yylex_destroy(scanner);
  ParseFree(lemon_mapcss, free);

  return ret;
}


gboolean
vtile_mapcss_load (VTileMapCSS *mapcss, const char *filename,
                   GError **error)
{
  GFile *file;
  GFileInfo *info;
  GFileInputStream *stream;
  goffset size;
  gssize bytes_read;
  gboolean status;
  guint8 *buffer;

  file = g_file_new_for_path (filename);
  info = g_file_query_info (file,
                            G_FILE_ATTRIBUTE_STANDARD_SIZE,
                            G_FILE_QUERY_INFO_NONE,
                            NULL,
                            NULL);
  if (!info) {
    g_object_unref (file);
    g_set_error (error,
                 VTILE_MAPCSS_ERROR,
                 VTILE_MAPCSS_ERROR_PARSE,
                 "Failed to query '%s'",
                 filename);
    return FALSE;
  }

  size = g_file_info_get_size (info);
  buffer = g_malloc (size + 2);

  stream = g_file_read (file, NULL, NULL);
  status = g_input_stream_read_all ((GInputStream *) stream,
                                    buffer,
                                    size,
                                    &bytes_read,
                                    NULL,
                                    NULL);
  g_object_unref (file);
  g_object_unref (info);
  g_object_unref (stream);

  buffer[size] = 0;
  buffer[size + 1] = 0;

  if (!status) {
    g_set_error (error,
                 VTILE_MAPCSS_ERROR,
                 VTILE_MAPCSS_ERROR_PARSE,
                 "Failed to read '%s'",
                 filename);
    g_free (buffer);

    return FALSE;
  }

  status = vtile_mapcss_parse (mapcss, buffer, size + 2, error);
  g_free (buffer);

  return status;
}

void vtile_mapcss_set_syntax_error (VTileMapCSS *mapcss,
                                    char *valid_tokens)
{
  char *msg;

  msg = g_strdup_printf ("Unexpected token '%s' at %u:%u, expected: %s\n",
                         mapcss->priv->text, mapcss->priv->lineno,
                         mapcss->priv->column, valid_tokens);
  mapcss->priv->parse_error = msg;

  g_free (valid_tokens);
}

void vtile_mapcss_set_type_error (VTileMapCSS *mapcss)
{
  char *msg;
  gint i;

  msg = g_strdup_printf ("Unexpected type at %u:%u\n",
                         mapcss->priv->lineno, mapcss->priv->column);
  mapcss->priv->parse_error = msg;
}

void
vtile_mapcss_add_selector (VTileMapCSS *mapcss,
                           VTileMapCSSSelector *selector)
{
  GQueue *selector_queue = NULL;
  gboolean push = TRUE;
  GList *l;
  char * name;
  gint i;

  name = vtile_mapcss_selector_get_name (selector);
  selector_queue = g_hash_table_lookup (mapcss->priv->selectors, name);
  if (!selector_queue) {
    selector_queue = g_queue_new ();
    g_hash_table_insert (mapcss->priv->selectors, name, selector_queue);
  }

  for (i = 0; i < selector_queue->length; i++) {
    VTileMapCSSSelector *current = g_queue_peek_nth (selector_queue, i);

    if (vtile_mapcss_selector_equals (current, selector)) {
      vtile_mapcss_selector_merge (current, selector);
      push = FALSE;
      break;
    }
  }

  if (push)
    g_queue_push_tail (selector_queue, selector);
}

static void
vtile_mapcss_apply_selector (VTileMapCSSSelector *selector,
                             VTileMapCSSStyle *style)
{
  GHashTable *declarations;
  char **keys;
  gint n, i;

  declarations = vtile_mapcss_selector_get_declarations (selector);
  keys = (char **) g_hash_table_get_keys_as_array (declarations, &n);

  for (i = 0; i < n; i++) {
    char *property;
    VTileMapCSSValue *value;

    property = g_strdup (keys[i]);
    value = vtile_mapcss_value_copy (g_hash_table_lookup (declarations, keys[i]));
    g_hash_table_insert (style->properties, property, value);
  }
}

static gboolean
vtile_mapcss_match_tests (VTileMapCSSSelector *selector,
                          GHashTable *tags)
{
  GList *l = NULL;
  gboolean match = TRUE;
  GList *tests = vtile_mapcss_selector_get_tests (selector);

  if (!tests)
    return TRUE;

  for (l = tests; l != NULL; l = l->next) {
    VTileMapCSSTest *test = (VTileMapCSSTest *) l->data;
    char *value = NULL;

    if (tags)
      value = g_hash_table_lookup (tags, test->tag);

    switch (test->operator) {
    case VTILE_MAPCSS_TEST_TAG_IS_SET:
      if (!value)
        match = FALSE;
      break;

    case VTILE_MAPCSS_TEST_TAG_IS_NOT_SET:
      if (value)
        match = FALSE;
      break;

    case VTILE_MAPCSS_TEST_TAG_EQUALS:
      if (!value)
        match = FALSE;
      else if (g_strcmp0 (value, test->value))
        match = FALSE;
      break;

    case VTILE_MAPCSS_TEST_TAG_NOT_EQUALS:
      if (!value)
        match = FALSE;
      else if (!g_strcmp0 (value, test->value))
        match = FALSE;
      break;
    }
  }

  return match;
}

static gboolean
vtile_mapcss_match_zoom (VTileMapCSSSelector *selector,
                         guint zoom_level)
{
  guint *ranges;

  ranges = vtile_mapcss_selector_get_zoom_levels (selector);
  if (!ranges)
    return TRUE;

  return zoom_level >= ranges[0] && zoom_level <= ranges[1];
}

VTileMapCSSStyle *
vtile_mapcss_get_style (VTileMapCSS *mapcss,
                        const char *selector_name,
                        GHashTable *tags,
                        guint zoom_level)
{
  VTileMapCSSStyle *style;
  GQueue *queue;
  gint i;

  style = vtile_mapcss_style_new ();

  queue = g_hash_table_lookup (mapcss->priv->selectors, selector_name);
  if (queue) {
    for (i = 0; i < queue->length; i++) {
      VTileMapCSSSelector *selector = g_queue_peek_nth (queue, i);
      if (vtile_mapcss_match_zoom (selector, zoom_level) &&
          vtile_mapcss_match_tests (selector, tags))
        vtile_mapcss_apply_selector (selector, style);
    }
  }

  return style;
}

gint
vtile_mapcss_get_num_styles (VTileMapCSS *mapcss)
{
  gint n, i, count = 0;
  char **keys;

  keys = (char **) g_hash_table_get_keys_as_array (mapcss->priv->selectors, &n);

  for (i = 0; i < n; i++) {
    GQueue *queue = g_hash_table_lookup (mapcss->priv->selectors, keys[i]);

    count += queue->length;
  }

  return count;
}
