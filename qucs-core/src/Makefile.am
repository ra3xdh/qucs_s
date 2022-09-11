## Process this file with automake to produce Makefile.in
#
# src/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2003-2009 Stefan Jahn <stefan@lkcc.org>
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

EXTRA_DIST = gperfapphash.cpp

SUBDIRS = math components interface . converter

# needed to generate parse_spice.hpp from parse_spice.ypp
AM_YFLAGS = -d -t

AM_CPPFLAGS = -I$(top_srcdir)/src/math -I$(top_srcdir)/src/components \
	-I$(top_builddir)/src/components \
	-I$(top_srcdir)/src/interface -I$(builddir)

lib_LTLIBRARIES = libqucsator.la

libqucsator_la_LIBADD = math/libqucsmath.la   \
	components/libcomponent.la	      \
	components/microstrip/libmicrostrip.la \
	components/devices/libdevices.la	      \
	components/digital/libdigital.la       \
	components/verilog/libverilog.la \
	interface/libinterface.la
## In case we want to build windows dlls
libqucsator_la_LDFLAGS = -shared -no-undefined

pkginclude_HEADERS = compat.h logging.h object.h vector.h consts.h node.h \
  net.h circuit.h integrator.h states.h states.cpp valuelist.h \
  constants.h netdefs.h property.h ptrlist.h characteristic.h pair.h \
  operatingpoint.h

noinst_TEMPLATES = tridiag.cpp hash.cpp \
	tmatrix.cpp tvector.cpp eqnsys.cpp states.cpp \
	nasolver.cpp

noinst_HEADERS = $(noinst_TEMPLATES)            \
	check_dataset.h \
	check_touchstone.h  spsolver.h dcsolver.h variable.h       \
	parasweep.h sweep.h libqucsator.h evaluate.h matvec.h acsolver.h   \
	transient.h netdefs.h hbsolver.h poly.h     \
	spline.h tridiag.h fourier.h hash.h applications.h     \
	range.h history.h devstates.h check_citi.h check_zvr.h  \
	check_mdl.h differentiate.h  \
	check_csv.h analyses.h receiver.h interpolator.h \
	logging.h net.h input.h dataset.h equation.h tvector.h tmatrix.h \
	environment.h exceptionstack.h check_netlist.h module.h nasolver.h \
	states.h analysis.h trsolver.h nasolution.h eqnsys.h compat.h \
	exception.h object.h node.h circuit.h constants.h vector.h \
	nodeset.h nodelist.h strlist.h operatingpoint.h  consts.h  \
	integrator.h valuelist.h gperfappgen.h

libqucsator_la_SOURCES = dataset.cpp check_dataset.cpp \
	check_touchstone.cpp vector.cpp object.cpp          \
	property.cpp \
	variable.cpp   \
	strlist.cpp logging.c exception.cpp exceptionstack.cpp               \
	matvec.cpp \
	check_citi.cpp \
	sweep.cpp    \
	check_zvr.cpp \
	check_mdl.cpp check_csv.cpp \
	circuit.cpp check_netlist.cpp \
	net.cpp input.cpp        \
	analysis.cpp spsolver.cpp dcsolver.cpp nodelist.cpp environment.cpp  \
	parasweep.cpp equation.cpp evaluate.cpp acsolver.cpp                 \
	trsolver.cpp transient.cpp integrator.cpp nodeset.cpp hbsolver.cpp   \
	spline.cpp fourier.cpp history.cpp       \
	range.cpp devstates.cpp differentiate.cpp module.cpp receiver.cpp    \
	interpolator.cpp \
	parse_citi.ypp scan_citi.lpp \
	parse_csv.ypp scan_csv.lpp \
	parse_dataset.ypp scan_dataset.lpp \
	parse_mdl.ypp scan_mdl.lpp \
	parse_netlist.ypp scan_netlist.lpp \
	parse_touchstone.ypp scan_touchstone.lpp \
	parse_zvr.ypp scan_zvr.lpp

# needed for ylwrap
scan_citi.cpp: LEX_OUTPUT_ROOT = lex.citi_
scan_csv.cpp: LEX_OUTPUT_ROOT = lex.csv_
scan_dataset.cpp: LEX_OUTPUT_ROOT = lex.dataset_
scan_mdl.cpp: LEX_OUTPUT_ROOT = lex.mdl_
scan_netlist.cpp: LEX_OUTPUT_ROOT = lex.netlist_
scan_touchstone.cpp: LEX_OUTPUT_ROOT = lex.touchstone_
scan_zvr.cpp: LEX_OUTPUT_ROOT = lex.zvr_

# just make sure, everything is in place
bin_PROGRAMS = qucsator

qucsator_LDADD = libqucsator.la

qucsator_LDFLAGS = -Wl,-rpath,$(libdir)

qucsator_SOURCES = ucs.cpp

all-am: qucsdefs.h


## Here we create the various input parsing code files using
## lex and yacc, these files will be distributed
generated_FILES = qucsdefs.h

qucsdefs.h: qucsator$(EXEEXT)
	./$< -l > $@

## The qucsator equation definitions use a hash lookup table
## generated using gperf and put in gperfapphash.cpp which
## is #included in the body of equation.cpp
equation.cpp: gperfapphash.cpp

BUILT_SOURCES = equation.cpp \
	gperfappgen.h \
	parse_citi.hpp \
	parse_csv.hpp \
	parse_dataset.hpp \
	parse_mdl.hpp \
	parse_netlist.hpp \
	parse_touchstone.hpp \
	parse_zvr.hpp


## Here we generate gperfapphash.cpp for equation.cpp.
##
## To generate the hash table code we must first create a
## program to create the hash input file for gperf, called
## gperfappgen.
noinst_PROGRAMS = gperfappgen

gperfappgen_SOURCES = gperfappgen.cpp

if COND_WIN32
FILTER=| dos2unix
endif

gperfapphash.gph: gperfappgen$(EXEEXT)
	./$< $(FILTER) > $@

gperfapphash.cpp: gperfapphash.gph
	$(GPERF) -I -m 8 $< | sed -e 's/{""},/{"",0},/g' > $@

gperfappgen.h: applications.h
	sed -e 's/evaluate::[a-zA-Z0-9_]*/NULL/g' < $< > gperfappgen.h

# this is redundant (trying to figure out appveyor problem)
gperfappgen.o: gperfappgen.h

## Store the gperf names in their own variable as the are
## not distributed but must be removed in a maintainer-clean
gperf_FILES = gperfapphash.cpp gperfapphash.gph gperfappgen.h

CLEANFILES = *~ *.orig *.rej *.output
MAINTAINERCLEANFILES = Makefile.in $(generated_FILES) $(gperf_FILES)
DISTCLEANFILES = $(generated_FILES) $(gperf_FILES)
