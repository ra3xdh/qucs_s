## Process this file with automake to produce Makefile.in
# 
# tutorial/getstarted/pics/Makefile.am
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

PDFS = igain_1.pdf igain_3.pdf outputbjt_1.pdf igain_2.pdf igain_4.pdf \
  outputbjt_2.pdf bjtac_1.pdf bjtac_2.pdf bjtacft_1.pdf bjtacft_2.pdf \
  igain_ib.pdf bjttr_1.pdf bjttr_2.pdf bjtsp_1.pdf bjtsp_2.pdf bjtsp_3.pdf \
  bpac_1.pdf bpac_2.pdf bpsp_1.pdf bpsp_2.pdf igain_beta_1.pdf

PNGS = finalhighpassrc.png highpassrc.png highpasssimblock.png \
  simblockconfig.png \
  \
  appsettings.png divider_2.png helpdialog.png componentlibrary.png \
  divider_3.png componentprops_1.png divider_4.png componentprops.png \
  divider_5.png markerdialog.png componentstab.png divider_6.png \
  newproject.png contenttab.png divider_save.png nodelabel.png \
  diagramdialog_1.png empty_dpl.png propdirect.png diagramdialog_divided.png \
  eqndialog.png diagramdialog.png fileprops.png start.png divided_dpl.png \
  final_divider.png divider_1.png diagramdialog_sets.png besselfilter.png


tutorial: $(XPDF)

SUFFIXES = .fig .pdf

.fig.pdf:
	$(FIG2DEV) -L pdf $< > $@

CLEANFILES = $(XPDF) *~ *.bak *.orig *.rej
MAINTAINERCLEANFILES = Makefile.in
