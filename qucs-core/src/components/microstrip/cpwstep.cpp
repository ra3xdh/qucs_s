/*
 * cpwstep.cpp - coplanar waveguide step class implementation
 *
 * Copyright (C) 2005, 2008 Stefan Jahn <stefan@lkcc.org>
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

#include "component.h"
#include "substrate.h"
#include "cpwline.h"
#include "cpwstep.h"

using namespace qucs;

cpwstep::cpwstep () : circuit (2) {
  type = CIR_CPWSTEP;
}

// Returns the coplanar step capacitances per unit length.
void cpwstep::calcCends (nr_double_t frequency,
			 nr_double_t& C1, nr_double_t& C2) {

  // get properties of substrate and coplanar step
  nr_double_t W1 = getPropertyDouble ("W1");
  nr_double_t W2 = getPropertyDouble ("W2");
  nr_double_t s  = getPropertyDouble ("S");
  nr_double_t s1 = (s - W1) / 2;
  nr_double_t s2 = (s - W2) / 2;
  substrate * subst = getSubstrate ();
  nr_double_t er = subst->getPropertyDouble ("er");
  nr_double_t h  = subst->getPropertyDouble ("h");
  nr_double_t t  = subst->getPropertyDouble ("t");
  int backMetal  = !strcmp (getPropertyString ("Backside"), "Metal");

  nr_double_t ZlEff, ErEff, ZlEffFreq, ErEffFreq;
  cpwline::analyseQuasiStatic (W1, s1, h, t, er, backMetal, ZlEff, ErEff);
  cpwline::analyseDispersion  (W1, s1, h, er, ZlEff, ErEff, frequency,
			       ZlEffFreq, ErEffFreq);
  C1 = ErEffFreq / C0 / ZlEffFreq;
  cpwline::analyseQuasiStatic (W2, s2, h, t, er, backMetal, ZlEff, ErEff);
  cpwline::analyseDispersion  (W2, s2, h, er, ZlEff, ErEff, frequency,
			       ZlEffFreq, ErEffFreq);
  C2 = ErEffFreq / C0 / ZlEffFreq;
}

void cpwstep::initSP (void) {
  allocMatrixS ();
  checkProperties ();
}

void cpwstep::calcSP (nr_double_t frequency) {
  nr_complex_t z = 2.0 / calcY (frequency) / z0;
  nr_complex_t s11 = -1.0 / (z + 1.0);
  nr_complex_t s21 = +z / (z + 1.0);
  setS (NODE_1, NODE_1, s11);
  setS (NODE_2, NODE_2, s11);
  setS (NODE_1, NODE_2, s21);
  setS (NODE_2, NODE_1, s21);
}

void cpwstep::checkProperties (void) {
  nr_double_t W1 = getPropertyDouble ("W1");
  nr_double_t W2 = getPropertyDouble ("W2");
  nr_double_t s  = getPropertyDouble ("S");
  if (W1 == W2) {
    logprint (LOG_ERROR, "ERROR: Strip widths of step discontinuity do not "
	      "differ\n");
  }
  if (W1 >= s || W2 >= s) {
    logprint (LOG_ERROR, "ERROR: Strip widths of step discontinuity larger "
	      "than groundplane gap\n");
  }
  substrate * subst = getSubstrate ();
  nr_double_t er = subst->getPropertyDouble ("er");
  if (er < 2 || er > 14) {
    logprint (LOG_ERROR, "WARNING: Model for coplanar step valid for "
	      "2 < er < 14 (er = %g)\n", er);
  }
}

nr_complex_t cpwstep::calcY (nr_double_t frequency) {
  nr_double_t W1 = getPropertyDouble ("W1");
  nr_double_t W2 = getPropertyDouble ("W2");
  nr_double_t s  = getPropertyDouble ("S");
  nr_double_t s1 = (s - W1) / 2;
  nr_double_t s2 = (s - W2) / 2;
  nr_double_t a, c, c1, c2, x1, x2;
  nr_double_t o = 2 * pi * frequency;
  calcCends (frequency, c1, c2);
  x1 = c1 * s1;
  x2 = c2 * s2;
  a = s1 > s2 ? s2 / s1 : s1 / s2;
  c = one_over_pi * ((a * a + 1) / a * std::log ((1 + a) / (1 - a)) -
		2 * std::log (4 * a / (1 - a * a)));
  c = c * (x1 + x2) / 2;
  return nr_complex_t (0, c * o);
}

void cpwstep::initDC (void) {
  setVoltageSources (1);
  setInternalVoltageSource (true);
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
}

void cpwstep::initAC (void) {
  setVoltageSources (2);
  setInternalVoltageSource (true);
  allocMatrixMNA ();
  setB (NODE_1, VSRC_1, +1.0); setB (NODE_1, VSRC_2, +0.0);
  setB (NODE_2, VSRC_1, +0.0); setB (NODE_2, VSRC_2, +1.0);
  setC (VSRC_1, NODE_1, -1.0); setC (VSRC_1, NODE_2, +0.0);
  setC (VSRC_2, NODE_1, +0.0); setC (VSRC_2, NODE_2, -1.0);
  setE (VSRC_1, +0.0); setE (VSRC_2, +0.0);
  checkProperties ();
}

void cpwstep::calcAC (nr_double_t frequency) {
  nr_complex_t z = 1.0 / calcY (frequency);
  setD (VSRC_1, VSRC_1, z); setD (VSRC_2, VSRC_2, z);
  setD (VSRC_1, VSRC_2, z); setD (VSRC_2, VSRC_1, z);
}

// properties
PROP_REQ [] = {
  { "W1", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "W2", PROP_REAL, { 2e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "S", PROP_REAL, { 4e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "Subst", PROP_STR, { PROP_NO_VAL, "Subst1" }, PROP_NO_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Backside", PROP_STR, { PROP_NO_VAL, "Metal" },
    PROP_RNG_STR2 ("Metal", "Air") },
  PROP_NO_PROP };
struct define_t cpwstep::cirdef =
  { "CSTEP", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
