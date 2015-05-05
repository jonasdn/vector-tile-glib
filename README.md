##vector-tile-glib

Vector-tile-glib is a C library that can be used to render the [Mapbox Vector tile format](https://www.mapbox.com/developers/vector-tiles/) using [MapCSS](http://wiki.openstreetmap.org/wiki/MapCSS/0.2).
It is written using [GLib](http://en.wikipedia.org/wiki/GLib) and [GObject](http://en.wikipedia.org/wiki/GObject) making it available in all languages that have GObject introspection bindings.

#### Dependencies
In order to build and use vector-tile-glib you need the following installed:

 * Gio
 * libprotobuf-c
 * cairo
 *  pangocairo
 * protoc-c
 * Lemon
 * Flex 

### Building
Build the library like this:
    
    $ ./autogen.sh --enable-gtk-doc
    $ ./configure
    $ make
    $ make install


### Tools
There are also some tools available in the repository to explore the library.

#### tile-to-png
This takes a mapbox file and converts it to a PNG image using the sample.mss file:

    $ ./tile-to-png -h
    Usage:
      lt-tile-to-png [OPTION...] - test rendering to png image

    Help Options:
      -h, --help       Show help options

    Application Options:
      -s, --size       The size of the tile, default: 256
      -z, --zoom       The zoom-level of the tile, default: 0
      -o, --output     Output PNG filename, default: 'image.png'

#### dump-info
This tool will print all features and stylable tags of a mapbox file.

    $ ./dump-info -h
    Usage:
      lt-dump-info [OPTION...] - print information in tile

    Help Options:
      -h, --help       Show help options

#### get-tile
This tool will download a mapbox file from the Mapzen tile service. You specify a search-term or latitude/longitude and a zoom-level. To use this you also need to have libsoup and geocode-glib installed.

    $ ./get-tile -h
    Usage:
      get-tile [OPTION...] - download mapbox tile

    Help Options:
      -h, --help       Show help options

    Application Options:
      --latitude       Latitude to get tile y for
      --longitude      Longitude to get tile y for
      -z, --zoom       zoom level
      -s, --search     Location to get tile for
