## Process this file with automake to produce Makefile.in
#
# src/components/microstrip/Makefile.am
#
# Automake input file for the component library.
#
# Copyright (C) 2004, 2005, 2009 Stefan Jahn <stefan@lkcc.org>
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

# the microstrip library rules

noinst_LTLIBRARIES = libmicrostrip.la

libmicrostrip_la_SOURCES = substrate.cpp msline.cpp mscorner.cpp msmbend.cpp   \
	msstep.cpp msopen.cpp msgap.cpp mscoupled.cpp mslange.cpp mstee.cpp mscross.cpp   \
	msvia.cpp cpwline.cpp cpwopen.cpp cpwshort.cpp cpwgap.cpp cpwstep.cpp \
	bondwire.cpp msrstub.cpp spiralinductor.cpp circularloop.cpp

noinst_HEADERS = substrate.h msline.h mscorner.h msmbend.h msstep.h msopen.h \
	msgap.h mscoupled.h mslange.h mstee.h mscross.h msvia.h cpwline.h cpwopen.h    \
	cpwshort.h cpwgap.h cpwstep.h bondwire.h msrstub.h spiralinductor.h circularloop.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/src/math \
  -I$(top_srcdir)/src/components -I$(top_srcdir)/src/components/devices

CLEANFILES = *~ *.orig *.rej

MAINTAINERCLEANFILES = Makefile.in
