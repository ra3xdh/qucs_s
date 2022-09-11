/*
 * mscorner.cpp - microstrip corner class implementation
 *
 * Copyright (C) 2004, 2006, 2008 Stefan Jahn <stefan@lkcc.org>
 * Copyright (C) 2004 Michael Margraf <Michael.Margraf@alumni.TU-Berlin.DE>
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
#include "mscorner.h"

using namespace qucs;

mscorner::mscorner () : circuit (2) {
  type = CIR_MSCORNER;
}

void mscorner::initCheck (void) {
  // get properties of substrate and corner
  nr_double_t W = getPropertyDouble ("W");
  substrate * subst = getSubstrate ();
  nr_double_t er = subst->getPropertyDouble ("er");
  h = subst->getPropertyDouble ("h");

  // local variables
  nr_double_t Wh = W/h;

  // check validity
  if (Wh < 0.2 || Wh > 6.0) {
    logprint (LOG_ERROR, "WARNING: Model for microstrip corner defined for "
	      "0.2 <= W/h <= 6.0 (W/h = %g)\n", Wh);
  }
  if (er < 2.36 || er > 10.4) {
    logprint (LOG_ERROR, "WARNING: Model for microstrip corner defined for "
	      "2.36 <= er <= 10.4 (er = %g)\n", er);
  }

  // capacitance in pF
  C = W * ((10.35 * er + 2.5) * Wh + (2.6 * er + 5.64));
  // inductivity in nH
  L = 220.0 * h * (1.0 - 1.35 * qucs::exp (-0.18 * qucs::pow (Wh, 1.39)));
}

void mscorner::initSP (void) {
  // allocate S-parameter matrix
  allocMatrixS ();
  initCheck ();
}

void mscorner::calcSP (nr_double_t frequency) {
  setMatrixS (ztos (calcMatrixZ (frequency)));
}

matrix mscorner::calcMatrixZ (nr_double_t frequency) {
  // check frequency validity
  if (frequency * h > 12e6) {
    logprint (LOG_ERROR, "WARNING: Model for microstrip corner defined for "
	      "freq*h <= 12MHz (is %g)\n", frequency * h);
  }

  // create Z-parameter matrix
  matrix z (2);
  nr_complex_t z21 = nr_complex_t (0.0, -0.5e12 / (pi * frequency * C));
  nr_complex_t z11 = nr_complex_t (0.0, 2e-9 * pi * frequency * L) + z21;
  z.set (0, 0, z11);
  z.set (0, 1, z21);
  z.set (1, 0, z21);
  z.set (1, 1, z11);
  return z;
}

void mscorner::initDC (void) {
  // a DC short (voltage source V = 0 volts)
  setVoltageSources (1);
  setInternalVoltageSource (1);
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
}

void mscorner::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
  initCheck ();
}

void mscorner::calcAC (nr_double_t frequency) {
  setMatrixY (ztoy (calcMatrixZ (frequency)));
}

// properties
PROP_REQ [] = {
  { "W", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "Subst", PROP_STR, { PROP_NO_VAL, "Subst1" }, PROP_NO_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  PROP_NO_PROP };
struct define_t mscorner::cirdef =
  { "MCORN", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
