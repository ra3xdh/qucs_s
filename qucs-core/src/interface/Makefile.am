## Process this file with automake to produce Makefile.in
#
# src/interface/Makefile.am
#
# Automake input file for the component library.
#
# Copyright (C) 2003-2011 Stefan Jahn <stefan@lkcc.org>
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

# the interface library rules

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/src/math \
	-I$(top_srcdir)/src/components -I$(top_builddir)/src/components

pkginclude_HEADERS = qucs_interface.h

noinst_HEADERS = e_trsolver.h

noinst_LTLIBRARIES = libinterface.la


libinterface_la_SOURCES = qucs_interface.cpp e_trsolver.cpp

CLEANFILES = *~ *.orig *.rej

MAINTAINERCLEANFILES = Makefile.in

