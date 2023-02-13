# qucs/components library

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )

#INCLUDES = $(X11_INCLUDES) $(QT_INCLUDES) -I$(top_srcdir)/qucs

SET(COMPONENTS_SRCS
DLS_1ton.cpp		dff_SR.cpp		logical_buf.cpp		potentiometer.cpp
DLS_nto1.cpp		diac.cpp		logical_inv.cpp		rectline.cpp
EKV26MOS.cpp		digi_sim.cpp		logical_nand.cpp	relais.cpp
digi_source.cpp		logical_nor.cpp		resistor.cpp
MESFET.cpp		diode.cpp		logical_or.cpp		rfedd.cpp
ac_sim.cpp		dmux2to4.cpp		logical_xnor.cpp	rfedd2p.cpp
am_modulator.cpp	dmux3to8.cpp		logical_xor.cpp		rlcg.cpp
ampere_ac.cpp		dmux4to16.cpp		mod_amp.cpp		rs_flipflop.cpp
ampere_dc.cpp		ecvs.cpp
			eqndefined.cpp		#module.cpp
source_ac.cpp
ampere_noise.cpp	equation.cpp		mosfet.cpp		sp_sim.cpp
amplifier.cpp		etr_sim.cpp
			fa1b.cpp		mosfet_sub.cpp		sparamfile.cpp
andor4x2.cpp		fa2b.cpp		mscorner.cpp		spicedialog.cpp
andor4x3.cpp		gatedDlatch.cpp		mscoupled.cpp
andor4x4.cpp		greytobinary4bit.cpp	mscross.cpp		spicefile.cpp
attenuator.cpp		ground.cpp		msgap.cpp
biast.cpp		gyrator.cpp		mslange.cpp		subcircuit.cpp
binarytogrey4bit.cpp	ha1b.cpp		msline.cpp		subcirport.cpp
bjt.cpp			hb_sim.cpp		msmbend.cpp		substrate.cpp
bjtsub.cpp			msopen.cpp		switch.cpp
bondwire.cpp				msrstub.cpp		symtrafo.cpp
msstep.cpp		tff_SR.cpp
mstee.cpp		thyristor.cpp
capacitor.cpp		msvia.cpp		tline.cpp
cccs.cpp		mutual.cpp		tline_4port.cpp
ccvs.cpp		mutual2.cpp		tr_sim.cpp
circulator.cpp		mux2to1.cpp		transformer.cpp
coaxialline.cpp		mux4to1.cpp		triac.cpp
comp_1bit.cpp		hpribin4bit.cpp		mux8to1.cpp		tunneldiode.cpp
comp_2bit.cpp		hybrid.cpp		nigbt.cpp		twistedpair.cpp
comp_4bit.cpp		iexp.cpp		noise_ii.cpp		vafile.cpp
component.cpp		ifile.cpp		noise_iv.cpp		vccs.cpp
componentdialog.cpp	inductor.cpp		noise_vv.cpp		vcvs.cpp
iprobe.cpp		opamp.cpp		verilogfile.cpp		vcresistor.cpp
coplanar.cpp		ipulse.cpp		opt_sim.cpp		vexp.cpp
coupler.cpp		irect.cpp		optimizedialog.cpp	vfile.cpp
cpwgap.cpp		isolator.cpp	vhdlfile.cpp
cpwopen.cpp		jfet.cpp		pad2bit.cpp		volt_ac.cpp
cpwshort.cpp		jk_flipflop.cpp		pad3bit.cpp		volt_dc.cpp
cpwstep.cpp		jkff_SR.cpp		pad4bit.cpp		volt_noise.cpp
ctline.cpp		libcomp.cpp		param_sweep.cpp		vprobe.cpp
d_flipflop.cpp		log_amp.cpp		phaseshifter.cpp	vpulse.cpp
dc_sim.cpp		logic_0.cpp		photodiode.cpp		vrect.cpp
dcblock.cpp		logic_1.cpp		phototransistor.cpp
dcfeed.cpp		logical_and.cpp		pm_modulator.cpp
vcresistor.cpp
vacomponent.cpp
mutualx.cpp

# SPICE devices

bsim3v34nMOS.cpp
bsim3v34pMOS.cpp
bsim4v30nMOS.cpp
bsim4v30pMOS.cpp
hic0_full.cpp
hic2_full.cpp
hicumL0V1p2.cpp
hicumL0V1p2g.cpp
hicumL0V1p3.cpp
hicumL2V2p1.cpp
hicumL2V2p23.cpp
hicumL2V2p24.cpp
hicumL2V2p31n.cpp
HBT_X.cpp
)

SET(COMPONENTS_HDRS
ac_sim.h
am_modulator.h
ampere_ac.h
ampere_dc.h
ampere_noise.h
amplifier.h
andor4x2.h
andor4x3.h
andor4x4.h
attenuator.h
biast.h
binarytogrey4bit.h
bjt.h
bjtsub.h
bondwire.h
capacitor.h
cccs.h
ccvs.h
circulator.h
coaxialline.h
comp_1bit.h
comp_2bit.h
comp_4bit.h
component.h
componentdialog.h
components.h
coplanar.h
coupler.h
cpwgap.h
cpwopen.h
cpwshort.h
cpwstep.h
ctline.h
d_flipflop.h
dc_sim.h
dcblock.h
dcfeed.h
dff_SR.h
diac.h
digi_sim.h
digi_source.h
diode.h
DLS_1ton.h
DLS_nto1.h
dmux2to4.h
dmux3to8.h
dmux4to16.h
ecvs.h
EKV26MOS.h
eqndefined.h
equation.h
etr_sim.h
fa1b.h
fa2b.h
gatedDlatch.h
greytobinary4bit.h
ground.h
gyrator.h
ha1b.h
hb_sim.h
hpribin4bit.h
hybrid.h
iexp.h
ifile.h
inductor.h
iprobe.h
ipulse.h
irect.h
isolator.h
jfet.h
jk_flipflop.h
jkff_SR.h
libcomp.h
log_amp.h
logic_0.h
logic_1.h
logical_and.h
logical_buf.h
logical_inv.h
logical_nand.h
logical_nor.h
logical_or.h
logical_xnor.h
logical_xor.h
MESFET.h
mod_amp.h
mosfet_sub.h
mosfet.h
mscorner.h
mscoupled.h
mscross.h
msgap.h
mslange.h
msline.h
msmbend.h
msopen.h
msrstub.h
msstep.h
mstee.h
msvia.h
mutual.h
mutual2.h
mux2to1.h
mux4to1.h
mux8to1.h
nigbt.h
noise_ii.h
noise_iv.h
noise_vv.h
opamp.h
opt_sim.h
optimizedialog.h
pad2bit.h
pad3bit.h
pad4bit.h
param_sweep.h
phaseshifter.h
photodiode.h
phototransistor.h
pm_modulator.h
potentiometer.h
rectline.h
relais.h
resistor.h
rfedd.h
rfedd2p.h
rlcg.h
rs_flipflop.h
source_ac.h
sp_sim.h
sparamfile.h
spicedialog.h
spicefile.h
subcircuit.h
subcirport.h
substrate.h
switch.h
symtrafo.h
tff_SR.h
thyristor.h
tline_4port.h
tline.h
tr_sim.h
transformer.h
triac.h
tunneldiode.h
twistedpair.h
vacomponent.h
vafile.h
vccs.h
vcresistor.h
vcvs.h
verilogfile.h
vexp.h
vfile.h
vhdlfile.h
volt_ac.h
volt_dc.h
volt_noise.h
vprobe.h
vpulse.h
vrect.h

# SPICE devices

bsim3v34nMOS.h
bsim3v34pMOS.h
bsim4v30nMOS.h
bsim4v30pMOS.h
hic0_full.h
hic2_full.h
hicumL0V1p2.h
hicumL0V1p2g.h
hicumL0V1p3.h
hicumL2V2p1.h
hicumL2V2p23.h
hicumL2V2p24.h
hicumL2V2p31n.h
HBT_X.h
)



SET(COMPONENTS_MOC_HDRS
componentdialog.h
spicedialog.h
optimizedialog.h
spicefile.h
)

IF(WITH_QT6)
QT6_WRAP_CPP(COMPONENTS_MOC_SRCS ${COMPONENTS_MOC_HDRS})
ELSE()
QT5_WRAP_CPP(COMPONENTS_MOC_SRCS ${COMPONENTS_MOC_HDRS})
ENDIF()

ADD_LIBRARY(components STATIC ${COMPONENTS_HDRS} ${COMPONENTS_SRCS} ${COMPONENTS_MOC_SRCS} )





