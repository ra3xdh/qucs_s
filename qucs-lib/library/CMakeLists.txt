
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
Transformers.lib
Xanalogue.lib
MESFETs.lib
XyceDigital.lib
Xyce_Digital_TTL_Technology.lib
)

SET(BLACKLIST
qucs.blacklist
ngspice.blacklist
xyce.blacklist
)

INSTALL( FILES ${COMPLIBS} ${BLACKLIST} DESTINATION share/${QUCS_NAME}/library )

ADD_SUBDIRECTORY( AnalogueCM )
ADD_SUBDIRECTORY( MESFETs )
ADD_SUBDIRECTORY( XyceDigital)
