noinst_PROGRAMS = tile-to-png dump-info

AM_CPPFLAGS =								\
	$(VECTOR_TILE_CFLAGS)						\
	-I$(top_srcdir)/src
LDADD = $(VECTOR_TILE_LIBS) $(top_srcdir)/src/libvector-tile-glib.la

tile_to_png_SOURCES = tile-to-png.c
dump_info_SOURCES = dump-info.c

if HAVE_GEOCODE_GLIB
if HAVE_SOUP
noinst_PROGRAMS += get-tile
get_tile_SOURCES = get-tile.c
get_tile_LDADD =							\
	$(GEOCODE_GLIB_LIBS)						\
	$(SOUP_LIBS)							\
	-lm
get_tile_CFLAGS = $(GEOCODE_GLIB_CFLAGS) $(SOUP_CFLAGS)
endif
endif


.PHONY: debug
debug: tile-to-png
	libtool --mode=execute gdb --args ./tile-to-png 24641.mapbox

.PHONY: memcheck
memcheck: tile-to-png
	G_DEBUG=gc-friendly G_SLICE=always-malloc libtool --mode=execute valgrind --log-file=vallog --tool=memcheck --leak-check=full ./tile-to-png 24641.mapbox
