## Process this file with automake to produce Makefile.in
#
# src/components/digital/Makefile.am
#
# Automake input file for the component library.
#
# Copyright (C) 2005, 2008 Stefan Jahn <stefan@lkcc.org>
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

# the digital library rules

noinst_LTLIBRARIES = libdigital.la

libdigital_la_SOURCES = digital.cpp inverter.cpp nor.cpp or.cpp nand.cpp \
  and.cpp xnor.cpp xor.cpp digisource.cpp buffer.cpp

noinst_HEADERS = digital.h inverter.h nor.h or.h nand.h and.h xnor.h xor.h \
  digisource.h buffer.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/src/math \
  -I$(top_srcdir)/src/components

CLEANFILES = *~ *.orig *.rej

MAINTAINERCLEANFILES = Makefile.in
