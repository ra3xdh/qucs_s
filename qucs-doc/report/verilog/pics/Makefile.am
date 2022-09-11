## Process this file with automake to produce Makefile.in
# 
# report/verilog/pics/Makefile.am
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

# this is required when using pdflatex (default)
all: admsflow.pdf

EXTRA_DIST = $(PNGS) $(PDFS) $(XFIG)

PNGS = componentdialog.png hicumschematic.png

PDFS = acgain_dpl.pdf charac_dpl.pdf fgummel_dpl.pdf transient_dpl.pdf \
  acgain_sch.pdf charac_sch.pdf fgummel.pdf transient_sch.pdf fbh_charac.pdf \
  sparam_dpl.pdf adms.pdf fbh_fgummel.pdf sparam_sch.pdf

XFIG = admsflow.fig

XPDF = $(XFIG:.fig=.pdf)

report: $(XPDF)

SUFFIXES = .fig .pdf

.fig.pdf:
	$(FIG2DEV) -L pdf $< > $@

CLEANFILES = $(XPDF) *~ *.bak *.orig *.rej
MAINTAINERCLEANFILES = Makefile.in
