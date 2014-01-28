# qucs/componenets library

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR} )

#INCLUDES = $(X11_INCLUDES) $(QT_INCLUDES) -I$(top_srcdir)/qucs

SET(COMPONENTS_SRCS
DLS_1ton.cpp		dff_SR.cpp		logical_buf.cpp		potentiometer.cpp
DLS_nto1.cpp		diac.cpp		logical_inv.cpp		rectline.cpp
EKV26MOS.cpp		digi_sim.cpp		logical_nand.cpp	relais.cpp
HBT_X.cpp		digi_source.cpp		logical_nor.cpp		resistor.cpp
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
bjtsub.cpp		hic0_full.cpp		msopen.cpp		switch.cpp
bondwire.cpp		hic2_full.cpp		msrstub.cpp		symtrafo.cpp
bsim3v34nMOS.cpp	hicumL0V1p2.cpp		msstep.cpp		tff_SR.cpp
bsim3v34pMOS.cpp	hicumL0V1p2g.cpp	mstee.cpp		thyristor.cpp
capacitor.cpp		hicumL0V1p3.cpp		msvia.cpp		tline.cpp
cccs.cpp		hicumL2V2p1.cpp		mutual.cpp		tline_4port.cpp
ccvs.cpp		hicumL2V2p23.cpp	mutual2.cpp		tr_sim.cpp
circulator.cpp		hicumL2V2p24.cpp	mux2to1.cpp		transformer.cpp
coaxialline.cpp		hicumL2V2p31n.cpp	mux4to1.cpp		triac.cpp
comp_1bit.cpp		hpribin4bit.cpp		mux8to1.cpp		tunneldiode.cpp
comp_2bit.cpp		hybrid.cpp		nigbt.cpp		twistedpair.cpp
comp_4bit.cpp		iexp.cpp		noise_ii.cpp		vafile.cpp
component.cpp		ifile.cpp		noise_iv.cpp		vccs.cpp
componentdialog.cpp	inductor.cpp		noise_vv.cpp		vcvs.cpp
iprobe.cpp		opamp.cpp		verilogfile.cpp
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
bsim4v30nMOS.cpp 
bsim4v30pMOS.cpp 
)

SET(COMPONENTS_MOC_HDRS
componentdialog.h
spicedialog.h
optimizedialog.h
spicefile.h
)

QT4_WRAP_CPP(COMPONENTS_MOC_SRCS ${COMPONENTS_MOC_HDRS})

ADD_LIBRARY(components ${COMPONENTS_SRCS} ${COMPONENTS_MOC_SRCS} ${QT_INCLUDES})





