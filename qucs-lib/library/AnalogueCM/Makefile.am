## Process this file with automake to produce Makefile.in
# 
# qucs-lib/library/AnalogueCM/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2016 Mike Brinson mbrin72043@yahoo.co.uk
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


CMLIBS = etd.mod etd.ifs FNXSPICECM.mod FNXSPICECM.ifs RFind.mod RFind.ifs

EXTRA_DIST = $(CMLIBS)

# installation of component libraries
librarydir = ${prefix}/share/qucs/library/AnalogueCM
library_DATA = $(CMLIBS)

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in
