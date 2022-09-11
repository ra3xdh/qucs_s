## Process this file with automake to produce Makefile.in
# 
# tutorial/opamp/pics/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2006, 2007 Stefan Jahn <stefan@lkcc.org>
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

PDFS = dwr1.pdf fig17_dpl.pdf fig24_dpl.pdf fig31_dpl.pdf fig3_sch.pdf \
  fig10_diag.pdf fig18a_sch.pdf fig25_sch.pdf fig31_sch.pdf fig40_sch.pdf \
  fig10_sch.pdf fig18b_sch.pdf fig26_sch.pdf fig33_dpl.pdf fig41_dpl.pdf \
  fig11_sch.pdf fig1_dpl.pdf fig27_dpl.pdf fig34_dpl.pdf fig8_sch.pdf \
  fig12_sch.pdf fig1_sch.pdf fig28_sch.pdf fig34_sch.pdf fig9_sch.pdf \
  fig13_sch.pdf fig20_dpl.pdf fig29_dpl.pdf fig37_dpl.pdf fig_bdiag.pdf \
  fig14_dpl.pdf fig20_sch.pdf fig29_sch.pdf fig38_sch.pdf fig15_sch.pdf \
  fig22_sch.pdf fig2_dpl.pdf fig39_dpl.pdf fig16_sch.pdf fig23_sch.pdf \
  fig2_sch.pdf fig3_dpl.pdf \
  \
  fig44_sch.pdf opamp_fig42.pdf opamp_fig48.pdf opamp_fig49.pdf \
  opamp_fig55.pdf opamp_fig57.pdf opamp_fig59.pdf opamp_fig56.pdf \
  opamp_fig58.pdf  

tutorial: $(XPDF)

SUFFIXES = .fig .pdf

.fig.pdf:
	$(FIG2DEV) -L pdf $< > $@

CLEANFILES = $(XPDF) *~ *.bak *.orig *.rej
MAINTAINERCLEANFILES = Makefile.in
