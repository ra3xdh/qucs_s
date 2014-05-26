#!/bin/bash

## file: package.sh
#
## This script create a tarball from the Qucs source repository.
#
## Run this script from: ~/git/qucs/
## Outputs in:           ~/git/qucs/release/


## Package contains sources for:
#  * Qucs, Qucsfilter... (Qt GUI components)
#  * Qucs-core (backend)
#  * Qucs-doc (only the generated PDF documents)
#  * Examples
#  * ASCO 0.4.9 (patched version)
#  * ADMS 2.3.2+ (git submodule, might not be the latest stable release)


## Note:
# * ADMS is optional. The needed sources are already generated.
# * ADMS is needed to use the Verilog-A dynamic loader.
# * ASCO is also optional.


##
# Relevant options for packagers:
# * --disable-asco , skip build of shipped ASCO [default no]
# * --disable-adms , skip build of shipped ADMS [default no]



# Set Release tag
# TODO get it from git tags or VERSION file

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

if [ -d release ]
then
	echo Directory release exists, removing ...
	rm -rf release
fi

echo Exporting git tree recursively...

# Recursive to clone also ADMS
# flatten subdiretories
git clone --recursive ./ release/
mv release/examples release/qucs/examples/examples
#mv release/qucs-core/deps/adms/ release/qucs/adms/
mv release/qucs-core release/qucs/

# TODO remove .gitignore, .travis, ...

# tag directory
mv release/qucs release/qucs-$RELEASE
rm -rf release/.git

# Build documentation in source dir
cd $REPO/release/qucs-doc
./autogen.sh

cd tutorial
make tutorial
make book
cd ..

cd report
make report
make book
cd ..

cd technical
make technical
ps2pdf technical.ps
cd ..


#including pdf versions of qucs-doc in target dir
cd $REPO/release/qucs-doc
DOC_SUBDIRS="report technical tutorial"
for DOC_SUBDIR in ${DOC_SUBDIRS} ; do
	cd $DOC_SUBDIR
	mkdir -p ../../qucs-$RELEASE/qucs-doc/$DOC_SUBDIR
	find -name "*.pdf" |grep -v pics| xargs cp -t ../../qucs-$RELEASE/qucs-doc/$DOC_SUBDIR
	cd ..
done
# done with qucs-doc
cd $REPO/release
rm -rf qucs-doc


# cleanup and prepare target dir
cd $REPO/release/qucs-$RELEASE
cd qucs-doc
./autogen.sh
make distclean
cd ..

cd examples
./autogen.sh
make distclean
cd ..


# Include the asco source
cd $REPO/release/qucs-$RELEASE
if [ -f ~/Downloads/ASCO-0.4.9.tar.gz ]
then
	cp ~/Downloads/ASCO-0.4.9.tar.gz .
else
	wget https://downloads.sourceforge.net/project/asco/asco/0.4.9/ASCO-0.4.9.tar.gz -P ~/Downloads/
fi

# Extract ASCO tarball, patch and prepare build system to redistribute
tar -zxvf ASCO-0.4.9.tar.gz
rm ASCO-0.4.9.tar.gz
mv ASCO-0.4.9 asco
cd asco
patch -p1 < ../../../qucs/contrib/patch_asco_unbuffer.diff
#https://sourceforge.net/p/qucs/bugs/143/
patch -p1 < ../../../qucs/contrib/asco-nmlatest.c.patch
touch NEWS
tar -zxvf Autotools.tar.gz
patch -p1 < ../../../qucs/contrib/patch_asco_osx.diff
./autogen.sh
automake --add-missing
aclocal
cd ..

# set configure.ac into RELEASE mode
sed -i '' 's/# RELEASE="yes"/RELEASE="yes"/g' configure.ac

# bootstrap qucs, qucs-edit, qucs-filter...
cd $REPO/release/qucs-$RELEASE
./autogen.sh
make distclean
rm -rf autom4te.cache
#libtoolize
case `uname` in
  *Darwin*) LIBTOOLIZE=glibtoolize ;;
  *)        LIBTOOLIZE=libtoolize ;;
esac
$LIBTOOLIZE


echo Bootstrap qucs-core...
# bootstrap qucs-core
cd qucs-core
./bootstrap.sh
./configure --enable-maintainer-mode
make
./configure
make distclean # Verilog-A sources are kept ??
rm -rf autom4te.cache
cd ..

echo Bootstrap ADMS...
# bootstrap amds
cd qucs-core/deps/adms
./bootstrap.sh
./configure --enable-maintainer-mode
make dist
rm -rf autom4te.cache
rm -rf adms-*.tar.gz
cd ../../..

echo Creating source archive...
cd $REPO/release/
tar -zcvhf qucs-$RELEASE.tar.gz qucs-$RELEASE
rm -rf qucs-$RELEASE
tar -zxvf qucs-$RELEASE.tar.gz #make the symbolic links actual files


