/*
 * msgap.cpp - microstrip gap class implementation
 *
 * Copyright (C) 2004, 2008, 2009 Stefan Jahn <stefan@lkcc.org>
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
#include "msopen.h"
#include "msgap.h"

using namespace qucs;

msgap::msgap () : circuit (2) {
  type = CIR_MSGAP;
}

void msgap::calcSP (nr_double_t frequency) {
  setMatrixS (ytos (calcMatrixY (frequency)));
}

matrix msgap::calcMatrixY (nr_double_t frequency) {

  /* how to get properties of this component, e.g. W */
  nr_double_t W1 = getPropertyDouble ("W1");
  nr_double_t W2 = getPropertyDouble ("W2");
  nr_double_t s  = getPropertyDouble ("S");
  const char * SModel  = getPropertyString ("MSModel");
  const char * DModel  = getPropertyString ("MSDispModel");

  /* how to get properties of the substrate, e.g. Er, H */
  substrate * subst = getSubstrate ();
  nr_double_t er    = subst->getPropertyDouble ("er");
  nr_double_t h     = subst->getPropertyDouble ("h");
  nr_double_t t     = subst->getPropertyDouble ("t");

  nr_double_t Q1, Q2, Q3, Q4, Q5;
  bool flip = false;
  if (W2 < W1) {  // equations are valid for 1 <= W2/W1 <= 3
    Q1 = W1;
    W1 = W2;
    W2 = Q1;
    flip = true;
  }

  // calculate parallel open end capacitances
  nr_double_t C1 = msopen::calcCend (frequency, W1, h, t, er,
				     SModel, DModel, "Kirschning");
  nr_double_t C2 = msopen::calcCend (frequency, W2, h, t, er,
				     SModel, DModel, "Kirschning");

  W2 /= W1;
  W1 /= h;
  s  /= h;

  // local variables
  Q5 = 1.23 / (1.0 + 0.12 * qucs::pow (W2 - 1.0, 0.9));
  Q1 = 0.04598 * (0.03 + qucs::pow (W1, Q5)) * (0.272 + 0.07 * er);
  Q2 = 0.107 * (W1 + 9.0) * qucs::pow (s, 3.23) +
    2.09 * qucs::pow (s, 1.05) * (1.5 + 0.3 * W1) / (1.0 + 0.6 * W1);
  Q3 = qucs::exp (-0.5978 * qucs::pow (W2, +1.35)) - 0.55;
  Q4 = qucs::exp (-0.5978 * qucs::pow (W2, -1.35)) - 0.55;

  nr_double_t Cs = 5e-10 * h * qucs::exp (-1.86 * s) * Q1 *
    (1.0 + 4.19 * (1.0 - qucs::exp (-0.785 * qucs::sqrt (1.0 / W1) * W2)));
  C1 *= (Q2 + Q3) / (Q2 + 1.0);
  C2 *= (Q2 + Q4) / (Q2 + 1.0);

  if (flip) { // if necessary flip ports back
    Q1 = C1;
    C1 = C2;
    C2 = Q1;
  }

  // build Y-parameter matrix
  nr_complex_t y21 = nr_complex_t (0.0, -2.0 * pi * frequency * Cs);
  nr_complex_t y11 = nr_complex_t (0.0,  2.0 * pi * frequency * (C1 + Cs));
  nr_complex_t y22 = nr_complex_t (0.0,  2.0 * pi * frequency * (C2 + Cs));
  matrix y (2);
  y.set (0, 0, y11);
  y.set (0, 1, y21);
  y.set (1, 0, y21);
  y.set (1, 1, y22);
  return y;
}

void msgap::initDC (void) {
  allocMatrixMNA ();
  clearY ();
}

void msgap::calcAC (nr_double_t frequency) {
  setMatrixY (calcMatrixY (frequency));
}

// properties
PROP_REQ [] = {
  { "W1", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "W2", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "S" , PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "Subst", PROP_STR, { PROP_NO_VAL, "Subst1" }, PROP_NO_RANGE },
  { "MSDispModel", PROP_STR, { PROP_NO_VAL, "Kirschning" }, PROP_RNG_DIS },
  { "MSModel", PROP_STR, { PROP_NO_VAL, "Hammerstad" }, PROP_RNG_MOD },
  PROP_NO_PROP };
PROP_OPT [] = {
  PROP_NO_PROP };
struct define_t msgap::cirdef =
  { "MGAP", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
