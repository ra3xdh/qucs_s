## Process this file with automake to produce Makefile.in
#
# qucs-filter/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2004, 2006 Stefan Jahn <stefan@lkcc.org>
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

bin_PROGRAMS = qucsfilter

dist_man_MANS = qucsfilter.1

MOCHEADERS = qucsfilter.h helpdialog.h
MOCFILES = $(MOCHEADERS:.h=.moc.cpp)

qucsfilter_SOURCES = main.cpp qucsfilter.cpp helpdialog.cpp lc_filter.cpp \
  qf_poly.cpp qf_filter.cpp qf_cauer.cpp qrc_qucsfilter.cpp

qrc_qucsfilter.cpp: qucsfilter.qrc
	$(RCC) -o $@ $<

nodist_qucsfilter_SOURCES = $(MOCFILES)

AM_CPPFLAGS = $(X11_INCLUDES) $(QT_INCLUDES)
qucsfilter_LDFLAGS = $(X11_LDFLAGS) $(QT_LDFLAGS)
qucsfilter_LDADD = $(X11_LIBS) $(QT_LIBS)

noinst_HEADERS = $(MOCHEADERS) lc_filter.h qf_poly.h qf_filter.h qf_cauer.h \
  qf_matrix.h

SUFFIXES = .moc.cpp

.h.moc.cpp:
	$(MOC) -o $@ $<

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in *.moc.cpp
DISTCLEANFILES = *.moc.cpp
