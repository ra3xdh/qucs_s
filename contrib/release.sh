#!/bin/bash
cd ../..

if [ $# -ne 0 ]
then
  RELEASE=$1
else
  RELEASE=$(date +"%y%m%d")
  RELEASE="0.0.17."${RELEASE:0:6}
fi
echo Building release: $RELEASE

if [ -d release ]
then
	echo Directory release exists, removing ...
	rm -rf release
fi

echo exporting git tree...

git clone ./ release/
mv release/qucs-core release/qucs/
mv release/qucs release/qucs-$RELEASE
rm -rf release/.git


#Include the asco archive
cd release/qucs-$RELEASE
wget https://downloads.sourceforge.net/project/asco/asco/0.4.8/ASCO-0.4.8.tar.gz
tar -zxvf ASCO-0.4.8.tar.gz
rm ASCO-0.4.8.tar.gz
mv ASCO-0.4.8 asco
cd asco
touch NEWS
tar -zxvf Autotools.tar.gz
./autogen.sh
automake --add-missing
aclocal
cd ..

###include the freehdl archive
#wget http://freehdl.seul.org/~enaroska/freehdl-0.0.7.tar.gz
#tar -zxvf freehdl-0.0.7.tar.gz
#rm freehdl-0.0.7.tar.gz
#mv freehdl-0.0.7 freehdl

#include verilog in the archive
#wget ftp://icarus.com/pub/eda/verilog/v0.9/verilog-0.9.6.tar.gz
#tar -zxvf verilog-0.9.6.tar.gz
#rm verilog-0.9.6.tar.gz
#mv verilog-0.9.6 verilog


sed -i 's/# AC_CONFIG_SUBDIRS(qucs-core)/AC_CONFIG_SUBDIRS(qucs-core)/g' configure.ac
sed -i 's/# RELEASEDIRS="qucs-core"/RELEASEDIRS="qucs-core"/g' configure.ac
#sed -i 's/# AC_CONFIG_SUBDIRS(freehdl)/AC_CONFIG_SUBDIRS(freehdl)/g' configure.ac
#sed -i 's/# RELEASEDIRS="$RELEASEDIRS freehdl"/RELEASEDIRS="$RELEASEDIRS freehdl"/g' configure.ac
#sed -i 's/# AC_CONFIG_SUBDIRS(verilog)/AC_CONFIG_SUBDIRS(verilog)/g' configure.ac
#sed -i 's/# RELEASEDIRS="$RELEASEDIRS verilog"/RELEASEDIRS="$RELEASEDIRS verilog"/g' configure.ac
sed -i 's/# AC_CONFIG_SUBDIRS(asco)/AC_CONFIG_SUBDIRS(asco)/g' configure.ac
sed -i 's/# RELEASEDIRS="$RELEASEDIRS asco"/RELEASEDIRS="$RELEASEDIRS asco"/g' configure.ac

./autogen.sh
make distclean
rm -rf autom4te.cache


cd qucs-core
./autogen.sh
make
./configure
make distclean
rm -rf autom4te.cache
cd ..
cd ..

echo creating source archive...

tar -zcvf qucs-$RELEASE.tar.gz qucs-$RELEASE

DISTS="precise quantal"

cp qucs-$RELEASE.tar.gz qucs_$RELEASE.orig.tar.gz

cd qucs-$RELEASE
COUNT=-0 #last version number in repository
for DIST in ${DISTS} ; do
	COUNT=$(($COUNT-1))
	dch -D $DIST -m -v $RELEASE$COUNT -b
	debuild -S -k8AD5905E
	./configure 
done


echo "Building mingw32"
make clean
INNOSETUP="/home/franss/.wine/drive_c/Program Files (x86)/Inno Setup 5/Compil32.exe"
cd ..
WINDIR=$PWD/qucs-win32-bin
cd qucs-$RELEASE
export QTDIR=~/.wine/drive_c/Qt/4.8.4/ 
./mingw-configure --prefix=$WINDIR
sed -i 's/-fno-rtti/ /g' qucs-filter-v2/Makefile
cp ../../qucs/qucs/qucsdigi.bat qucs #is deleted by the linux build for some reason
make
make install

cp contrib/innosetup/gpl.rtf $WINDIR
cp -r contrib/innosetup/misc $WINDIR
#wget http://bleyer.org/icarus/iverilog-0.9.6_setup.exe #contains trojan?
wget http://bleyer.org/icarus/iverilog-0.9.5_setup.exe
mv iverilog-0.9.5_setup.exe $WINDIR
wget https://downloads.sourceforge.net/project/qucs/qucs-binary/0.0.16/freehdl-0.0.8-setup.exe
mv freehdl-0.0.8-setup.exe $WINDIR
wget https://downloads.sourceforge.net/project/qucs/qucs-binary/0.0.15/mingw32-g%2B%2B-0.0.2-setup.exe
mv mingw32-g++-0.0.2-setup.exe $WINDIR

cp $QTDIR/bin/mingwm10.dll $WINDIR/bin
cp $QTDIR/bin/Qt3Support4.dll  $WINDIR/bin
cp $QTDIR/bin/QtCore4.dll  $WINDIR/bin
cp $QTDIR/bin/QtGui4.dll  $WINDIR/bin
cp $QTDIR/bin/QtNetwork4.dll  $WINDIR/bin
cp $QTDIR/bin/QtXml4.dll  $WINDIR/bin
cp /usr/lib/gcc/i586-mingw32msvc/4.2.1-sjlj/*.dll $WINDIR/bin

wine "$INNOSETUP" /cc contrib/innosetup/qucs.iss
mv contrib/innosetup/Output/qucs-0.0.17-setup.exe ../qucs-$RELEASE.exe












