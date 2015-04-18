noinst_PROGRAMS = tile-to-png test-dump-info

AM_CPPFLAGS = $(VECTOR_TILE_CFLAGS) -I$(top_srcdir)/src

tile_to_png_SOURCES = tile-to-png.c
tile_to_png_LDADD = $(VECTOR_TILE_LIBS) ../src/libvector-tile-glib.la

dump_info_SOURCES = dump-info.c
dump_info_LDADD = $(VECTOR_TILE_LIBS) ../src/libvector-tile-glib.la

.PHONY: debug
debug: tile-to-png
	libtool --mode=execute gdb --args ./tile-to-png 24641.mapbox

.PHONY: memcheck
memcheck: tile-to-png
	G_DEBUG=gc-friendly G_SLICE=always-malloc libtool --mode=execute valgrind --log-file=vallog --tool=memcheck --leak-check=full ./tile-to-png 24641.mapbox
