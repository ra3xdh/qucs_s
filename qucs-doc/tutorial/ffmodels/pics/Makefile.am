## Process this file with automake to produce Makefile.in
# 
# tutorial/ffmodels/pics/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2006 Stefan Jahn <stefan@lkcc.org>
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

PDFS = fig10.pdf fig14.pdf fig18.pdf fig21.pdf fig25.pdf fig5.pdf fig9.pdf \
  fig11.pdf fig15.pdf fig19.pdf fig22.pdf fig2.pdf fig6.pdf fig12.pdf \
  fig16.pdf fig1.pdf fig23.pdf fig3.pdf fig7.pdf fig13.pdf fig17.pdf \
  fig20.pdf fig24.pdf fig4.pdf fig8.pdf

tutorial: $(XPDF)

SUFFIXES = .fig .pdf

.fig.pdf:
	$(FIG2DEV) -L pdf $< > $@

CLEANFILES = $(XPDF) *~ *.bak *.orig *.rej
MAINTAINERCLEANFILES = Makefile.in
