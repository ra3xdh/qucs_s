#!/bin/bash

## file: qucs-installer-win32.sh
#
## This file creates the Qucs Windows x86_32 installer.

# Requires binaries into:   ~/.wine/drive_c/qucs-win32-bin/
# Requires Inno setup file: ~/git/qucs/qucs/contrib/innosetup/qucs.iss

# run from ~/git/qucs

# Output ~/git/qucs/release/Output


## Install Inno Setup 5
#
# See also script `qucs_build_win32.sh`
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
  RELEASE="0.0.19."${RELEASE:0:6}

  # Get Git short hash
  GIT=$(git log --pretty=format:'%h' -n 1)
  # Append git short hash
  RELEASE=${RELEASE}-git-${GIT}
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
#cp ${REPO}/qucs/contrib/innosetup/infobefore.rtf $WINDIR
cp -r ${REPO}/qucs/contrib/innosetup/misc $WINDIR

# add Icarus Verilog installer
echo Copy Icarus Verilog...
#ICARUS_VER="0.9.7"
ICARUS_VER="20130827" # fixed Win7 issue, DLL hell, picking up wrong libs.
icarus=iverilog-${ICARUS_VER}_setup.exe
if [ -f ${HOME}/Downloads/$icarus ]
then
	cp ${HOME}/Downloads/$icarus .
else
  wget http://bleyer.org/icarus/$icarus -P ${HOME}/Downloads/
	cp ${HOME}/Downloads/$icarus .
fi
cp $icarus $WINDIR


# add FreeHDL into installer
#freehdl=~/Downloads/freehdl-0.0.8-setup.exe
freehdl=freehdl-0.0.8-1-setup.exe # recompiled, repackaged, no admin
if [ -e ${HOME}/Downloads/$freehdl ]; then
  echo Copy FreeHDL...
	cp ${HOME}/Downloads/$freehdl .
else
  # TODO upload
  echo Downloading FreeHDL...
	wget https://downloads.sourceforge.net/project/qucs/freehdl/$freehdl -P ${HOME}/Downloads/
	cp ${HOME}/Downloads/$freehdl .
fi
cp $freehdl $WINDIR


# Add Mingw-w64 to installer
# Use repackaged instarller version from the original from http://sourceforge.net/projects/mingw-w64/
# See installer in ~/git/qucs/qucs/contrib/innosetup/mingw-w64.iss
mingw=mingw-w64-i686-4.8.2-release-posix-dwarf-rt_v3-rev3-setup.exe
if [ -e ${HOME}/Downloads/$mingw ]; then
  echo Copy MinGW...
	cp ${HOME}/Downloads/$mingw .
else
  # TODO upload it somewhere or figure out a way to use a .7z file with Inno setup
  echo Downloading Mingw...
	wget https://downloads.sourceforge.net/project/qucs/freehdl/$mingw -P ${HOME}/Downloads/
	cp ${HOME}/Downloads/$mingw .
fi
cp $mingw $WINDIR


echo Copy runtime libraries...
cp $QTDIR/bin/Qt3Support4.dll $WINDIR/bin
cp $QTDIR/bin/QtCore4.dll     $WINDIR/bin
cp $QTDIR/bin/QtGui4.dll      $WINDIR/bin
cp $QTDIR/bin/QtNetwork4.dll  $WINDIR/bin
cp $QTDIR/bin/QtXml4.dll      $WINDIR/bin
cp $QTDIR/bin/QtSql4.dll      $WINDIR/bin
cp $QTDIR/bin/QtSvg4.dll      $WINDIR/bin
cp $QTDIR/bin/QtScript4.dll   $WINDIR/bin

cp $QTDIR/bin/libgcc_s_dw2-1.dll  $WINDIR/bin
cp $QTDIR/bin/libstdc++-6.dll     $WINDIR/bin
cp $QTDIR/bin/libwinpthread-1.dll $WINDIR/bin

#cp $QTDIR/bin/mingwm10.dll $WINDIR/bin
#cp /usr/lib/gcc/i586-mingw32msvc/4.2.1-sjlj/*.dll $WINDIR/bin
#cp /usr/lib/gcc/i686-w64-mingw32/4.6/*.dll $WINDIR/bin
#cp $HOME/.wine/drive_c/mingw32/bin/*.dll $WINDIR/bin

echo Create Inno Setup installer...
cp  ${REPO}/qucs/contrib/innosetup/qucs.iss .
wine "$INNOSETUP" /cc qucs.iss
rm qucs.iss

echo Saving copy to ${HOME}/Downloads/qucs-$RELEASE-win32.exe
# TODO version is hardcoded on the Inno Setup file
cp ./Output/qucs-0.0.19-setup.exe ${HOME}/Downloads/qucs-$RELEASE-win32.exe
#rm -r release/Output


echo Done!
