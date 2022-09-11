## Process this file with automake to produce Makefile.in
# 
# report/activefilter/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2015 Vadim Kuznetsov <ra3xdh@gmail.com>
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

EXTRA_DIST = $(SOURCES) $(BIBFILE)

SOURCES = activefilter.tex content.tex
BIBFILE = activefilter.bib
TARGETS = activefilter.pdf

activefilter.pdf: $(SOURCES) $(BIBFILE)

SUFFIXES = .tex .dvi .ps

# this is not required, as we are using pdflatex now.
# .tex.dvi:
# 	$(LATEX) --shell-escape $<
# 	$(LATEX) --shell-escape $<
# 	$(LATEX) -interaction=batchmode --shell-escape $<
# 
# 	bibtex $*
# 
# .dvi.ps:
# 	$(DVIPS) $< -o $@

activefilter.pdf: texpdfHACK = export BIBINPUTS=$(srcdir):; bibtex activefilter


# installation of documentation
docdatadir = ${prefix}/share/qucs/docs/report
docdata_DATA = ${TARGETS}

# insane hack. allows latex to execute arbitrary code
# (used to render some plots)
# TODO: specify all commands within Makefile
activefilter.pdf: PDFLATEX_FLAGS=--shell-escape

CLEANFILES = *~ *.bak *.aux *.log *.toc *.pdf *.bbl *.blg *.dvi *.brf *.out *.idx *.pgf-plot.table *.pgf-plot.gnuplot
MAINTAINERCLEANFILES = Makefile.in
DISTCLEANFILES = *.ps *.pdf
