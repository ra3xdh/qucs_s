#!/bin/sh

GTEST_VERSION=${GTEST_VERSION-1.7.0}

cd $GTEST_PATH;
rm -rf gtest-$GTEST_VERSION.zip
wget https://googletest.googlecode.com/files/gtest-$GTEST_VERSION.zip;
unzip gtest-$GTEST_VERSION.zip;
rm gtest-$GTEST_VERSION.zip
rm -rf gtest/
mv gtest-$GTEST_VERSION/ gtest/
