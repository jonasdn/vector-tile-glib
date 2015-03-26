AC_INIT([vector-tile-glib], [0.0.1],[jonas@threetimestwo.org])
AM_INIT_AUTOMAKE([1.9 foreign])

AC_PROG_CC

PKG_CHECK_MODULES(VECTOR_TILE,
	gio-2.0 >= 2.42.1
	libprotobuf-c >= 1.0.1
	cairo >= 1.4)

AC_CHECK_PROG(PROTOCC_CHECK, protoc-c, yes)
if test x"$PROTOCC_CHECK" != x"yes"; then
   AC_MSG_ERROR([Please install the Protocol Buffers C compiler.])
fi

LT_INIT([disable-static])

AC_CONFIG_FILES([Makefile src/Makefile demos/Makefile])

AC_OUTPUT
