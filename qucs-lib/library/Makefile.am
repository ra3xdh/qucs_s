## Process this file with automake to produce Makefile.in
# 
# qucs-lib/library/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2004, 2005, 2008 Stefan Jahn <stefan@lkcc.org>
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

SUBDIRS =  AnalogueCM


COMPLIBS = Transistors.lib Substrates.lib Diodes.lib MOSFETs.lib OpAmps.lib \
           Bridges.lib LEDs.lib JFETs.lib Z-Diodes.lib NMOSFETs.lib \
	   PMOSFETs.lib Ideal.lib Regulators.lib Varistors.lib Cores.lib Transformers.lib \
	   SpiceOpamp.lib \
	   AnalogueCM.lib Xanalogue.lib qucs.blacklist ngspice.blacklist xyce.blacklist
	       
	       

EXTRA_DIST = $(COMPLIBS) 


# installation of component libraries
librarydir = ${prefix}/share/qucs/library
library_DATA = $(EXTRA_DIST)

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in
