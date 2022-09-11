## Process this file with automake to produce Makefile.in
# 
# tutorial/timer555/pics/Makefile.am
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

PDFS = 555_timer_fig10.pdf 555_timer_fig19.pdf 555_timer_fig5.pdf \
  555_timer_fig11.pdf 555_timer_fig1.pdf  555_timer_fig6.pdf \
  555_timer_fig12.pdf 555_timer_fig20.pdf 555_timer_fig7.pdf \
  555_timer_fig13.pdf 555_timer_fig21.pdf 555_timer_fig8.pdf \
  555_timer_fig14.pdf 555_timer_fig22.pdf 555_timer_fig9.pdf \
  555_timer_fig15.pdf 555_timer_fig23.pdf 555_timer_fig16.pdf \
  555_timer_fig2.pdf 555_timer_fig17.pdf  555_timer_fig3.pdf \
  555_timer_fig18.pdf 555_timer_fig4.pdf

tutorial: $(XPDF)

SUFFIXES = .fig .pdf

.fig.pdf:
	$(FIG2DEV) -L pdf $< > $@

CLEANFILES = $(XPDF) *~ *.bak *.orig *.rej
MAINTAINERCLEANFILES = Makefile.in
