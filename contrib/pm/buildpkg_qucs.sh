#!/bin/sh
#
# Build a simple flat package for Qucs
#
# 0) extract the release tarball
# 1) select the target configure for Qucs
# 2) Make
# 3) Install to DESTDIR, eg. `make instal DESTDIR=/tmp/installdir`
# 4) This script will create a package out of the 'DESTDIR' directory
#
#

## References:
# http://vincent.bernat.im/en/blog/2013-autoconf-osx-packaging.html
# http://crushbeercrushcode.org/2014/01/using-pkgbuild-and-productbuild-on-os-x-10-7/
# http://stackoverflow.com/questions/11487596/making-os-x-installer-packages-like-a-pro-xcode4-developer-id-mountain-lion-re

## About uninstaller:
# http://superuser.com/questions/36567/how-do-i-uninstall-any-apple-pkg-package-file

# TODO extract / cd into tarball

# SDK is now handled by the configure.ac
# set -stdlib=libstdc++ for OS X 10.6
# set -stdlib=libc++     for OS X 10.7 and higher

# Only include ASCO
# Verilog-A, FreeHdl, require a compiler, better use Homebrew or Macports and build everything.

## Only Intel 64

## configure one of the following targets

VER=7 # 10.7+
#VER=6 # 10.6
#VER=5 # 10.5 #issues detecting this SDK

if [ $VER -eq 7 ]; then
  echo "Configure for OSX 10.7 to 10.9 (libc++)"

./configure --prefix=/usr/local --disable-adms \
--with-macosx-sdk=10.9 \
--with-macosx-version-min-required=10.7 \
--with-macosx-version-max-allowed=10.9
fi

if [ $VER -eq 6 ]; then
  echo "Configure for OSX 10.6 (libstdc++)"
./configure --prefix=/usr/local --disable-adms \
--with-macosx-sdk=10.6 \
--with-macosx-version-min-required=10.6 \
--with-macosx-version-max-allowed=10.6
fi

# use skd 10.6 with minversion 10.5
if [ $VER -eq 5 ]; then
  echo "Configure for OSX 10.5 (libstdc++)"

./configure --prefix=/usr/local --disable-adms \
--with-macosx-sdk=10.5 \
--with-macosx-version-min-required=10.5 \
--with-macosx-version-max-allowed=10.5
fi


# build
make -j 8


# might need to install with sudo, to set correct permissions
# Install to a separate directory for capture.
DEST=/tmp/installdir10$VER

if [ -d $DEST ]; then
  sudo rm -rf $DEST
fi
mkdir $DEST

sudo make install DESTDIR=$DEST

# source location of package resources
SRC=$HOME/git/qucs/qucs/contrib/pm

# where to put the resources
mkdir resources

# copy package resources
cp $SRC/qucslogo4_transparent.png resources/
cp $SRC/Welcome.rtf resources/
cp $SRC/License.rtf resources/
cp $SRC/Readme.rtf resources/

# tag the Welcome file
sed -i 'voo' "s/10\.[5-7]/10\.${VER}/g" resources/Welcome.rtf

# copy locally the postinstall script
mkdir scripts
cp $SRC/postinstall scripts/
#sudo chmod +x scripts/postinstall

# copy locally the customized Distribution file
cp $SRC/Distribution.xml .

# Simple flat package stamp
version="0.0.18"
#date=`date "+%Y%m%d"`

# pickup DESTDIR and build flat package
rm ./qucs-${version}.pkg
command="pkgbuild \
         --root $DEST \
         --scripts ./scripts
         --identifier org.qucs.pkg \
         --version 0.0.18 \
         --install-location / \
         ./qucs-${version}.pkg"

echo "${command}"
${command}

# check out which Distribution data we have so far
# productbuild --synthesize --package qucs-0.0.18.pkg Distribution.xml
# edit the Distrubution.xml file if necessary

# finish package
productbuild --distribution ./Distribution.xml ./qucs-${version}-OSX10.${VER}.pkg --resources ./resources --package-path ./qucs-${version}.pkg


