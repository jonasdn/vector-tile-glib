lib_LTLIBRARIES = libvector-tile-glib.la
libvector_tile_glib_la_PUBLICSOURCES =					\
	vector-tile-mapbox.c						\
	vector-tile-mapcss.c

libvector_tile_glib_la_HEADERS =					\
	vector-tile-mapbox.h						\
	vector-tile-mapcss.h						\
	vector-tile-mapcss-style.h					\
	vector-tile-mapcss-private.h

libvector_tile_glib_la_SOURCES =					\
	vector-tile-mapbox.c						\
	vector-tile-mapcss.c						\
	vector-tile-mapcss-selector.c					\
	vector-tile-mapcss-declaration.c				\
	vector-tile-mapcss-value.c					\
	vector-tile-mapcss-test.c					\
	vector-tile-mapcss-flex.c					\
	vector-tile-mapcss-lemon.c					\
	vector-tile-mapcss-style.c					\
	vector_tile.pb-c.c

libvector_tile_glib_la_HEADERS =					\
	vector-tile-mapbox.h						\
	vector-tile-mapcss.h

libvector_tile_glib_ladir = $(includedir)/vector-tile-mapbox/
libvector_tile_glib_la_LIBADD = $(VECTOR_TILE_LIBS)
libvector_tile_glib_la_CFLAGS = $(VECTOR_TILE_CFLAGS)

proto_file = vector_tile.proto
proto_source = $(proto_file:.proto=.pb-c.c)
proto_header = $(proto_file:.proto=.pb-c.h)

lemon_file_mapcss = vector-tile-mapcss-lemon.y
lemon_source_mapcss = $(lemon_file_mapcss:.y=.c)
lemon_header_mapcss = $(lemon_file_mapcss:.y=.h)

flex_file_mapcss = vector-tile-mapcss-flex.l
flex_source_mapcss = $(flex_file_mapcss:.l=.c)
flex_header_mapcss = $(flex_file_mapcss:.l=.h)

BUILT_SOURCES =								\
	$(proto_source)							\
	$(proto_header)							\
	$(lemon_source_mapcss)						\
	$(lemon_header_mapcss)						\
	$(flex_source_mapcss)						\
	$(flex_header_mapcss)

CLEANFILES = $(BUILT_SOURCES)

$(proto_source): $(proto_file)
	protoc-c --c_out=$(srcdir) $<

$(lemon_source_mapcss): $(lemon_file_mapcss)
	-lemon $<

$(flex_source_mapcss): $(flex_file_mapcss)
	flex --outfile=$@ --header-file=$(flex_header_mapcss) $<
