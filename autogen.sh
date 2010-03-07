#! /bin/sh

libtoolize --force --automake --copy
aclocal
autoheader
autoconf
automake -a --copy
