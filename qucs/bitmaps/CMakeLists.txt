
SET(XPMS  ac_voltage.png arrow.png capacitor.png dc_current.png dc_voltage.png
  inductor.png line.png msline.png polar.png rect.png rectangle.png
  resistor.png resistor_us.png smith.png source.png symtrans.png tabular.png
  text.png tline.png transformer.png gnd.png dc.png hb.png sparameter.png
  sweep.png tran.png cccs.png ccvs.png vccs.png vcvs.png ac.png cpwshort.png
  attenuator.png biast.png circulator.png dcblock.png dcfeed.png diode.png
  isolator.png mscorner.png mscross.png msstep.png mstee.png substrate.png
  subport.png spfile1.png spfile2.png spfile3.png coplanar.png ellipse.png
  mscoupled.png gyrator.png cpwgap.png vprobe.png switch.png relais.png
  pshifter.png iprobe.png noise_current.png noise_volt.png msmbend.png
  msopen.png filledellipse.png filledrect.png ac_current.png npn.png pnp.png
  nfet.png pfet.png nmosfet.png pmosfet.png dmosfet.png msgap.png npnsub.png
  pnpsub.png dmosfet_sub.png nmosfet_sub.png pmosfet_sub.png ellipsearc.png
  vpulse.png ipulse.png vrect.png irect.png msvia.png amplifier.png opamp.png
  ysmith.png smithpolar.png polarsmith.png spicefile.png rect3d.png curve.png
  cpwopen.png cpwstep.png mutual.png mutual2.png noise_ii.png noise_vv.png
  noise_iv.png or.png nor.png and.png nand.png xor.png xnor.png digi.png
  inverter.png digi_source.png timing.png truth.png rsflipflop.png
  dflipflop.png jkflipflop.png coupler.png coaxial.png am_mod.png pm_mod.png
  vhdlfile.png subcircuit.png optimize.png
)

# toolbar images
SET(PNGS  fileopen.png filesave.png editdelete.png editcut.png editcopy.png
  deactiv.png bottom.png editpaste.png equation.png fileclose.png pointer.png
  fileprint.png filesaveall.png gear.png mirror.png nodename.png marker.png
  port.png rebuild.png redo.png rotate_ccw.png top.png undo.png viewmag1.png
  viewmag+.png viewmag-.png viewmagfit.png wire.png filenew.png mirrory.png
  ground.png textnew.png quit.png back.png forward.png home.png previous.png
  next.png #close_tab.png clearFilterText.png sub.png textbold.png textitalic.png
  #textleft.png textright.png super.png textcenter.png textjustify.png 
  text.png
  #textunder.png
)

# application images
SET(ICONS  big.button.qucs.xpm big.qucs.xpm tiny.button.qucs.xpm tiny.qucs.xpm )

INSTALL(FILES ${XPMS} DESTINATION share/qucs/bitmaps/)
INSTALL(FILES ${PNGS} DESTINATION share/qucs/bitmaps/)
INSTALL(FILES ${ICONS} DESTINATION share/qucs/bitmaps/)
