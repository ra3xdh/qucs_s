
SET(COMPLIBS
AnalogueCM.lib
Bridges.lib
Diodes.lib
Ideal.lib
JFETs.lib
LEDs.lib
MOSFETs.lib
NMOSFETs.lib
OpAmps.lib
PMOSFETs.lib
Regulators.lib
Substrates.lib
Transistors.lib
Varistors.lib
Z-Diodes.lib
SpiceOpamp.lib
Xanalogue.lib
)

SET(BLACKLIST
qucs.blacklist
ngspice.blacklist
xyce.blacklist
)

INSTALL( FILES ${COMPLIBS} ${BLACKLIST} DESTINATION share/qucs/library )

ADD_SUBDIRECTORY( AnalogueCM )
