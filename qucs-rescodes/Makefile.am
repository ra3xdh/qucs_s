## Process this file with automake to produce Makefile.in
#
# qucs-rescodes/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2012 Sudhakar M K <sudhakar.m.kumar@gamil.com>
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

bin_PROGRAMS = qucsrescodes

#dist_man_MANS = qucsattenuator.1

MOCHEADERS = helpdialog.h mycolorbox.h myresistancebox.h mywidget.h qresistor.h
MOCFILES = $(MOCHEADERS:.h=.moc.cpp)

qucsrescodes_SOURCES = \
  main.cpp \
	helpdialog.cpp \
  mycolorbox.cpp \
	myresistancebox.cpp \
	notation.cpp \
	qresistor.cpp \
	resistor.cpp \
  qrc_qucsrescodes.cpp

qrc_qucsrescodes.cpp: qucsrescodes.qrc
	$(RCC) -o $@ $<

nodist_qucsrescodes_SOURCES = $(MOCFILES)

INCLUDES = $(X11_INCLUDES) $(QT_INCLUDES)
qucsrescodes_LDFLAGS = $(X11_LDFLAGS) $(QT_LDFLAGS)
qucsrescodes_LDADD = $(X11_LIBS) $(QT_LIBS)

noinst_HEADERS = $(MOCHEADERS) classproto.h

%.moc.cpp: %.h
	$(MOC) -o $@ $<

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in *.moc.cpp
DISTCLEANFILES = *.moc.cpp
