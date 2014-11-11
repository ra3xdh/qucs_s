## Process this file with automake to produce Makefile.in
#
# qucs/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2004-2011 Stefan Jahn <stefan@lkcc.org>
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

SUBDIRS = components diagrams paintings dialogs octave

if COND_WIN32
bin_SCRIPTS = qucsdigi.bat qucsveri.bat qucsdigilib.bat
else
bin_SCRIPTS = qucsdigi qucsveri qucsdigilib
endif

bin_PROGRAMS = qucs

MOCHEADERS = qucs.h schematic.h textdoc.h octave_window.h messagedock.h
MOCFILES = $(MOCHEADERS:.h=.moc.cpp)

qucs_SOURCES = node.cpp element.cpp qucsdoc.cpp wire.cpp mouseactions.cpp   \
  qucs.cpp main.cpp wirelabel.cpp qucs_init.cpp qucs_actions.cpp            \
  viewpainter.cpp mnemo.cpp schematic.cpp schematic_element.cpp textdoc.cpp \
  schematic_file.cpp syntax.cpp module.cpp octave_window.cpp qrc_qucs.cpp   \
  messagedock.cpp misc.cpp

qrc_qucs.cpp: qucs.qrc
	$(RCC) -o $@ $<

nodist_qucs_SOURCES = $(MOCFILES)

AM_CPPFLAGS = $(X11_INCLUDES) $(QT_INCLUDES) -I$(top_srcdir)/qucs-lib
qucs_LDFLAGS = $(X11_LDFLAGS) $(QT_LDFLAGS)
qucs_LDADD = components/libcomponents.a diagrams/libdiagrams.a \
  paintings/libpaintings.a dialogs/libdialogs.a \
  $(X11_LIBS) $(QT_LIBS)

noinst_HEADERS = $(MOCHEADERS) main.h wire.h qucsdoc.h element.h node.h \
  wirelabel.h viewpainter.h mnemo.h mouseactions.h syntax.h module.h misc.h

hicolor16dir = ${prefix}/share/icons/hicolor/16x16/apps
hicolor16_DATA =  bitmaps/hicolor/16x16/apps/qucs.png
hicolor22dir = ${prefix}/share/icons/hicolor/22x22/apps
hicolor22_DATA =bitmaps/hicolor/22x22/apps/qucs.png
hicolor32dir = ${prefix}/share/icons/hicolor/32x32/apps
hicolor32_DATA =bitmaps/hicolor/32x32/apps/qucs.png
hicolor48dir = ${prefix}/share/icons/hicolor/48x48/apps
hicolor48_DATA =bitmaps/hicolor/48x48/apps/qucs.png
hicolor64dir = ${prefix}/share/icons/hicolor/64x64/apps
hicolor64_DATA =bitmaps/hicolor/64x64/apps/qucs.png
hicolor128dir = ${prefix}/share/icons/hicolor/128x128/apps
hicolor128_DATA =bitmaps/hicolor/128x128/apps/qucs.png
hicolor256dir = ${prefix}/share/icons/hicolor/256x256/apps
hicolor256_DATA =bitmaps/hicolor/256x256/apps/qucs.png
hicolor512dir = ${prefix}/share/icons/hicolor/512x512/apps
hicolor512_DATA =bitmaps/hicolor/512x512/apps/qucs.png
hicolorscdir = ${prefix}/share/icons/hicolor/scalable/apps
hicolorsc_DATA =bitmaps/hicolor/scalable/apps/qucs.svg


desktopdir = ${prefix}/share/applications
desktop_DATA = qucs.desktop


EXTRA_DIST = \
	qucsdigi qucsdigilib qucsdigi.bat qucsdigilib.bat qucsveri qucsveri.bat \
	$(hicolor16_DATA) $(hicolor22_DATA) $(hicolor32_DATA) $(hicolor48_DATA) \
	$(hicolor64_DATA) $(hicolor128_DATA) $(hicolor256_DATA) $(hicolor512_DATA) \
	$(hicolorsc_DATA) $(desktop_DATA)


.h.moc.cpp:
	$(MOC) -o $@ $<

CLEANFILES = *~ *.rej *.orig
MAINTAINERCLEANFILES = Makefile.in *.moc.cpp *.qm
DISTCLEANFILES = *.moc.cpp *.qm
