noinst_PROGRAMS = test-mapcss-parse test-mapcss-values

AM_CPPFLAGS = $(VECTOR_TILE_CFLAGS) -I$(top_srcdir)/src

test_mapcss_parse_SOURCES = test-mapcss-parse.c
test_mapcss_parse_LDADD = $(VECTOR_TILE_LIBS) ../src/libvector-tile-glib.la

test_mapcss_values_SOURCES = test-mapcss-values.c
test_mapcss_values_LDADD = $(VECTOR_TILE_LIBS) ../src/libvector-tile-glib.la

TESTS = test-mapcss-parse test-mapcss-values
