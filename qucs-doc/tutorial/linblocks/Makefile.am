## Process this file with automake to produce Makefile.in
#
# tutorial/linblocks/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2019 Romano Giannetti <romano.giannetti@gmail.com>
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

SOURCES = linblocks.tex theproblem.eps
TARGETS = linblocks.pdf
BUILT_SOURCES = theproblem.pdf

tutorial: $(TARGETS)

SUFFIXES = .dvi .tex .pdf

# installation of documentation
docdatadir = ${prefix}/share/qucs/docs/tutorial
docdata_DATA = ${TARGETS}

CLEANFILES = *~ *.bak *.aux *.log *.toc *.pdf *.bbl *.blg \
	     *.dvi *.brf *.out *.idx *-eps-converted-to.pdf

MAINTAINERCLEANFILES = Makefile.in
DISTCLEANFILES = *.ps *.pdf
