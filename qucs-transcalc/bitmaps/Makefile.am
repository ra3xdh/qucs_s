## Process this file with automake to produce Makefile.in
# 
# qucs-transcalc/bitmaps/Makefile.am
#
# Automake input file.
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

XPMS = 

PNGS = c_microstrip.png coax.png microstrip.png stripline.png rectwaveguide.png cpw.png \
	cpw_back.png \
	big.qucs.xpm

ICONS = 

EXTRA_DIST = $(XPMS) $(PNGS) $(ICONS)

# installation of pictures
picdatadir = ${prefix}/share/qucs/bitmaps
picdata_DATA = $(EXTRA_DIST)

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in
