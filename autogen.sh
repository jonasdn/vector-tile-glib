#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PKG_NAME="vector-tile-glib"

gtkdocize || {
    echo "No gtkdocize..."
}

which gnome-autogen.sh || {
    echo "You need to install gnome-common from the GNOME git"
    exit 1
}

REQUIRED_AUTOCONF_VERSION=2.59
REQUIRED_AUTOMAKE_VERSION=1.9
REQUIRED_INTLTOOL_VERSION=0.40.0
REQUIRED_PKG_CONFIG_VERSION=0.22
. gnome-autogen.sh
