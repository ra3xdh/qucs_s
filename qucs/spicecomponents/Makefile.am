## Process this file with automake to produce Makefile.in
#
# qucs/components/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2004-2011 Stefan Jahn <stefan@lkcc.org>
# New models 2013 addded Mike Brinson  mbrin72043@yahoo.co.uk
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

SUBDIRS=symbols

noinst_LIBRARIES = libspicecomponents.a

#MOCHEADERS = componentdialog.h spicedialog.h optimizedialog.h spicefile.h
#MOCFILES = $(MOCHEADERS:.h=.moc.cpp)

libspicecomponents_a_SOURCES = src_eqndef.cpp \
	sp_parameter.cpp \
	sp_globalpar.cpp \
	sp_options.cpp \
	sp_nutmeg.cpp \
	sp_ic.cpp \
	sp_nodeset.cpp \
	vsffm.cpp \
	isffm.cpp \
	vAmpMod.cpp \
	iAmpMod.cpp \
	vPWL.cpp \
	iPWL.cpp \
	eNL.cpp \
	gNL.cpp \
	vTRNOISE.cpp \
	iTRNOISE.cpp \
	vTRRANDOM.cpp \
	C_SPICE.cpp \
	L_SPICE.cpp \
	R_SPICE.cpp \
	K_SPICE.cpp \
	MESFET_SPICE.cpp \
	PMF_MESFET_SPICE.cpp \
	Cmeter_SPICE.cpp \
	LTL_SPICE.cpp \
        UDRCTL_SPICE.cpp \
        LTRA_SPICE.cpp \
        DIODE_SPICE.cpp \
        NPN_SPICE.cpp \
        PNP_SPICE.cpp \
        NJF_SPICE.cpp \
        PJF_SPICE.cpp \
        NMOS_SPICE.cpp \
        PMOS_SPICE.cpp \
        volt_ac_SPICE.cpp \
	sp_customsim.cpp \
	sp_fourier.cpp \
	sp_disto.cpp \
	sp_noise.cpp \
        S4Q_V.cpp \
        S4Q_Ieqndef.cpp \
        S4Q_I.cpp \
        S4Q_S.cpp \
        S4Q_W.cpp \
        Cmeter_SPICE.cpp \
        SE_V_amp.cpp \
        DE_V_amp.cpp \
        SE_V_limit.cpp\
        DE_V_limit.cpp\
	SE_V_int.cpp \
	DE_V_int.cpp \
	SE_V_diff.cpp \
	DE_V_diff.cpp \
	sp_model.cpp \
	sp_include.cpp \
	sp_pz.cpp \
    ABCV.cpp \
	ABV.cpp \
	APBPCV.cpp \
	DivV.cpp \
	core.cpp \
	Icouple.cpp \
	APBV.cpp \
	SDTF.cpp \
	XAPWL.cpp \
	xspicegeneric.cpp \
	xsp_cmlib.cpp \
	xsp_codemodel.cpp \
	spicegeneric.cpp \
	spicelibcomp.cpp


#nodist_libcomponents_a_SOURCES = $(MOCFILES)

noinst_HEADERS = src_eqndef.h \
	sp_parameter.h \
	sp_globalpar.h \
	sp_options.h \
	sp_nodeset.h \
	sp_nutmeg.h \
	sp_ic.h \
	vsffm.h \
	isffm.h \
	vAmpMod.h \
	iAmpMod.h \
	vPWL.h \
	iPWL.h \
	eNL.h \
	gNL.h \
	vTRNOISE.h \
	iTRNOISE.h \
	vTRRANDOM.h \
	C_SPICE.h \
	L_SPICE.h \
	R_SPICE.h \
	K_SPICE.h \
	MESFET_SPICE.h \
	PMF_MESFET_SPICE.h \
	Cmeter_SPICE.h \
	LTL_SPICE.h \
	UDRCTL_SPICE.h \
	LTRA_SPICE.h \
	DIODE_SPICE.h \
	NPN_SPICE.h \
	PNP_SPICE.h \
	NJF_SPICE.h \
	PJF_SPICE.h \
	NMOS_SPICE.h \
	PMOS_SPICE.h \
	volt_ac_SPICE.h \
	sp_customsim.h \
	sp_fourier.h \
        sp_disto.h \
	sp_noise.h \
        S4Q_V.h \
        S4Q_Ieqndef.h \
        S4Q_I.h \
        S4Q_S.h \
        S4Q_W.h \
        Cmeter_SPICE.h \
        SE_V_amp.h \
        DE_V_amp.h \
        SE_V_limit.h\
        DE_V_limit.h\
	SE_V_int.h \
	DE_V_int.h \
	SE_V_diff.h \
	DE_V_diff.h \
	sp_model.h \
	sp_include.h \
	sp_pz.h \
        ABCV.h \
	ABV.h \
	APBPCV.h \
	DivV.h \
	core.h \
	Icouple.h \
	APBV.h \
	SDTF.h \
	XAPWL.h \
	xspicegeneric.h \
	xsp_cmlib.h \
	xsp_codemodel.h \
	spicegeneric.h \
	spicelibcomp.h


AM_CPPFLAGS = $(X11_INCLUDES) $(QT_INCLUDES) -I$(top_srcdir)/qucs

#SUFFIXES = .moc.cpp

#.h.moc.cpp:
#	$(MOC) -o $@ $<

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in *.moc.cpp
DISTCLEANFILES = *.moc.cpp
