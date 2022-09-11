/*
 * twistedpair.cpp - twisted pair line class implementation
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
#include "twistedpair.h"

using namespace qucs;

twistedpair::twistedpair () : circuit (4) {
  type = CIR_TWISTEDPAIR;
}

void twistedpair::initSP (void) {
  allocMatrixS ();
  calcLength ();
}

void twistedpair::calcSP (nr_double_t frequency) {
  calcPropagation (frequency);

  nr_complex_t g = nr_complex_t (alpha, beta);
  nr_double_t p = 2 * z0 + zl;
  nr_double_t n = 2 * z0 - zl;
  nr_complex_t e = qucs::exp (2.0 * g * len);
  nr_complex_t d = p * p * e - n * n;

  nr_complex_t s11 = zl * (p * e + n) / d;
  nr_complex_t s14 = 1.0 - s11;
  nr_complex_t s12 = 4.0 * zl * z0 * qucs::exp (g * len) / d;

  setS (NODE_1, NODE_1, +s11); setS (NODE_2, NODE_2, +s11);
  setS (NODE_3, NODE_3, +s11); setS (NODE_4, NODE_4, +s11);
  setS (NODE_1, NODE_4, +s14); setS (NODE_4, NODE_1, +s14);
  setS (NODE_2, NODE_3, +s14); setS (NODE_3, NODE_2, +s14);
  setS (NODE_1, NODE_2, +s12); setS (NODE_2, NODE_1, +s12);
  setS (NODE_3, NODE_4, +s12); setS (NODE_4, NODE_3, +s12);
  setS (NODE_1, NODE_3, -s12); setS (NODE_3, NODE_1, -s12);
  setS (NODE_2, NODE_4, -s12); setS (NODE_4, NODE_2, -s12);
}

void twistedpair::calcNoiseSP (nr_double_t) {
  if (len < 0) return;
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  matrix s = getMatrixS ();
  matrix e = eye (getSize ());
  setMatrixN (celsius2kelvin (T) / T0 * (e - s * transpose (conj (s))));
}

void twistedpair::initDC (void) {
  nr_double_t d   = getPropertyDouble ("d");
  nr_double_t rho = getPropertyDouble ("rho");
  calcLength ();

  if (d != 0.0 && rho != 0.0 && len != 0.0) {
    // tiny resistances
    nr_double_t g1 = pi * sqr (d / 2) / rho / len;
    nr_double_t g2 = g1;
    setVoltageSources (0);
    allocMatrixMNA ();
    setY (NODE_1, NODE_1, +g1); setY (NODE_2, NODE_2, +g1);
    setY (NODE_1, NODE_2, -g1); setY (NODE_2, NODE_1, -g1);
    setY (NODE_3, NODE_3, +g2); setY (NODE_4, NODE_4, +g2);
    setY (NODE_3, NODE_4, -g2); setY (NODE_4, NODE_3, -g2);
  }
  else {
    // DC shorts
    setVoltageSources (2);
    allocMatrixMNA ();
    voltageSource (VSRC_1, NODE_1, NODE_2);
    voltageSource (VSRC_2, NODE_3, NODE_4);
  }
}

void twistedpair::initAC (void) {
  calcLength ();
  if (len != 0.0) {
    setVoltageSources (0);
    allocMatrixMNA ();
  } else {
    setVoltageSources (2);
    allocMatrixMNA ();
    voltageSource (VSRC_1, NODE_1, NODE_2);
    voltageSource (VSRC_2, NODE_3, NODE_4);
  }
}

nr_double_t twistedpair::calcLoss (nr_double_t frequency) {
  nr_double_t d    = getPropertyDouble ("d");
  nr_double_t rho  = getPropertyDouble ("rho");
  nr_double_t mur  = getPropertyDouble ("mur");
  nr_double_t tand = getPropertyDouble ("tand");

  nr_double_t delta, rout, rin, ad, ac, l0;
  // calculate conductor losses
  rout = d / 2;
  if (frequency > 0.0) {
    delta = qucs::sqrt (rho / (pi * frequency * MU0 * mur));
    rin = rout - delta;
    if (rin < 0.0) rin = 0.0;
  }
  else rin = 0.0;
  ac = (rho * one_over_pi) / (rout * rout - rin * rin) / zl;

  // calculate dielectric losses
  l0 = C0 / frequency;
  ad = pi * tand * qucs::sqrt (ereff) / l0;

  alpha = ac + ad;
  return alpha;
}

nr_double_t twistedpair::calcLength (void) {
  nr_double_t l  = getPropertyDouble ("L");
  nr_double_t T  = getPropertyDouble ("T");
  nr_double_t D  = getPropertyDouble ("D");
  len = l * T * pi * D * qucs::sqrt (1 + 1 / sqr (T * pi * D));
  return len;
}

void twistedpair::calcPropagation (nr_double_t frequency) {
  nr_double_t d  = getPropertyDouble ("d");
  nr_double_t D  = getPropertyDouble ("D");
  nr_double_t er = getPropertyDouble ("er");
  nr_double_t T  = getPropertyDouble ("T");

  nr_double_t q, p;
  p = qucs::atan (T * pi * D);
  q = 0.25 + 0.001 * p * p;  // soft PTFE
  q = 0.25 + 0.0004 * p * p; // usual
  ereff = 1.0 + q * (er - 1.0);
  zl = Z0 / pi / qucs::sqrt (ereff) * qucs::acosh (D / d);
  beta = 2 * pi * frequency / C0 * qucs::sqrt (ereff);
  angle = rad2deg (p);
  alpha = calcLoss (frequency);
}

void twistedpair::saveCharacteristics (nr_double_t) {
  setCharacteristic ("Zl", zl);
  setCharacteristic ("Er", ereff);
  setCharacteristic ("Length", len);
  setCharacteristic ("Angle", angle);
}

void twistedpair::calcAC (nr_double_t frequency) {
  if (len != 0.0) {
    calcPropagation (frequency);
    nr_complex_t g = nr_complex_t (alpha, beta);
    nr_complex_t y11 = coth (g * len) / zl;
    nr_complex_t y21 = -cosech (g * len) / zl;
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

void twistedpair::calcNoiseAC (nr_double_t) {
  if (len < 0) return;
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  setMatrixN (4 * celsius2kelvin (T) / T0 * real (getMatrixY ()));
}

void twistedpair::initTR (void) {
  initDC ();
}

// properties
PROP_REQ [] = {
  { "d", PROP_REAL, { 0.5e-3, PROP_NO_STR }, PROP_POS_RANGEX },
  { "D", PROP_REAL, { 0.8e-3, PROP_NO_STR }, PROP_POS_RANGEX },
  { "L", PROP_REAL, { 1500e-3, PROP_NO_STR }, PROP_NO_RANGE },
  { "T", PROP_REAL, { 100, PROP_NO_STR }, PROP_POS_RANGE },
  { "er", PROP_REAL, { 4, PROP_NO_STR }, PROP_RNGII (1, 100) },
  { "mur", PROP_REAL, { 1, PROP_NO_STR }, PROP_RNGII (1, 100) },
  { "tand", PROP_REAL, { 4e-4, PROP_NO_STR }, PROP_POS_RANGE },
  { "rho", PROP_REAL, { 0.022e-6, PROP_NO_STR }, PROP_POS_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };
struct define_t twistedpair::cirdef =
  { "TWIST", 4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
