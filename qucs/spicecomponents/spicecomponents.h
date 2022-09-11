/***************************************************************************
                               spicecomponents.h
                              --------------
    begin                : Sun 17 2015
    copyright            : (C) 2015 by Vadim Kuznetsov and Mike Brinson
    email                : ra3xdh@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SPICECOMPONENTS_H
#define SPICECOMPONENTS_H
//
// Qucs-S ngspice and Xyce components
#include "S4Q_V.h"
#include "S4Q_Ieqndef.h"
#include "S4Q_I.h"
#include "S4Q_S.h"
#include "S4Q_W.h"
//
//
#include "src_eqndef.h"
#include "vsffm.h"
#include "isffm.h"
#include "vAmpMod.h"
#include "iAmpMod.h"
#include "vPWL.h"
#include "iPWL.h"
#include "eNL.h"
#include "gNL.h"
#include "vTRNOISE.h"
#include "iTRNOISE.h"
#include "vTRRANDOM.h"
#include "C_SPICE.h"
#include "L_SPICE.h"
#include "R_SPICE.h"
#include "K_SPICE.h"
#include "LTL_SPICE.h"
#include "UDRCTL_SPICE.h"
#include "LTRA_SPICE.h"
#include "volt_ac_SPICE.h"
#include "DIODE_SPICE.h"
#include "NPN_SPICE.h"
#include "PNP_SPICE.h"
#include "NJF_SPICE.h"
#include "PJF_SPICE.h"
#include "NMOS_SPICE.h"
#include "PMOS_SPICE.h"
#include "MOS_SPICE.h"
#include "MESFET_SPICE.h"
#include "PMF_MESFET_SPICE.h"

// XSPICE analogue blocks
#include "Icouple.h"
#include "core.h"
#include "SDTF.h"
#include "XAPWL.h"

// Spice netlist sections
#include "sp_parameter.h"
#include "sp_globalpar.h"
#include "sp_options.h"
#include "sp_nutmeg.h"
#include "sp_nodeset.h"
#include "sp_ic.h"
#include "sp_model.h"
#include "sp_include.h"
#include "sp_func.h"
#include "sp_spiceinit.h"
#include "incl_script.h"

// Spice simulations
#include "sp_fourier.h"
#include "sp_disto.h"
#include "sp_customsim.h"
#include "sp_noise.h"
#include "sp_pz.h"
#include "xyce_script.h"
#include "sp_sens.h"
#include "sp_sens_ac.h"
#include "sp_sens_xyce.h"
#include "sp_sens_tr_xyce.h"
#include "sp_spectrum.h"

// XSpice file components
#include "xspicegeneric.h"
#include "xsp_cmlib.h"
#include "xsp_codemodel.h"

// SPICE file devices
#include "spicegeneric.h"
#include "spicelibcomp.h"

#endif
