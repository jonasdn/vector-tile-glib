lib_LTLIBRARIES = libvector-tile-glib.la

proto_file = vector_tile.proto
proto_source = $(proto_file:.proto=.pb-c.c)
proto_header = $(proto_file:.proto=.pb-c.h)

lemon_file_mapcss = vector-tile-mapcss-lemon.y
lemon_source_mapcss = $(lemon_file_mapcss:.y=.c)
lemon_header_mapcss = $(lemon_file_mapcss:.y=.h)

flex_file_mapcss = vector-tile-mapcss-flex.l
flex_source_mapcss = $(flex_file_mapcss:.l=.c)
flex_header_mapcss = $(flex_file_mapcss:.l=.h)

EXTRA_DIST =								\
	vector-tile-enum-types.c.template				\
	vector-tile-enum-types.h.template				\
	$(proto_file)

BUILT_SOURCES =								\
	$(proto_source)							\
	$(proto_header)							\
	$(lemon_source_mapcss)						\
	$(lemon_header_mapcss)						\
	$(flex_source_mapcss)						\
	$(flex_header_mapcss)						\
	vector-tile-enum-types.c					\
	vector-tile-enum-types.h

libvector_tile_glib_la_PUBLICSOURCES =					\
	vector-tile-mapbox.c						\
	vector-tile-mapcss.c

libvector_tile_glib_la_HEADERS =					\
	vector-tile-mapbox.h						\
	vector-tile-boxed.h						\
	vector-tile-mapcss.h						\
	vector-tile-mapcss-style.h					\
	vector-tile-mapcss-selector.h					\
	vector-tile-mapcss-value.h					\
	vector-tile-mapcss-private.h

libvector_tile_glib_la_SOURCES =					\
	vector-tile-boxed.c						\
	vector-tile-mapbox.c						\
	vector-tile-mapcss.c						\
	vector-tile-mapcss-selector.c					\
	vector-tile-mapcss-value.c					\
	vector-tile-mapcss-test.c					\
	vector-tile-mapcss-style.c					\
	$(BUILT_SOURCES)						\
	$(libvector_tile_glib_la_HEADERS)

libvector_tile_glib_ladir = $(includedir)/vector-tile-glib/
libvector_tile_glib_la_LIBADD = $(VECTOR_TILE_LIBS)
libvector_tile_glib_la_LDFLAGS =					\
	--version-info $(VTILE_LT_VERSION)				\
	-no-undefined

libvector_tile_glib_la_CFLAGS = $(VECTOR_TILE_CFLAGS)

$(proto_header): $(proto_source)
$(proto_source): $(srcdir)/$(proto_file)
	$(info srcdir: $(srcdir))
	protoc-c --c_out=$(srcdir) $<

$(lemon_header_mapcss): $(lemon_source_mapcss)
$(lemon_source_mapcss): $(srcdir)/$(lemon_file_mapcss)
	lemon $<

$(flex_header_mapcss): $(flex_source_mapcss)
$(flex_source_mapcss): $(srcdir)/$(flex_file_mapcss)
	flex --outfile=$@ --header-file=$(flex_header_mapcss) $<

vector-tile-enum-types.h: vector-tile-mapcss-selector.h
	$(AM_V_GEN) ($(GLIB_MKENUMS)					\
		--template $(srcdir)/vector-tile-enum-types.h.template	\
		$^ > xgen-$(@F) && mv -f xgen-$(@F) $@)

vector-tile-enum-types.c: vector-tile-mapcss-selector.h vector-tile-enum-types.h
	$(AM_V_GEN) ($(GLIB_MKENUMS)					\
		--template $(srcdir)/vector-tile-enum-types.c.template	\
		$^ > xgen-$(@F) && mv -f xgen-$(@F) $@)


if HAVE_INTROSPECTION
-include $(INTROSPECTION_MAKEFILE)
VectorTileGlib-1.0.gir: libvector-tile-glib.la
VectorTileGlib_1_0_gir_INCLUDES =					\
	Gio-2.0								\
	GObject-2.0							\
	cairo-1.0
VectorTileGlib_1_0_gir_FILES =						\
	vector-tile-boxed.c						\
	vector-tile-boxed.h						\
	vector-tile-mapcss.c						\
	vector-tile-mapcss-value.c					\
	vector-tile-mapcss-style.c					\
	vector-tile-mapbox.c						\
	vector-tile-mapbox.h						\
	vector-tile-mapcss.h						\
	vector-tile-mapcss-style.h					\
	vector-tile-mapcss-selector.c					\
	vector-tile-mapcss-selector.h
VectorTileGlib_1_0_gir_CFLAGS =						\
	$(VECTOR_TILE_CFLAGS)						\
	-I$(top_srcdir)							\
	-I$(top_builddir)						\
	-I$(srcdir)
VectorTileGlib_1_0_gir_NAMESPACE = VectorTileGlib
VectorTileGlib_1_0_gir_LIBS = libvector-tile-glib.la
VectorTileGlib_1_0_gir_PACKAGES = gobject-2.0 glib-2.0 gio-2.0 cairo
VectorTileGlib_1_0_gir_EXPORT_PACKAGES = vector-tile-glib-1.0
VectorTileGlib_1_0_gir_SCANNERFLAGS =					\
	--warn-all							\
	--symbol-prefix=vtile						\
	--identifier-prefix=VTile
INTROSPECTION_GIRS = VectorTileGlib-1.0.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

endif

CLEANFILES = $(gir_DATA) $(typelib_DATA) $(BUILT_SOURCES)
