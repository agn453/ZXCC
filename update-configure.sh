#!/bin/sh
#
# Update the configure files if you get an error about a missing
# file (like WARNING: "'aclocal-1.11' is missing on your system.")
# because you have a later version installed.  Requires that the
# automake package (and prerequisites) are installed.
#
aclocal
autoconf
automake --add-missing
for dir in cpmio cpmredir ; do
        cd $dir
        aclocal
        autoconf
        automake --add-missing
        cd ..
done
