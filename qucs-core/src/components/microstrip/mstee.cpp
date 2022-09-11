/*
 * mstee.cpp - microstrip t-junction class implementation
 *
 * Copyright (C) 2004, 2005, 2006 Stefan Jahn <stefan@lkcc.org>
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

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <algorithm>

#include "component.h"
#include "substrate.h"
#include "device.h"
#include "msline.h"
#include "mstee.h"

using namespace qucs;
using namespace qucs::device;

mstee::mstee () : circuit (3) {
  lineA = lineB = line2 = NULL;
  type = CIR_MSTEE;
}

void mstee::initSP (void) {
  allocMatrixS ();
  initLines ();
  lineA->initSP ();
  lineB->initSP ();
  line2->initSP ();
}

void mstee::initNoiseSP (void) {
  allocMatrixN ();
  lineA->initNoiseSP ();
  lineB->initNoiseSP ();
  line2->initNoiseSP ();
}

void mstee::initLines (void) {
  lineA = splitMicrostrip (this, lineA, getNet (), "LineA", "NodeA", NODE_1);
  lineA->setProperty ("W", getPropertyDouble ("W1"));
  lineA->setProperty ("Temp", getPropertyDouble ("Temp"));
  lineA->setProperty ("Model", getPropertyString ("MSModel"));
  lineA->setProperty ("DispModel", getPropertyString ("MSDispModel"));
  lineA->setSubstrate (getSubstrate ());

  lineB = splitMicrostrip (this, lineB, getNet (), "LineB", "NodeB", NODE_2);
  lineB->setProperty ("W", getPropertyDouble ("W2"));
  lineB->setProperty ("Temp", getPropertyDouble ("Temp"));
  lineB->setProperty ("Model", getPropertyString ("MSModel"));
  lineB->setProperty ("DispModel", getPropertyString ("MSDispModel"));
  lineB->setSubstrate (getSubstrate ());

  line2 = splitMicrostrip (this, line2, getNet (), "Line2", "Node2", NODE_3);
  line2->setProperty ("W", getPropertyDouble ("W3"));
  line2->setProperty ("Temp", getPropertyDouble ("Temp"));
  line2->setProperty ("Model", getPropertyString ("MSModel"));
  line2->setProperty ("DispModel", getPropertyString ("MSDispModel"));
  line2->setSubstrate (getSubstrate ());
}

void mstee::calcSP (nr_double_t frequency) {
  calcPropagation (frequency);

  lineA->setProperty ("L", La);
  lineB->setProperty ("L", Lb);
  line2->setProperty ("L", L2);
  lineA->calcSP (frequency);
  lineB->calcSP (frequency);
  line2->calcSP (frequency);

  // calculate S-parameters
  nr_complex_t n1 = Ta2 * nr_complex_t (1 + 1 / Tb2, Bt * z0);
  nr_complex_t n2 = Tb2 * nr_complex_t (1 + 1 / Ta2, Bt * z0);
  nr_complex_t n3 = nr_complex_t (1 / Ta2 + 1 / Tb2, Bt * z0);
  setS (NODE_1, NODE_1, (1.0 - n1) / (1.0 + n1));
  setS (NODE_2, NODE_2, (1.0 - n2) / (1.0 + n2));
  setS (NODE_3, NODE_3, (1.0 - n3) / (1.0 + n3));
  setS (NODE_1, NODE_3, 2.0 * std::sqrt (Ta2) / (1.0 + n1));
  setS (NODE_3, NODE_1, 2.0 * std::sqrt (Ta2) / (1.0 + n1));
  setS (NODE_2, NODE_3, 2.0 * std::sqrt (Tb2) / (1.0 + n2));
  setS (NODE_3, NODE_2, 2.0 * std::sqrt (Tb2) / (1.0 + n2));
  setS (NODE_1, NODE_2, 2.0 / (std::sqrt (Ta2 * Tb2) * nr_complex_t (1, Bt * z0) +
			       std::sqrt (Ta2 / Tb2) + std::sqrt (Tb2 / Ta2)));
  setS (NODE_2, NODE_1, 2.0 / (std::sqrt (Ta2 * Tb2) * nr_complex_t (1, Bt * z0) +
			       std::sqrt (Ta2 / Tb2) + std::sqrt (Tb2 / Ta2)));
}

void mstee::calcPropagation (nr_double_t f) {

  const char * SModel = getPropertyString ("MSModel");
  const char * DModel = getPropertyString ("MSDispModel");
  substrate * subst = getSubstrate ();
  nr_double_t er = subst->getPropertyDouble ("er");
  nr_double_t h  = subst->getPropertyDouble ("h");
  nr_double_t t  = subst->getPropertyDouble ("t");
  nr_double_t Wa = getPropertyDouble ("W1");
  nr_double_t Wb = getPropertyDouble ("W2");
  nr_double_t W2 = getPropertyDouble ("W3");

  nr_double_t Zla, Zlb, Zl2, Era, Erb, Er2;

  // computation of impedances and effective dielectric constants
  nr_double_t ZlEff, ErEff, WEff;
  msline::analyseQuasiStatic (Wa, h, t, er, SModel, ZlEff, ErEff, WEff);
  msline::analyseDispersion  (Wa, h, er, ZlEff, ErEff, f, DModel,
			      Zla, Era);
  msline::analyseQuasiStatic (Wb, h, t, er, SModel, ZlEff, ErEff, WEff);
  msline::analyseDispersion  (Wb, h, er, ZlEff, ErEff, f, DModel,
			      Zlb, Erb);
  msline::analyseQuasiStatic (W2, h, t, er, SModel, ZlEff, ErEff, WEff);
  msline::analyseDispersion  (W2, h, er, ZlEff, ErEff, f, DModel,
			      Zl2, Er2);

  // local variables
  nr_double_t Da, Db, D2, fpa, fpb, lda, ldb, da, db, d2, r, q;

  // equivalent parallel plate line widths
  Da = Z0 / Zla * h / std::sqrt (Era);
  Db = Z0 / Zlb * h / std::sqrt (Erb);
  D2 = Z0 / Zl2 * h / std::sqrt (Er2);

  // first higher order mode cut-off frequencies
  fpa = 0.4e6 * Zla / h;
  fpb = 0.4e6 * Zlb / h;

  // effective wavelengths of quasi-TEM mode
  lda = C0 / std::sqrt (Era) / f;
  ldb = C0 / std::sqrt (Erb) / f;

  // main arm displacements
  da = 0.055 * D2 * Zla / Zl2 * (1 - 2 * Zla / Zl2 * sqr (f / fpa));
  db = 0.055 * D2 * Zlb / Zl2 * (1 - 2 * Zlb / Zl2 * sqr (f / fpb));

  // length of lines in the main arms
  La = 0.5 * W2 - da;
  Lb = 0.5 * W2 - db;

  // displacement and length of line in the side arm
  r = std::sqrt (Zla * Zlb) / Zl2;
  q = sqr (f) / fpa / fpb;
  d2 = std::sqrt (Da * Db) * (0.5 - r * (0.05 + 0.7 * std::exp (-1.6 * r) +
				    0.25 * r * q - 0.17 * std::log (r)));
  L2 = 0.5 * std::max (Wa, Wb) - d2;

  // turn ratio of transformers in main arms
  Ta2 = 1 - pi * sqr (f / fpa) *
        (sqr (Zla / Zl2) / 12 + sqr (0.5 - d2 / Da));
  Tb2 = 1 - pi * sqr (f / fpb) *
        (sqr (Zlb / Zl2) / 12 + sqr (0.5 - d2 / Db));
  Ta2 = std::max (Ta2, NR_TINY);
  Tb2 = std::max (Tb2, NR_TINY);

  // shunt susceptance
  Bt = 5.5 * std::sqrt (Da * Db / lda / ldb) * (er + 2) / er /
    Zl2 / std::sqrt (Ta2 * Tb2) * std::sqrt (da * db) / D2 *
    (1 + 0.9 * std::log (r) + 4.5 * r * q - 4.4 * std::exp (-1.3 * r) -
     20 * sqr (Zl2 / Z0));
}

/* This function can be used to create an extra microstrip circuit.
   If the 'line' argument is NULL then the new circuit is created, the
   nodes get re-arranged and it is inserted into the given
   netlist. The given arguments can be explained as follows.
   base:     calling circuit (this)
   line:     additional microstrip line circuit (can be NULL)
   subnet:   the netlist object
   c:        name of the additional circuit
   n:        name of the inserted (internal) node
   internal: number of new internal node (the original external node) */
circuit * splitMicrostrip (circuit * base, circuit * line, net * subnet,
			   const char * c, const char * n, int internal) {
  if (line == NULL) {
    line = new msline ();
    const std::string &name = circuit::createInternal (c, base->getName ());
    const std::string &node = circuit::createInternal (n, base->getName ());
    line->setName (name);
    line->setNode (0, base->getNode(internal)->getName ());
    line->setNode (1, node, 1);
    subnet->insertCircuit (line);
  }
  base->setNode (internal, line->getNode(1)->getName (), 1);
  return line;
}

/* This function is the counterpart of the above routine.  It removes
   the microstrip circuit from the netlist and re-assigns the original
   node. */
void disableMicrostrip (circuit * base, circuit * line, net * subnet,
			int internal) {
  if (line != NULL) {
    subnet->removeCircuit (line, 0);
    base->setNode (internal, line->getNode(1)->getName (), 0);
  }
}

void mstee::initDC (void) {
  setVoltageSources (2);
  setInternalVoltageSource (1);
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
  voltageSource (VSRC_2, NODE_1, NODE_3);
  if (deviceEnabled (lineA)) {
    disableMicrostrip (this, lineA, getNet (), NODE_1);
  }
  if (deviceEnabled (lineB)) {
    disableMicrostrip (this, lineB, getNet (), NODE_2);
  }
  if (deviceEnabled (line2)) {
    disableMicrostrip (this, line2, getNet (), NODE_3);
  }
}

void mstee::initAC (void) {
  setVoltageSources (3);
  setInternalVoltageSource (1);
  allocMatrixMNA ();
  setB (NODE_1, VSRC_1, +1); setB (NODE_2, VSRC_2, +1);
  setB (NODE_3, VSRC_3, +1);
  setC (VSRC_1, NODE_1, -1); setC (VSRC_2, NODE_2, -1);
  setC (VSRC_3, NODE_3, -1);
  initLines ();
  lineA->initAC ();
  lineB->initAC ();
  line2->initAC ();
}

void mstee::initNoiseAC (void) {
  allocMatrixN (getVoltageSources ());
  lineA->initNoiseAC ();
  lineB->initNoiseAC ();
  line2->initNoiseAC ();
}

void mstee::calcAC (nr_double_t frequency) {
  calcPropagation (frequency);

  lineA->setProperty ("L", La);
  lineB->setProperty ("L", Lb);
  line2->setProperty ("L", L2);
  lineA->calcAC (frequency);
  lineB->calcAC (frequency);
  line2->calcAC (frequency);

  // calculate Z-parameters
  setD (VSRC_1, VSRC_1, nr_complex_t (0, -1 / Ta2 / Bt));
  setD (VSRC_1, VSRC_2, nr_complex_t (0, -1 / std::sqrt (Ta2 * Tb2) / Bt));
  setD (VSRC_1, VSRC_3, nr_complex_t (0, -1 / std::sqrt (Ta2) / Bt));
  setD (VSRC_2, VSRC_1, nr_complex_t (0, -1 / std::sqrt (Ta2 * Tb2) / Bt));
  setD (VSRC_2, VSRC_2, nr_complex_t (0, -1 / Tb2 / Bt));
  setD (VSRC_2, VSRC_3, nr_complex_t (0, -1 / std::sqrt (Tb2) / Bt));
  setD (VSRC_3, VSRC_1, nr_complex_t (0, -1 / std::sqrt (Ta2) / Bt));
  setD (VSRC_3, VSRC_2, nr_complex_t (0, -1 / std::sqrt (Tb2) / Bt));
  setD (VSRC_3, VSRC_3, nr_complex_t (0, -1 / Bt));
}

void mstee::initTR (void) {
  initDC ();
}

// properties
PROP_REQ [] = {
  { "W1", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "W2", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "W3", PROP_REAL, { 2e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "Subst", PROP_STR, { PROP_NO_VAL, "Subst1" }, PROP_NO_RANGE },
  { "MSDispModel", PROP_STR, { PROP_NO_VAL, "Kirschning" },	PROP_RNG_DIS },
  { "MSModel", PROP_STR, { PROP_NO_VAL, "Hammerstad" }, PROP_RNG_MOD },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };
struct define_t mstee::cirdef =
  { "MTEE", 3, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
