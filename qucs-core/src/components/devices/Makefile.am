## Process this file with automake to produce Makefile.in
#
# src/components/devices/Makefile.am
#
# Automake input file for the component library.
#
# Copyright (C) 2004, 2007, 2008, 2011 Stefan Jahn <stefan@lkcc.org>
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

# the non-linear devices library rules

noinst_LTLIBRARIES = libdevices.la

libdevices_la_SOURCES = device.cpp diode.cpp jfet.cpp bjt.cpp mosfet.cpp \
  eqndefined.cpp diac.cpp thyristor.cpp triac.cpp tunneldiode.cpp

pkginclude_HEADERS = device.h

noinst_HEADERS = device.h diode.h jfet.h bjt.h mosfet.h eqndefined.h diac.h \
  thyristor.h triac.h tunneldiode.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/src/math \
  -I$(top_srcdir)/src/components

CLEANFILES = *~ *.orig *.rej

MAINTAINERCLEANFILES = Makefile.in
