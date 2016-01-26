#!/bin/sh

# Run doxygen for qucs
# TODO create doxygen files for the other qucs-tools

VERSION=$(cat ../../VERSION)
sed "s/0\.\0\.18/${VERSION}/g" doxygen.cfg  > temp
mv temp doxygen.cfg
doxygen doxygen.cfg
