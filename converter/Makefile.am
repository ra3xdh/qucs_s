## Process this file with automake to produce Makefile.in
#
# src/converter/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2004, 2005, 2006, 2007 Stefan Jahn <stefan@lkcc.org>
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

bin_PROGRAMS = qucsconv

qucsconv_LDADD = ../libqucsator.la ../math/libqucsmath.la

qucsconv_LDFLAGS = -Wl,-rpath,$(libdir)

AM_CPPFLAGS = -I$(top_builddir)/src \
              -I$(top_srcdir)/src \
              -I$(top_srcdir)/src/math

# needed to generate parse_spice.hpp from parse_spice.ypp
AM_YFLAGS = -d -t

# needed for ylwrap
scan_spice.cpp: LEX_OUTPUT_ROOT = lex.spice_
scan_vcd.cpp: LEX_OUTPUT_ROOT = lex.vcd_

qucsconv_SOURCES = qucsconv.cpp parse_spice.ypp scan_spice.lpp \
	check_spice.cpp qucs_producer.cpp parse_vcd.ypp scan_vcd.lpp \
	check_vcd.cpp csv_producer.cpp touchstone_producer.cpp \
	matlab_producer.cpp

noinst_HEADERS = check_spice.h qucs_producer.h check_vcd.h \
	csv_producer.h touchstone_producer.h matlab_producer.h

CLEANFILES = *~ *.orig *.rej *.output
MAINTAINERCLEANFILES = Makefile.in
