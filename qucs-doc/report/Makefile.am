## Process this file with automake to produce Makefile.in
# 
# report/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2007, 2008 Stefan Jahn <stefan@lkcc.org>
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

include $(top_srcdir)/qucstex.mk

SUBDIRS = verilog StoQ_test1 StoQ_test2 StoQ_test3 StoQ_test4 StoQ_test5 \
  curtice mod_amp log_amp potentiometer mesfet EKV26 photodiode activefilter

EXTRA_DIST = $(SOURCES) $(TEXFILES)

TEXFILES = titlepage.tex basics.tex reportbook.tex
TARGETS = reportbook.pdf
PICDIRS = verilog

pics-book:
	@for d in $(PICDIRS); do \
	  (cd $$d && $(MAKE) $(AM_MAKEFLAGS) book) || exit $$?; \
	done

book: pics-book $(TARGETS)

report:
	@for d in $(SUBDIRS); do \
	  (cd $$d && $(MAKE) $(AM_MAKEFLAGS) report) || exit $$?; \
	done

SUFFIXES = .dvi .tex .pdf .ps .eps .fig

.tex.dvi:
	$(LATEX) $<
	$(LATEX) $<
	$(LATEX) -interaction=batchmode $<

.fig.eps:
	$(FIG2DEV) -L eps $< > $@

.eps.pdf:
	$(EPSTOPDF) $< -o=$@

.dvi.ps:
	$(DVIPS) $< -o $@

# installation of documentation
docdatadir = ${prefix}/share/qucs/docs/report
docdata_DATA = $(TARGETS)

CLEANFILES = *~ *.bak *.aux *.log *.toc *.pdf *.bbl *.blg *.dvi *.brf *.out *.idx
MAINTAINERCLEANFILES = Makefile.in
DISTCLEANFILES = *.ps *.pdf
