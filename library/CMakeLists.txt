
SET(COMPLIBS
555_timer.lib
AudioIC.lib
Bridges.lib
BJT_Extended.lib
Cores.lib
Crystal.lib
Diodes.lib
DiodesSchottky.lib
Diodes_Extended.lib
Digital_AUX.lib
Digital_CD.lib
Digital_HC.lib
Digital_LV.lib
Digital_XSPICE.lib
GeDiodes.lib
Ideal.lib
JFETs.lib
LEDs.lib
LaserDiodes.lib
Loudspeaker.lib
MixerIC.lib
MOSFETs.lib
NMOSFETs.lib
OpAmps.lib
Optocoupler.lib
PhotovoltaicRelay.lib
PMOSFETs.lib
PWM_Controller.lib
Regulators.lib
Substrates.lib
Transistors.lib
Varistors.lib
Z-Diodes.lib
SpiceOpamp.lib
SPICE_TLine.lib
Thermistor.lib
Thyristor.lib
Transformers.lib
Xanalogue.lib
XyceDigital.lib
Xyce_Digital_TTL_Technology.lib
BF998.lib
Tubes.lib
Varactor.lib
Varactor_RF.lib
PIN_Diodes.lib
VoltageComparators.lib
VoltageReferences.lib
VoltageRegulators.lib
)

SET(BLACKLIST
qucs.blacklist
ngspice.blacklist
xyce.blacklist
)

INSTALL( FILES ${COMPLIBS} ${BLACKLIST} DESTINATION share/${QUCS_NAME}/library )
INSTALL( DIRECTORY "symbols" DESTINATION share/${QUCS_NAME}/ )

ADD_SUBDIRECTORY( XyceDigital)

