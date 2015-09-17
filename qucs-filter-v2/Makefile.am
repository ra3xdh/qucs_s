## Process this file with automake to produce Makefile.in
#
# qucs-filter-v2/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2009 Stefan Jahn <stefan@lkcc.org>
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

noinst_PROGRAMS = qucsfilter

MOCHEADERS = qf_dialog.h qf_box.h
MOCFILES = $(MOCHEADERS:.h=.moc.cpp)

qucsfilter_SOURCES = qf_api.cpp\
       qf_bessel.cpp\
       qf_blinch.cpp\
       qf_butcheb.cpp\
       qf_cauer.cpp\
       qf_common.cpp\
       qf_comp.cpp\
       qf_filter.cpp\
       qf_icheb.cpp\
       qf_legendre.cpp\
       qf_matrix.cpp\
       qf_optimize.cpp\
       qf_poly.cpp\
       qf_qr.cpp\
       qf_tform.cpp\
       qf_zigzag.cpp\
       qf_main.cpp\
       qf_settings.cpp\
       qf_dialog.cpp\
       qf_box.cpp\
       qrc_qucsfilter.cpp


EXTRA_DIST = qucsfilter.qrc

qrc_qucsfilter.cpp: qucsfilter.qrc
	$(RCC) -o $@ $<

nodist_qucsfilter_SOURCES = $(MOCFILES)

AM_CPPFLAGS = $(X11_INCLUDES) $(QT_INCLUDES)
qucsfilter_LDFLAGS = $(X11_LDFLAGS) $(QT_LDFLAGS)
qucsfilter_LDADD = $(X11_LIBS) $(QT_LIBS)

noinst_HEADERS = $(MOCHEADERS) \
       qf_api.h\
       qf_bessel.h\
       qf_blinch.h\
       qf_butcheb.h\
       qf_capacity.h\
       qf_cauer.h\
       qf_common.h\
       qf_comp.h\
       qf_filter.h\
       qf_icheb.h\
       qf_legendre.h\
       qf_matrix.h\
       qf_poly.h\
       qf_qr.h\
       qf_tform.h\
       qf_transform.h\
       qf_settings.h\
       qf_zigzag.h

SUFFIXES = .moc.cpp

.h.moc.cpp:
	$(MOC) -o $@ $<

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in *.moc.cpp
DISTCLEANFILES = *.moc.cpp
