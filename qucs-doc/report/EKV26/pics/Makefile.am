## Process this file with automake to produce Makefile.in
# 
# report/EKV26/pics/Makefile.am
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

PDFS = EKV_Fig1.pdf EKV_Fig3.pdf EKV_Fig5.pdf EKV_Fig7.pdf EKV_Fig9.pdf \
  EKV_Fig2.pdf EKV_Fig4.pdf EKV_Fig6.pdf EKV_Fig8.pdf EKV_fig10.pdf \
  EKV_fig11.pdf EKV_fig12.pdf

report: $(XPDF)

SUFFIXES = .pdf .fig

.fig.pdf:
	$(FIG2DEV) -L pdf $< > $@

CLEANFILES = $(XPDF) *~ *.bak *.orig *.rej
MAINTAINERCLEANFILES = Makefile.in
