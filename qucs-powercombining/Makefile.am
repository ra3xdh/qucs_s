## Process this file with automake to produce Makefile.in
#
# qucs-powercombining/Makefile.am
#
# Automake input file.
#
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

bin_PROGRAMS = qucspowercombining

MOCHEADERS = qucspowercombiningtool.h
MOCFILES = $(MOCHEADERS:.h=.moc.cpp)

dist_man_MANS = qucspowercombining.1

include ../common.mk

qucspowercombining_SOURCES = main.cpp qucspowercombiningtool.cpp qrc_qucspowercombining.cpp

EXTRA_DIST = qucspowercombining.qrc
qrc_qucspowercombining.cpp: qucspowercombining.qrc
	$(RCC) -o $@ $<

nodist_qucspowercombining_SOURCES = $(MOCFILES)

AM_CPPFLAGS = $(X11_INCLUDES) $(QT_CFLAGS)
qucspowercombining_LDFLAGS = $(X11_LDFLAGS) $(QT_LIBS)
qucspowercombining_LDADD = $(X11_LIBS) $(QT_LIBS)

noinst_HEADERS = $(MOCHEADERS)

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in *.moc.cpp
DISTCLEANFILES = *.moc.cpp
