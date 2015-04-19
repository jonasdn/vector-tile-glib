noinst_PROGRAMS = tile-to-png dump-info get-tile

AM_CPPFLAGS = $(VECTOR_TILE_CFLAGS) -I$(top_srcdir)/src

tile_to_png_SOURCES = tile-to-png.c
tile_to_png_LDADD = $(VECTOR_TILE_LIBS) ../src/libvector-tile-glib.la

get_tile_SOURCES = get-tile.c
get_tile_LDADD = $(VECTOR_TILE_LIBS) ../src/libvector-tile-glib.la
get_tile_LDFLAGS = `pkg-config --libs geocode-glib-1.0`
get_tile_CFLAGS = `pkg-config --cflags geocode-glib-1.0`

dump_info_SOURCES = dump-info.c
dump_info_LDADD = $(VECTOR_TILE_LIBS) ../src/libvector-tile-glib.la

.PHONY: debug
debug: tile-to-png
	libtool --mode=execute gdb --args ./tile-to-png 24641.mapbox

.PHONY: memcheck
memcheck: tile-to-png
	G_DEBUG=gc-friendly G_SLICE=always-malloc libtool --mode=execute valgrind --log-file=vallog --tool=memcheck --leak-check=full ./tile-to-png 24641.mapbox
