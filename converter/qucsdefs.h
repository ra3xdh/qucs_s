/*
 * qucsdefs.h - netlist definitions for the Qucs netlists
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef __QUCSDEFS_H__
#define __QUCSDEFS_H__

static struct property_t req_MLIN[] = {
  { "W", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "L", 1, { 0.01, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Subst", 2, { 0, "Subst1" }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "DispModel", 2, { 0, "Kirschning" }, { '.', 0, 0, '.',
    { "Kirschning", "Kobayashi", "Yamashita", "Getsinger", "Schneider", "Pramanick", "Hammerstad", ((char *) 0) } } },
  { "Model", 2, { 0, "Hammerstad" }, { '.', 0, 0, '.',
    { "Wheeler", "Schneider", "Hammerstad", ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_MLIN[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_MLIN = {
  "MLIN", 2, 0, 0, 0, req_MLIN, opt_MLIN };

static struct property_t req_Buf[] = {
  { "V", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Buf[] = {
  { "t", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "TR", 1, { 10, ((char *) -1) }, { '[', 1, 100, ']',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Buf = {
  "Buf", 2, 0, 0, 1, req_Buf, opt_Buf };

static struct property_t req_nigbt[] = {
  { "Agd", 1, { 5e-06, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "Area", 1, { 1e-05, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "Kp", 1, { 0.38, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Tau", 1, { 7.1e-06, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Wb", 1, { 9e-05, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "BVf", 1, { 1, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "BVn", 1, { 4, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "Cgs", 1, { 1.24e-08, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Coxd", 1, { 3.5e-08, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Jsne", 1, { 6.5e-13, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Kf", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Mun", 1, { 0.0015, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Mup", 1, { 0.045, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Nb", 1, { 2e+14, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Theta", 1, { 0.02, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Vt", 1, { 4.7, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Vtd", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Tnom", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_nigbt[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_nigbt = {
  "nigbt", 3, 0, 0, 1, req_nigbt, opt_nigbt };

static struct property_t req_Iac[] = {
  { "I", 1, { 0.001, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Iac[] = {
  { "Phase", 1, { 0, ((char *) -1) }, { '[', -360, 360, ']',
    { ((char *) 0) } } },
  { "Theta", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "f", 1, { 1e+09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Iac = {
  "Iac", 2, 0, 0, 0, req_Iac, opt_Iac };

static struct property_t req_AM_Mod[] = {
  { "U", 1, { 1, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "f", 1, { 1e+09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "m", 1, { 1, ((char *) -1) }, { '[', 0, 1, ']',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_AM_Mod[] = {
  { "Phase", 1, { 0, ((char *) -1) }, { '[', -360, 360, ']',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_AM_Mod = {
  "AM_Mod", 3, 0, 0, 0, req_AM_Mod, opt_AM_Mod };

static struct property_t req_TLIN4P[] = {
  { "Z", 1, { 50, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "L", 1, { 0.001, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_TLIN4P[] = {
  { "Alpha", 1, { 1, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_TLIN4P = {
  "TLIN4P", 4, 0, 0, 0, req_TLIN4P, opt_TLIN4P };

static struct property_t req_SPIRALIND[] = {
  { "Subst", 2, { 0, "Subst1" }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Geometry", 2, { 0, "Circular" }, { '.', 0, 0, '.',
    { "Circular", "Square", "Hexagonal", "Octogonal", ((char *) 0) } } },
  { "W", 1, { 2.5e-05, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Di", 1, { 0.0002, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "S", 1, { 2.5e-05, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "N", 1, { 3, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_SPIRALIND[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_SPIRALIND = {
  "SPIRALIND", 2, 0, 0, 0, req_SPIRALIND, opt_SPIRALIND };

static struct property_t req_MCOUPLED[] = {
  { "W", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "L", 1, { 0.01, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "S", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Subst", 2, { 0, "Subst1" }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Model", 2, { 0, "Kirschning" }, { '.', 0, 0, '.',
    { "Kirschning", "Hammerstad", ((char *) 0) } } },
  { "DispModel", 2, { 0, "Kirschning" }, { '.', 0, 0, '.',
    { "Kirschning", "Getsinger", ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_MCOUPLED[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_MCOUPLED = {
  "MCOUPLED", 4, 0, 0, 0, req_MCOUPLED, opt_MCOUPLED };

static struct property_t req_MOSFET[] = {
  { "Is", 1, { 1e-14, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "N", 1, { 1, ((char *) -1) }, { '[', 0.1, 100, ']',
    { ((char *) 0) } } },
  { "Vt0", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Lambda", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Kp", 1, { 2e-05, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Gamma", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Phi", 1, { 0.6, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_MOSFET[] = {
  { "Rd", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Rs", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Rg", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "L", 1, { 0.0001, ((char *) -1) }, { ']', 0, 1, ']',
    { ((char *) 0) } } },
  { "Ld", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "W", 1, { 0.0001, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "Tox", 1, { 1e-07, ((char *) -1) }, { ']', 0, 1, ']',
    { ((char *) 0) } } },
  { "Cgso", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Cgdo", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Cgbo", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Cbd", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Cbs", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Pb", 1, { 0.8, ((char *) -1) }, { ']', 0, 10, ']',
    { ((char *) 0) } } },
  { "Mj", 1, { 0.5, ((char *) -1) }, { '[', 0, 1, ']',
    { ((char *) 0) } } },
  { "Fc", 1, { 0.5, ((char *) -1) }, { '[', 0, 1, '[',
    { ((char *) 0) } } },
  { "Cjsw", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Mjsw", 1, { 0.33, ((char *) -1) }, { '[', 0, 1, ']',
    { ((char *) 0) } } },
  { "Tt", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Kf", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Af", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Ffe", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Nsub", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Nss", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Tpg", 0, { 1, ((char *) -1) }, { '[', -1, 1, ']',
    { ((char *) 0) } } },
  { "Uo", 1, { 600, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Rsh", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Nrd", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Nrs", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Cj", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Js", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Ad", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "As", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Pd", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Ps", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { "Tnom", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { "Type", 2, { 0, "nfet" }, { '.', 0, 0, '.',
    { "nfet", "pfet", ((char *) 0) } } },
  { "capModel", 0, { 2, ((char *) -1) }, { '[', 1, 2, ']',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_MOSFET = {
  "MOSFET", 4, 0, 0, 1, req_MOSFET, opt_MOSFET };

static struct property_t req_Inv[] = {
  { "V", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Inv[] = {
  { "t", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "TR", 1, { 10, ((char *) -1) }, { '[', 1, 100, ']',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Inv = {
  "Inv", 2, 0, 0, 1, req_Inv, opt_Inv };

static struct property_t req_comp_2bit[] = {
  { "TR", 1, { 6, ((char *) -1) }, { '[', 1, 20, ']',
    { ((char *) 0) } } },
  { "Delay", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_comp_2bit[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_comp_2bit = {
  "comp_2bit", 7, 0, 0, 1, req_comp_2bit, opt_comp_2bit };

static struct property_t req_Idc[] = {
  { "I", 1, { 0.001, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Idc[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Idc = {
  "Idc", 2, 0, 0, 0, req_Idc, opt_Idc };

static struct property_t req_CIRCULARLOOP[] = {
  { "Subst", 2, { 0, "Subst1" }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "W", 1, { 2.5e-05, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "a", 1, { 0.0002, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_CIRCULARLOOP[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_CIRCULARLOOP = {
  "CIRCULARLOOP", 2, 0, 0, 0, req_CIRCULARLOOP, opt_CIRCULARLOOP };

static struct property_t req_DigiSource[] = {
  { "init", 2, { 0, "low" }, { '.', 0, 0, '.',
    { "low", "high", ((char *) 0) } } },
  { "times", 3, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_DigiSource[] = {
  { "V", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_DigiSource = {
  "DigiSource", 1, 0, 0, 0, req_DigiSource, opt_DigiSource };

static struct property_t req_Isolator[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Isolator[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { "Z1", 1, { 50, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Z2", 1, { 50, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Isolator = {
  "Isolator", 2, 0, 0, 0, req_Isolator, opt_Isolator };

static struct property_t req_phototransistor[] = {
  { "Bf", 1, { 100, ((char *) -1) }, { '[', 1, 0, '.',
    { ((char *) 0) } } },
  { "Br", 1, { 0.1, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "Is", 1, { 1e-10, ((char *) -1) }, { '[', 1e-30, 0, '.',
    { ((char *) 0) } } },
  { "Nf", 1, { 1, ((char *) -1) }, { '[', 1e-30, 0, '.',
    { ((char *) 0) } } },
  { "Nr", 1, { 1, ((char *) -1) }, { '[', 1e-30, 0, '.',
    { ((char *) 0) } } },
  { "Vaf", 1, { 100, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "Var", 1, { 100, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "Mje", 1, { 0.33, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "Vje", 1, { 0.75, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "Cje", 1, { 1e-12, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Mjc", 1, { 0.33, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "Vjc", 1, { 0.75, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "Cjc", 1, { 2e-12, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Tr", 1, { 1e-07, ((char *) -1) }, { '[', 1e-15, 0, '.',
    { ((char *) 0) } } },
  { "Tf", 1, { 1e-10, ((char *) -1) }, { '[', 1e-15, 0, '.',
    { ((char *) 0) } } },
  { "Ikf", 1, { 10, ((char *) -1) }, { '[', 1e-20, 0, '.',
    { ((char *) 0) } } },
  { "Ikr", 1, { 10, ((char *) -1) }, { '[', 1e-20, 0, '.',
    { ((char *) 0) } } },
  { "Rc", 1, { 10, ((char *) -1) }, { '[', 1e-09, 0, '.',
    { ((char *) 0) } } },
  { "Re", 1, { 1, ((char *) -1) }, { '[', 1e-09, 0, '.',
    { ((char *) 0) } } },
  { "Rb", 1, { 100, ((char *) -1) }, { '[', 1e-09, 0, '.',
    { ((char *) 0) } } },
  { "Kf", 1, { 1e-12, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Ffe", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Af", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Responsivity", 1, { 1.5, ((char *) -1) }, { '[', 1e-30, 0, '.',
    { ((char *) 0) } } },
  { "P0", 1, { 2612.2, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P1", 1, { -14.89, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P2", 1, { 0.030332, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P3", 1, { -2.5708e-05, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P4", 1, { 7.6923e-09, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_phototransistor[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_phototransistor = {
  "phototransistor", 5, 0, 0, 1, req_phototransistor, opt_phototransistor };

static struct property_t req_sTr[] = {
  { "T1", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "T2", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_sTr[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_sTr = {
  "sTr", 6, 0, 0, 0, req_sTr, opt_sTr };

static struct property_t req_JFET[] = {
  { "Is", 1, { 1e-14, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "N", 1, { 1, ((char *) -1) }, { '[', 1, 100, ']',
    { ((char *) 0) } } },
  { "Vt0", 1, { -2, ((char *) -1) }, { '.', 0, 0, ']',
    { ((char *) 0) } } },
  { "Lambda", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Beta", 1, { 0.0001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "M", 1, { 0.5, ((char *) -1) }, { '[', 0, 1, ']',
    { ((char *) 0) } } },
  { "Pb", 1, { 1, ((char *) -1) }, { ']', 0, 10, ']',
    { ((char *) 0) } } },
  { "Fc", 1, { 0.5, ((char *) -1) }, { '[', 0, 1, '[',
    { ((char *) 0) } } },
  { "Cgs", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Cgd", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_JFET[] = {
  { "Rd", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Rs", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Isr", 1, { 1e-14, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Nr", 1, { 2, ((char *) -1) }, { '[', 1, 100, ']',
    { ((char *) 0) } } },
  { "Kf", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Af", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Ffe", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { "Type", 2, { 0, "nfet" }, { '.', 0, 0, '.',
    { "nfet", "pfet", ((char *) 0) } } },
  { "Xti", 1, { 3, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Vt0tc", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Betatce", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Tnom", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { "Area", 1, { 1, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_JFET = {
  "JFET", 3, 0, 0, 1, req_JFET, opt_JFET };

static struct property_t req_comp_1bit[] = {
  { "TR", 1, { 6, ((char *) -1) }, { '[', 1, 20, ']',
    { ((char *) 0) } } },
  { "Delay", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_comp_1bit[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_comp_1bit = {
  "comp_1bit", 5, 0, 0, 1, req_comp_1bit, opt_comp_1bit };

static struct property_t req_SCR[] = {
  { "Igt", 1, { 5e-05, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "Vbo", 1, { 30, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_SCR[] = {
  { "Cj0", 1, { 1e-11, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Is", 1, { 1e-10, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "N", 1, { 2, ((char *) -1) }, { '[', 0.1, 100, ']',
    { ((char *) 0) } } },
  { "Ri", 1, { 10, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "Rg", 1, { 5, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_SCR = {
  "SCR", 3, 0, 0, 1, req_SCR, opt_SCR };

static struct property_t req_Diode[] = {
  { "Is", 1, { 1e-15, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "N", 1, { 1, ((char *) -1) }, { '[', 1e-06, 100, ']',
    { ((char *) 0) } } },
  { "M", 1, { 0.5, ((char *) -1) }, { '[', 0, 2, ']',
    { ((char *) 0) } } },
  { "Cj0", 1, { 1e-14, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Vj", 1, { 0.7, ((char *) -1) }, { ']', 0, 10, ']',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Diode[] = {
  { "Rs", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Isr", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Nr", 1, { 2, ((char *) -1) }, { '[', 0.1, 100, ']',
    { ((char *) 0) } } },
  { "Bv", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Ibv", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Ikf", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Tt", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Fc", 1, { 0.5, ((char *) -1) }, { '[', 0, 1, '[',
    { ((char *) 0) } } },
  { "Cp", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Kf", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Af", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Ffe", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { "Xti", 1, { 3, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Eg", 1, { 1.11, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Tbv", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Trs", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Ttt1", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Ttt2", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Tm1", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Tm2", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Tnom", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { "Area", 1, { 1, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Diode = {
  "Diode", 2, 0, 0, 1, req_Diode, opt_Diode };

static struct property_t req_MLANGE[] = {
  { "W", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "L", 1, { 0.01, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "S", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Subst", 2, { 0, "Subst1" }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Model", 2, { 0, "Kirschning" }, { '.', 0, 0, '.',
    { "Kirschning", "Hammerstad", ((char *) 0) } } },
  { "DispModel", 2, { 0, "Kirschning" }, { '.', 0, 0, '.',
    { "Kirschning", "Getsinger", ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_MLANGE[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_MLANGE = {
  "MLANGE", 4, 0, 0, 0, req_MLANGE, opt_MLANGE };

static struct property_t req_MCORN[] = {
  { "W", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Subst", 2, { 0, "Subst1" }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_MCORN[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_MCORN = {
  "MCORN", 2, 0, 0, 0, req_MCORN, opt_MCORN };

static struct property_t req_greytobinary4bit[] = {
  { "TR", 1, { 6, ((char *) -1) }, { '[', 1, 20, ']',
    { ((char *) 0) } } },
  { "Delay", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_greytobinary4bit[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_greytobinary4bit = {
  "greytobinary4bit", 8, 0, 0, 1, req_greytobinary4bit, opt_greytobinary4bit };

static struct property_t req_Inoise[] = {
  { "i", 1, { 1e-06, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Inoise[] = {
  { "a", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "c", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "e", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Inoise = {
  "Inoise", 2, 0, 0, 0, req_Inoise, opt_Inoise };

static struct property_t req_mux8to1[] = {
  { "TR", 1, { 6, ((char *) -1) }, { '[', 1, 20, ']',
    { ((char *) 0) } } },
  { "Delay", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_mux8to1[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_mux8to1 = {
  "mux8to1", 13, 0, 0, 1, req_mux8to1, opt_mux8to1 };

static struct property_t req_MRSTUB[] = {
  { "ri", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "ro", 1, { 0.01, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "alpha", 1, { 90, ((char *) -1) }, { '[', 0, 180, ']',
    { ((char *) 0) } } },
  { "Subst", 2, { 0, "Subst1" }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_MRSTUB[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_MRSTUB = {
  "MRSTUB", 1, 0, 0, 0, req_MRSTUB, opt_MRSTUB };

static struct property_t req_CSTEP[] = {
  { "W1", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "W2", 1, { 0.002, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "S", 1, { 0.004, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Subst", 2, { 0, "Subst1" }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_CSTEP[] = {
  { "Backside", 2, { 0, "Metal" }, { '.', 0, 0, '.',
    { "Metal", "Air", ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_CSTEP = {
  "CSTEP", 2, 0, 0, 0, req_CSTEP, opt_CSTEP };

static struct property_t req_Attenuator[] = {
  { "L", 1, { 10, ((char *) -1) }, { '[', 1, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Attenuator[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { "Zref", 1, { 50, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Attenuator = {
  "Attenuator", 2, 0, 0, 0, req_Attenuator, opt_Attenuator };

static struct property_t req_Vexp[] = {
  { "U1", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "U2", 1, { 1, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "T1", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "T2", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Vexp[] = {
  { "Tr", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Tf", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Vexp = {
  "Vexp", 2, 0, 0, 0, req_Vexp, opt_Vexp };

static struct property_t req_RTD[] = {
  { "Ip", 1, { 0.004, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Iv", 1, { 0.0006, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Vv", 1, { 0.8, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Cj0", 1, { 8e-14, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "M", 1, { 0.5, ((char *) -1) }, { '[', 0, 2, ']',
    { ((char *) 0) } } },
  { "Vj", 1, { 0.5, ((char *) -1) }, { ']', 0, 10, ']',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_RTD[] = {
  { "Wr", 1, { 2.7e-20, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "eta", 1, { 1e-20, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "dW", 1, { 4.5e-21, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Tmax", 1, { 0.95, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "de", 1, { 0.9, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "dv", 1, { 2, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "nv", 1, { 16, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "te", 1, { 6e-13, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { "Area", 1, { 1, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_RTD = {
  "RTD", 2, 0, 0, 1, req_RTD, opt_RTD };

static struct property_t req_Diac[] = {
  { "Ibo", 1, { 5e-05, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "Vbo", 1, { 30, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Diac[] = {
  { "Cj0", 1, { 1e-11, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Is", 1, { 1e-10, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "N", 1, { 2, ((char *) -1) }, { '[', 0.1, 100, ']',
    { ((char *) 0) } } },
  { "Ri", 1, { 10, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Diac = {
  "Diac", 2, 0, 0, 1, req_Diac, opt_Diac };

static struct property_t req_PM_Mod[] = {
  { "U", 1, { 1, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "f", 1, { 1e+09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "M", 1, { 1, ((char *) -1) }, { '[', 0, 1, ']',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_PM_Mod[] = {
  { "Phase", 1, { 0, ((char *) -1) }, { '[', -360, 360, ']',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_PM_Mod = {
  "PM_Mod", 3, 0, 0, 0, req_PM_Mod, opt_PM_Mod };

static struct property_t req_Switch[] = {
  { "init", 2, { 0, "off" }, { '.', 0, 0, '.',
    { "on", "off", ((char *) 0) } } },
  { "time", 3, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Switch[] = {
  { "Ron", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Roff", 1, { 1e+12, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { "MaxDuration", 1, { 1e-06, ((char *) -1) }, { '[', 1e-11, 0, '.',
    { ((char *) 0) } } },
  { "Transition", 2, { 0, "spline" }, { '.', 0, 0, '.',
    { "abrupt", "linear", "spline", ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Switch = {
  "Switch", 2, 0, 0, 0, req_Switch, opt_Switch };

static struct property_t req_Irect[] = {
  { "I", 1, { 0.001, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "TH", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "TL", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Irect[] = {
  { "Tr", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Tf", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Td", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Irect = {
  "Irect", 2, 0, 0, 0, req_Irect, opt_Irect };

static struct property_t req_MGAP[] = {
  { "W1", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "W2", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "S", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Subst", 2, { 0, "Subst1" }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "MSDispModel", 2, { 0, "Kirschning" }, { '.', 0, 0, '.',
    { "Kirschning", "Kobayashi", "Yamashita", "Getsinger", "Schneider", "Pramanick", "Hammerstad", ((char *) 0) } } },
  { "MSModel", 2, { 0, "Hammerstad" }, { '.', 0, 0, '.',
    { "Wheeler", "Schneider", "Hammerstad", ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_MGAP[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_MGAP = {
  "MGAP", 2, 0, 0, 0, req_MGAP, opt_MGAP };

static struct property_t req_MUT2[] = {
  { "L1", 1, { 0.001, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "L2", 1, { 0.001, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "L3", 1, { 0.001, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "k12", 1, { 0.9, ((char *) -1) }, { ']', -1, 1, '[',
    { ((char *) 0) } } },
  { "k13", 1, { 0.9, ((char *) -1) }, { ']', -1, 1, '[',
    { ((char *) 0) } } },
  { "k23", 1, { 0.9, ((char *) -1) }, { ']', -1, 1, '[',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_MUT2[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_MUT2 = {
  "MUT2", 6, 0, 0, 0, req_MUT2, opt_MUT2 };

static struct property_t req_INDQ[] = {
  { "L", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Q", 1, { 100, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "f", 1, { 1e+08, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Mode", 2, { 0, "Linear" }, { '.', 0, 0, '.',
    { "Linear", "SquareRoot", "Constant", ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_INDQ[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_INDQ = {
  "INDQ", 2, 0, 0, 0, req_INDQ, opt_INDQ };

static struct property_t req_SPDfile[] = {
  { "File", 2, { 0, "spfile.snp" }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_SPDfile[] = {
  { "Data", 2, { 0, "polar" }, { '.', 0, 0, '.',
    { "rectangular", "polar", ((char *) 0) } } },
  { "Interpolator", 2, { 0, "linear" }, { '.', 0, 0, '.',
    { "linear", "cubic", ((char *) 0) } } },
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { "duringDC", 2, { 0, "open" }, { '.', 0, 0, '.',
    { "open", "short", "shortall", "unspecified", ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_SPDfile = {
  "SPDfile", -1, 0, 0, 0, req_SPDfile, opt_SPDfile };

static struct property_t req_XOR[] = {
  { "V", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_XOR[] = {
  { "t", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "TR", 1, { 10, ((char *) -1) }, { '[', 1, 100, ']',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_XOR = {
  "XOR", -1, 0, 0, 1, req_XOR, opt_XOR };

static struct property_t req_MOPEN[] = {
  { "W", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Subst", 2, { 0, "Subst1" }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "MSDispModel", 2, { 0, "Kirschning" }, { '.', 0, 0, '.',
    { "Kirschning", "Kobayashi", "Yamashita", "Getsinger", "Schneider", "Pramanick", "Hammerstad", ((char *) 0) } } },
  { "MSModel", 2, { 0, "Hammerstad" }, { '.', 0, 0, '.',
    { "Wheeler", "Schneider", "Hammerstad", ((char *) 0) } } },
  { "Model", 2, { 0, "Kirschning" }, { '.', 0, 0, '.',
    { "Kirschning", "Hammerstad", "Alexopoulos", ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_MOPEN[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_MOPEN = {
  "MOPEN", 1, 0, 0, 0, req_MOPEN, opt_MOPEN };

static struct property_t req_MMBEND[] = {
  { "W", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Subst", 2, { 0, "Subst1" }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_MMBEND[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_MMBEND = {
  "MMBEND", 2, 0, 0, 0, req_MMBEND, opt_MMBEND };

static struct property_t req_NAND[] = {
  { "V", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_NAND[] = {
  { "t", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "TR", 1, { 10, ((char *) -1) }, { '[', 1, 100, ']',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_NAND = {
  "NAND", -1, 0, 0, 1, req_NAND, opt_NAND };

static struct property_t req_IProbe[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_IProbe[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_IProbe = {
  "IProbe", 2, 0, 0, 0, req_IProbe, opt_IProbe };

static struct property_t req_CIRCLINE[] = {
  { "a", 1, { 0.0286, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "L", 1, { 1.5, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "er", 1, { 1, ((char *) -1) }, { '[', 1, 100, ']',
    { ((char *) 0) } } },
  { "mur", 1, { 1, ((char *) -1) }, { '[', 1, 100, ']',
    { ((char *) 0) } } },
  { "tand", 1, { 0.0004, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "rho", 1, { 2.2e-08, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_CIRCLINE[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { "Material", 2, { 0, "unspecified" }, { '.', 0, 0, '.',
    { "unspecified", "Copper", "StainlessSteel", "Gold", ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_CIRCLINE = {
  "CIRCLINE", 2, 0, 0, 0, req_CIRCLINE, opt_CIRCLINE };

static struct property_t req_TWIST[] = {
  { "d", 1, { 0.0005, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "D", 1, { 0.0008, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "L", 1, { 1.5, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "T", 1, { 100, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "er", 1, { 4, ((char *) -1) }, { '[', 1, 100, ']',
    { ((char *) 0) } } },
  { "mur", 1, { 1, ((char *) -1) }, { '[', 1, 100, ']',
    { ((char *) 0) } } },
  { "tand", 1, { 0.0004, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "rho", 1, { 2.2e-08, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_TWIST[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_TWIST = {
  "TWIST", 4, 0, 0, 0, req_TWIST, opt_TWIST };

static struct property_t req_Amp[] = {
  { "G", 1, { 10, ((char *) -1) }, { '[', 1, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Amp[] = {
  { "Z1", 1, { 50, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Z2", 1, { 50, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "NF", 1, { 1, ((char *) -1) }, { '[', 1, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Amp = {
  "Amp", 2, 0, 0, 0, req_Amp, opt_Amp };

static struct property_t req_MESFET[] = {
  { "LEVEL", 0, { 1, ((char *) -1) }, { '[', 1, 5, ']',
    { ((char *) 0) } } },
  { "Vto", 1, { -1.8, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Beta", 1, { 0.003, ((char *) -1) }, { '[', 1e-09, 0, '.',
    { ((char *) 0) } } },
  { "Alpha", 1, { 2.25, ((char *) -1) }, { '[', 1e-09, 0, '.',
    { ((char *) 0) } } },
  { "Lambda", 1, { 0.05, ((char *) -1) }, { '[', 1e-09, 0, '.',
    { ((char *) 0) } } },
  { "B", 1, { 0.3, ((char *) -1) }, { '[', 1e-09, 0, '.',
    { ((char *) 0) } } },
  { "Qp", 1, { 2.1, ((char *) -1) }, { '[', 1e-09, 0, '.',
    { ((char *) 0) } } },
  { "Delta", 1, { 0.1, ((char *) -1) }, { '[', 1e-09, 0, '.',
    { ((char *) 0) } } },
  { "Vmax", 1, { 0.5, ((char *) -1) }, { '[', 1e-09, 0, '.',
    { ((char *) 0) } } },
  { "Vdelta1", 1, { 0.3, ((char *) -1) }, { '[', 1e-09, 0, '.',
    { ((char *) 0) } } },
  { "Vdelta2", 1, { 0.2, ((char *) -1) }, { '[', 1e-09, 0, '.',
    { ((char *) 0) } } },
  { "Gamma", 1, { 0.015, ((char *) -1) }, { '[', 1e-09, 0, '.',
    { ((char *) 0) } } },
  { "Nsc", 1, { 1, ((char *) -1) }, { '[', 1e-09, 0, '.',
    { ((char *) 0) } } },
  { "Is", 1, { 1e-14, ((char *) -1) }, { '[', 1e-20, 0, '.',
    { ((char *) 0) } } },
  { "N", 1, { 1, ((char *) -1) }, { '[', 1e-09, 0, '.',
    { ((char *) 0) } } },
  { "Vbi", 1, { 1, ((char *) -1) }, { '[', 1e-09, 0, '.',
    { ((char *) 0) } } },
  { "Bv", 1, { 60, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Xti", 1, { 3, ((char *) -1) }, { '[', 1e-09, 0, '.',
    { ((char *) 0) } } },
  { "Fc", 1, { 0.5, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "Tau", 1, { 1e-09, ((char *) -1) }, { '[', 1e-20, 0, '.',
    { ((char *) 0) } } },
  { "Rin", 1, { 0.001, ((char *) -1) }, { '[', 1e-20, 0, '.',
    { ((char *) 0) } } },
  { "Area", 1, { 1, ((char *) -1) }, { '[', 1, 0, '.',
    { ((char *) 0) } } },
  { "Eg", 1, { 1.11, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "M", 1, { 0.5, ((char *) -1) }, { '[', 1e-09, 0, '.',
    { ((char *) 0) } } },
  { "Cgd", 1, { 2e-13, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Cgs", 1, { 1e-12, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Cds", 1, { 1e-12, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Betatc", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Alphatc", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Gammatc", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Ng", 1, { 2.65, ((char *) -1) }, { '[', 1, 0, '.',
    { ((char *) 0) } } },
  { "Nd", 1, { -0.19, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "ILEVELS", 0, { 3, ((char *) -1) }, { '[', 0, 4, ']',
    { ((char *) 0) } } },
  { "ILEVELD", 0, { 3, ((char *) -1) }, { '[', 0, 4, ']',
    { ((char *) 0) } } },
  { "QLEVELS", 0, { 2, ((char *) -1) }, { '[', 0, 3, ']',
    { ((char *) 0) } } },
  { "QLEVELD", 0, { 2, ((char *) -1) }, { '[', 0, 3, ']',
    { ((char *) 0) } } },
  { "QLEVELDS", 0, { 2, ((char *) -1) }, { '[', 0, 3, ']',
    { ((char *) 0) } } },
  { "Vtotc", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Rg", 1, { 5.1, ((char *) -1) }, { '[', 1e-09, 0, '.',
    { ((char *) 0) } } },
  { "Rd", 1, { 1.3, ((char *) -1) }, { '[', 1e-09, 0, '.',
    { ((char *) 0) } } },
  { "Rs", 1, { 1.3, ((char *) -1) }, { '[', 1e-09, 0, '.',
    { ((char *) 0) } } },
  { "Rgtc", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Rdtc", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Rstc", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Ibv", 1, { 0.001, ((char *) -1) }, { '[', 1e-25, 0, '.',
    { ((char *) 0) } } },
  { "Rf", 1, { 10, ((char *) -1) }, { '[', 1e-09, 0, '.',
    { ((char *) 0) } } },
  { "R1", 1, { 10, ((char *) -1) }, { '[', 1e-09, 0, '.',
    { ((char *) 0) } } },
  { "Af", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Kf", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Gdsnoi", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Tnom", 1, { 26.85, ((char *) -1) }, { '[', -273, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_MESFET[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_MESFET = {
  "MESFET", 3, 0, 0, 1, req_MESFET, opt_MESFET };

static struct property_t req_potentiometer[] = {
  { "R_pot", 1, { 10000, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "Rotation", 1, { 120, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Taper_Coeff", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "LEVEL", 0, { 1, ((char *) -1) }, { '[', 1, 3, ']',
    { ((char *) 0) } } },
  { "Max_Rotation", 1, { 240, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Conformity", 1, { 0.2, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Linearity", 1, { 0.2, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Contact_Res", 1, { 1, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "Temp_Coeff", 1, { 100, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Tnom", 1, { 26.85, ((char *) -1) }, { '[', -273, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_potentiometer[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_potentiometer = {
  "potentiometer", 3, 0, 0, 1, req_potentiometer, opt_potentiometer };

static struct property_t req_DCBlock[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_DCBlock[] = {
  { "C", 1, { 1e-06, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_DCBlock = {
  "DCBlock", 2, 0, 0, 0, req_DCBlock, opt_DCBlock };

static struct property_t req_fa2b[] = {
  { "TR", 1, { 6, ((char *) -1) }, { '[', 1, 20, ']',
    { ((char *) 0) } } },
  { "Delay", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_fa2b[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_fa2b = {
  "fa2b", 8, 0, 0, 1, req_fa2b, opt_fa2b };

static struct property_t req_Gyrator[] = {
  { "R", 1, { 50, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Gyrator[] = {
  { "Zref", 1, { 50, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Gyrator = {
  "Gyrator", 4, 0, 0, 0, req_Gyrator, opt_Gyrator };

static struct property_t req_XNOR[] = {
  { "V", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_XNOR[] = {
  { "t", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "TR", 1, { 10, ((char *) -1) }, { '[', 1, 100, ']',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_XNOR = {
  "XNOR", -1, 0, 0, 1, req_XNOR, opt_XNOR };

static struct property_t req_fa1b[] = {
  { "TR", 1, { 6, ((char *) -1) }, { '[', 1, 20, ']',
    { ((char *) 0) } } },
  { "Delay", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_fa1b[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_fa1b = {
  "fa1b", 5, 0, 0, 1, req_fa1b, opt_fa1b };

static struct property_t req_mod_amp[] = {
  { "GBP", 1, { 1e+06, ((char *) -1) }, { '[', 1, 0, '.',
    { ((char *) 0) } } },
  { "AOLDC", 1, { 106, ((char *) -1) }, { '[', 0.01, 0, '.',
    { ((char *) 0) } } },
  { "FP2", 1, { 3e+06, ((char *) -1) }, { '[', 0.01, 0, '.',
    { ((char *) 0) } } },
  { "RO", 1, { 75, ((char *) -1) }, { '[', 0.01, 0, '.',
    { ((char *) 0) } } },
  { "CD", 1, { 1e-12, ((char *) -1) }, { '[', 1e-20, 0, '.',
    { ((char *) 0) } } },
  { "RD", 1, { 2e+06, ((char *) -1) }, { '[', 0.01, 0, '.',
    { ((char *) 0) } } },
  { "IOFF", 1, { 2e-08, ((char *) -1) }, { '[', 1e-20, 0, '.',
    { ((char *) 0) } } },
  { "IB", 1, { 8e-08, ((char *) -1) }, { '[', 1e-20, 0, '.',
    { ((char *) 0) } } },
  { "VOFF", 1, { 0.0007, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "CMRRDC", 1, { 90, ((char *) -1) }, { '[', 1, 0, '.',
    { ((char *) 0) } } },
  { "FCM", 1, { 200, ((char *) -1) }, { '[', 0.01, 0, '.',
    { ((char *) 0) } } },
  { "PSRT", 1, { 500000, ((char *) -1) }, { '[', 1, 0, '.',
    { ((char *) 0) } } },
  { "NSRT", 1, { 500000, ((char *) -1) }, { '[', 1, 0, '.',
    { ((char *) 0) } } },
  { "VLIMP", 1, { 14, ((char *) -1) }, { '[', 0.01, 0, '.',
    { ((char *) 0) } } },
  { "VLIMN", 1, { -14, ((char *) -1) }, { '.', 0, 0, ']',
    { ((char *) 0) } } },
  { "ILMAX", 1, { 0.035, ((char *) -1) }, { '[', 1e-09, 0, '.',
    { ((char *) 0) } } },
  { "CSCALE", 1, { 50, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_mod_amp[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_mod_amp = {
  "mod_amp", 3, 0, 0, 1, req_mod_amp, opt_mod_amp };

static struct property_t req_gatedDlatch[] = {
  { "TR_H", 1, { 6, ((char *) -1) }, { '[', 1, 20, ']',
    { ((char *) 0) } } },
  { "TR_L", 1, { 5, ((char *) -1) }, { '[', 1, 20, ']',
    { ((char *) 0) } } },
  { "Delay", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_gatedDlatch[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_gatedDlatch = {
  "gatedDlatch", 4, 0, 0, 1, req_gatedDlatch, opt_gatedDlatch };

static struct property_t req_jkff_SR[] = {
  { "TR_H", 1, { 6, ((char *) -1) }, { '[', 1, 20, ']',
    { ((char *) 0) } } },
  { "TR_L", 1, { 5, ((char *) -1) }, { '[', 1, 20, ']',
    { ((char *) 0) } } },
  { "Delay", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_jkff_SR[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_jkff_SR = {
  "jkff_SR", 7, 0, 0, 1, req_jkff_SR, opt_jkff_SR };

static struct property_t req_Ifile[] = {
  { "File", 2, { 0, "ifile.dat" }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Ifile[] = {
  { "Interpolator", 2, { 0, "linear" }, { '.', 0, 0, '.',
    { "hold", "linear", "cubic", ((char *) 0) } } },
  { "Repeat", 2, { 0, "no" }, { '.', 0, 0, '.',
    { "yes", "no", ((char *) 0) } } },
  { "G", 1, { 1, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "T", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Ifile = {
  "Ifile", 2, 0, 0, 0, req_Ifile, opt_Ifile };

static struct property_t req_DCFeed[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_DCFeed[] = {
  { "L", 1, { 1e-06, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_DCFeed = {
  "DCFeed", 2, 0, 0, 0, req_DCFeed, opt_DCFeed };

static struct property_t req_Ipulse[] = {
  { "I1", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "I2", 1, { 1, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "T1", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "T2", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Ipulse[] = {
  { "Tr", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Tf", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Ipulse = {
  "Ipulse", 2, 0, 0, 0, req_Ipulse, opt_Ipulse };

static struct property_t req_Def[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Def[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Def = {
  "Def", -1, 1, 0, 0, req_Def, opt_Def };

static struct property_t req_binarytogrey4bit[] = {
  { "TR", 1, { 6, ((char *) -1) }, { '[', 1, 20, ']',
    { ((char *) 0) } } },
  { "Delay", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_binarytogrey4bit[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_binarytogrey4bit = {
  "binarytogrey4bit", 8, 0, 0, 1, req_binarytogrey4bit, opt_binarytogrey4bit };

static struct property_t req_comp_4bit[] = {
  { "TR", 1, { 6, ((char *) -1) }, { '[', 1, 20, ']',
    { ((char *) 0) } } },
  { "Delay", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_comp_4bit[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_comp_4bit = {
  "comp_4bit", 11, 0, 0, 1, req_comp_4bit, opt_comp_4bit };

static struct property_t req_IVnoise[] = {
  { "i1", 1, { 1e-06, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "v2", 1, { 1e-06, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "C", 1, { 0.5, ((char *) -1) }, { '[', -1, 1, ']',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_IVnoise[] = {
  { "a", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "c", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "e", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_IVnoise = {
  "IVnoise", 4, 0, 0, 0, req_IVnoise, opt_IVnoise };

static struct property_t req_CCCS[] = {
  { "G", 1, { 1, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_CCCS[] = {
  { "T", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_CCCS = {
  "CCCS", 4, 0, 0, 0, req_CCCS, opt_CCCS };

static struct property_t req_AC[] = {
  { "Type", 2, { 0, "lin" }, { '.', 0, 0, '.',
    { "lin", "log", "list", "const", ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_AC[] = {
  { "Noise", 2, { 0, "no" }, { '.', 0, 0, '.',
    { "yes", "no", ((char *) 0) } } },
  { "Start", 1, { 1e+09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Stop", 1, { 1e+10, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Points", 0, { 10, ((char *) -1) }, { '[', 2, 0, '.',
    { ((char *) 0) } } },
  { "Values", 3, { 10, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_AC = {
  "AC", 0, 1, 0, 0, req_AC, opt_AC };

static struct property_t req_MSTEP[] = {
  { "W1", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "W2", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Subst", 2, { 0, "Subst1" }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "MSDispModel", 2, { 0, "Kirschning" }, { '.', 0, 0, '.',
    { "Kirschning", "Kobayashi", "Yamashita", "Getsinger", "Schneider", "Pramanick", "Hammerstad", ((char *) 0) } } },
  { "MSModel", 2, { 0, "Hammerstad" }, { '.', 0, 0, '.',
    { "Wheeler", "Schneider", "Hammerstad", ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_MSTEP[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_MSTEP = {
  "MSTEP", 2, 0, 0, 0, req_MSTEP, opt_MSTEP };

static struct property_t req_C[] = {
  { "C", 1, { 1e-12, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_C[] = {
  { "V", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_C = {
  "C", 2, 0, 0, 0, req_C, opt_C };

static struct property_t req_VCVS[] = {
  { "G", 1, { 1, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_VCVS[] = {
  { "T", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_VCVS = {
  "VCVS", 4, 0, 0, 0, req_VCVS, opt_VCVS };

static struct property_t req_Sub[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Sub[] = {
  { "Type", 2, { 0, "DEF1" }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Sub = {
  "Sub", -1, 0, 0, 0, req_Sub, opt_Sub };

static struct property_t req_ha1b[] = {
  { "TR", 1, { 6, ((char *) -1) }, { '[', 1, 20, ']',
    { ((char *) 0) } } },
  { "Delay", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_ha1b[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_ha1b = {
  "ha1b", 4, 0, 0, 1, req_ha1b, opt_ha1b };

static struct property_t req_log_amp[] = {
  { "Kv", 1, { 1, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Dk", 1, { 0.3, ((char *) -1) }, { '[', -100, 100, ']',
    { ((char *) 0) } } },
  { "Ib1", 1, { 5e-12, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Ibr", 1, { 5e-12, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "M", 1, { 5, ((char *) -1) }, { '[', 1, 0, '.',
    { ((char *) 0) } } },
  { "N", 1, { 0.1, ((char *) -1) }, { '[', 0, 100, ']',
    { ((char *) 0) } } },
  { "Vosout", 1, { 0.003, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Rinp", 1, { 1e+06, ((char *) -1) }, { '[', 1, 0, '.',
    { ((char *) 0) } } },
  { "Fc", 1, { 1000, ((char *) -1) }, { '[', 1, 0, '.',
    { ((char *) 0) } } },
  { "Ro", 1, { 0.001, ((char *) -1) }, { '[', 0.001, 0, '.',
    { ((char *) 0) } } },
  { "Ntc", 1, { 0.002, ((char *) -1) }, { '[', -100, 100, ']',
    { ((char *) 0) } } },
  { "Vosouttc", 1, { 8e-05, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Dktc", 1, { 0.03, ((char *) -1) }, { '[', -100, 100, ']',
    { ((char *) 0) } } },
  { "Ib1tc", 1, { 5e-13, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Ibrtc", 1, { 5e-13, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Tnom", 1, { 26.85, ((char *) -1) }, { '[', -273, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_log_amp[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_log_amp = {
  "log_amp", 3, 0, 0, 1, req_log_amp, opt_log_amp };

static struct property_t req_RFEDD[] = {
  { "Type", 2, { 0, "Y" }, { '.', 0, 0, '.',
    { "Y", "Z", "S", "H", "G", "A", "T", ((char *) 0) } } },
  { "duringDC", 2, { 0, "open" }, { '.', 0, 0, '.',
    { "open", "short", "unspecified", "zerofrequency", ((char *) 0) } } },
  { "P11", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_RFEDD[] = {
  { "P12", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P13", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P14", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P15", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P16", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P17", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P18", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P21", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P22", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P23", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P24", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P25", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P26", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P27", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P28", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P31", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P32", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P33", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P34", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P35", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P36", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P37", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P38", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P41", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P42", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P43", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P44", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P45", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P46", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P47", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P48", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P51", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P52", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P53", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P54", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P55", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P56", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P57", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P58", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P61", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P62", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P63", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P64", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P65", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P66", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P67", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P68", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P71", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P72", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P73", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P74", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P75", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P76", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P77", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P78", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P81", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P82", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P83", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P84", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P85", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P86", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P87", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "P88", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_RFEDD = {
  "RFEDD", -1, 0, 0, 0, req_RFEDD, opt_RFEDD };

static struct property_t req_BJT[] = {
  { "Is", 1, { 1e-16, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Nf", 1, { 1, ((char *) -1) }, { '[', 0.1, 100, ']',
    { ((char *) 0) } } },
  { "Nr", 1, { 1, ((char *) -1) }, { '[', 0.1, 100, ']',
    { ((char *) 0) } } },
  { "Ikf", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Ikr", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Vaf", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Var", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Ise", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Ne", 1, { 1.5, ((char *) -1) }, { '[', 0.1, 100, ']',
    { ((char *) 0) } } },
  { "Isc", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Nc", 1, { 2, ((char *) -1) }, { '[', 0.1, 100, ']',
    { ((char *) 0) } } },
  { "Bf", 1, { 100, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "Br", 1, { 1, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "Rbm", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Irb", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Cje", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Vje", 1, { 0.75, ((char *) -1) }, { ']', 0, 10, ']',
    { ((char *) 0) } } },
  { "Mje", 1, { 0.33, ((char *) -1) }, { '[', 0, 1, ']',
    { ((char *) 0) } } },
  { "Cjc", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Vjc", 1, { 0.75, ((char *) -1) }, { ']', 0, 10, ']',
    { ((char *) 0) } } },
  { "Mjc", 1, { 0.33, ((char *) -1) }, { '[', 0, 1, ']',
    { ((char *) 0) } } },
  { "Xcjc", 1, { 1, ((char *) -1) }, { '[', 0, 1, ']',
    { ((char *) 0) } } },
  { "Cjs", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Vjs", 1, { 0.75, ((char *) -1) }, { ']', 0, 10, ']',
    { ((char *) 0) } } },
  { "Mjs", 1, { 0, ((char *) -1) }, { '[', 0, 1, ']',
    { ((char *) 0) } } },
  { "Fc", 1, { 0.5, ((char *) -1) }, { '[', 0, 1, ']',
    { ((char *) 0) } } },
  { "Vtf", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Tf", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Xtf", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Itf", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Tr", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_BJT[] = {
  { "Rc", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Re", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Rb", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Kf", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Af", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Ffe", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Kb", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Ab", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Fb", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { "Type", 2, { 0, "npn" }, { '.', 0, 0, '.',
    { "npn", "pnp", ((char *) 0) } } },
  { "Ptf", 1, { 0, ((char *) -1) }, { '[', -180, 180, ']',
    { ((char *) 0) } } },
  { "Xtb", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Xti", 1, { 3, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Eg", 1, { 1.11, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Tnom", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { "Area", 1, { 1, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_BJT = {
  "BJT", 4, 0, 0, 1, req_BJT, opt_BJT };

static struct property_t req_dmux4to16[] = {
  { "TR", 1, { 6, ((char *) -1) }, { '[', 1, 20, ']',
    { ((char *) 0) } } },
  { "Delay", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_dmux4to16[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_dmux4to16 = {
  "dmux4to16", 21, 0, 0, 1, req_dmux4to16, opt_dmux4to16 };

static struct property_t req_Vfile[] = {
  { "File", 2, { 0, "vfile.dat" }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Vfile[] = {
  { "Interpolator", 2, { 0, "linear" }, { '.', 0, 0, '.',
    { "hold", "linear", "cubic", ((char *) 0) } } },
  { "Repeat", 2, { 0, "no" }, { '.', 0, 0, '.',
    { "yes", "no", ((char *) 0) } } },
  { "G", 1, { 1, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "T", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Vfile = {
  "Vfile", 2, 0, 0, 0, req_Vfile, opt_Vfile };

static struct property_t req_SPfile[] = {
  { "File", 2, { 0, "spfile.snp" }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_SPfile[] = {
  { "Data", 2, { 0, "polar" }, { '.', 0, 0, '.',
    { "rectangular", "polar", ((char *) 0) } } },
  { "Interpolator", 2, { 0, "linear" }, { '.', 0, 0, '.',
    { "linear", "cubic", ((char *) 0) } } },
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { "duringDC", 2, { 0, "open" }, { '.', 0, 0, '.',
    { "open", "short", "shortall", "unspecified", ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_SPfile = {
  "SPfile", -1, 0, 0, 0, req_SPfile, opt_SPfile };

static struct property_t req_MCROSS[] = {
  { "W1", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "W2", 1, { 0.002, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "W3", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "W4", 1, { 0.002, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Subst", 2, { 0, "Subst1" }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "MSDispModel", 2, { 0, "Kirschning" }, { '.', 0, 0, '.',
    { "Kirschning", "Kobayashi", "Yamashita", "Getsinger", "Schneider", "Pramanick", "Hammerstad", ((char *) 0) } } },
  { "MSModel", 2, { 0, "Hammerstad" }, { '.', 0, 0, '.',
    { "Wheeler", "Schneider", "Hammerstad", ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_MCROSS[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_MCROSS = {
  "MCROSS", 4, 0, 0, 0, req_MCROSS, opt_MCROSS };

static struct property_t req_MUT[] = {
  { "L1", 1, { 0.001, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "L2", 1, { 0.001, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "k", 1, { 0.9, ((char *) -1) }, { ']', -1, 1, '[',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_MUT[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_MUT = {
  "MUT", 4, 0, 0, 0, req_MUT, opt_MUT };

static struct property_t req_dmux2to4[] = {
  { "TR", 1, { 6, ((char *) -1) }, { '[', 1, 20, ']',
    { ((char *) 0) } } },
  { "Delay", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_dmux2to4[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_dmux2to4 = {
  "dmux2to4", 7, 0, 0, 1, req_dmux2to4, opt_dmux2to4 };

static struct property_t req_DC[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_DC[] = {
  { "MaxIter", 0, { 150, ((char *) -1) }, { '[', 2, 10000, ']',
    { ((char *) 0) } } },
  { "abstol", 1, { 1e-12, ((char *) -1) }, { ']', 0, 1, ']',
    { ((char *) 0) } } },
  { "vntol", 1, { 1e-06, ((char *) -1) }, { ']', 0, 1, ']',
    { ((char *) 0) } } },
  { "reltol", 1, { 0.001, ((char *) -1) }, { ']', 0, 1, ']',
    { ((char *) 0) } } },
  { "saveOPs", 2, { 0, "no" }, { '.', 0, 0, '.',
    { "yes", "no", ((char *) 0) } } },
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { "saveAll", 2, { 0, "no" }, { '.', 0, 0, '.',
    { "yes", "no", ((char *) 0) } } },
  { "convHelper", 2, { 0, "none" }, { '.', 0, 0, '.',
    { "none", "SourceStepping", "gMinStepping", "LineSearch", "Attenuation", "SteepestDescent", ((char *) 0) } } },
  { "Solver", 2, { 0, "CroutLU" }, { '.', 0, 0, '.',
    { "CroutLU", "DoolittleLU", "HouseholderQR", "HouseholderLQ", "GolubSVD", ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_DC = {
  "DC", 0, 1, 0, 0, req_DC, opt_DC };

static struct property_t req_L[] = {
  { "L", 1, { 1e-09, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_L[] = {
  { "I", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_L = {
  "L", 2, 0, 0, 0, req_L, opt_L };

static struct property_t req_OR[] = {
  { "V", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_OR[] = {
  { "t", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "TR", 1, { 10, ((char *) -1) }, { '[', 1, 100, ']',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_OR = {
  "OR", -1, 0, 0, 1, req_OR, opt_OR };

static struct property_t req_MUTX[] = {
  { "L", 3, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "k", 3, { 0.9, ((char *) -1) }, { '[', -1, 1, ']',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_MUTX[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_MUTX = {
  "MUTX", -1, 0, 0, 0, req_MUTX, opt_MUTX };

static struct property_t req_COPEN[] = {
  { "W", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "S", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "G", 1, { 0.005, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Subst", 2, { 0, "Subst1" }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_COPEN[] = {
  { "Backside", 2, { 0, "Metal" }, { '.', 0, 0, '.',
    { "Metal", "Air", ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_COPEN = {
  "COPEN", 1, 0, 0, 0, req_COPEN, opt_COPEN };

static struct property_t req_TLIN[] = {
  { "Z", 1, { 50, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "L", 1, { 0.001, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_TLIN[] = {
  { "Alpha", 1, { 1, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_TLIN = {
  "TLIN", 2, 0, 0, 0, req_TLIN, opt_TLIN };

static struct property_t req_VProbe[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_VProbe[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_VProbe = {
  "VProbe", 2, 0, 0, 0, req_VProbe, opt_VProbe };

static struct property_t req_dmux3to8[] = {
  { "TR", 1, { 6, ((char *) -1) }, { '[', 1, 20, ']',
    { ((char *) 0) } } },
  { "Delay", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_dmux3to8[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_dmux3to8 = {
  "dmux3to8", 12, 0, 0, 1, req_dmux3to8, opt_dmux3to8 };

static struct property_t req_hpribin4bit[] = {
  { "TR", 1, { 6, ((char *) -1) }, { '[', 1, 20, ']',
    { ((char *) 0) } } },
  { "Delay", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_hpribin4bit[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_hpribin4bit = {
  "hpribin4bit", 7, 0, 0, 1, req_hpribin4bit, opt_hpribin4bit };

static struct property_t req_mux2to1[] = {
  { "TR", 1, { 6, ((char *) -1) }, { '[', 1, 20, ']',
    { ((char *) 0) } } },
  { "Delay", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_mux2to1[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_mux2to1 = {
  "mux2to1", 5, 0, 0, 1, req_mux2to1, opt_mux2to1 };

static struct property_t req_R[] = {
  { "R", 1, { 50, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_R[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { "Tc1", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Tc2", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Tnom", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_R = {
  "R", 2, 0, 0, 0, req_R, opt_R };

static struct property_t req_HB[] = {
  { "n", 0, { 1, ((char *) -1) }, { '[', 1, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_HB[] = {
  { "f", 1, { 1e+09, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "iabstol", 1, { 1e-12, ((char *) -1) }, { ']', 0, 1, ']',
    { ((char *) 0) } } },
  { "vabstol", 1, { 1e-06, ((char *) -1) }, { ']', 0, 1, ']',
    { ((char *) 0) } } },
  { "reltol", 1, { 0.001, ((char *) -1) }, { ']', 0, 1, ']',
    { ((char *) 0) } } },
  { "MaxIter", 0, { 150, ((char *) -1) }, { '[', 2, 10000, ']',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_HB = {
  "HB", 0, 1, 0, 0, req_HB, opt_HB };

static struct property_t req_Circulator[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Circulator[] = {
  { "Z1", 1, { 50, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Z2", 1, { 50, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Z3", 1, { 50, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Circulator = {
  "Circulator", 3, 0, 0, 0, req_Circulator, opt_Circulator };

static struct property_t req_vcresistor[] = {
  { "gain", 1, { 1, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_vcresistor[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_vcresistor = {
  "vcresistor", 4, 0, 0, 1, req_vcresistor, opt_vcresistor };

static struct property_t req_Relais[] = {
  { "Vt", 1, { 0.5, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Vh", 1, { 0.1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Relais[] = {
  { "Ron", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Roff", 1, { 1e+12, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Relais = {
  "Relais", 4, 0, 0, 1, req_Relais, opt_Relais };

static struct property_t req_Triac[] = {
  { "Igt", 1, { 5e-05, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "Vbo", 1, { 30, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Triac[] = {
  { "Cj0", 1, { 1e-11, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Is", 1, { 1e-10, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "N", 1, { 2, ((char *) -1) }, { '[', 0.1, 100, ']',
    { ((char *) 0) } } },
  { "Ri", 1, { 10, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "Rg", 1, { 5, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Triac = {
  "Triac", 3, 0, 0, 1, req_Triac, opt_Triac };

static struct property_t req_Vrect[] = {
  { "U", 1, { 1, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "TH", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "TL", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Vrect[] = {
  { "Tr", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Tf", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Td", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Vrect = {
  "Vrect", 2, 0, 0, 0, req_Vrect, opt_Vrect };

static struct property_t req_CGAP[] = {
  { "W", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "G", 1, { 0.0005, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Subst", 2, { 0, "Subst1" }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_CGAP[] = {
  { "S", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_CGAP = {
  "CGAP", 2, 0, 0, 0, req_CGAP, opt_CGAP };

static struct property_t req_pad3bit[] = {
  { "Number", 0, { 0, ((char *) -1) }, { '[', 0, 15, ']',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_pad3bit[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_pad3bit = {
  "pad3bit", 3, 0, 0, 1, req_pad3bit, opt_pad3bit };

static struct property_t req_EDD[] = {
  { "I1", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Q1", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_EDD[] = {
  { "I2", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Q2", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "I3", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Q3", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "I4", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Q4", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "I5", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Q5", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "I6", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Q6", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "I7", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Q7", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "I8", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Q8", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "I9", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Q9", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "I10", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Q10", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "I11", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Q11", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "I12", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Q12", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "I13", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Q13", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "I14", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Q14", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "I15", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Q15", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "I16", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Q16", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "I17", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Q17", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "I18", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Q18", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "I19", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Q19", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "I20", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Q20", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_EDD = {
  "EDD", -1, 0, 0, 1, req_EDD, opt_EDD };

static struct property_t req_dff_SR[] = {
  { "TR_H", 1, { 6, ((char *) -1) }, { '[', 1, 20, ']',
    { ((char *) 0) } } },
  { "TR_L", 1, { 5, ((char *) -1) }, { '[', 1, 20, ']',
    { ((char *) 0) } } },
  { "Delay", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_dff_SR[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_dff_SR = {
  "dff_SR", 6, 0, 0, 1, req_dff_SR, opt_dff_SR };

static struct property_t req_tff_SR[] = {
  { "TR_H", 1, { 6, ((char *) -1) }, { '[', 1, 20, ']',
    { ((char *) 0) } } },
  { "TR_L", 1, { 5, ((char *) -1) }, { '[', 1, 20, ']',
    { ((char *) 0) } } },
  { "Delay", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_tff_SR[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_tff_SR = {
  "tff_SR", 6, 0, 0, 1, req_tff_SR, opt_tff_SR };

static struct property_t req_Tr[] = {
  { "T", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Tr[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Tr = {
  "Tr", 4, 0, 0, 0, req_Tr, opt_Tr };

static struct property_t req_logic_0[] = {
  { "LEVEL", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_logic_0[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_logic_0 = {
  "logic_0", 1, 0, 0, 1, req_logic_0, opt_logic_0 };

static struct property_t req_Vpulse[] = {
  { "U1", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "U2", 1, { 1, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "T1", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "T2", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Vpulse[] = {
  { "Tr", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Tf", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Vpulse = {
  "Vpulse", 2, 0, 0, 0, req_Vpulse, opt_Vpulse };

static struct property_t req_logic_1[] = {
  { "LEVEL", 1, { 1, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_logic_1[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_logic_1 = {
  "logic_1", 1, 0, 0, 1, req_logic_1, opt_logic_1 };

static struct property_t req_ECVS[] = {
  { "U", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_ECVS[] = {
  { "Interpolator", 2, { 0, "linear" }, { '.', 0, 0, '.',
    { "hold", "linear", "cubic", ((char *) 0) } } },
  { "Tnext", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_ECVS = {
  "ECVS", 2, 0, 0, 0, req_ECVS, opt_ECVS };

static struct property_t req_Iexp[] = {
  { "I1", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "I2", 1, { 1, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "T1", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "T2", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Iexp[] = {
  { "Tr", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Tf", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Iexp = {
  "Iexp", 2, 0, 0, 0, req_Iexp, opt_Iexp };

static struct property_t req_AND[] = {
  { "V", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_AND[] = {
  { "t", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "TR", 1, { 10, ((char *) -1) }, { '[', 1, 100, ']',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_AND = {
  "AND", -1, 0, 0, 1, req_AND, opt_AND };

static struct property_t req_CSHORT[] = {
  { "W", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "S", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Subst", 2, { 0, "Subst1" }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_CSHORT[] = {
  { "Backside", 2, { 0, "Metal" }, { '.', 0, 0, '.',
    { "Metal", "Air", ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_CSHORT = {
  "CSHORT", 1, 0, 0, 0, req_CSHORT, opt_CSHORT };

static struct property_t req_BiasT[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_BiasT[] = {
  { "C", 1, { 1e-06, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "L", 1, { 1e-06, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_BiasT = {
  "BiasT", 3, 0, 0, 0, req_BiasT, opt_BiasT };

static struct property_t req_OpAmp[] = {
  { "G", 1, { 1e+06, ((char *) -1) }, { '[', 1, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_OpAmp[] = {
  { "Umax", 1, { 15, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_OpAmp = {
  "OpAmp", 3, 0, 0, 1, req_OpAmp, opt_OpAmp };

static struct property_t req_pad2bit[] = {
  { "Number", 0, { 0, ((char *) -1) }, { '[', 0, 15, ']',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_pad2bit[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_pad2bit = {
  "pad2bit", 2, 0, 0, 1, req_pad2bit, opt_pad2bit };

static struct property_t req_IInoise[] = {
  { "i1", 1, { 1e-06, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "i2", 1, { 1e-06, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "C", 1, { 0.5, ((char *) -1) }, { '[', -1, 1, ']',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_IInoise[] = {
  { "a", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "c", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "e", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_IInoise = {
  "IInoise", 4, 0, 0, 0, req_IInoise, opt_IInoise };

static struct property_t req_DLS_nto1[] = {
  { "LEVEL", 1, { 5, ((char *) -1) }, { '[', 1, 0, '.',
    { ((char *) 0) } } },
  { "Delay", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_DLS_nto1[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_DLS_nto1 = {
  "DLS_nto1", 2, 0, 0, 1, req_DLS_nto1, opt_DLS_nto1 };

static struct property_t req_Pac[] = {
  { "f", 1, { 1e+09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Z", 1, { 50, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "Num", 0, { 1, ((char *) -1) }, { '[', 1, 256, ']',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Pac[] = {
  { "P", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Pac = {
  "Pac", 2, 0, 0, 0, req_Pac, opt_Pac };

static struct property_t req_RLCG[] = {
  { "R", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "L", 1, { 6e-07, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "C", 1, { 2.4e-10, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "G", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Length", 1, { 0.001, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_RLCG[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_RLCG = {
  "RLCG", 2, 0, 0, 0, req_RLCG, opt_RLCG };

static struct property_t req_SUBST[] = {
  { "er", 1, { 9.8, ((char *) -1) }, { '[', 1, 100, ']',
    { ((char *) 0) } } },
  { "h", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "t", 1, { 3.5e-05, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "tand", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "rho", 1, { 2.2e-08, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "D", 1, { 1.5e-07, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_SUBST[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_SUBST = {
  "SUBST", 0, 0, 1, 0, req_SUBST, opt_SUBST };

static struct property_t req_MVIA[] = {
  { "D", 1, { 0.0001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Subst", 2, { 0, "Subst1" }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_MVIA[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_MVIA = {
  "MVIA", 2, 0, 0, 0, req_MVIA, opt_MVIA };

static struct property_t req_VCCS[] = {
  { "G", 1, { 1, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_VCCS[] = {
  { "T", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_VCCS = {
  "VCCS", 4, 0, 0, 0, req_VCCS, opt_VCCS };

static struct property_t req_EKV26MOS[] = {
  { "LEVEL", 1, { 1, ((char *) -1) }, { '[', 1, 2, ']',
    { ((char *) 0) } } },
  { "L", 1, { 5e-07, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "W", 1, { 1e-05, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Np", 1, { 1, ((char *) -1) }, { '[', 1, 0, '.',
    { ((char *) 0) } } },
  { "Ns", 1, { 1, ((char *) -1) }, { '[', 1, 0, '.',
    { ((char *) 0) } } },
  { "Cox", 1, { 0.00345, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Xj", 1, { 1.5e-07, ((char *) -1) }, { '[', 1e-08, 1e-06, ']',
    { ((char *) 0) } } },
  { "Dw", 1, { -2e-08, ((char *) -1) }, { '.', 0, 0, ']',
    { ((char *) 0) } } },
  { "Dl", 1, { -5e-08, ((char *) -1) }, { '.', 0, 0, ']',
    { ((char *) 0) } } },
  { "Vto", 1, { 0.6, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Gamma", 1, { 0.71, ((char *) -1) }, { '[', 0, 2, ']',
    { ((char *) 0) } } },
  { "Phi", 1, { 0.97, ((char *) -1) }, { '[', 0.3, 2, ']',
    { ((char *) 0) } } },
  { "Kp", 1, { 0.00015, ((char *) -1) }, { '[', 1e-05, 0, '.',
    { ((char *) 0) } } },
  { "Theta", 1, { 0.05, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "EO", 1, { 8.8e+07, ((char *) -1) }, { '[', 1e+06, 0, '.',
    { ((char *) 0) } } },
  { "Ucrit", 1, { 4.5e+06, ((char *) -1) }, { '[', 2e+06, 2.5e+07, ']',
    { ((char *) 0) } } },
  { "Lambda", 1, { 0.23, ((char *) -1) }, { '[', 0.1, 0, '.',
    { ((char *) 0) } } },
  { "Weta", 1, { 0.05, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Leta", 1, { 0.28, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Q0", 1, { 0.00028, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Lk", 1, { 5e-07, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Tcv", 1, { 0.0015, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Bex", 1, { -1.5, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Ucex", 1, { 1.7, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Ibbt", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Hdif", 1, { 9e-07, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Rsh", 1, { 510, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Rsc", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Rdc", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Cgso", 1, { 1.5e-10, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Cgdo", 1, { 1.5e-10, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Cgbo", 1, { 4e-10, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Iba", 1, { 2e+08, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Ibb", 1, { 3.5e+08, ((char *) -1) }, { '[', 1e+08, 0, '.',
    { ((char *) 0) } } },
  { "Ibn", 1, { 1, ((char *) -1) }, { '[', 0.1, 0, '.',
    { ((char *) 0) } } },
  { "Kf", 1, { 1e-27, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Af", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Avto", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Akp", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Agamma", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "N", 1, { 1, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "Is", 1, { 1e-14, ((char *) -1) }, { '[', 1e-20, 0, '.',
    { ((char *) 0) } } },
  { "Bv", 1, { 100, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "Ibv", 1, { 0.001, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "Vj", 1, { 1, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "Cj0", 1, { 3e-13, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "M", 1, { 0.5, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "Area", 1, { 1, ((char *) -1) }, { '[', 0.001, 0, '.',
    { ((char *) 0) } } },
  { "Fc", 1, { 0.5, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "Tt", 1, { 1e-10, ((char *) -1) }, { '[', 1e-20, 0, '.',
    { ((char *) 0) } } },
  { "Xti", 1, { 3, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "Xpart", 1, { 0.4, ((char *) -1) }, { '[', 0, 1, ']',
    { ((char *) 0) } } },
  { "Tnom", 1, { 26.85, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_EKV26MOS[] = {
  { "nmos", 0, { 1, ((char *) -1) }, { '[', 0, 1, ']',
    { ((char *) 0) } } },
  { "pmos", 0, { 0, ((char *) -1) }, { '[', 0, 1, ']',
    { ((char *) 0) } } },
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_EKV26MOS = {
  "EKV26MOS", 4, 0, 0, 1, req_EKV26MOS, opt_EKV26MOS };

static struct property_t req_CCVS[] = {
  { "G", 1, { 1, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_CCVS[] = {
  { "T", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_CCVS = {
  "CCVS", 4, 0, 0, 0, req_CCVS, opt_CCVS };

static struct property_t req_PShift[] = {
  { "phi", 1, { 1e-90, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_PShift[] = {
  { "Zref", 1, { 50, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_PShift = {
  "PShift", 2, 0, 0, 0, req_PShift, opt_PShift };

static struct property_t req_CAPQ[] = {
  { "C", 1, { 1e-12, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Q", 1, { 100, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "f", 1, { 1e+08, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Mode", 2, { 0, "Linear" }, { '.', 0, 0, '.',
    { "Linear", "SquareRoot", "Constant", ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_CAPQ[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_CAPQ = {
  "CAPQ", 2, 0, 0, 0, req_CAPQ, opt_CAPQ };

static struct property_t req_WProbe[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_WProbe[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_WProbe = {
  "WProbe", 4, 0, 0, 0, req_WProbe, opt_WProbe };

static struct property_t req_pad4bit[] = {
  { "Number", 0, { 0, ((char *) -1) }, { '[', 0, 15, ']',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_pad4bit[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_pad4bit = {
  "pad4bit", 4, 0, 0, 1, req_pad4bit, opt_pad4bit };

static struct property_t req_ETR[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_ETR[] = {
  { "IntegrationMethod", 2, { 0, "Trapezoidal" }, { '.', 0, 0, '.',
    { "Euler", "Trapezoidal", "Gear", "AdamsMoulton", ((char *) 0) } } },
  { "Order", 0, { 2, ((char *) -1) }, { '[', 1, 6, ']',
    { ((char *) 0) } } },
  { "InitialStep", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "MinStep", 1, { 1e-16, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "MaxStep", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "MaxIter", 0, { 150, ((char *) -1) }, { '[', 2, 10000, ']',
    { ((char *) 0) } } },
  { "abstol", 1, { 1e-12, ((char *) -1) }, { ']', 0, 1, ']',
    { ((char *) 0) } } },
  { "vntol", 1, { 1e-06, ((char *) -1) }, { ']', 0, 1, ']',
    { ((char *) 0) } } },
  { "reltol", 1, { 0.001, ((char *) -1) }, { ']', 0, 1, ']',
    { ((char *) 0) } } },
  { "LTEabstol", 1, { 1e-06, ((char *) -1) }, { ']', 0, 1, ']',
    { ((char *) 0) } } },
  { "LTEreltol", 1, { 0.001, ((char *) -1) }, { ']', 0, 1, ']',
    { ((char *) 0) } } },
  { "LTEfactor", 1, { 1, ((char *) -1) }, { '[', 1, 16, ']',
    { ((char *) 0) } } },
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { "Solver", 2, { 0, "CroutLU" }, { '.', 0, 0, '.',
    { "CroutLU", "DoolittleLU", "HouseholderQR", "HouseholderLQ", "GolubSVD", ((char *) 0) } } },
  { "relaxTSR", 2, { 0, "no" }, { '.', 0, 0, '.',
    { "yes", "no", ((char *) 0) } } },
  { "initialDC", 2, { 0, "yes" }, { '.', 0, 0, '.',
    { "yes", "no", ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_ETR = {
  "ETR", 0, 1, 0, 0, req_ETR, opt_ETR };

static struct property_t req_SW[] = {
  { "Type", 2, { 0, "lin" }, { '.', 0, 0, '.',
    { "lin", "log", "list", "const", ((char *) 0) } } },
  { "Param", 2, { 0, "R1" }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Sim", 2, { 0, "DC1" }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_SW[] = {
  { "Points", 0, { 5, ((char *) -1) }, { '[', 2, 0, '.',
    { ((char *) 0) } } },
  { "Stop", 1, { 50, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Start", 1, { 5, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Values", 3, { 5, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_SW = {
  "SW", 0, 1, 0, 0, req_SW, opt_SW };

static struct property_t req_andor4x2[] = {
  { "TR", 1, { 6, ((char *) -1) }, { '[', 1, 20, ']',
    { ((char *) 0) } } },
  { "Delay", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_andor4x2[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_andor4x2 = {
  "andor4x2", 9, 0, 0, 1, req_andor4x2, opt_andor4x2 };

static struct property_t req_Vdc[] = {
  { "U", 1, { 1, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Vdc[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Vdc = {
  "Vdc", 2, 0, 0, 0, req_Vdc, opt_Vdc };

static struct property_t req_andor4x3[] = {
  { "TR", 1, { 6, ((char *) -1) }, { '[', 1, 20, ']',
    { ((char *) 0) } } },
  { "Delay", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_andor4x3[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_andor4x3 = {
  "andor4x3", 13, 0, 0, 1, req_andor4x3, opt_andor4x3 };

static struct property_t req_BOND[] = {
  { "D", 1, { 2.5e-05, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "L", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "H", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "mur", 1, { 1, ((char *) -1) }, { '[', 1, 100, ']',
    { ((char *) 0) } } },
  { "rho", 1, { 2.2e-08, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Model", 2, { 0, "FREESPACE" }, { '.', 0, 0, '.',
    { "FREESPACE", "MIRROR", "DESCHARLES", ((char *) 0) } } },
  { "Subst", 2, { 0, "Subst1" }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_BOND[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_BOND = {
  "BOND", 2, 0, 0, 0, req_BOND, opt_BOND };

static struct property_t req_CLIN[] = {
  { "W", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "S", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "L", 1, { 0.01, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Subst", 2, { 0, "Subst1" }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_CLIN[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { "Backside", 2, { 0, "Metal" }, { '.', 0, 0, '.',
    { "Metal", "Air", ((char *) 0) } } },
  { "Approx", 2, { 0, "no" }, { '.', 0, 0, '.',
    { "yes", "no", ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_CLIN = {
  "CLIN", 2, 0, 0, 0, req_CLIN, opt_CLIN };

static struct property_t req_NOR[] = {
  { "V", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_NOR[] = {
  { "t", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "TR", 1, { 10, ((char *) -1) }, { '[', 1, 100, ']',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_NOR = {
  "NOR", -1, 0, 0, 1, req_NOR, opt_NOR };

static struct property_t req_andor4x4[] = {
  { "TR", 1, { 6, ((char *) -1) }, { '[', 1, 20, ']',
    { ((char *) 0) } } },
  { "Delay", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_andor4x4[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_andor4x4 = {
  "andor4x4", 17, 0, 0, 1, req_andor4x4, opt_andor4x4 };

static struct property_t req_SP[] = {
  { "Type", 2, { 0, "lin" }, { '.', 0, 0, '.',
    { "lin", "log", "list", "const", ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_SP[] = {
  { "Noise", 2, { 0, "no" }, { '.', 0, 0, '.',
    { "yes", "no", ((char *) 0) } } },
  { "NoiseIP", 0, { 1, ((char *) -1) }, { '[', 1, 256, ']',
    { ((char *) 0) } } },
  { "NoiseOP", 0, { 2, ((char *) -1) }, { '[', 1, 256, ']',
    { ((char *) 0) } } },
  { "Start", 1, { 1e+09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Stop", 1, { 1e+10, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Points", 0, { 10, ((char *) -1) }, { '[', 2, 0, '.',
    { ((char *) 0) } } },
  { "Values", 3, { 10, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "saveCVs", 2, { 0, "no" }, { '.', 0, 0, '.',
    { "yes", "no", ((char *) 0) } } },
  { "saveAll", 2, { 0, "no" }, { '.', 0, 0, '.',
    { "yes", "no", ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_SP = {
  "SP", 0, 1, 0, 0, req_SP, opt_SP };

static struct property_t req_MTEE[] = {
  { "W1", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "W2", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "W3", 1, { 0.002, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Subst", 2, { 0, "Subst1" }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "MSDispModel", 2, { 0, "Kirschning" }, { '.', 0, 0, '.',
    { "Kirschning", "Kobayashi", "Yamashita", "Getsinger", "Schneider", "Pramanick", "Hammerstad", ((char *) 0) } } },
  { "MSModel", 2, { 0, "Hammerstad" }, { '.', 0, 0, '.',
    { "Wheeler", "Schneider", "Hammerstad", ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_MTEE[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_MTEE = {
  "MTEE", 3, 0, 0, 0, req_MTEE, opt_MTEE };

static struct property_t req_Coupler[] = {
  { "k", 1, { 0.707107, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "phi", 1, { 0, ((char *) -1) }, { '[', -180, 180, ']',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Coupler[] = {
  { "Z", 1, { 50, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Coupler = {
  "Coupler", 4, 0, 0, 0, req_Coupler, opt_Coupler };

static struct property_t req_DLS_1ton[] = {
  { "LEVEL", 1, { 5, ((char *) -1) }, { '[', 1, 0, '.',
    { ((char *) 0) } } },
  { "Delay", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_DLS_1ton[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_DLS_1ton = {
  "DLS_1ton", 2, 0, 0, 1, req_DLS_1ton, opt_DLS_1ton };

static struct property_t req_Vac[] = {
  { "U", 1, { 1, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Vac[] = {
  { "Phase", 1, { 0, ((char *) -1) }, { '[', -360, 360, ']',
    { ((char *) 0) } } },
  { "Theta", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "f", 1, { 1e+09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Vac = {
  "Vac", 2, 0, 0, 0, req_Vac, opt_Vac };

static struct property_t req_photodiode[] = {
  { "N", 1, { 1.35, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "Rseries", 1, { 0.001, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "Is", 1, { 3.4e-13, ((char *) -1) }, { '[', 1e-20, 0, '.',
    { ((char *) 0) } } },
  { "Bv", 1, { 60, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "Ibv", 1, { 0.001, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "Vj", 1, { 0.7, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "Cj0", 1, { 6e-11, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "M", 1, { 0.5, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "Area", 1, { 1, ((char *) -1) }, { '[', 1, 0, '.',
    { ((char *) 0) } } },
  { "Tnom", 1, { 26.85, ((char *) -1) }, { '[', -273, 0, '.',
    { ((char *) 0) } } },
  { "Fc", 1, { 0.5, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "Tt", 1, { 1e-08, ((char *) -1) }, { '[', 1e-20, 0, '.',
    { ((char *) 0) } } },
  { "Xti", 1, { 3, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "Eg", 1, { 1.16, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "Responsivity", 1, { 0.5, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "Rsh", 1, { 5e+08, ((char *) -1) }, { '[', 1e-06, 0, '.',
    { ((char *) 0) } } },
  { "QEpercent", 1, { 80, ((char *) -1) }, { '[', 0, 100, ']',
    { ((char *) 0) } } },
  { "Lambda", 1, { 900, ((char *) -1) }, { '[', 100, 2000, ']',
    { ((char *) 0) } } },
  { "LEVEL", 0, { 1, ((char *) -1) }, { '[', 1, 2, ']',
    { ((char *) 0) } } },
  { "Kf", 1, { 1e-12, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Af", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Ffe", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_photodiode[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_photodiode = {
  "photodiode", 3, 0, 0, 1, req_photodiode, opt_photodiode };

static struct property_t req_NodeSet[] = {
  { "U", 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_NodeSet[] = {
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_NodeSet = {
  "NodeSet", 1, 0, 0, 0, req_NodeSet, opt_NodeSet };

static struct property_t req_TR[] = {
  { "Type", 2, { 0, "lin" }, { '.', 0, 0, '.',
    { "lin", "log", ((char *) 0) } } },
  { "Start", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Stop", 1, { 0.001, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Points", 0, { 10, ((char *) -1) }, { '[', 2, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_TR[] = {
  { "IntegrationMethod", 2, { 0, "Trapezoidal" }, { '.', 0, 0, '.',
    { "Euler", "Trapezoidal", "Gear", "AdamsMoulton", ((char *) 0) } } },
  { "Order", 0, { 2, ((char *) -1) }, { '[', 1, 6, ']',
    { ((char *) 0) } } },
  { "InitialStep", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "MinStep", 1, { 1e-16, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "MaxStep", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "MaxIter", 0, { 150, ((char *) -1) }, { '[', 2, 10000, ']',
    { ((char *) 0) } } },
  { "abstol", 1, { 1e-12, ((char *) -1) }, { ']', 0, 1, ']',
    { ((char *) 0) } } },
  { "vntol", 1, { 1e-06, ((char *) -1) }, { ']', 0, 1, ']',
    { ((char *) 0) } } },
  { "reltol", 1, { 0.001, ((char *) -1) }, { ']', 0, 1, ']',
    { ((char *) 0) } } },
  { "LTEabstol", 1, { 1e-06, ((char *) -1) }, { ']', 0, 1, ']',
    { ((char *) 0) } } },
  { "LTEreltol", 1, { 0.001, ((char *) -1) }, { ']', 0, 1, ']',
    { ((char *) 0) } } },
  { "LTEfactor", 1, { 1, ((char *) -1) }, { '[', 1, 16, ']',
    { ((char *) 0) } } },
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { "Solver", 2, { 0, "CroutLU" }, { '.', 0, 0, '.',
    { "CroutLU", "DoolittleLU", "HouseholderQR", "HouseholderLQ", "GolubSVD", ((char *) 0) } } },
  { "relaxTSR", 2, { 0, "no" }, { '.', 0, 0, '.',
    { "yes", "no", ((char *) 0) } } },
  { "initialDC", 2, { 0, "yes" }, { '.', 0, 0, '.',
    { "yes", "no", ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_TR = {
  "TR", 0, 1, 0, 0, req_TR, opt_TR };

static struct property_t req_CTLIN[] = {
  { "Ze", 1, { 50, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Zo", 1, { 50, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "L", 1, { 0.001, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_CTLIN[] = {
  { "Ere", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Ero", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Ae", 1, { 1, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "Ao", 1, { 1, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_CTLIN = {
  "CTLIN", 4, 0, 0, 0, req_CTLIN, opt_CTLIN };

static struct property_t req_TAPEREDLINE[] = {
  { "Z1", 1, { 50, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "Z2", 1, { 100, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "L", 1, { 0.075, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "Weighting", 2, { 0, "Exponential" }, { '.', 0, 0, '.',
    { "Exponential", "Linear", "Triangular", "Klopfenstein", ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_TAPEREDLINE[] = {
  { "Gamma_max", 1, { 0.1, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "Alpha", 1, { 0, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_TAPEREDLINE = {
  "TAPEREDLINE", 2, 0, 0, 0, req_TAPEREDLINE, opt_TAPEREDLINE };

static struct property_t req_RECTLINE[] = {
  { "a", 1, { 0.0286, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "b", 1, { 0.01016, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "L", 1, { 1.5, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "er", 1, { 1, ((char *) -1) }, { '[', 1, 100, ']',
    { ((char *) 0) } } },
  { "mur", 1, { 1, ((char *) -1) }, { '[', 1, 100, ']',
    { ((char *) 0) } } },
  { "tand", 1, { 0.0004, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "rho", 1, { 2.2e-08, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_RECTLINE[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { "Material", 2, { 0, "unspecified" }, { '.', 0, 0, '.',
    { "unspecified", "Copper", "StainlessSteel", "Gold", ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_RECTLINE = {
  "RECTLINE", 2, 0, 0, 0, req_RECTLINE, opt_RECTLINE };

static struct property_t req_COAX[] = {
  { "D", 1, { 0.00295, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "d", 1, { 0.0009, ((char *) -1) }, { ']', 0, 0, '.',
    { ((char *) 0) } } },
  { "L", 1, { 1.5, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } },
  { "er", 1, { 2.29, ((char *) -1) }, { '[', 1, 100, ']',
    { ((char *) 0) } } },
  { "mur", 1, { 1, ((char *) -1) }, { '[', 1, 100, ']',
    { ((char *) 0) } } },
  { "tand", 1, { 0.0004, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "rho", 1, { 2.2e-08, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_COAX[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_COAX = {
  "COAX", 2, 0, 0, 0, req_COAX, opt_COAX };

static struct property_t req_Hybrid[] = {
  { "phi", 1, { 0, ((char *) -1) }, { '[', -180, 180, ']',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Hybrid[] = {
  { "Zref", 1, { 50, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Hybrid = {
  "Hybrid", 4, 0, 0, 0, req_Hybrid, opt_Hybrid };

static struct property_t req_Vnoise[] = {
  { "u", 1, { 1e-06, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_Vnoise[] = {
  { "a", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "c", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "e", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_Vnoise = {
  "Vnoise", 2, 0, 0, 0, req_Vnoise, opt_Vnoise };

static struct property_t req_VVnoise[] = {
  { "v1", 1, { 1e-06, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "v2", 1, { 1e-06, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "C", 1, { 0.5, ((char *) -1) }, { '[', -1, 1, ']',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_VVnoise[] = {
  { "a", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "c", 1, { 1, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { "e", 1, { 0, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_VVnoise = {
  "VVnoise", 4, 0, 0, 0, req_VVnoise, opt_VVnoise };

static struct property_t req_mux4to1[] = {
  { "TR", 1, { 6, ((char *) -1) }, { '[', 1, 20, ']',
    { ((char *) 0) } } },
  { "Delay", 1, { 1e-09, ((char *) -1) }, { '[', 0, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct property_t opt_mux4to1[] = {
  { "Temp", 1, { 26.85, ((char *) -1) }, { '[', -273.15, 0, '.',
    { ((char *) 0) } } },
  { ((char *) 0), 1, { 0, ((char *) -1) }, { '.', 0, 0, '.',
    { ((char *) 0) } } }
};

static struct define_t def_mux4to1 = {
  "mux4to1", 8, 0, 0, 1, req_mux4to1, opt_mux4to1 };

static struct define_t def_End = {
  ((char *) 0), -1, 1, 0, 0, req_Def, opt_Def };

// List of available components.
struct define_t qucs_definition_available[] =
{
  def_MLIN,
  def_Buf,
  def_nigbt,
  def_Iac,
  def_AM_Mod,
  def_TLIN4P,
  def_SPIRALIND,
  def_MCOUPLED,
  def_MOSFET,
  def_Inv,
  def_comp_2bit,
  def_Idc,
  def_CIRCULARLOOP,
  def_DigiSource,
  def_Isolator,
  def_phototransistor,
  def_sTr,
  def_JFET,
  def_comp_1bit,
  def_SCR,
  def_Diode,
  def_MLANGE,
  def_MCORN,
  def_greytobinary4bit,
  def_Inoise,
  def_mux8to1,
  def_MRSTUB,
  def_CSTEP,
  def_Attenuator,
  def_Vexp,
  def_RTD,
  def_Diac,
  def_PM_Mod,
  def_Switch,
  def_Irect,
  def_MGAP,
  def_MUT2,
  def_INDQ,
  def_SPDfile,
  def_XOR,
  def_MOPEN,
  def_MMBEND,
  def_NAND,
  def_IProbe,
  def_CIRCLINE,
  def_TWIST,
  def_Amp,
  def_MESFET,
  def_potentiometer,
  def_DCBlock,
  def_fa2b,
  def_Gyrator,
  def_XNOR,
  def_fa1b,
  def_mod_amp,
  def_gatedDlatch,
  def_jkff_SR,
  def_Ifile,
  def_DCFeed,
  def_Ipulse,
  def_Def,
  def_binarytogrey4bit,
  def_comp_4bit,
  def_IVnoise,
  def_CCCS,
  def_AC,
  def_MSTEP,
  def_C,
  def_VCVS,
  def_Sub,
  def_ha1b,
  def_log_amp,
  def_RFEDD,
  def_BJT,
  def_dmux4to16,
  def_Vfile,
  def_SPfile,
  def_MCROSS,
  def_MUT,
  def_dmux2to4,
  def_DC,
  def_L,
  def_OR,
  def_MUTX,
  def_COPEN,
  def_TLIN,
  def_VProbe,
  def_dmux3to8,
  def_hpribin4bit,
  def_mux2to1,
  def_R,
  def_HB,
  def_Circulator,
  def_vcresistor,
  def_Relais,
  def_Triac,
  def_Vrect,
  def_CGAP,
  def_pad3bit,
  def_EDD,
  def_dff_SR,
  def_tff_SR,
  def_Tr,
  def_logic_0,
  def_Vpulse,
  def_logic_1,
  def_ECVS,
  def_Iexp,
  def_AND,
  def_CSHORT,
  def_BiasT,
  def_OpAmp,
  def_pad2bit,
  def_IInoise,
  def_DLS_nto1,
  def_Pac,
  def_RLCG,
  def_SUBST,
  def_MVIA,
  def_VCCS,
  def_EKV26MOS,
  def_CCVS,
  def_PShift,
  def_CAPQ,
  def_WProbe,
  def_pad4bit,
  def_ETR,
  def_SW,
  def_andor4x2,
  def_Vdc,
  def_andor4x3,
  def_BOND,
  def_CLIN,
  def_NOR,
  def_andor4x4,
  def_SP,
  def_MTEE,
  def_Coupler,
  def_DLS_1ton,
  def_Vac,
  def_photodiode,
  def_NodeSet,
  def_TR,
  def_CTLIN,
  def_TAPEREDLINE,
  def_RECTLINE,
  def_COAX,
  def_Hybrid,
  def_Vnoise,
  def_VVnoise,
  def_mux4to1,
  def_End
};

#endif /* __QUCSDEFS_H__ */
