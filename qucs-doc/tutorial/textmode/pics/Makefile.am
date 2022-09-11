## Process this file with automake to produce Makefile.in
#
# tutorial/textmode/pics/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2016 Guilherme Brondani Torri <guitorri@gmail.com>
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

# TODO: these are not source files
# (source code is embedded in content.tex)
EXTRA_DIST = $(EPSS)

EPSS = \
  rc_ac.eps \
  rc_ac_circuit.eps \
  rc_ac_python.eps \
  rc_ac_sweep_python.eps \
  rc_ac_tr.eps

all: $(EPSS:%.eps=%.pdf)

CLEANFILES = *~ *.orig *.rej *.pdf
MAINTAINERCLEANFILES = Makefile.in


