## Process this file with automake to produce Makefile.in
#
# src/components/Makefile.am
#
# Automake input file for the component library.
#
# Copyright (C) 2003-2011 Stefan Jahn <stefan@lkcc.org>
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

# the component library rules

SUBDIRS = microstrip devices digital verilog

noinst_LTLIBRARIES = libcomponent.la

libcomponent_la_SOURCES = attenuator.cpp dcblock.cpp open.cpp vccs.cpp        \
	biastee.cpp circulator.cpp dcfeed.cpp pac.cpp vcvs.cpp capacitor.cpp \
	ground.cpp resistor.cpp cccs.cpp cross.cpp trafo.cpp inductor.cpp    \
	tee.cpp ccvs.cpp isolator.cpp itrafo.cpp strafo.cpp vdc.cpp idc.cpp  \
        vac.cpp phaseshifter.cpp gyrator.cpp tline.cpp iprobe.cpp wprobe.cpp iac.cpp    \
	spfile.cpp vnoise.cpp inoise.cpp vpulse.cpp ipulse.cpp vrect.cpp     \
	irect.cpp amplifier.cpp opamp.cpp iinoise.cpp mutual.cpp mutual2.cpp \
	vvnoise.cpp ivnoise.cpp coupler.cpp coaxline.cpp vprobe.cpp vam.cpp  \
	vpm.cpp tswitch.cpp relais.cpp short.cpp twistedpair.cpp tline4p.cpp \
        vexp.cpp iexp.cpp mutualx.cpp vfile.cpp ifile.cpp rfedd.cpp          \
        rectline.cpp rlcg.cpp hybrid.cpp ctline.cpp ecvs.cpp circline.cpp \
        taperedline.cpp capq.cpp indq.cpp spembed.cpp spdeembed.cpp

pkginclude_HEADERS = component.h components.h component_id.h

noinst_HEADERS = attenuator.h dcblock.h open.h vccs.h biastee.h circulator.h \
	dcfeed.h pac.h vcvs.h capacitor.h ground.h resistor.h cccs.h cross.h \
	trafo.h inductor.h tee.h ccvs.h isolator.h component.h itrafo.h      \
	strafo.h vdc.h idc.h vac.h component_id.h phaseshifter.h gyrator.h   \
        tline.h iprobe.h wprobe.h iac.h spfile.h vnoise.h inoise.h vpulse.h ipulse.h  \
	vrect.h irect.h amplifier.h opamp.h iinoise.h mutual.h mutual2.h     \
	vvnoise.h ivnoise.h coupler.h coaxline.h vprobe.h vam.h vpm.h        \
	tswitch.h relais.h short.h twistedpair.h tline4p.h vexp.h iexp.h     \
	mutualx.h vfile.h ifile.h rfedd.h rectline.h components.h rlcg.h     \
        hybrid.h ctline.h ecvs.h taperedline.h capq.h indq.h      \
	component.h components.h component_id.h circline.h spembed.h spdeembed.h

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/src/math

CLEANFILES = *~ *.orig *.rej

MAINTAINERCLEANFILES = Makefile.in
