/*
 * coaxline.cpp - coaxial cable class implementation
 *
 * Copyright (C) 2006, 2008, 2009, 2011 Stefan Jahn <stefan@lkcc.org>
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
#include "coaxline.h"

using namespace qucs;

coaxline::coaxline () : circuit (2) {
  alpha = beta = zl = fc = 0;
  type = CIR_COAXLINE;
}

void coaxline::calcPropagation (nr_double_t frequency) {
  nr_double_t er   = getPropertyDouble ("er");
  nr_double_t mur  = getPropertyDouble ("mur");
  nr_double_t rho  = getPropertyDouble ("rho");
  nr_double_t tand = getPropertyDouble ("tand");
  nr_double_t d    = getPropertyDouble ("d");
  nr_double_t D    = getPropertyDouble ("D");
  nr_double_t ad, ac, rs;

  // check cutoff frequency
  if (frequency > fc) {
    logprint (LOG_ERROR, "WARNING: Operating frequency (%g) beyond "
	      "cutoff frequency (%g).\n", frequency, fc);
  }

  // calculate losses
  ad = pi / C0 * frequency * std::sqrt (er) * tand;
  rs = std::sqrt (pi * frequency * mur * MU0 * rho);
  ac = std::sqrt (er) * (1 / d + 1 / D) / std::log (D / d) * rs / Z0;

  // calculate propagation constants and reference impedance
  alpha = ac + ad;
  beta  = std::sqrt (er * mur) * 2 * pi * frequency / C0;
  zl = Z0 / 2 / pi / std::sqrt (er) * std::log (D / d);
}

void coaxline::calcNoiseSP (nr_double_t) {
  nr_double_t l = getPropertyDouble ("L");
  if (l < 0) return;
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  matrix s = getMatrixS ();
  matrix e = eye (getSize ());
  setMatrixN (celsius2kelvin (T) / T0 * (e - s * transpose (conj (s))));
}

void coaxline::initCheck (void) {
  nr_double_t d   = getPropertyDouble ("d");
  nr_double_t D   = getPropertyDouble ("D");
  nr_double_t er  = getPropertyDouble ("er");
  nr_double_t mur = getPropertyDouble ("mur");

  // check validity
  if (d >= D) {
    logprint (LOG_ERROR,
	      "ERROR: Inner diameter larger than outer diameter.\n");
  }
  nr_double_t f1, f2, cl;
  cl = C0 / std::sqrt (mur * er);
  f1 = cl / (pi_over_2 * (D + d)); // TE_11
  f2 = cl / (1 * (D - d));      // TM_N1
  fc = std::min (f1, f2);
}

void coaxline::saveCharacteristics (nr_double_t) {
  setCharacteristic ("Zl", zl);
}

void coaxline::initSP (void) {
  // allocate S-parameter matrix
  allocMatrixS ();
  initCheck ();
}

void coaxline::calcSP (nr_double_t frequency) {
  nr_double_t l = getPropertyDouble ("L");

  // calculate propagation constants
  calcPropagation (frequency);

  // calculate S-parameters
  nr_double_t z = zl / z0;
  nr_double_t y = 1 / z;
  nr_complex_t g = nr_complex_t (alpha, beta);
  nr_complex_t n = 2.0 * cosh (g * l) + (z + y) * sinh (g * l);
  nr_complex_t s11 = (z - y) * sinh (g * l) / n;
  nr_complex_t s21 = 2.0 / n;
  setS (NODE_1, NODE_1, s11); setS (NODE_2, NODE_2, s11);
  setS (NODE_1, NODE_2, s21); setS (NODE_2, NODE_1, s21);
}

void coaxline::initDC (void) {
  nr_double_t l   = getPropertyDouble ("L");
  nr_double_t d   = getPropertyDouble ("d");
  nr_double_t rho = getPropertyDouble ("rho");

  if (d != 0.0 && rho != 0.0 && l != 0.0) {
    // a tiny resistance
    nr_double_t g = pi * sqr (d / 2) / rho / l;
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

void coaxline::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
  initCheck ();
}

void coaxline::calcAC (nr_double_t frequency) {
  nr_double_t l = getPropertyDouble ("L");

  // calculate propagation constants
  calcPropagation (frequency);

  // calculate Y-parameters
  nr_complex_t g = nr_complex_t (alpha, beta);
  nr_complex_t y11 = coth (g * l) / zl;
  nr_complex_t y21 = -cosech (g * l) / zl;
  setY (NODE_1, NODE_1, y11); setY (NODE_2, NODE_2, y11);
  setY (NODE_1, NODE_2, y21); setY (NODE_2, NODE_1, y21);
}

void coaxline::calcNoiseAC (nr_double_t) {
  nr_double_t l = getPropertyDouble ("L");
  if (l < 0) return;
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  setMatrixN (4 * celsius2kelvin (T) / T0 * real (getMatrixY ()));
}

// properties
PROP_REQ [] = {
  { "D", PROP_REAL, { 2.95e-3, PROP_NO_STR }, PROP_POS_RANGEX },
  { "d", PROP_REAL, { 0.9e-3, PROP_NO_STR }, PROP_POS_RANGEX },
  { "L", PROP_REAL, { 1500e-3, PROP_NO_STR }, PROP_NO_RANGE },
  { "er", PROP_REAL, { 2.29, PROP_NO_STR }, PROP_RNGII (1, 100) },
  { "mur", PROP_REAL, { 1, PROP_NO_STR }, PROP_RNGII (1, 100) },
  { "tand", PROP_REAL, { 4e-4, PROP_NO_STR }, PROP_POS_RANGE },
  { "rho", PROP_REAL, { 0.022e-6, PROP_NO_STR }, PROP_POS_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };
struct define_t coaxline::cirdef =
  { "COAX", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
