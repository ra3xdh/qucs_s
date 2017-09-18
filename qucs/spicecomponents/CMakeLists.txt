# qucs/componenets library

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR} )

#INCLUDES = $(X11_INCLUDES) $(QT_INCLUDES) -I$(top_srcdir)/qucs

SET(COMPONENTS_SRCS
src_eqndef.cpp

#SPICE sections

sp_parameter.cpp
sp_globalpar.cpp
sp_options.cpp
sp_nutmeg.cpp
sp_ic.cpp
sp_nodeset.cpp
sp_model.cpp
sp_include.cpp
sp_func.cpp
incl_script.cpp

#end of SPICE section

vsffm.cpp
isffm.cpp
vAmpMod.cpp
iAmpMod.cpp
vPWL.cpp
iPWL.cpp
eNL.cpp
gNL.cpp
vTRNOISE.cpp
iTRNOISE.cpp
vTRRANDOM.cpp
C_SPICE.cpp
L_SPICE.cpp
R_SPICE.cpp
K_SPICE.cpp
MESFET_SPICE.cpp
PMF_MESFET_SPICE.cpp
LTL_SPICE.cpp
UDRCTL_SPICE.cpp
LTRA_SPICE.cpp
DIODE_SPICE.cpp
NPN_SPICE.cpp
PNP_SPICE.cpp
NJF_SPICE.cpp
PJF_SPICE.cpp
NMOS_SPICE.cpp
PMOS_SPICE.cpp
volt_ac_SPICE.cpp

# SPICE simulations list

sp_customsim.cpp
sp_fourier.cpp
sp_disto.cpp
sp_noise.cpp
sp_pz.cpp
xyce_script.cpp
sp_sens.cpp
sp_sens_ac.cpp
sp_sens_xyce.cpp

# 
# Qucs-S new components cpp list
#


S4Q_V.cpp
S4Q_Ieqndef.cpp
S4Q_I.cpp
S4Q_S.cpp
S4Q_W.cpp
#
# XSPICE cpp list
#

Icouple.cpp
core.cpp
SDTF.cpp
XAPWL.cpp

#XSPICE CodeModel
xspicegeneric.cpp
xsp_cmlib.cpp
xsp_codemodel.cpp

#SPICE file devices
spicegeneric.cpp
spicelibcomp.cpp
)

SET(COMPONENTS_HDRS
spicecomponents.h
src_eqndef.h

#SPICE sections

sp_parameter.h
sp_globalpar.h
sp_options.h
sp_nutmeg.h
sp_ic.h
sp_nodeset.h
sp_model.h
sp_include.h
sp_func.h
incl_script.h

#end SPICE sections

vsffm.h
isffm.h
vAmpMod.h
iAmpMod.h
vPWL.h
iPWL.h
eNL.h
gNL.h
vTRNOISE.h
iTRNOISE.h
vTRRANDOM.h
C_SPICE.h
L_SPICE.h
R_SPICE.h
K_SPICE.h
MESFET_SPICE.h
PMF_MESFET_SPICE.h
LTL_SPICE.h
UDRCTL_SPICE.h
LTRA_SPICE.h
DIODE_SPICE.h
NPN_SPICE.h
PNP_SPICE.h
NJF_SPICE.h
PJF_SPICE.h
NMOS_SPICE.h
PMOS_SPICE.h
volt_ac_SPICE.h
sp_customsim.h
sp_fourier.h
sp_disto.h
sp_noise.h
sp_pz.h
sp_sens.h
sp_sens_ac.h
sp_sens_xyce.h
xyce_script.h

# 
# Qucs-S new components h list
#

S4Q_V.h
S4Q_Ieqndef.h
S4Q_I.h
S4Q_S.h
S4Q_W.h
#
# XSPICE component headers
#

Icouple.h
core.h
SDTF.h
XAPWL.h

#XSPICE codemodel

xspicegeneric.h
xsp_cmlib.h
xsp_codemodel.h

#SPICE file devices
spicegeneric.h
spicelibcomp.h
)



#QT4_WRAP_CPP(COMPONENTS_MOC_SRCS ${COMPONENTS_MOC_HDRS})

ADD_LIBRARY(spicecomponents ${COMPONENTS_HDRS} ${COMPONENTS_SRCS} ${COMPONENTS_MOC_SRCS} )

ADD_SUBDIRECTORY( symbols )




