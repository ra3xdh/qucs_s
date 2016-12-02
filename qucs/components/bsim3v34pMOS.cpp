/*
 * bsim3v34pMOS.cpp - device implementations for bsim3v34pMOS module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#include "bsim3v34pMOS.h"

bsim3v34pMOS::bsim3v34pMOS()
{
  Description = QObject::tr ("bsim3v34pMOS verilog device");

  Props.append (new Property ("L", "3.5e-6", false,
    QObject::tr ("-")));
  Props.append (new Property ("W", "5.0e-6", false,
    QObject::tr ("-")));
  Props.append (new Property ("PS", "8.0e-6", false,
    QObject::tr ("-")));
  Props.append (new Property ("PD", "8.0e-6", false,
    QObject::tr ("-")));
  Props.append (new Property ("AS", "12.0e-12", false,
    QObject::tr ("-")));
  Props.append (new Property ("AD", "12.0e-12", false,
    QObject::tr ("-")));
  Props.append (new Property ("NRS", "10.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("NRD", "10.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("NQSMOD", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("GMIN", "1e-12", false,
    QObject::tr ("-")));
  Props.append (new Property ("VERSION", "3.24", false,
    QObject::tr ("-")));
  Props.append (new Property ("PARAMCHK", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("MOBMOD", "1", false,
    QObject::tr ("-")));
  Props.append (new Property ("CAPMOD", "3", false,
    QObject::tr ("-")));
  Props.append (new Property ("NOIMOD", "4", false,
    QObject::tr ("-")));
  Props.append (new Property ("BINUNIT", "1", false,
    QObject::tr ("-")));
  Props.append (new Property ("TOX", "150.0e-10", false,
    QObject::tr ("-")));
  Props.append (new Property ("TOXM", "150.0e-10", false,
    QObject::tr ("-")));
  Props.append (new Property ("CDSC", "2.4e-4", false,
    QObject::tr ("-")));
  Props.append (new Property ("CDSCB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CDSCD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CIT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("NFACTOR", "1", false,
    QObject::tr ("-")));
  Props.append (new Property ("XJ", "0.15e-6", false,
    QObject::tr ("-")));
  Props.append (new Property ("VSAT", "8.0e4", false,
    QObject::tr ("-")));
  Props.append (new Property ("AT", "3.3e4", false,
    QObject::tr ("-")));
  Props.append (new Property ("A0", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("AGS", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("A1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("A2", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("KETA", "-0.047", false,
    QObject::tr ("-")));
  Props.append (new Property ("NSUB", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("NCH", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("NGATE", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("GAMMA1", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("GAMMA2", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("VBX", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("VBM", "-3.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("XT", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("K1", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("KT1", "-0.11", false,
    QObject::tr ("-")));
  Props.append (new Property ("KT1L", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("KT2", "0.022", false,
    QObject::tr ("-")));
  Props.append (new Property ("K2", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("K3", "80.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("K3B", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("W0", "2.5e-6", false,
    QObject::tr ("-")));
  Props.append (new Property ("NLX", "1.74e-7", false,
    QObject::tr ("-")));
  Props.append (new Property ("DVT0", "2.2", false,
    QObject::tr ("-")));
  Props.append (new Property ("DVT1", "0.53", false,
    QObject::tr ("-")));
  Props.append (new Property ("DVT2", "-0.032", false,
    QObject::tr ("-")));
  Props.append (new Property ("DVT0W", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DVT1W", "5.3e6", false,
    QObject::tr ("-")));
  Props.append (new Property ("DVT2W", "-0.032", false,
    QObject::tr ("-")));
  Props.append (new Property ("DROUT", "0.56", false,
    QObject::tr ("-")));
  Props.append (new Property ("DSUB", "0.56", false,
    QObject::tr ("-")));
  Props.append (new Property ("VTHO", "-0.7", false,
    QObject::tr ("-")));
  Props.append (new Property ("VTH0", "-0.7", false,
    QObject::tr ("-")));
  Props.append (new Property ("UA", "2.25e-9", false,
    QObject::tr ("-")));
  Props.append (new Property ("UA1", "4.31e-9", false,
    QObject::tr ("-")));
  Props.append (new Property ("UB", "5.87e-19", false,
    QObject::tr ("-")));
  Props.append (new Property ("UB1", "-7.61e-18", false,
    QObject::tr ("-")));
  Props.append (new Property ("UC", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("UC1", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("U0", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("UTE", "-1.5", false,
    QObject::tr ("-")));
  Props.append (new Property ("VOFF", "-0.08", false,
    QObject::tr ("-")));
  Props.append (new Property ("TNOM", "26.85", false,
    QObject::tr ("-")));
  Props.append (new Property ("CGSO", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CGDO", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CGBO", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("XPART", "0.4", false,
    QObject::tr ("-")));
  Props.append (new Property ("ELM", "5.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DELTA", "0.01", false,
    QObject::tr ("-")));
  Props.append (new Property ("RSH", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("RDSW", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PRWG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PRWB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PRT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("ETA0", "0.08", false,
    QObject::tr ("-")));
  Props.append (new Property ("ETAB", "-0.07", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCLM", "1.3", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDIBLC1", "0.39", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDIBLC2", "0.0086", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDIBLCB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PSCBE1", "4.24e8", false,
    QObject::tr ("-")));
  Props.append (new Property ("PSCBE2", "1.0e-5", false,
    QObject::tr ("-")));
  Props.append (new Property ("PVAG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("JS", "1.0E-4", false,
    QObject::tr ("-")));
  Props.append (new Property ("JSW", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PB", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("NJ", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("XTI", "3.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("MJ", "0.5", false,
    QObject::tr ("-")));
  Props.append (new Property ("PBSW", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("MJSW", "0.33", false,
    QObject::tr ("-")));
  Props.append (new Property ("PBSWG", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("MJSWG", "0.33", false,
    QObject::tr ("-")));
  Props.append (new Property ("CJ", "5.0E-4", false,
    QObject::tr ("-")));
  Props.append (new Property ("VFBCV", "-1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("VFB", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CJSW", "5.0E-10", false,
    QObject::tr ("-")));
  Props.append (new Property ("CJSWG", "5.0e-10", false,
    QObject::tr ("-")));
  Props.append (new Property ("TPB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("TCJ", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("TPBSW", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("TCJSW", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("TPBSWG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("TCJSWG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("ACDE", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("MOIN", "15.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("NOFF", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("VOFFCV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LINT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LLC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LLN", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LW", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LWC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LWN", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LWL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LWLC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LMIN", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LMAX", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WR", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WINT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DWG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DWB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WLC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WLN", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WW", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WWC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WWN", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WWL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WWLC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WMIN", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WMAX", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("B0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("B1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CGSL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CGDL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CKAPPA", "0.6", false,
    QObject::tr ("-")));
  Props.append (new Property ("CF", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CLC", "0.1e-6", false,
    QObject::tr ("-")));
  Props.append (new Property ("CLE", "0.6", false,
    QObject::tr ("-")));
  Props.append (new Property ("DWC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DLC", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("ALPHA0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("ALPHA1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("BETA0", "30.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("IJTH", "0.1", false,
    QObject::tr ("-")));
  Props.append (new Property ("LCDSC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LCDSCB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LCDSCD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LCIT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LNFACTOR", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LXJ", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LVSAT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LAT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LA0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LAGS", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LA1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LA2", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LKETA", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LNSUB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LNCH", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LNGATE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LGAMMA1", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LGAMMA2", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LVBX", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LVBM", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LXT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LK1", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LKT1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LKT1L", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LKT2", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LK2", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LK3", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LK3B", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LW0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LNLX", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LDVT0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LDVT1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LDVT2", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LDVT0W", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LDVT1W", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LDVT2W", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LDROUT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LDSUB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LVTH0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LVTHO", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LUA", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LUA1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LUB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LUB1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LUC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LUC1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LU0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LUTE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LVOFF", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LELM", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LDELTA", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LRDSW", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LPRWG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LPRWB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LPRT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LETA0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LETAB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LPCLM", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LPDIBLC1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LPDIBLC2", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LPDIBLCB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LPSCBE1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LPSCBE2", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LPVAG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LWR", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LDWG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LDWB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LB0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LB1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LCGSL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LCGDL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LCKAPPA", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LCF", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LCLC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LCLE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LALPHA0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LALPHA1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LBETA0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LVFBCV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LVFB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LACDE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LMOIN", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LNOFF", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LVOFFCV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WCDSC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WCDSCB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WCDSCD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WCIT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WNFACTOR", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WXJ", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WVSAT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WAT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WA0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WAGS", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WA1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WA2", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WKETA", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WNSUB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WNCH", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WNGATE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WGAMMA1", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WGAMMA2", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WVBX", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WVBM", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WXT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WK1", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WKT1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WKT1L", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WKT2", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WK2", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WK3", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WK3B", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WW0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WNLX", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WDVT0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WDVT1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WDVT2", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WDVT0W", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WDVT1W", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WDVT2W", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WDROUT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WDSUB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WVTH0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WVTHO", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WUA", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WUA1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WUB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WUB1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WUC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WUC1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WU0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WUTE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WVOFF", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WELM", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WDELTA", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WRDSW", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPRWG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPRWB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPRT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WETA0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WETAB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPCLM", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPDIBLC1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPDIBLC2", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPDIBLCB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPSCBE1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPSCBE2", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WPVAG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WWR", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WDWG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WDWB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WB0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WB1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WCGSL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WCGDL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WCKAPPA", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WCF", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WCLC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WCLE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WALPHA0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WALPHA1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WBETA0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WVFBCV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WVFB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WACDE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WMOIN", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WNOFF", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WVOFFCV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCDSC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCDSCB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCDSCD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCIT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PNFACTOR", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PXJ", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PVSAT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PAT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PA0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PAGS", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PA1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PA2", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PKETA", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PNSUB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PNCH", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PNGATE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PGAMMA1", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PGAMMA2", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PVBX", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PVBM", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PXT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PK1", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PKT1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PKT1L", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PKT2", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PK2", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PK3", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PK3B", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PW0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PNLX", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDVT0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDVT1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDVT2", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDVT0W", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDVT1W", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDVT2W", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDROUT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDSUB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PVTH0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PVTHO", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PUA", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PUA1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PUB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PUB1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PUC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PUC1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PU0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PUTE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PVOFF", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PELM", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDELTA", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PRDSW", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPRWG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPRWB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPRT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PETA0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PETAB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPCLM", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPDIBLC1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPDIBLC2", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPDIBLCB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPSCBE1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPSCBE2", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PPVAG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PWR", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDWG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDWB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PB0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PB1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCGSL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCGDL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCKAPPA", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCF", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCLC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCLE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PALPHA0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PALPHA1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PBETA0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PVFBCV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PVFB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PACDE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PMOIN", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PNOFF", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PVOFFCV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("KF", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("AF", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("EF", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("Temp", "26.85", false,
    QObject::tr ("simulation temperature")));

  createSymbol ();
  tx = x2 + 4;
  ty = y1 + 4;
  Model = "bsim3v34pMOS";
  Name  = "BSIM3_";
}

Component * bsim3v34pMOS::newOne()
{
  bsim3v34pMOS * p = new bsim3v34pMOS();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * bsim3v34pMOS::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("bsim3v34pMOS");
  BitmapFile = (char *) "pmosfet_sub";

  if(getNewOne) return new bsim3v34pMOS();
  return 0;
}

void bsim3v34pMOS::createSymbol()
{

 Lines.append(new Line(-14,-13,-14, 13,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(-30,  0,-14,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-10,-11,  0,-11,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0,-11,  0,-30,QPen(Qt::darkBlue,2)));
  
  Lines.append(new Line(-10, 11,  0, 11,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0, 11,  0, 30,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-10,  0, 20,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-10,-16,-10, -7,QPen(Qt::darkBlue,3)));
  
  Lines.append(new Line(-10,  7,-10, 16,QPen(Qt::darkBlue,3)));
  Lines.append(new Line( -4, 24,  4, 20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -5,  5,  0,   0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -5,  -5, 0,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-10, -3,-10,  3,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(-10, -8,-10, -6,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(-10,  8,-10,  6,QPen(Qt::darkBlue,3)));


  Ports.append(new Port(  0,-30)); // drain
  Ports.append(new Port(-30,  0)); // gate
  Ports.append(new Port(  0, 30)); // source
  Ports.append(new Port( 20,  0)); // bulk

  x1 = -30; y1 = -30;
  x2 =  20; y2 =  30;
}
