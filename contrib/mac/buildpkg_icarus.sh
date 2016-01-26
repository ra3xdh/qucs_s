#!/bin/sh
#
# Build a simple flat package for Icarus Verilog
#
# 1) Configure and install icarus to the final prefix '/usr/local'
# 2) Run 'buildpkg_icarus_copy.sh' to copy from the above prefix
#    to the 'payload_icarus' directory.
# 3) This script will create a package out of the 'payload_icarus' directory
#
# TODO add welcome, info, license
#

if [ $# -eq 0 ]
  then
    echo "*** No FreeHDL payload prefix supplied."
    echo "Provide the path to payload (copy from install prefix)"
    echo "ex.:"
    echo "  \$buildpkg_freehdl.sh --root ~/git/qucs/qucs/contrib/pm/payload_icarus/"
    echo ''
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

