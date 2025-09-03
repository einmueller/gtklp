#!/bin/sh

LANG=
LC_ALL=C
#LC_ALL=UTF-8
export LANG LC_ALL

echo Running libtoolize...
libtoolize --copy --force
echo Running gettextize...
#gettextize --copy -f --intl
gettextize --copy -f
echo Running aclocal...
aclocal -I m4 
echo Running autoheader...
autoheader --force
echo Running autoconf...
autoconf --force 
echo Running automake...
automake --add-missing --copy --include-deps --foreign --force-missing
echo "DON'T EVER CHANGE gettext.h"
echo Now run configure, make, make install.
