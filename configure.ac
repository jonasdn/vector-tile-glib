AC_INIT([vector-tile-glib], [0.1],[jonas@threetimestwo.org])
AM_INIT_AUTOMAKE([1.9 foreign])

AC_PROG_CC

AC_CONFIG_MACRO_DIR([m4])

PKG_CHECK_MODULES(VECTOR_TILE,
	gio-2.0 >= 2.42.1
	libprotobuf-c >= 1.0.1
	cairo >= 1.4
        pangocairo)

PKG_CHECK_MODULES([GEOCODE_GLIB], [geocode-glib-1.0],
   found_geocode=yes, found_geocode=no)
AM_CONDITIONAL(HAVE_GEOCODE_GLIB, test "x$found_geocode" = "xyes")

PKG_CHECK_MODULES([SOUP], [libsoup-gnome-2.4],
   found_soup=yes, found_soup=no)
AM_CONDITIONAL(HAVE_SOUP, test "x$found_soup" = "xyes")

GOBJECT_INTROSPECTION_CHECK([0.6.3])

AC_CHECK_PROG(PROTOCC_CHECK, protoc-c, yes)
if test x"$PROTOCC_CHECK" != x"yes"; then
   AC_MSG_ERROR([Please install the Protocol Buffers C compiler.])
fi

AC_CHECK_PROG(LEMON_CHECK, lemon, yes)
if test x"$LEMON_CHECK" != x"yes"; then
   AC_MSG_ERROR([Please install the Lemon parser generator.])
fi

AC_CHECK_PROG(FLEX_CHECK, flex, yes)
if test x"$FLEX_CHECK" != x"yes"; then
   AC_MSG_ERROR([Please install the Flex scanner generator.])
fi

LT_INIT([disable-static])

VTILE_LT_VERSION=0:1:0
AC_SUBST(VTILE_LT_VERSION)

GLIB_MKENUMS=`$PKG_CONFIG --variable=glib_mkenums glib-2.0`
AC_SUBST(GLIB_MKENUMS)

# check for gtk-doc
m4_ifdef([GTK_DOC_CHECK], [
GTK_DOC_CHECK([1.14],[--flavour no-tmpl])
],[
AM_CONDITIONAL([ENABLE_GTK_DOC], false)
])

AC_CONFIG_FILES([
        vector-tile-glib-1.0.pc
        Makefile
        src/Makefile
        tools/Makefile
        test/Makefile
        docs/Makefile
        docs/reference/Makefile
        docs/reference/version.xml])

AC_OUTPUT
