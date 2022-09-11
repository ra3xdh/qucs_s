/*
 * msmbend.cpp - microstrip mitered bend class implementation
 *
 * Copyright (C) 2004, 2008 Stefan Jahn <stefan@lkcc.org>
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
#include "msmbend.h"

using namespace qucs;

msmbend::msmbend () : circuit (2) {
  type = CIR_MSMBEND;
}

void msmbend::calcSP (nr_double_t frequency) {
  setMatrixS (ztos (calcMatrixZ (frequency)));
}

matrix msmbend::calcMatrixZ (nr_double_t frequency) {

  /* how to get properties of this component, e.g. W */
  nr_double_t W = getPropertyDouble ("W");

  /* how to get properties of the substrate, e.g. Er, H */
  substrate * subst = getSubstrate ();
  nr_double_t er    = subst->getPropertyDouble ("er");
  nr_double_t h     = subst->getPropertyDouble ("h");

  /* local variables */
  nr_complex_t z11, z21;
  nr_double_t L, C, Wh = W / h;

  // check validity
  if ((Wh < 0.2) || (Wh > 6.0)) {
    logprint (LOG_ERROR, "WARNING: Model for microstrip mitered bend defined "
	      "for 0.2 <= W/h <= 6.0\n");
  }
  if ((er < 2.36) || (er > 10.4)) {
    logprint (LOG_ERROR, "WARNING: Model for microstrip mitered bend defined "
	      "for 2.36 <= er <= 10.4\n");
  }
  if (frequency * h > 12e6) {
    logprint (LOG_ERROR, "WARNING: Model for microstrip mitered bend defined "
	      "for freq*h <= 12MHz\n");
  }

  // capacitance in pF
  C = W * ((3.93 * er + 0.62) * Wh + (7.6 * er + 3.80));
  // inductance in nH
  L = 440.0 * h * (1.0 - 1.062 * qucs::exp (-0.177 * qucs::pow (Wh, 0.947)));

  // calculate Z-parameters
  z21 = nr_complex_t (0.0, -0.5e12 / (pi * frequency * C));
  z11 = nr_complex_t (0.0, 2e-9 * pi * frequency * L) + z21;
  matrix z (2);
  z.set (0, 0, z11);
  z.set (0, 1, z21);
  z.set (1, 0, z21);
  z.set (1, 1, z11);
  return z;
}

void msmbend::initDC (void) {
  // a DC short (voltage source V = 0 volts)
  setVoltageSources (1);
  setInternalVoltageSource (1);
  allocMatrixMNA ();
  clearY ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
}

void msmbend::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
}

void msmbend::calcAC (nr_double_t frequency) {
  setMatrixY (ztoy (calcMatrixZ (frequency)));
}

// properties
PROP_REQ [] = {
  { "W", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "Subst", PROP_STR, { PROP_NO_VAL, "Subst1" }, PROP_NO_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  PROP_NO_PROP };
struct define_t msmbend::cirdef =
  { "MMBEND", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
