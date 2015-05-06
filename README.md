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


## MapCSS

Currently the following MapCSS constructs and properties are supported,
let me know what you are missing the most!


### Selector types

    node
    way
    line
    canvas
    area

### Tests

    [tag=value]
    [tag!=value]
    [tag]
    [!tag]

### Zoom filters

    area|zn
    area|zn-n+x


### Properties

    fill-color
    fill-opacity
    z-index
    width
    color
    opacity
    dashes
    linecap
    linejoin
    casing-width
    casing-color
    casing-opacity
    casing-dashes
    casing-linejoin
    casing-linecap
    text
    font-family
    font-size
    font-weight
    font-style
    font-variant
    text-decoration
    text-transform
    text-color
    text-opacity
    text-halo-color
    text-halo-radius

### Example

    canvas {
        fill-color: #FAEBD7;
    }

    node|z5-10[place=country] {
        text-color: #4A90D9;
        text: name;
        text-halo-radius: 2;
        text-halo-color:white;
    }

    node|z5-13[place=city] {
        font-family: cantarell;
        font-weight: bold;
        text-color: #000000;
        font-size: 14;
        text: name;
        text-halo-radius: 2;
        text-halo-color: white;
    }

    way|z8-17[highway] {
        color: #d2ccc3;
        casing-color: #c7b8a4;
        casing-width: 1;
    }

    way|z8-17[highway=motorway],
    way|z8-17[highway=motorway_link] {
        color: #ffaf60;
        casing-color: #f57900;
        z-index: 7;
    }

    area|z16-19[building] {
        text: name;
        text-color: #222233;
        text-halo-color: white;
        text-halo-radius: 1;
        font-size: 8;
        font-family: cantarell;
    }

    area|z16-19[landuse] {
        text: name;
        text-color: #222233;
        text-halo-color: white;
        text-halo-radius: 1;
        font-size: 8;
        font-family: cantarell;
    }
