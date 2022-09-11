/*
 * tline.cpp - ideal transmission line class implementation
 *
 * Copyright (C) 2004, 2006, 2008 Stefan Jahn <stefan@lkcc.org>
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
#include "tline.h"

using namespace qucs;

tline::tline () : circuit (2) {
  type = CIR_TLINE;
}

void tline::calcSP (nr_double_t frequency) {
  nr_double_t l = getPropertyDouble ("L");
  nr_double_t z = getPropertyDouble ("Z");
  nr_double_t a = getPropertyDouble ("Alpha");
  nr_double_t r = (z - z0) / (z + z0);
  nr_double_t b = 2 * pi * frequency / C0;
  a = std::log (a) / 2;
  nr_complex_t p = std::exp (-l * nr_complex_t (a, b));
  nr_complex_t s11 = r * (1.0 - p * p) / (1.0 - p * p * r * r);
  nr_complex_t s21 = p * (1.0 - r * r) / (1.0 - p * p * r * r);
  setS (NODE_1, NODE_1, s11); setS (NODE_2, NODE_2, s11);
  setS (NODE_1, NODE_2, s21); setS (NODE_2, NODE_1, s21);
}

void tline::calcNoiseSP (nr_double_t) {
  nr_double_t T = getPropertyDouble ("Temp");
  nr_double_t l = getPropertyDouble ("L");
  nr_double_t z = getPropertyDouble ("Z");
  nr_double_t a = getPropertyDouble ("Alpha");
  a = std::log (a) / 2;
  a = std::exp (a * l);
  nr_double_t r = (z - z0) / (z + z0);
  nr_double_t f = (a - 1) * (r * r - 1) / sqr (a - r * r) * celsius2kelvin (T) / T0;
  nr_double_t n11 = -f * (r * r + a);
  nr_double_t n21 = +f * 2 * r * std::sqrt (a);
  setN (NODE_1, NODE_1, n11); setN (NODE_2, NODE_2, n11);
  setN (NODE_1, NODE_2, n21); setN (NODE_2, NODE_1, n21);
}

void tline::calcNoiseAC (nr_double_t) {
  nr_double_t T = getPropertyDouble ("Temp");
  nr_double_t l = getPropertyDouble ("L");
  nr_double_t z = getPropertyDouble ("Z");
  nr_double_t a = getPropertyDouble ("Alpha");
  a = std::log (a) / 2;
  if (a * l != 0.0) {
    a = std::exp (a * l);
    nr_double_t f = 4.0 * celsius2kelvin (T) / T0 / z / (a - 1);
    nr_double_t n11 = +f * (a + 1);
    nr_double_t n21 = -f * 2 * std::sqrt (a);
    setN (NODE_1, NODE_1, n11); setN (NODE_2, NODE_2, n11);
    setN (NODE_1, NODE_2, n21); setN (NODE_2, NODE_1, n21);
  }
}

void tline::initDC (void) {
  nr_double_t z = getPropertyDouble ("Z");
  nr_double_t a = getPropertyDouble ("Alpha");
  nr_double_t l = getPropertyDouble ("L");
  a = std::log (a) / 2;
  if (a * l != 0.0) {
    setVoltageSources (0);
    allocMatrixMNA ();
    a = std::exp (a * l);
    nr_double_t f = 1 / z / (a - 1);
    nr_double_t y11 = +f * (a + 1);
    nr_double_t y21 = -f * 2 * std::sqrt (a);
    setY (NODE_1, NODE_1, y11); setY (NODE_2, NODE_2, y11);
    setY (NODE_1, NODE_2, y21); setY (NODE_2, NODE_1, y21);
  } else {
    setVoltageSources (1);
    allocMatrixMNA ();
    voltageSource (VSRC_1, NODE_1, NODE_2);
  }
}

void tline::initAC (void) {
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

void tline::calcAC (nr_double_t frequency) {
  nr_double_t l = getPropertyDouble ("L");
  nr_double_t z = getPropertyDouble ("Z");
  nr_double_t a = getPropertyDouble ("Alpha");
  nr_double_t b = 2 * pi * frequency / C0;
  a = std::log (a) / 2;
  if (l != 0.0) {
    nr_complex_t y11 = +1 / z / tanh (nr_complex_t (a, b) * l);
    nr_complex_t y21 = -1 / z / sinh (nr_complex_t (a, b) * l);
    setY (NODE_1, NODE_1, y11); setY (NODE_2, NODE_2, y11);
    setY (NODE_1, NODE_2, y21); setY (NODE_2, NODE_1, y21);
  }
}

void tline::initTR (void) {
  nr_double_t l = getPropertyDouble ("L");
  nr_double_t z = getPropertyDouble ("Z");
  deleteHistory ();
  if (l > 0.0) {
    setVoltageSources (2);
    allocMatrixMNA ();
    setHistory (true);
    initHistory (l / C0);
    setB (NODE_1, VSRC_1, +1); setB (NODE_2, VSRC_2, +1);
    setC (VSRC_1, NODE_1, +1); setC (VSRC_2, NODE_2, +1);
    setD (VSRC_1, VSRC_1, -z); setD (VSRC_2, VSRC_2, -z);
  } else {
    setVoltageSources (1);
    allocMatrixMNA ();
    voltageSource (VSRC_1, NODE_1, NODE_2);
  }
}

void tline::calcTR (nr_double_t t) {
  nr_double_t l = getPropertyDouble ("L");
  nr_double_t a = getPropertyDouble ("Alpha");
  nr_double_t z = getPropertyDouble ("Z");
  nr_double_t T = l / C0;
  a = std::log (a) / 2;
  if (T > 0.0) {
    T = t - T;
    a = std::exp (-a / 2 * l);
    setE (VSRC_1, a * (getV (NODE_2, T) + z * getJ (VSRC_2, T)));
    setE (VSRC_2, a * (getV (NODE_1, T) + z * getJ (VSRC_1, T)));
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
struct define_t tline::cirdef =
  { "TLIN", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
