noinst_PROGRAMS = tile-to-png

AM_CPPFLAGS = $(VECTOR_TILE_CFLAGS) -I$(top_srcdir)/src
tile_to_png_SOURCES = tile-to-png.c
tile_to_png_LDADD = $(VECTOR_TILE_LIBS) ../src/libvector-tile-glib.la
