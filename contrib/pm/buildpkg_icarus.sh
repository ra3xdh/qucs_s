#!/bin/sh

# Build a simple flat package for icarus-verilog
#
# ---------------------------------------
# Configure and install icarus-verilog to
# the 'payload' or 'root' directory.
# You can also point the root to another prefix,
# as long it only contains icarus related files.
#
# ex.
#
# ~/myIcarus/
#    bin/
#      iverilog
#      iverilog-vpi
#      vvp
#    include/
#    lib/
#    share/
#
# TODO add welcome, info, license
#

# Install to a 'temporary' and exclusive location
# configure to use the final 'system' lib and include 
#./configure --prefix=/Users/guilherme/local/icarus/ --libdir=/usr/local/lib --includedir=/usr/local/include
# point to this script to the 'temporary'

if [ $# -eq 0 ]
  then
    echo "*** No icarus-verilog root prefix supplied."
    echo "Configure and install icarus-verilog to an exclusive prefix."
    echo "Provide the prefix as root to build a flat-package."
    echo "ex.:"
    echo "  \$build_pkg.sh --root /usr/home/myIcarus/"
    echo ''
    echo "The directory myIcarus should contains only icarus related files."
    echo "ex.:"
    echo "   /usr/home/myIcarus/"
    echo "      bin/"
    echo "      lib/"
    echo "      share/"

    exit
fi

# Simple flat package stamp
version="0.9.6"
date=`date "+%Y%m%d"`

command="pkgbuild \
         --identifier org.icarus-verilog.pkg \
         --version 1.0 \
         $@ \
         --install-location /usr/local \
         ./icarus-verilog-${version}-${date}.pkg"

#echo "${command}"
${command}

