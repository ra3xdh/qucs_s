## Process this file with automake to produce Makefile.in
# 
# qucs-transcalc/examples/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2005 Stefan Jahn <stefan@lkcc.org>
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

EXAMPLES = hp.trc ustrip50fr4.trc ustrip_jansen.trc cstrip_kj_1.trc \
	cstrip_kj_2.trc

EXTRA_DIST = $(EXAMPLES)

# installation of transmission lines
linedatadir = ${prefix}/share/qucs/tline
linedata_DATA = $(EXTRA_DIST)

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in
