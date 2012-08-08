## Process this file with automake to produce Makefile.in
# 
# qucs-help/docs/en/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2004, 2005, 2006, 2011 Stefan Jahn <stefan@lkcc.org>
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

HTML = index.html short.html start.html mathfunc.html subcircuit.html       \
  programs.html internal.html characters.html matching.html start_digi.html \
  start_opt.html octave.html
PIC  = qucsmain.png paste.png wire.png select.png contab.png subcircuit.png \
  qucsdigi.png optimization1.png optimization2.png optimization3.png        \
  optimization4.png optimization5.png optimization6.png optimization7.png   \
  optimization8.png

EXTRA_DIST = $(HTML) $(PIC)

# installation of documentation
docdatadir = ${prefix}/share/qucs/docs/en
docdata_DATA = $(EXTRA_DIST)

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in
