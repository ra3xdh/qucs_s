/*
 * rlcg.cpp - lossy RLCG transmission line class implementation
 *
 * Copyright (C) 2009 Stefan Jahn <stefan@lkcc.org>
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
#include "rlcg.h"

using namespace qucs;

rlcg::rlcg () : circuit (2) {
  type = CIR_RLCG;
}

// Calculates propagation constant and characteristic complex impedance.
void rlcg::calcPropagation (nr_double_t frequency) {
  nr_double_t R = getPropertyDouble ("R");
  nr_double_t L = getPropertyDouble ("L");
  nr_double_t C = getPropertyDouble ("C");
  nr_double_t G = getPropertyDouble ("G");
  nr_complex_t Z = nr_complex_t (R, 2 * pi * frequency * L);
  nr_complex_t Y = nr_complex_t (G, 2 * pi * frequency * C);
  g = std::sqrt (Z * Y);
  z = std::sqrt (Z / Y);
}

void rlcg::calcSP (nr_double_t frequency) {
  nr_double_t l = getPropertyDouble ("Length");
  calcPropagation (frequency);
  nr_complex_t r = (z - z0) / (z + z0);
  nr_complex_t p = std::exp (-l * g);
  nr_complex_t s11 = r * (1.0 - p * p) / (1.0 - p * p * r * r);
  nr_complex_t s21 = p * (1.0 - r * r) / (1.0 - p * p * r * r);
  setS (NODE_1, NODE_1, s11); setS (NODE_2, NODE_2, s11);
  setS (NODE_1, NODE_2, s21); setS (NODE_2, NODE_1, s21);
}

void rlcg::saveCharacteristics (nr_double_t) {
  setCharacteristic ("Zl", real (z));
}

void rlcg::calcNoiseSP (nr_double_t) {
  nr_double_t l = getPropertyDouble ("Length");
  if (l == 0.0) return;
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  matrix s = getMatrixS ();
  matrix e = eye (getSize ());
  setMatrixN (celsius2kelvin (T) / T0 * (e - s * transpose (conj (s))));
}

void rlcg::calcNoiseAC (nr_double_t) {
  nr_double_t l = getPropertyDouble ("Length");
  if (l == 0.0) return;
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  setMatrixN (4 * celsius2kelvin (T) / T0 * real (getMatrixY ()));
}

void rlcg::initDC (void) {
  nr_double_t R = getPropertyDouble ("R");
  nr_double_t l = getPropertyDouble ("Length");
  if (R != 0.0 && l != 0.0) {
    // a tiny resistance
    nr_double_t g = 1.0 / R / l;
    setVoltageSources (0);
    allocMatrixMNA ();
    setY (NODE_1, NODE_1, +g); setY (NODE_2, NODE_2, +g);
    setY (NODE_1, NODE_2, -g); setY (NODE_2, NODE_1, -g);
  }
  else {
    // a DC short
    setVoltageSources (1);
    setInternalVoltageSource (1);
    allocMatrixMNA ();
    voltageSource (VSRC_1, NODE_1, NODE_2);
  }
}

void rlcg::initAC (void) {
  nr_double_t l = getPropertyDouble ("L");
  if (l != 0.0) {
    setVoltageSources (0);
    allocMatrixMNA ();
  } else {
    setVoltageSources (1);
    allocMatrixMNA ();
    voltageSource (VSRC_1, NODE_1, NODE_2);
  }
}

void rlcg::calcAC (nr_double_t frequency) {
  nr_double_t l = getPropertyDouble ("Length");
  if (l != 0.0) {
    calcPropagation (frequency);
    nr_complex_t y11 = +1.0 / z / tanh (g * l);
    nr_complex_t y21 = -1.0 / z / sinh (g * l);
    setY (NODE_1, NODE_1, y11); setY (NODE_2, NODE_2, y11);
    setY (NODE_1, NODE_2, y21); setY (NODE_2, NODE_1, y21);
  }
}

void rlcg::initTR (void) {
  initDC ();
}

// properties
PROP_REQ [] = {
  { "R", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_POS_RANGE },
  { "L", PROP_REAL, { 0.6e-6, PROP_NO_STR }, PROP_POS_RANGEX },
  { "C", PROP_REAL, { 240e-12, PROP_NO_STR }, PROP_POS_RANGEX },
  { "G", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_POS_RANGE },
  { "Length", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_NO_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };
struct define_t rlcg::cirdef =
  { "RLCG", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
