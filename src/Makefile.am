lib_LTLIBRARIES = libvector-tile-glib.la
libvector_tile_glib_la_PUBLICSOURCES = vector-tile-mapbox.c
libvector_tile_glib_la_SOURCES = vector-tile-mapbox.c vector_tile.pb-c.c

libvector_tile_glib_ladir = $(includedir)/vector-tile-mapbox/
libvector_tile_glib_la_HEADERS = vector-tile-mapbox.h
libvector_tile_glib_la_LIBADD = $(VECTOR_TILE_LIBS)
libvector_tile_glib_la_CFLAGS = $(VECTOR_TILE_CFLAGS)

proto_files = vector_tile.proto
proto_source =								\
	$(proto_files:.proto=.pb-c.c)					\
	$(proto_files:.proto=.pb-c.h)

BUILT_SOURCES = $(proto_source)
CLEANFILES = $(BUILT_SOURCES)

$(proto_source): $(proto_files)
	protoc-c --c_out=$(srcdir) $<
