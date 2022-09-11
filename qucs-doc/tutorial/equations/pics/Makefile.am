## Process this file with automake to produce Makefile.in
# 
# tutorial/equations/pics/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2007 Stefan Jahn <stefan@lkcc.org>
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

EXTRA_DIST = $(PDFS) $(PNGS)

PDFS = equ_fig10.pdf equ_fig17.pdf equ_fig21.pdf equ_fig3.pdf equ_fig8b.pdf \
  equ_fig11.pdf equ_fig18.pdf equ_fig22m.pdf equ_fig4.pdf equ_fig8.pdf \
  equ_fig12.pdf equ_fig19.pdf equ_fig24.pdf equ_fig5.pdf equ_fig9.pdf \
  equ_fig14.pdf equ_fig1a.pdf equ_fig2.pdf equ_fig7.pdf equ_fig16.pdf \
  equ_fig20.pdf equ_fig31.pdf equ_fig8a.pdf

PNGS = equ_fig25.png equ_fig27.png equ_fig29.png equ_fig26.png equ_fig28.png \
  equ_fig30.png

tutorial: $(XPDF)

SUFFIXES = .fig .pdf

.fig.pdf:
	$(FIG2DEV) -L pdf $< > $@

CLEANFILES = $(XPDF) *~ *.bak *.orig *.rej
MAINTAINERCLEANFILES = Makefile.in
