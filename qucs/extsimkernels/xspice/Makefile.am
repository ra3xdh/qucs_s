## Process this file with automake to produce Makefile.in
# 
# qucs/extsimkernels/xspice/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2015 Vadim Kuznetsov <ra3xdh@gmail.com>
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

RULES_MK = cmlib.linux.rules.mk cmlib.mingw32.rules.mk

EXTRA_DIST = $(RULES_MK)

# installation of transmission lines
xspicerulesdir = ${prefix}/share/qucs/xspice_cmlib
xspicerules_DATA = $(EXTRA_DIST)

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in
