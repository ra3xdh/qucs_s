## Process this file with automake to produce Makefile.in
#
# Makefile.am
#
# Automake input file.
#
# Copyright (C) 2014 Guilherme Brondani Torri <guitorri@gmail.com>
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this package; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
# Boston, MA 02110-1301, USA.
#

# FIXME: different paths used in configure.ac
AM_CPPFLAGS = \
    -I$(top_srcdir)/libs/gtest/include \
    -I$(top_srcdir)/libs/gtest/ \
    -I$(top_srcdir)/src \
    -I$(top_srcdir)/src/math \
    -I$(top_builddir)/src/components \
    -I$(top_srcdir)/src/components

AM_CXXFLAGS =

GTEST_TESTS = libqucsUnitTest

if USE_GTEST
GTEST_LIBQUCS = libqucsUnitTest
check_PROGRAMS = $(GTEST_LIBQUCS)
libqucsUnitTest_LDADD = $(top_builddir)/src/libqucsator.la
libqucsUnitTest_CPPFLAGS = $(AM_CPPFLAGS) @GTEST_CPPFLAGS@ \
                           -DGTEST_HAS_PTHREAD=0
libqucsUnitTest_SOURCES = testMain.cpp \
  test_libqucs.cpp \
	Fourier.cpp \
	Math.cpp \
	Matrix.cpp \
	Spline.cpp \
	Vector.cpp
else
libqucsUnitTest:
	echo "!#/bin/sh" > $@
	echo exit 77 >> $@
	chmod +x $@
endif

# TESTS -- Programs run automatically by "make check"
TESTS = $(GTEST_TESTS)
EXTRA_DIST = runqucsator.sh testDefine.h
CLEANFILES = $(GTEST_TESTS)
