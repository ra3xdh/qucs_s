## Process this file with automake to produce Makefile.in
#
# qucstex.mk
#
# makefile fragments for (la)tex invocation
# used for building qucs doc
#
# Copyright (C) 2016 Felix Salfelder
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

PDFLATEX_FLAGS = @PDFLATEX_FLAGS@

# TODO: just $(srcdir):
#       ".." etc. should be specified in MORE_TEXINPUTS if required
TEXINPUTS = $(srcdir):$(srcdir)/..:./..:$(MORE_TEXINPUTS)

TEX_ENV = export TEXINPUTS=$(TEXINPUTS)
BIB_ENV = export BIBINPUTS=$(srcdir):

.tex.pdf:
	$(TEX_ENV); $(PDFLATEX) $(PDFLATEX_FLAGS) $<
	$(TEX_ENV); $(PDFLATEX) $(PDFLATEX_FLAGS) $<
	$(texpdfHACK)
	$(TEX_ENV); $(PDFLATEX) $(PDFLATEX_FLAGS) -interaction=batchmode $<
	$(TEX_ENV); $(PDFLATEX) $(PDFLATEX_FLAGS) -interaction=batchmode $<

# just in case we want to revive the dvi->ps->pdf path one day...
.tex.dvi:
	$(TEX_ENV); $(LATEX) $(LATEX_ARGS) $<
	$(BIB_ENV); $(BIBTEX) $*
	$(TEX_ENV); $(LATEX) $(LATEX_ARGS) $<
	$(TEX_ENV); $(LATEX) $(LATEX_ARGS) $<
	$(texdviHACK)

.eps.pdf:
	$(EPSTOPDF) $< -o=$@
