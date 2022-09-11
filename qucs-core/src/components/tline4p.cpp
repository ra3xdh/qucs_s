/*
 * tline4p.cpp - ideal 4-terminal transmission line class implementation
 *
 * Copyright (C) 2007, 2008 Stefan Jahn <stefan@lkcc.org>
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
#include "tline4p.h"

using namespace qucs;

tline4p::tline4p () : circuit (4) {
  type = CIR_TLINE4P;
}

void tline4p::calcSP (nr_double_t frequency) {
  nr_double_t l = getPropertyDouble ("L");
  nr_double_t z = getPropertyDouble ("Z");
  nr_double_t a = getPropertyDouble ("Alpha");
  nr_double_t b = 2 * pi * frequency / C0;
  a = std::log (a) / 2;

  nr_complex_t g = nr_complex_t (a, b);
  nr_double_t p = 2 * z0 + z;
  nr_double_t n = 2 * z0 - z;
  nr_complex_t e = std::exp (2.0 * g * l);
  nr_complex_t d = p * p * e - n * n;

  nr_complex_t s11 = z * (p * e + n) / d;
  nr_complex_t s14 = 1.0 - s11;
  nr_complex_t s12 = 4.0 * z * z0 * std::exp (g * l) / d;

  setS (NODE_1, NODE_1, +s11); setS (NODE_2, NODE_2, +s11);
  setS (NODE_3, NODE_3, +s11); setS (NODE_4, NODE_4, +s11);
  setS (NODE_1, NODE_4, +s14); setS (NODE_4, NODE_1, +s14);
  setS (NODE_2, NODE_3, +s14); setS (NODE_3, NODE_2, +s14);
  setS (NODE_1, NODE_2, +s12); setS (NODE_2, NODE_1, +s12);
  setS (NODE_3, NODE_4, +s12); setS (NODE_4, NODE_3, +s12);
  setS (NODE_1, NODE_3, -s12); setS (NODE_3, NODE_1, -s12);
  setS (NODE_2, NODE_4, -s12); setS (NODE_4, NODE_2, -s12);
}

void tline4p::calcNoiseSP (nr_double_t) {
  nr_double_t l = getPropertyDouble ("L");
  if (l < 0) return;
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  matrix s = getMatrixS ();
  matrix e = eye (getSize ());
  setMatrixN (celsius2kelvin (T) / T0 * (e - s * transpose (conj (s))));
}

void tline4p::calcNoiseAC (nr_double_t) {
  nr_double_t l = getPropertyDouble ("L");
  if (l < 0) return;
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  setMatrixN (4 * celsius2kelvin (T) / T0 * real (getMatrixY ()));
}

void tline4p::initDC (void) {
  setVoltageSources (2);
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
  voltageSource (VSRC_2, NODE_3, NODE_4);
}

void tline4p::initAC (void) {
  nr_double_t l = getPropertyDouble ("L");
  if (l != 0.0) {
    setVoltageSources (0);
    allocMatrixMNA ();
  } else {
    setVoltageSources (2);
    allocMatrixMNA ();
    voltageSource (VSRC_1, NODE_1, NODE_2);
    voltageSource (VSRC_2, NODE_3, NODE_4);
  }
}

void tline4p::calcAC (nr_double_t frequency) {
  nr_double_t l = getPropertyDouble ("L");
  nr_double_t z = getPropertyDouble ("Z");
  nr_double_t a = getPropertyDouble ("Alpha");
  nr_double_t b = 2 * pi * frequency / C0;
  a = std::log (a) / 2;
  if (l != 0.0) {
    nr_complex_t g = nr_complex_t (a, b);
    nr_complex_t y11 = coth (g * l) / z;
    nr_complex_t y21 = -cosech (g * l) / z;
    setY (NODE_1, NODE_1, +y11); setY (NODE_2, NODE_2, +y11);
    setY (NODE_3, NODE_3, +y11); setY (NODE_4, NODE_4, +y11);
    setY (NODE_1, NODE_4, -y11); setY (NODE_4, NODE_1, -y11);
    setY (NODE_2, NODE_3, -y11); setY (NODE_3, NODE_2, -y11);
    setY (NODE_1, NODE_2, +y21); setY (NODE_2, NODE_1, +y21);
    setY (NODE_3, NODE_4, +y21); setY (NODE_4, NODE_3, +y21);
    setY (NODE_1, NODE_3, -y21); setY (NODE_3, NODE_1, -y21);
    setY (NODE_2, NODE_4, -y21); setY (NODE_4, NODE_2, -y21);
  }
}

void tline4p::initTR (void) {
  nr_double_t l = getPropertyDouble ("L");
  nr_double_t z = getPropertyDouble ("Z");
  deleteHistory ();
  if (l > 0.0) {
    setVoltageSources (2);
    allocMatrixMNA ();
    setHistory (true);
    initHistory (l / C0);
    setB (NODE_1, VSRC_1, +1); setB (NODE_2, VSRC_2, +1);
    setB (NODE_4, VSRC_1, -1); setB (NODE_3, VSRC_2, -1);
    setC (VSRC_1, NODE_1, +1); setC (VSRC_2, NODE_2, +1);
    setC (VSRC_1, NODE_4, -1); setC (VSRC_2, NODE_3, -1);
    setD (VSRC_1, VSRC_1, -z); setD (VSRC_2, VSRC_2, -z);
  } else {
    setVoltageSources (2);
    allocMatrixMNA ();
    voltageSource (VSRC_1, NODE_1, NODE_2);
    voltageSource (VSRC_2, NODE_3, NODE_4);
  }
}

void tline4p::calcTR (nr_double_t t) {
  nr_double_t l = getPropertyDouble ("L");
  nr_double_t a = getPropertyDouble ("Alpha");
  nr_double_t z = getPropertyDouble ("Z");
  nr_double_t T = l / C0;
  a = std::log (a) / 2;
  if (T > 0.0) {
    T = t - T;
    a = std::exp (-a / 2 * l);
    setE (VSRC_1, a * (getV (NODE_2, T) - getV (NODE_3, T) +
		       z * getJ (VSRC_2, T)));
    setE (VSRC_2, a * (getV (NODE_1, T) - getV (NODE_4, T) +
		       z * getJ (VSRC_1, T)));
  }
}

// properties
PROP_REQ [] = {
  { "Z", PROP_REAL, { 50, PROP_NO_STR }, PROP_POS_RANGE },
  { "L", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_NO_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Alpha", PROP_REAL, { 1, PROP_NO_STR }, PROP_POS_RANGEX },
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };
struct define_t tline4p::cirdef =
  { "TLIN4P", 4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
