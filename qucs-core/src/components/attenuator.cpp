/*
 * attenuator.cpp - attenuator class implementation
 *
 * Copyright (C) 2003, 2004, 2005, 2008 Stefan Jahn <stefan@lkcc.org>
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
#include "attenuator.h"

using namespace qucs;

attenuator::attenuator () : circuit (2) {
  type = CIR_ATTENUATOR;
}

void attenuator::initSP (void) {
  allocMatrixS ();
  nr_double_t a = getPropertyDouble ("L");
  nr_double_t z = getPropertyDouble ("Zref");
  nr_double_t r = (z - z0) / (z + z0);
  nr_double_t s11 = r * (a - 1) / (a - r * r);
  nr_double_t s21 = std::sqrt (a) * (1 - r * r) / (a - r * r);
  setS (NODE_1, NODE_1, s11);
  setS (NODE_2, NODE_2, s11);
  setS (NODE_1, NODE_2, s21);
  setS (NODE_2, NODE_1, s21);
}

void attenuator::calcNoiseSP (nr_double_t) {
  nr_double_t T = getPropertyDouble ("Temp");
  nr_double_t l = getPropertyDouble ("L");
  nr_double_t z = getPropertyDouble ("Zref");
  nr_double_t r = (z - z0) / (z + z0);
  nr_double_t f = (l - 1) * (r * r - 1) / sqr (l - r * r) * celsius2kelvin (T) / T0;
  setN (NODE_1, NODE_1, -f * (r * r + l));
  setN (NODE_2, NODE_2, -f * (r * r + l));
  setN (NODE_1, NODE_2, +f * 2 * r * std::sqrt (l));
  setN (NODE_2, NODE_1, +f * 2 * r * std::sqrt (l));
}

void attenuator::calcNoiseAC (nr_double_t) {
  nr_double_t T = getPropertyDouble ("Temp");
  nr_double_t l = getPropertyDouble ("L");
  nr_double_t z = getPropertyDouble ("Zref");
  nr_double_t f = 4.0 * celsius2kelvin (T) / T0 / z / (l - 1);
  setN (NODE_1, NODE_1, +f * (l + 1));
  setN (NODE_2, NODE_2, +f * (l + 1));
  setN (NODE_1, NODE_2, -f * 2 * std::sqrt (l));
  setN (NODE_2, NODE_1, -f * 2 * std::sqrt (l));
}

void attenuator::initDC (void) {
  nr_double_t a = getPropertyDouble ("L");
  if (a == 1.0) { // no attenuation
    setVoltageSources (1);
    allocMatrixMNA ();
    voltageSource (VSRC_1, NODE_1, NODE_2);
  }
#if AUGMENTED
  else { // compute Z-parameters
    setVoltageSources (2);
    allocMatrixMNA ();
    nr_double_t zref = getPropertyDouble ("Zref");
    nr_double_t z11 = zref * (a + 1) / (a - 1);
    nr_double_t z21 = zref * (std::sqrt (a) * 2) / (a - 1);
    setB (NODE_1, VSRC_1, +1.0); setB (NODE_1, VSRC_2, +0.0);
    setB (NODE_2, VSRC_1, +0.0); setB (NODE_2, VSRC_2, +1.0);
    setC (VSRC_1, NODE_1, -1.0); setC (VSRC_1, NODE_2, +0.0);
    setC (VSRC_2, NODE_1, +0.0); setC (VSRC_2, NODE_2, -1.0);
    setD (VSRC_1, VSRC_1, +z11); setD (VSRC_2, VSRC_2, +z11);
    setD (VSRC_1, VSRC_2, +z21); setD (VSRC_2, VSRC_1, +z21);
    setE (VSRC_1, +0.0); setE (VSRC_2, +0.0);
  }
  clearY ();
#else
  else { // compute Y-parameters
    setVoltageSources (0);
    allocMatrixMNA ();
    nr_double_t z = getPropertyDouble ("Zref");
    nr_double_t f = 1 / z / (a - 1);
    setY (NODE_1, NODE_1, f * (a + 1));
    setY (NODE_2, NODE_2, f * (a + 1));
    setY (NODE_1, NODE_2, -f * 2 * std::sqrt (a));
    setY (NODE_2, NODE_1, -f * 2 * std::sqrt (a));
  }
#endif
}

void attenuator::initAC (void) {
  nr_double_t a = getPropertyDouble ("L");

  if (a == 1.0) { // no attenuation
    setVoltageSources (1);
    allocMatrixMNA ();
    clearY ();
    voltageSource (VSRC_1, NODE_1, NODE_2);
  }
#if AUGMENTED
  else { // compute Z-parameters
    setVoltageSources (0);
    allocMatrixMNA ();
    nr_double_t zref = getPropertyDouble ("Zref");
    nr_double_t z11 = zref * (a + 1) / (a - 1);
    nr_double_t z21 = zref * (std::sqrt (a) * 2) / (a - 1);

    // build Z-parameter matrix and convert it to Y-parameters
    matrix z (2);
    z.set (NODE_1, NODE_1, z11); z.set (NODE_2, NODE_2, z11);
    z.set (NODE_1, NODE_2, z21); z.set (NODE_2, NODE_1, z21);
    setMatrixY (ztoy (z));
  }
#else
  else { // compute Y-parameters
    setVoltageSources (0);
    allocMatrixMNA ();
    nr_double_t z = getPropertyDouble ("Zref");
    nr_double_t f = 1 / z / (a - 1);
    setY (NODE_1, NODE_1, f * (a + 1));
    setY (NODE_2, NODE_2, f * (a + 1));
    setY (NODE_1, NODE_2, -f * 2 * std::sqrt (a));
    setY (NODE_2, NODE_1, -f * 2 * std::sqrt (a));
  }
#endif
}

void attenuator::initTR (void) {
  initDC ();
}

// properties
PROP_REQ [] = {
  { "L", PROP_REAL, { 10, PROP_NO_STR }, PROP_MIN_VAL (1) },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  { "Zref", PROP_REAL, { 50, PROP_NO_STR }, PROP_POS_RANGE },
  PROP_NO_PROP };
struct define_t attenuator::cirdef =
  { "Attenuator",
    2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
