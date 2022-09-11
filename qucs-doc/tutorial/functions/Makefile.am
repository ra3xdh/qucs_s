## Process this file with automake to produce Makefile.in
# 
# tutorial/functions/Makefile.am
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

include $(top_srcdir)/qucstex.mk

SUBDIRS = pics

EXTRA_DIST = $(SOURCES)

SOURCES = appendix.ist math.tex use.tex electronics.tex intro.tex syntaxoverview.tex functions.tex content.tex

TARGETS = functions.pdf

pics-tutorial:
	cd pics && $(MAKE) $(AM_MAKEFLAGS) tutorial

tutorial: pics-tutorial $(TARGETS)

book: pics-tutorial

SUFFIXES = .dvi .tex .pdf .ps .idx .ind

.tex.dvi:
	$(LATEX) $<
	$(LATEX) $<
	$(LATEX) -interaction=batchmode $<

texpdfHACK = $(MAKEINDEX) functions.idx -o functions.ind -s $(srcdir)/appendix.ist

.dvi.ps:
	$(DVIPS) $< -o $@

.idx.ind:
	$(MAKEINDEX) $< -o $@ -s appendix.ist

# installation of documentation
docdatadir = ${prefix}/share/qucs/docs/tutorial
docdata_DATA = ${TARGETS}

CLEANFILES = *~ *.bak *.aux *.log *.toc *.pdf *.bbl *.blg *.dvi *.brf *.out *.idx *.ind *.ilg
MAINTAINERCLEANFILES = Makefile.in
DISTCLEANFILES = *.ps *.pdf
