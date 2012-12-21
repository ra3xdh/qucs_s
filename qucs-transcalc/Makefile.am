## Process this file with automake to produce Makefile.in
# 
# qucs-transcalc/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2005, 2006 Stefan Jahn <stefan@lkcc.org>
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

bin_PROGRAMS = qucstrans

SUBDIRS = bitmaps examples

dist_man_MANS = qucstrans.1

MOCHEADERS = qucstrans.h helpdialog.h optionsdialog.h
MOCFILES = $(MOCHEADERS:.h=.moc.cpp)

qucstrans_SOURCES = main.cpp qucstrans.cpp helpdialog.cpp optionsdialog.cpp

qucstrans_LDADD = libtranscalc.a $(X11_LIBS) $(QT_LIBS)

nodist_qucstrans_SOURCES = $(MOCFILES)

AM_CPPFLAGS = $(X11_INCLUDES) $(QT_INCLUDES)
qucstrans_LDFLAGS = $(X11_LDFLAGS) $(QT_LDFLAGS)

noinst_HEADERS = $(MOCHEADERS) transline.h units.h microstrip.h coax.h \
	rectwaveguide.h c_microstrip.h coplanar.h

noinst_LIBRARIES = libtranscalc.a

libtranscalc_a_SOURCES = microstrip.cpp transline.cpp coax.cpp \
	rectwaveguide.cpp c_microstrip.cpp coplanar.cpp

SUFFIXES = .moc.cpp

.h.moc.cpp:
	$(MOC) -o $@ $<

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in *.moc.cpp
DISTCLEANFILES = *.moc.cpp
