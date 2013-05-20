## Process this file with automake to produce Makefile.in
# 
# qucs/bitmaps/Makefile.am
#
# Automake input file.
#
# Copyright (C) 2004-2011 Stefan Jahn <stefan@lkcc.org>
# New models added 2013 Mike Brinson mbrin72043@yahoo.co.uk
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

# component pictures
XPMS = ac_voltage.png arrow.png capacitor.png dc_current.png dc_voltage.png   \
  inductor.png line.png msline.png polar.png rect.png rectangle.png           \
  resistor.png resistor_us.png smith.png source.png symtrans.png tabular.png  \
  text.png tline.png transformer.png gnd.png dc.png hb.png sparameter.png     \
  sweep.png tran.png cccs.png ccvs.png vccs.png vcvs.png ac.png cpwshort.png  \
  attenuator.png biast.png circulator.png dcblock.png dcfeed.png diode.png    \
  isolator.png mscorner.png mscross.png msstep.png mstee.png substrate.png    \
  subport.png spfile1.png spfile2.png spfile3.png coplanar.png ellipse.png    \
  mscoupled.png mslange.png gyrator.png cpwgap.png vprobe.png switch.png relais.png       \
  pshifter.png iprobe.png noise_current.png noise_volt.png msmbend.png        \
  msopen.png filledellipse.png filledrect.png ac_current.png npn.png pnp.png  \
  nfet.png pfet.png nmosfet.png pmosfet.png dmosfet.png msgap.png npnsub.png  \
  pnpsub.png dmosfet_sub.png nmosfet_sub.png pmosfet_sub.png ellipsearc.png   \
  vpulse.png ipulse.png vrect.png irect.png msvia.png amplifier.png opamp.png \
  ysmith.png smithpolar.png polarsmith.png spicefile.png rect3d.png curve.png \
  cpwopen.png cpwstep.png mutual.png mutual2.png noise_ii.png noise_vv.png    \
  noise_iv.png or.png nor.png and.png nand.png xor.png xnor.png digi.png      \
  inverter.png digi_source.png timing.png truth.png rsflipflop.png            \
  dflipflop.png jkflipflop.png coupler.png coaxial.png am_mod.png pm_mod.png  \
  vhdlfile.png subcircuit.png optimize.png tline_4port.png twistedpair.png    \
  bondwire.png npn_therm.png npnsub_therm.png iexp.png vexp.png edd.png       \
  vfile.png ifile.png mod_amp.png thyristor.png triac.png diac.png            \
  log_amp.png pnp_therm.png pnpsub_therm.png potentiometer.png rfedd.png      \
  MESFET.png EKV26nMOS.png EKV26pMOS.png rectline.png photodiode.png          \
  buffer.png dff_SR.png jkff_SR.png tff_SR.png gatedDlatch.png logic_0.png    \
  logic_1.png mux2to1.png mux4to1.png mux8to1.png DLS_nto1.png DLS_1ton.png   \
  andor4x2.png andor4x3.png andor4x4.png dmux2to4.png dmux3to8.png            \
  dmux4to16.png ha1b.png fa1b.png fa2b.png pad2bit.png pad3bit.png            \
  pad4bit.png binarytogrey4bit.png greytobinary4bit.png comp_1bit.png         \
  comp_2bit.png comp_4bit.png hpribin4bit.png msrstub.png phototransistor.png \
  rlcg.png nigbt.png hybrid.png ctline.png tunneldiode.png hicumL2V2p31n.png  \
  bsim3v34nMOS.png  bsim3v34pMOS.png  

# toolbar pictures
PNGS = fileopen.png filesave.png editdelete.png editcut.png editcopy.png      \
  deactiv.png bottom.png editpaste.png equation.png fileclose.png pointer.png \
  fileprint.png filesaveall.png gear.png mirror.png nodename.png marker.png   \
  port.png rebuild.png redo.png rotate_ccw.png top.png undo.png viewmag1.png  \
  viewmag+.png viewmag-.png viewmagfit.png wire.png filenew.png mirrory.png   \
  ground.png textnew.png quit.png back.png forward.png home.png previous.png  \
  next.png

# application pictures
ICONS = big.button.qucs.xpm big.qucs.xpm tiny.button.qucs.xpm tiny.qucs.xpm

# application pictures
MACICONS = qucs.icns qucsattenuator.icns qucsfilter.icns qucshelp.icns \
  qucslib.icns qucsedit.icns qucstrans.icns

EXTRA_DIST = $(XPMS) $(PNGS) $(ICONS) $(MACICONS)

# installation of pictures
picdatadir = ${prefix}/share/qucs/bitmaps
picdata_DATA = $(XPMS) $(PNGS) $(ICONS)

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in
