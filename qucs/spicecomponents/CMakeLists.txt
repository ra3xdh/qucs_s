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
sp_customsim.cpp
sp_fourier.cpp
sp_disto.cpp
sp_noise.cpp
sp_pz.cpp

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

Cmeter_SPICE.cpp
SE_V_amp.cpp 
DE_V_amp.cpp
SE_V_int.cpp
DE_V_int.cpp
SE_V_diff.cpp
DE_V_diff.cpp
ABV.cpp
ABCV.cpp
APBV.cpp
APBPCV.cpp
DivV.cpp
Icouple.cpp
core.cpp
SDTF.cpp

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

Cmeter_SPICE.h
SE_V_amp.h
DE_V_amp.h
SE_V_int.h
DE_V_int.h
SE_V_diff.h
DE_V_diff.h
ABV.h
ABCV.h
APBV.h
APBPCV.h
DivV.h
Icouple.h
core.h
SDTF.h
)



#QT4_WRAP_CPP(COMPONENTS_MOC_SRCS ${COMPONENTS_MOC_HDRS})

ADD_LIBRARY(spicecomponents ${COMPONENTS_HDRS} ${COMPONENTS_SRCS} ${COMPONENTS_MOC_SRCS} )





