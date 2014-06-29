#!/bin/sh
#
# Build a simple flat package for Qucs
#
# 1) Configure Qucs to the final prefix '/usr/local'
# 2) Make
# 3) Install to DESTDIR, eg. `make instal DESTDIR=/tmp/installdir`
# 4) This script will create a package out of the 'DESTDIR' directory
#
#


# TODO extract / cd into tarball

# SDK is now handled by the configure.ac
# set -stdlib=libstdcc++ for OS X 10.6
# set -stdlib=libc++     for OS X 10.7 and higher
# SDK might me necessary.
#STDLIB=libc++
#OSXVER=10.7
#SDK=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX${OSXVER}.sdk
# configure source
#CFLAGS="-mmacosx-version-min=$OSXVER -arch x86_64" \
#CXXFLAGS="-stdlib=$STDLIB -mmacosx-version-min=$VER -arch x86_64" \
#LDDFLAGS="-stdlib=$STDLIB" \


./configure --prefix=/usr/local --disable-adms \
--with-macosx-sdk=10.6 \
--with-macos-version-min-required=10.6 \
--with-macos-version-max-allowed=10.6


# build
make -j 8

# might need to install with sudo, to set correct permissions
# Install to a separate directory for capture.
DEST=/tmp/installdir
mkdir $DEST

sudo make install DESTDIR=$DEST


# Simple flat package stamp
version="0.0.18"
date=`date "+%Y%m%d"`

# pickup DESTDIR and build flat package
command="pkgbuild \
         --root $DEST \
         --identifier org.qucs.pkg \
         --version 1.0 \
         --install-location / \
         ./qucs-${version}-${date}.pkg"

echo "${command}"
${command}


