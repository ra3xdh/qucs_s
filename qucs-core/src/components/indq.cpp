/*
 * indq.cpp - Lossy inductor class implementation
 *
 * Copyright (C) 2015 Andres Martinez-Mera <andresmartinezmera@gmail.com>
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
 */


#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "component.h"
#include "indq.h"
#include "iostream"
using namespace qucs;

//--------------------------------------------------------
// References:
// [1] Bahl. Lumped elements for RF and microwave circuits. Pg 22

indq::indq () : circuit (2) {
  type = CIR_INDQ;
}

// calculate equivalent series impedance of the lossy inductor
void indq::calcZs(nr_double_t frequency) {
  nr_double_t L = getPropertyDouble ("L");
  nr_double_t Q = getPropertyDouble ("Q");
  nr_double_t f = getPropertyDouble ("f");
  nr_double_t Xl = 2.*pi*frequency*L;
  nr_double_t Rs = 0.0;
  if ((f != 0.0) && (Q != 0.0) && (frequency!=0)) { // Q=0 or f=0 can be used to force ideal behavior (no losses)
    nr_double_t Qf = Q;
    if (!strcmp (getPropertyString ("Mode"), "Linear")) Qf *= frequency/f;
    if (!strcmp (getPropertyString ("Mode"), "SquareRoot")) Qf *= qucs::sqrt(frequency/f);
    Rs = Xl / Qf;
  }
  Zs = nr_complex_t (Rs, Xl);
}

//------------------------------------------------------------------
// This function calculates the S-parameters matrix of a lossy inductance
// Q = 2*pi*f*L/Rs
// where Rs is the series resistance and L the inductance
void indq::calcSP (nr_double_t frequency) {
 // calculate element series impedance
 calcZs(frequency);

 nr_complex_t z = Zs / z0;
 setS (NODE_1, NODE_1, z / (z + 2.0));
 setS (NODE_2, NODE_2, z / (z + 2.0));
 setS (NODE_1, NODE_2, 2.0 / (z + 2.0));
 setS (NODE_2, NODE_1, 2.0 / (z + 2.0));
}

void indq::calcNoiseSP (nr_double_t) {
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  matrix s = getMatrixS ();
  matrix e = eye (getSize ());
  setMatrixN (celsius2kelvin (T) / T0 * (e - s * transpose (conj (s))));
}

void indq::initDC (void) {
  //Short circuit
  setVoltageSources (1);
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
}

void indq::calcDC (void) {
  clearY ();
}

void indq::initAC (void) {
  nr_double_t L = getPropertyDouble ("L");

  // for non-zero inductance usual MNA entries
  if (L != 0.0) {
    setVoltageSources (0);
    allocMatrixMNA ();
  }
  // for zero inductance create a zero voltage source
  else {
    initDC ();
    calcDC ();
  }
}


void indq::initSP(void)
{
  allocMatrixS ();
}

void indq::calcAC (nr_double_t frequency) {
  // calculate element series impedance
  calcZs(frequency);
  // for non-zero impedance usual MNA entries
  if (Zs != 0.0) {
    nr_complex_t y11 = 1./Zs;
    nr_complex_t y12 = -1./Zs;
    nr_complex_t y21 = -1./Zs;
    nr_complex_t y22 = 1./Zs;
    setY (NODE_1, NODE_1, y11); setY (NODE_2, NODE_2, y22);
    setY (NODE_1, NODE_2, y12); setY (NODE_2, NODE_1, y21);
  }
}

void indq::calcNoiseAC (nr_double_t) {
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  setMatrixN (4 * celsius2kelvin (T) / T0 * real (getMatrixY ()));
}

// properties
PROP_REQ [] = {
  { "L", PROP_REAL, { 1e-9, PROP_NO_STR }, PROP_POS_RANGE },
  { "Q", PROP_REAL, { 100, PROP_NO_STR }, PROP_POS_RANGE },
  { "f", PROP_REAL, { 100e6, PROP_NO_STR }, PROP_NO_RANGE },
  { "Mode", PROP_STR, { PROP_NO_VAL, "Linear" },
    PROP_RNG_STR3 ("Linear", "SquareRoot", "Constant") },
    PROP_NO_PROP };
PROP_OPT [] = {
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };
struct define_t indq::cirdef =
  { "INDQ", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
