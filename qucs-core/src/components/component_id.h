/*
 * component_id.h - global component identifier header file
 *
 * Copyright (C) 2003-2011 Stefan Jahn <stefan@lkcc.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * $Id$
 *
 */

#ifndef __COMPONENT_ID_H__
#define __COMPONENT_ID_H__

namespace qucs {

/* Enumerate component type identifiers. */
enum circuit_type {
  CIR_UNKNOWN = -1,

  // linear helper components
  CIR_GROUND,
  CIR_OPEN,
  CIR_SHORT,
  CIR_TEE,
  CIR_CROSS,
  CIR_ITRAFO,

  // linear components
  CIR_RESISTOR,
  CIR_CAPACITOR,
  CIR_CAPQ,
  CIR_INDUCTOR,
  CIR_INDQ,
  CIR_MUTUAL,
  CIR_MUTUAL2,
  CIR_MUTUALX,
  CIR_VCCS,
  CIR_CCCS,
  CIR_CCVS,
  CIR_VCVS,
  CIR_DCBLOCK,
  CIR_DCFEED,
  CIR_BIASTEE,
  CIR_PAC,
  CIR_ATTENUATOR,
  CIR_CIRCULATOR,
  CIR_ISOLATOR,
  CIR_TRAFO,
  CIR_STRAFO,
  CIR_VDC,
  CIR_IDC,
  CIR_VAC,
  CIR_IAC,
  CIR_VEXP,
  CIR_IEXP,
  CIR_VFILE,
  CIR_IFILE,
  CIR_VAM,
  CIR_VPM,
  CIR_PHASESHIFTER,
  CIR_GYRATOR,
  CIR_TSWITCH,
  CIR_RELAIS,
  CIR_TLINE,
  CIR_CTLINE,
  CIR_TAPEREDLINE,
  CIR_COAXLINE,
  CIR_CIRCULAR,
  CIR_RECTANGULAR,
  CIR_TWISTEDPAIR,
  CIR_TLINE4P,
  CIR_RLCG,
  CIR_IPROBE,
  CIR_WPROBE,
  CIR_VPROBE,
  CIR_SPFILE,
  CIR_SPDFILE,
  CIR_VPULSE,
  CIR_IPULSE,
  CIR_VRECT,
  CIR_IRECT,
  CIR_AMPLIFIER,
  CIR_COUPLER,
  CIR_HYBRID,
  CIR_RFEDD,

  // noise sources
  CIR_VNOISE,
  CIR_INOISE,
  CIR_IINOISE,
  CIR_IVNOISE,
  CIR_VVNOISE,

  // microstrip components
  CIR_MSLINE,
  CIR_MSCORNER,
  CIR_MSMBEND,
  CIR_MSSTEP,
  CIR_MSOPEN,
  CIR_MSGAP,
  CIR_MSCOUPLED,
  CIR_MSLANGE,
  CIR_MSTEE,
  CIR_MSCROSS,
  CIR_MSVIA,
  CIR_MSRSTUB,
  CIR_BONDWIRE,
  CIR_SPIRALIND,
  CIR_CIRCULARLOOP,

  // coplanar components
  CIR_CPWLINE,
  CIR_CPWOPEN,
  CIR_CPWSHORT,
  CIR_CPWGAP,
  CIR_CPWSTEP,

  // non-linear components
  CIR_OPAMP,
  CIR_DIODE,
  CIR_JFET,
  CIR_BJT,
  CIR_MOSFET,
  CIR_EQNDEFINED,
  CIR_DIAC,
  CIR_TRIAC,
  CIR_THYRISTOR,
  CIR_TUNNELDIODE,

  // digital components
  CIR_INVERTER,
  CIR_NOR,
  CIR_OR,
  CIR_NAND,
  CIR_AND,
  CIR_XNOR,
  CIR_XOR,
  CIR_DIGISOURCE,
  CIR_BUFFER,

  // verilog devices
  CIR_mod_amp,
  CIR_log_amp,
  CIR_potentiometer,
  CIR_MESFET,
  CIR_EKV26MOS,
  CIR_photodiode,
  CIR_phototransistor,
  CIR_nigbt,

  // digital verilog devices
  CIR_dff_SR,
  CIR_tff_SR,
  CIR_jkff_SR,
  CIR_gatedDlatch,
  CIR_logic_1,
  CIR_logic_0,
  CIR_mux2to1,
  CIR_mux4to1,
  CIR_mux8to1,
  CIR_DLS_nto1,
  CIR_DLS_1ton,
  CIR_andor4x2,
  CIR_andor4x3,
  CIR_andor4x4,
  CIR_dmux2to4,
  CIR_dmux3to8,
  CIR_dmux4to16,
  CIR_ha1b,
  CIR_fa1b,
  CIR_fa2b,
  CIR_pad2bit,
  CIR_pad3bit,
  CIR_pad4bit,
  CIR_binarytogrey4bit,
  CIR_greytobinary4bit,
  CIR_comp_1bit,
  CIR_comp_2bit,
  CIR_comp_4bit,
  CIR_hpribin4bit,

  // external interface components
  CIR_ECVS,

};

} // namespace qucs

#endif /* __COMPONENT_ID_H__ */
