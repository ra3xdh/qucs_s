## Process this file with automake to produce Makefile.in
#
# qucs/extsimkernels/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2015 Guilherme Brondani Torri <guitorri@gmail.com>
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

SUBDIRS=xspice

noinst_LIBRARIES = libextsimkernels.a

MOCHEADERS = \
		abstractspicekernel.h \
		customsimdialog.h \
		externsimdialog.h \
		ngspice.h \
		xyce.h \
		simsettingsdialog.h

MOCFILES = $(MOCHEADERS:.h=.moc.cpp)

libextsimkernels_a_SOURCES = \
		abstractspicekernel.cpp \
		customsimdialog.cpp \
		externsimdialog.cpp \
		ngspice.cpp \
		qucs2spice.cpp \
		spicecompat.cpp \
		xyce.cpp  \
		simsettingsdialog.cpp \
		verilogawriter.cpp \
		xspice_cmbuilder.cpp \
		codemodelgen.cpp

nodist_libextsimkernels_a_SOURCES = $(MOCFILES)

noinst_HEADERS = $(MOCHEADERS) \
		abstractspicekernel.h \
		customsimdialog.h \
		externsimdialog.h \
		ngspice.h \
		qucs2spice.h \
		spicecompat.h \
		xyce.h \
		simsettingsdialog.h \
		verilogawriter.h \
		xspice_cmbuilder.h \
		codemodelgen.h

AM_CPPFLAGS = $(X11_INCLUDES) $(QT_INCLUDES) -I$(top_srcdir)/qucs

SUFFIXES = .moc.cpp

.h.moc.cpp:
	$(MOC) -o $@ $<

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in *.moc.cpp
DISTCLEANFILES = *.moc.cpp

