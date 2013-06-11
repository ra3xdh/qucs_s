/*
 * bsim4v30nMOS.cpp - device implementations for bsim4v30nMOS module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#include "bsim4v30nMOS.h"

bsim4v30nMOS::bsim4v30nMOS()
{
  Description = QObject::tr ("bsim4v30nMOS verilog device");


 Props.append (new Property ("GMIN", "1e-12", false,
    QObject::tr ("-")));
  Props.append (new Property ("PS", "12e-6", false,
    QObject::tr ("-")));
  Props.append (new Property ("PD", "12e-6", false,
    QObject::tr ("-")));
  Props.append (new Property ("AS", "12e-12", false,
    QObject::tr ("-")));
  Props.append (new Property ("AD", "12e-12", false,
    QObject::tr ("-")));
  Props.append (new Property ("CGBO", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CGDO", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CGSO", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("L", "3e-6", false,
    QObject::tr ("-")));
  Props.append (new Property ("W", "6e-6", false,
    QObject::tr ("-")));
  Props.append (new Property ("MOBMOD", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("RDSMOD", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("IGCMOD", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("IGBMOD", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CAPMOD", "2", false,
    QObject::tr ("-")));
  Props.append (new Property ("RGATEMOD", "2", false,
    QObject::tr ("-")));
  Props.append (new Property ("RBODYMOD", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DIOMOD", "1", false,
    QObject::tr ("-")));
  Props.append (new Property ("TEMPMOD", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("GEOMOD", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("RGEOMOD", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PERMOD", "1", false,
    QObject::tr ("-")));
  Props.append (new Property ("TNOIMOD", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("FNOIMOD", "0", false,
    QObject::tr ("-")));
  Props.append (new Property ("EPSROX", "3.9", false,
    QObject::tr ("-")));
  Props.append (new Property ("TOXE", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("TOXP", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("TOXM", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DTOX", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("XJ", "1.5e-7", false,
    QObject::tr ("-")));
  Props.append (new Property ("GAMMA1", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("GAMMA2", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("NDEP", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("NSUB", "6.0e16", false,
    QObject::tr ("-")));
  Props.append (new Property ("NGATE", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("NSD", "1.0e20", false,
    QObject::tr ("-")));
  Props.append (new Property ("VBX", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("XT", "1.55e-7", false,
    QObject::tr ("-")));
  Props.append (new Property ("RSH", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("RSHG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("VTH0", "0.6", false,
    QObject::tr ("-")));
  Props.append (new Property ("VFB", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PHIN", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("K1", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("K2", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("K3", "80.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("K3B", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("W0", "2.5e-6", false,
    QObject::tr ("-")));
  Props.append (new Property ("LPE0", "1.74e-7", false,
    QObject::tr ("-")));
  Props.append (new Property ("LPEB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("VBM", "-3.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DVT0", "2.2", false,
    QObject::tr ("-")));
  Props.append (new Property ("DVT1", "0.53", false,
    QObject::tr ("-")));
  Props.append (new Property ("DVT2", "-0.032", false,
    QObject::tr ("-")));
  Props.append (new Property ("DVTP0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DVTP1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DVT0W", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DVT1W", "5.3e6", false,
    QObject::tr ("-")));
  Props.append (new Property ("DVT2W", "-0.032", false,
    QObject::tr ("-")));
  Props.append (new Property ("U0", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("UA", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("UB", "1.0e-19", false,
    QObject::tr ("-")));
  Props.append (new Property ("UC", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("EU", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("VSAT", "8.0e4", false,
    QObject::tr ("-")));
  Props.append (new Property ("A0", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("AGS", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("B0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("B1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("KETA", "-0.047", false,
    QObject::tr ("-")));
  Props.append (new Property ("A1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("A2", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WINT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LINT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DWG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DWB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("VOFF", "-0.08", false,
    QObject::tr ("-")));
  Props.append (new Property ("VOFFL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("MINV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("NFACTOR", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("ETA0", "0.08", false,
    QObject::tr ("-")));
  Props.append (new Property ("ETAB", "-0.07", false,
    QObject::tr ("-")));
  Props.append (new Property ("DROUT", "0.56", false,
    QObject::tr ("-")));
  Props.append (new Property ("DSUB", "0.56", false,
    QObject::tr ("-")));
  Props.append (new Property ("CIT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CDSC", "2.4e-4", false,
    QObject::tr ("-")));
  Props.append (new Property ("CDSCB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CDSCD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PCLM", "1.3", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDIBL1", "0.39", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDIBL2", "0.0086", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDIBLB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PSCBE1", "4.24e8", false,
    QObject::tr ("-")));
  Props.append (new Property ("PSCBE2", "1.0e-5", false,
    QObject::tr ("-")));
  Props.append (new Property ("PVAG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DELTA", "0.01", false,
    QObject::tr ("-")));
  Props.append (new Property ("FPROUT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDITS", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDITSD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PDITSL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LAMBDA", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("VTL", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LC", "5.0e-9", false,
    QObject::tr ("-")));
  Props.append (new Property ("XN", "3.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("RDSW", "200.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("RDSWMIN", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("RDW", "100.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("RDWMIN", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("RSW", "100.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("RSWMIN", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PRWG", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PRWB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WR", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("NRS", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("NRD", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("ALPHA0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("ALPHA1", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("BETA0", "30.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("AGIDL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("BGIDL", "2.3e9", false,
    QObject::tr ("-")));
  Props.append (new Property ("CGIDL", "0.5", false,
    QObject::tr ("-")));
  Props.append (new Property ("EGIDL", "0.8", false,
    QObject::tr ("-")));
  Props.append (new Property ("AIGBACC", "0.43", false,
    QObject::tr ("-")));
  Props.append (new Property ("BIGBACC", "0.054", false,
    QObject::tr ("-")));
  Props.append (new Property ("CIGBACC", "0.075", false,
    QObject::tr ("-")));
  Props.append (new Property ("NIGBACC", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("AIGBINV", "0.35", false,
    QObject::tr ("-")));
  Props.append (new Property ("BIGBINV", "0.03", false,
    QObject::tr ("-")));
  Props.append (new Property ("CIGBINV", "0.006", false,
    QObject::tr ("-")));
  Props.append (new Property ("EIGBINV", "1.1", false,
    QObject::tr ("-")));
  Props.append (new Property ("NIGBINV", "3.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("AIGC", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("BIGC", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CIGC", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("AIGSD", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("BIGSD", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CIGSD", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DLCIG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("NIGC", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("POXEDGE", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PIGCD", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("NTOX", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("TOXREF", "3.0e-9", false,
    QObject::tr ("-")));
  Props.append (new Property ("XPART", "0.4", false,
    QObject::tr ("-")));
  Props.append (new Property ("CGS0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CGD0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CGB0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CGSL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CGDL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CKAPPAS", "0.6", false,
    QObject::tr ("-")));
  Props.append (new Property ("CKAPPAD", "0.6", false,
    QObject::tr ("-")));
  Props.append (new Property ("CF", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CLC", "1.0e-7", false,
    QObject::tr ("-")));
  Props.append (new Property ("CLE", "0.6", false,
    QObject::tr ("-")));
  Props.append (new Property ("DLC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DWC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("VFBCV", "-1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("NOFF", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("VOFFCV", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("ACDE", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("MOIN", "15.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("XRCRG1", "12.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("XRCRG2", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("RBPB", "50.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("RBPD", "50.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("RBPS", "50.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("RBDB", "50.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("RBSB", "50.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("GBMIN", "1.0e-12", false,
    QObject::tr ("-")));
  Props.append (new Property ("DMCG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DMCI", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DMDG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DMCGT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("NF", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("DWJ", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("MIN", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("XGW", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("XGL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("XL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("XW", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("NGCON", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("IJTHSREV", "0.1", false,
    QObject::tr ("-")));
  Props.append (new Property ("IJTHDREV", "0.1", false,
    QObject::tr ("-")));
  Props.append (new Property ("IJTHSFWD", "0.1", false,
    QObject::tr ("-")));
  Props.append (new Property ("IJTHDFWD", "0.1", false,
    QObject::tr ("-")));
  Props.append (new Property ("XJBVS", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("XJBVD", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("BVS", "10.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("BVD", "10.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("JSS", "1.0e-4", false,
    QObject::tr ("-")));
  Props.append (new Property ("JSD", "1.0e-4", false,
    QObject::tr ("-")));
  Props.append (new Property ("JSWS", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("JSWD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("JSWGS", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("JSWGD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("CJS", "5.0e-4", false,
    QObject::tr ("-")));
  Props.append (new Property ("CJD", "5.0e-4", false,
    QObject::tr ("-")));
  Props.append (new Property ("MJS", "0.5", false,
    QObject::tr ("-")));
  Props.append (new Property ("MJD", "0.5", false,
    QObject::tr ("-")));
  Props.append (new Property ("MJSWS", "0.33", false,
    QObject::tr ("-")));
  Props.append (new Property ("MJSWD", "0.33", false,
    QObject::tr ("-")));
  Props.append (new Property ("CJSWS", "5.0e-10", false,
    QObject::tr ("-")));
  Props.append (new Property ("CJSWD", "5.0e-10", false,
    QObject::tr ("-")));
  Props.append (new Property ("CJSWGS", "5.0e-10", false,
    QObject::tr ("-")));
  Props.append (new Property ("CJSWGD", "5.0e-10", false,
    QObject::tr ("-")));
  Props.append (new Property ("MJSWGS", "0.33", false,
    QObject::tr ("-")));
  Props.append (new Property ("MJSWGD", "0.33", false,
    QObject::tr ("-")));
  Props.append (new Property ("PBS", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PBD", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PBSWS", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PBSWD", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PBSWGS", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PBSWGD", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("TNOM", "27", false,
    QObject::tr ("-")));
  Props.append (new Property ("UTE", "-1.5", false,
    QObject::tr ("-")));
  Props.append (new Property ("KT1", "-0.11", false,
    QObject::tr ("-")));
  Props.append (new Property ("KT1L", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("KT2", "0.022", false,
    QObject::tr ("-")));
  Props.append (new Property ("UA1", "1.0e-9", false,
    QObject::tr ("-")));
  Props.append (new Property ("UB1", "-1.0e-18", false,
    QObject::tr ("-")));
  Props.append (new Property ("UC1", "-99.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("AT", "3.3e4", false,
    QObject::tr ("-")));
  Props.append (new Property ("PRT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("NJS", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("NJD", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("XTIS", "3.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("XTID", "3.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("TPB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("TPBSW", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("TPBSWG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("TCJ", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("TCJSW", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("TCJSWG", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("SA", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("SB", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("SD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("SAREF", "1e-6", false,
    QObject::tr ("-")));
  Props.append (new Property ("SBREF", "1e-6", false,
    QObject::tr ("-")));
  Props.append (new Property ("WLOD", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("KU0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("KVSAT", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("TKU0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LKU0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WKU0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PKU0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LLODKU0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WLODKU0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("KVTH0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LKVTH0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WKVTH0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("PKVTH0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LLODVTH", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WLODVTH", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("STK2", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LODK2", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("STETA0", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LODETA0", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WLN", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WW", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WWN", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WWL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LLN", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LW", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LWN", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LWL", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LLC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LWC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("LWLC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WLC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WWC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("WWLC", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("NTNOI", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("KF", "0.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("AF", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("EF", "1.0", false,
    QObject::tr ("-")));
  Props.append (new Property ("TEMP", "27", false,
    QObject::tr ("-")));

  createSymbol ();
  tx = x2 + 4;
  ty = y1 + 4;
  Model = "bsim4v30nMOS";
  Name  = "BSIM4_";
}

Component * bsim4v30nMOS::newOne()
{
  bsim4v30nMOS * p = new bsim4v30nMOS();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * bsim4v30nMOS::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("bsim4v30nMOS");
  BitmapFile = (char *) "bsim4v30nMOS";

  if(getNewOne) return new bsim4v30nMOS();
  return 0;
}

void bsim4v30nMOS::createSymbol()
{
  // put in here symbol drawing code and terminal definitions
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
  Lines.append(new Line( -9,  0, -4, -5,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -9,  0, -4,  5,QPen(Qt::darkBlue,2)));
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
