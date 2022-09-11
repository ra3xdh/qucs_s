## Process this file with automake to produce Makefile.in
# 
# report/mesfet/pics/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2008 Stefan Jahn <stefan@lkcc.org>
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

EXTRA_DIST = $(PDFS)

PDFS = MESFET_Fig10.pdf MESFET_Fig17.pdf MESFET_Fig24.pdf MESFET_Fig6.pdf \
  MESFET_Fig11.pdf MESFET_Fig18.pdf MESFET_Fig25.pdf MESFET_Fig7.pdf \
  MESFET_Fig12.pdf MESFET_Fig1.pdf MESFET_Fig26.pdf MESFET_Fig8.pdf \
  MESFET_Fig13.pdf MESFET_Fig20.pdf MESFET_Fig2.pdf MESFET_Fig9.pdf \
  MESFET_Fig14.pdf MESFET_Fig21.pdf MESFET_Fig3.pdf MESFET_Fig15.pdf \
  MESFET_Fig22.pdf MESFET_Fig4.pdf MESFET_Fig16.pdf MESFET_Fig23.pdf \
  MESFET_Fig5.pdf

report: $(XPDF)

SUFFIXES = .fig .pdf

.fig.pdf:
	$(FIG2DEV) -L pdf $< > $@

CLEANFILES = $(XPDF) *~ *.bak *.orig *.rej
MAINTAINERCLEANFILES = Makefile.in
