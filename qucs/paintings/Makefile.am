## Process this file with automake to produce Makefile.in
# 
# qucs/paintings/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2004 Stefan Jahn <stefan@lkcc.org>
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

noinst_LIBRARIES = libpaintings.a

MOCHEADERS = arrowdialog.h graphictextdialog.h filldialog.h id_dialog.h
MOCFILES = $(MOCHEADERS:.h=.moc.cpp)

libpaintings_a_SOURCES = rectangle.cpp painting.cpp arrow.cpp  \
  graphictextdialog.cpp graphictext.cpp graphicline.cpp ellipse.cpp           \
  arrowdialog.cpp filldialog.cpp portsymbol.cpp ellipsearc.cpp id_text.cpp    \
  id_dialog.cpp

nodist_libpaintings_a_SOURCES = $(MOCFILES)

noinst_HEADERS = $(MOCHEADERS) arrow.h ellipse.h graphicline.h graphictext.h  \
  painting.h paintings.h rectangle.h portsymbol.h ellipsearc.h id_text.h

AM_CPPFLAGS = $(X11_INCLUDES) $(QT_INCLUDES) -I$(top_srcdir)/qucs

SUFFIXES = .moc.cpp

.h.moc.cpp:
	$(MOC) -o $@ $<

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in *.moc.cpp
DISTCLEANFILES = *.moc.cpp
