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

#ifndef __VECTOR_TILE_MAPCSS_SELECTOR_H__
#define __VECTOR_TILE_MAPCSS_SELECTOR_H__

#include <gio/gio.h>

G_BEGIN_DECLS

/**
 * SECTION: VTileMapCSSSelector: (skip)
 *
 */

GType vtile_mapcss_selector_get_type (void) G_GNUC_CONST;

#define VTILE_TYPE_MAPCSS_SELECTOR                  (vtile_mapcss_selector_get_type ())
#define VTILE_MAPCSS_SELECTOR(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), VTILE_TYPE_MAPCSS_SELECTOR, VTileMapCSSSelector))
#define VTILE_IS_MAPCSS_SELECTOR(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VTILE_TYPE_MAPCSS_SELECTOR))
#define VTILE_MAPCSS_SELECTOR_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), VTILE_TYPE_MAPCSS_SELECTOR, VTileMapCSSSelectorClass))
#define VTILE_IS_MAPCSS_SELECTOR_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), VTILE_TYPE_MAPCSS_SELECTOR))
#define VTILE_MAPCSS_SELECTOR_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), VTILE_TYPE_MAPCSS_SELECTOR, VTileMapCSSSelectorClass))

typedef struct _VTileMapCSSSelector        VTileMapCSSSelector;
typedef struct _VTileMapCSSSelectorClass   VTileMapCSSSelectorClass;
typedef struct _VTileMapCSSSelectorPrivate VTileMapCSSSelectorPrivate;

struct _VTileMapCSSSelector {
  /* <private> */
  GObject parent_instance;
  VTileMapCSSSelectorPrivate *priv;
};

struct _VTileMapCSSSelectorClass {
  /* <private> */
  GObjectClass parent_class;
};

/**
 * VTileMapCSSSelectorType:
 * @VTILE_MAPCSS_SELECTOR_TYPE_CANVAS: canvas
 * @VTILE_MAPCSS_SELECTOR_TYPE_WAY: way
 * @VTILE_MAPCSS_SELECTOR_TYPE_NODE: node
 * @VTILE_MAPCSS_SELECTOR_TYPE_AREA: area
 * @VTILE_MAPCSS_SELECTOR_TYPE_LINE: line
 * @VTILE_MAPCSS_SELECTOR_TYPE_LAST: (skip)
 */
typedef enum {
  VTILE_MAPCSS_SELECTOR_TYPE_CANVAS,
  VTILE_MAPCSS_SELECTOR_TYPE_WAY,
  VTILE_MAPCSS_SELECTOR_TYPE_NODE,
  VTILE_MAPCSS_SELECTOR_TYPE_AREA,
  VTILE_MAPCSS_SELECTOR_TYPE_LINE,
  VTILE_MAPCSS_SELECTOR_TYPE_LAST
} VTileMapCSSSelectorType;

VTileMapCSSSelector *vtile_mapcss_selector_new (VTileMapCSSSelectorType type,
                                                GList *tests,
                                                gint *zoom_levels);
void vtile_mapcss_selector_add_declarations (VTileMapCSSSelector *selector,
                                             GHashTable *declarations);
GHashTable *vtile_mapcss_selector_get_declarations (VTileMapCSSSelector *selector);
GList *vtile_mapcss_selector_get_tests (VTileMapCSSSelector *selector);
VTileMapCSSSelectorType vtile_mapcss_selector_get_selector_type (VTileMapCSSSelector *selector);
guint *vtile_mapcss_selector_get_zoom_levels (VTileMapCSSSelector *selector);
gboolean vtile_mapcss_selector_equals (VTileMapCSSSelector *a,
                                       VTileMapCSSSelector *b);
void vtile_mapcss_selector_merge (VTileMapCSSSelector *a,
                                  VTileMapCSSSelector *b);

G_END_DECLS

#endif /* VECTOR_TILE_MAPCSS_SELECTOR */
