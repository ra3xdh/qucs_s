## Process this file with automake to produce Makefile.in
# 
# qucs-lib/library/XyceDigital/Makefile.am
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


CMLIBS = AND2.sym AND4.sym NAND2.sym NAND4.sym BUF.sym INV.sym \
         LOGIC0.sym LOGIC1.sym OR2.sym OR4.sym NOR2.sym NOR4.sym \
         XOR.sym NXOR.sym MUX2TO1.sym MUX4TO1.sym MUX8TO1.sym \
         GEN0OR1X4.sym GEN0OR1X8.sym FADDER.sym  ANDOR4X2.sym \
         A2DBRIDGE.sym A2DBRIDGEX2.sym A2DBRIDGEX4.sym A2DBRIDGEX8.sym \
         D2ABRIDGE.sym D2ABRIDGEX2.sym D2ABRIDGEX4.sym D2ABRIDGEX8.sym \
         PATGENX1.sym PATGENX2.sym PATGENX4.sym PATGENX8.sym PATGENX32.sym PATGENX128.sym


EXTRA_DIST = $(CMLIBS)

# installation of component libraries
librarydir = ${prefix}/share/qucs/library/XyceDigital
library_DATA = $(CMLIBS)

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in
