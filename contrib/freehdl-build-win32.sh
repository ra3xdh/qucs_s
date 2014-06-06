
## Cross compile FreeHDL

# run this script from the extracted source tarball
# cd ~/build/freehd-x.x.x/
# sh [path]/minw-freehdl.sh


# TODO automate build of GNU regex


# GNU regex is need to compile FreeHDL

# See:
# http://wiki.bindlebinaries.com/wiki/MinGW_Toolchain#GNU_regex

# cd ~/build
# wget http://ftp.gnu.org/old-gnu/regex/regex-0.12.tar.gz
# tar xfvz regex-0.12.tar.gz


## Linux (Inno Setup packaging not tested)

# sudo apt-get install mingw-w64
# cd ~/build/regex-0.12
# CC=i686-w64-mingw32-gcc ./configure --prefix=/usr/local/i686-w64-mingw32
# make
## issues wit tex... ignore or $sudo apt-get install texlive-binaries
## add regex to mingw-w64
# i686-w64-mingw32-ar ru libregex.a regex.o
# sudo cp regex.h /usr/i686-w64-mingw32/include/
# sudo cp libregex.a /usr/i686-w64-mingw32/lib/

# permission issue while compiling FreeHDL
# cd /usr/i686-w64-mingw32/include
# sudo chmod --reference regstr.h regex.h

## Mac OS X

# install Wine
# download/install MinGW 4.8.2 (see build_win32.sh)

# override DEFS otherwise it complains about missing bzero, bcopy, bcmp

# cd ~/build/regex-0.12
# CC='wine i686-w64-mingw32-gcc.exe' CCFLAGS=-static-libstdc++ DEFS='-DSTDC_HEADERS=1 -DHAVE_STRING_H=1 -DHAVE_ALLOCA_H=1' ./configure --host=i686-w64-mingw32
# make
## add regex to mingw-w64
# wine ~/.wine/drive_c/mingw32/bin/ar.exe ru libregex.a regex.o
# cp regex.h ~/.wine/drive_c/mingw32/i686-w64-mingw/include/
# cp libregex.a ~/.wine/drive_c/mingw32/lib



## Compile FreeHDL, see below
# Note: remember to copy libregex.a into freehdl_cross/lib (we do not modify mingw-w64, just repackage into an installer)

# TODO download freeHDL source

# run from ~/build/freehd-x.x.x/

echo "Building FreeHDL"

# Temporary install prefix

WINDIR=${HOME}/.wine/drive_c/freehdl_cross

if [ -d $WINDIR ]
then
	echo Directory freehdl_cross exists, removing ...
	rm -rf $WINDIR
fi


platform='unknown'
unamestr=`uname`
## Linux
if [[ "$unamestr" == 'Linux' ]]; then
  platform='linux'

  PREFIX=i686-w64-mingw32
  export CC=$PREFIX-gcc
  export CXX=$PREFIX-g++
  export CPP=$PREFIX-cpp
  export RANLIB=$PREFIX-ranlib
  export PATH="/usr/$PREFIX/bin:$PATH"

## MAC
elif [[ "$unamestr" == 'Darwin' ]]; then
   platform='darwin'

  export AR="wine ar.exe"
  export AS="wine as.exe"
  export CXX="wine g++.exe"
  export CC="wine gcc.exe"
  export LD="wine ld.exe"
  export DLLTOOL="wine dlltool.exe"
  export NM="wine nm.exe"
  export OBJDUMP="wine objdump.exe"
  export RANLIB="wine ranlib.exe"
  export STRIP="wine strip.exe"
  export WINDRES="wine windres.exe"
fi

echo "Current platform $platform"
echo "Install to ${WINDIR}"
echo "Configuring for i686-w64-mingw32 cross ..."
echo ""

TARGET=i686-w64-mingw32
HOST=i686-w64-mingw32
BUILD=i686-w64-mingw32

# no need for triplets with mingw-w64?
# http://sourceforge.net/apps/trac/mingw-w64/wiki/TypeTriplets

CXXFLAGS=-static-libstdc++ ./configure --prefix=${WINDIR} --target=$TARGET --host=$HOST --build=$BUILD --program-prefix="" --disable-dependency-tracking

make #-j 8
make install

echo Done!
