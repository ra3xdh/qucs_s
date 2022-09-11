## Process this file with automake to produce Makefile.in
# 
# tutorial/spicetoqucs/pics/Makefile.am
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

EXTRA_DIST = $(PDFS)

PDFS = stoq_fig13.pdf stoq_fig19.pdf stoq_fig2.pdf stoq_fig14.pdf \
  stoq_fig20.pdf stoq_fig4.pdf stoq_fig17.pdf stoq_fig22.pdf stoq_fig5.pdf \
  stoq_fig10.pdf stoq_fig18.pdf stoq_fig23.pdf stoq_fig9.pdf

tutorial: $(XPDF)

SUFFIXES = .fig .pdf

.fig.pdf:
	$(FIG2DEV) -L pdf $< > $@

CLEANFILES = $(XPDF) *~ *.bak *.orig *.rej
MAINTAINERCLEANFILES = Makefile.in
