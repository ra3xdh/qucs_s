## Process this file with automake to produce Makefile.in
# 
# tutorial/digital/pics/Makefile.am
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

PDFS = comb1s.pdf g2bitsch.pdf mux21.pdf prim1tt.pdf regt.pdf dffl.pdf \
  g4bit.pdf mux21sch.pdf prim2sch.pdf t1ex2sch.pdf dfflsch.pdf g4bitsch.pdf \
  mux2to1.pdf prim2tt.pdf tcomb1.pdf dtut1.pdf lone.pdf mux2to1sch.pdf \
  quadmux.pdf tcomb1s.pdf fadder2bit.pdf lonesch.pdf mux4to1.pdf \
  quadmuxsch.pdf tdex1.pdf fadder2bitsch.pdf lzero.pdf mux4to1sch.pdf \
  reg4bit.pdf testrtl.pdf g2bit.pdf lzerosch.pdf prim1sch.pdf reg4bitsch.pdf \
  \
  digital_ud1_fa.pdf dig_ud1_ex1_dpl.pdf dig_ud1_ex2_dpl.pdf \
  dig_ud1_ex3_dpl.pdf dig_ud1_sex1_sch.pdf dig_ud1_sex2_sch.pdf \
  dig_ud1_sex3_sch.pdf dig_ud1_sex4_sch.pdf dig_ud1_sex5_sch.pdf \
  dig_ud1_sex6_sch.pdf mult_dpl.pdf mult_sch.pdf \
  \
  dig_ud2_fig10.pdf dig_ud2_fig4.pdf dig_ud2_fig7.pdf dig_ud2_fig11.pdf \
  dig_ud2_fig2.pdf dig_ud2_fig5.pdf dig_ud2_fig8.pdf dig_ud2_fig1.pdf \
  dig_ud2_fig3.pdf dig_ud2_fig6.pdf dig_ud2_fig9.pdf

tutorial: $(XPDF)

SUFFIXES = .fig .pdf

.fig.pdf:
	$(FIG2DEV) -L pdf $< > $@

CLEANFILES = $(XPDF) *~ *.bak *.orig *.rej
MAINTAINERCLEANFILES = Makefile.in
