/*
 * phaseshifter.cpp - phase shifter class implementation
 *
 * Copyright (C) 2004, 2008 Stefan Jahn <stefan@lkcc.org>
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
#include "phaseshifter.h"

using namespace qucs;

phaseshifter::phaseshifter () : circuit (2) {
  type = CIR_PHASESHIFTER;
}

void phaseshifter::initSP (void) {
  nr_double_t p = deg2rad (getPropertyDouble ("phi"));
  nr_double_t z = getPropertyDouble ("Zref");
  nr_double_t r = (z0 - z) / (z0 + z);
  nr_complex_t d = 1.0 - qucs::polar (r * r, 2 * p);
  nr_complex_t s11 = r * (qucs::polar (1.0, 2 * p) - 1.0) / d;
  nr_complex_t s21 = (1.0 - r * r) * qucs::polar (1.0, p) / d;
  allocMatrixS ();
  setS (NODE_1, NODE_1, s11);
  setS (NODE_2, NODE_2, s11);
  setS (NODE_1, NODE_2, s21);
  setS (NODE_2, NODE_1, s21);
}

void phaseshifter::initDC (void) {
  setVoltageSources (1);
  allocMatrixMNA ();
  clearY ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
}

void phaseshifter::initAC (void) {
  nr_double_t p = deg2rad (getPropertyDouble ("phi"));

  if (p == 0.0) { // no phase shift, thus a short
    initDC ();
  }
  else { // compute Y-parameters directly
    setVoltageSources (0);
    allocMatrixMNA ();
    nr_double_t z = getPropertyDouble ("Zref");
    nr_double_t y11 =  1 / z / std::tan (p);
    nr_double_t y21 = -1 / z / std::sin (p);
    setY (NODE_1, NODE_1, nr_complex_t (0, y11)); setY (NODE_2, NODE_2, nr_complex_t (0, y11));
    setY (NODE_1, NODE_2, nr_complex_t (0, y21)); setY (NODE_2, NODE_1, nr_complex_t (0, y21));
  }
}

// properties
PROP_REQ [] = {
  { "phi", PROP_REAL, { 1e-90, PROP_NO_STR }, PROP_NO_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Zref", PROP_REAL, { 50, PROP_NO_STR }, PROP_POS_RANGE },
  PROP_NO_PROP };
struct define_t phaseshifter::cirdef =
  { "PShift", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
