#!/bin/bash

## file: bundle_win32.sh
#
## This file creates the Qucs Windows x86_32 installer.

# Requires binaries into:   ~/.wine/drive_c/qucs-win32-bin/
# Requires Inno setup file: ~/git/qucs/qucs/contrib/innosetup/qucs.iss

# run from ~/gi/qucs

# Output ~/git/qucs/release/Output


## Install Inno Setup 5
#
# See also script `build_win32.sh`
#
# Install older Inno version due to issues with wine
# Error: * Could not find dependent assembly L"Microsoft.Windows.Common-Controls" (6.0.0.0)
#
# Install http://files.jrsoftware.org/is/5/isetup-5.5.0.exe	29-May-2012 06:51 	1.8M
# $ wine isetup-5.5.0.exe



if [ $# -ne 0 ]
then
  RELEASE=$1
else
  RELEASE=$(date +"%y%m%d")
  RELEASE="0.0.18."${RELEASE:0:6}
fi
echo Building release: $RELEASE


REPO=${PWD}
echo Working from: ${REPO}
# TODO test location


INNOSETUP="${HOME}/.wine/drive_c/Program Files/Inno Setup 5/Compil32.exe"
WINDIR=${HOME}/.wine/drive_c/qucs-win32-bin

# Set QTDIR
export QTDIR=${HOME}/.wine/drive_c/Qt/4.8.6


# add to installer content GPL notice, Icons, site locations
echo Copy Inno contents...
cp ${REPO}/qucs/contrib/innosetup/gpl.rtf $WINDIR
cp -r ${REPO}/qucs/contrib/innosetup/misc $WINDIR

# add Icarus Verilog installer
echo Copy Icarus Verilog...
ICARUS_VER="0.9.7"
if [ -f ~/Downloads/iverilog-${ICARUS_VER}_setup.exe ]
then
	cp ~/Downloads/iverilog-${ICARUS_VER}_setup.exe .
else
	#wget http://bleyer.org/icarus/iverilog-0.9.6_setup.exe
  wget http://bleyer.org/icarus/iverilog-${ICARUS_VER}_setup.exe -P ~/Downloads/
fi
mv iverilog-${ICARUS_VER}_setup.exe $WINDIR


# add FreeHDL into installer
echo Copy FreeHDL...
if [ -f ~/Downloads/freehdl-0.0.8-setup.exe ]
then
	cp ~/Downloads/freehdl-0.0.8-setup.exe .
else
	wget https://downloads.sourceforge.net/project/qucs/freehdl/freehdl-0.0.8-setup.exe -P ~/Downloads/
fi
mv freehdl-0.0.8-setup.exe $WINDIR


# TODO update this?
echo Copy MinGW...
if [ -f ~/Downloads/mingw32-g++-0.0.2-setup.exe ]
then
	cp ~/Downloads/mingw32-g++-0.0.2-setup.exe .
else
	wget https://downloads.sourceforge.net/project/qucs/freehdl/mingw32-g%2B%2B-0.0.2-setup.exe -P ~/Downloads/
fi
mv mingw32-g++-0.0.2-setup.exe $WINDIR


echo Copy runtime libraries...
cp $QTDIR/bin/Qt3Support4.dll  $WINDIR/bin
cp $QTDIR/bin/QtCore4.dll  $WINDIR/bin
cp $QTDIR/bin/QtGui4.dll  $WINDIR/bin
cp $QTDIR/bin/QtNetwork4.dll  $WINDIR/bin
cp $QTDIR/bin/QtXml4.dll  $WINDIR/bin
cp $QTDIR/bin/QtSql4.dll  $WINDIR/bin
cp $QTDIR/bin/QtSvg4.dll $WINDIR/bin
cp $QTDIR/bin/QtScript4.dll $WINDIR/bin

cp $QTDIR/bin/libgcc_s_dw2-1.dll $WINDIR/bin
cp $QTDIR/bin/libstdc++-6.dll $WINDIR/bin
cp $QTDIR/bin/libwinpthread-1.dll $WINDIR/bin

#cp $QTDIR/bin/mingwm10.dll $WINDIR/bin
#cp /usr/lib/gcc/i586-mingw32msvc/4.2.1-sjlj/*.dll $WINDIR/bin
#cp /usr/lib/gcc/i686-w64-mingw32/4.6/*.dll $WINDIR/bin
#cp $HOME/.wine/drive_c/mingw32/bin/*.dll $WINDIR/bin

echo Create Inno Setup installer...
cp  ${REPO}/qucs/contrib/innosetup/qucs.iss release/qucs.iss
wine "$INNOSETUP" /cc release/qucs.iss
rm release/qucs.iss

mv release/Output/qucs-0.0.18-setup.exe release/qucs-$RELEASE.exe
rm -r release/Output


echo Done!
