#!/bin/sh
#
# Build a simple flat package for FreeHDL
#
# 1) Configure and install FreeHDL to the final prefix '/usr/local'
# 2) Run 'buildpkg_freehdl_copy.sh' to copy from the above prefix
#    to the 'payload_freehdl' directory.
# 3) This script will create a package out of the 'payload_freehdl' directory
#
# TODO add welcome, info, license
#

if [ $# -eq 0 ]
  then
    echo "*** No FreeHDL payload prefix supplied."
    echo "Provide the path to payload (copy from install prefix)"
    echo "ex.:"
    echo "  \$buildpkg_freehdl.sh --root ~/git/qucs/qucs/contrib/pm/payload_freehdl/"
    echo ''
    exit
fi

# Simple flat package stamp
version="0.0.8"
date=`date "+%Y%m%d"`

command="pkgbuild \
         --identifier org.freehdl.pkg \
         --version 1.0 \
         $@ \
         --install-location /usr/local \
         ./freehdl-${version}-${date}.pkg"

#echo "${command}"
${command}

