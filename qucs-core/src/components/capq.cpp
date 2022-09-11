/*
 * capq.cpp - Lossy capacitor class implementation
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
 * $Id$
 *
 */


#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "component.h"
#include "capq.h"
#include "iostream"
using namespace qucs;

//--------------------------------------------------------
// References:
// [1] Bahl. Lumped elements for RF and microwave circuits. Pg 167

capq::capq () : circuit (2) {
  type = CIR_CAPQ;
}

void capq::calcYp (nr_double_t frequency) {
 nr_double_t C = getPropertyDouble ("C");
 nr_double_t Q = getPropertyDouble ("Q");
 nr_double_t f = getPropertyDouble ("f");
 nr_double_t Bc = 2.*pi*frequency*C;
 nr_double_t Gp = 0;
 if ((f!=0) && (Q!=0) && (frequency!=0)) // Q=0 or f=0 can be used to force ideal behavior (no losses)
 {
   nr_double_t Qf=Q;
   if (!strcmp (getPropertyString ("Mode"), "Linear")) Qf*=frequency/f;
   if (!strcmp (getPropertyString ("Mode"), "SquareRoot"))Qf*=qucs::sqrt(frequency/f);
   Gp = Bc / Qf;
 }
 Yp = nr_complex_t (Gp, Bc);
}

//------------------------------------------------------------------
// This function calculates the S-parameters matrix of a lossy capacitor
// Q = 1 / (2*pi*f*Rs*C) where Rs is the series resistance and C the capacitance or also
// Q = 2*pi*f*C*Gp where Gp is the parallel conductance

void capq::calcSP (nr_double_t frequency) {
  calcYp(frequency);
  nr_complex_t y = 2.0 * z0 * Yp;
  setS (NODE_1, NODE_1, 1.0 / (1.0 + y));
  setS (NODE_2, NODE_2, 1.0 / (1.0 + y));
  setS (NODE_1, NODE_2, y / (1.0 + y));
  setS (NODE_2, NODE_1, y / (1.0 + y));
}

void capq::calcNoiseSP (nr_double_t) {
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  matrix s = getMatrixS ();
  matrix e = eye (getSize ());
  setMatrixN (celsius2kelvin (T) / T0 * (e - s * transpose (conj (s))));
}

void capq::initDC (void) {
  allocMatrixMNA ();
  // open circuit
  clearY ();
}

void capq::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
}


void capq::initSP(void)
{
  allocMatrixS ();
}

void capq::calcAC (nr_double_t frequency) {
 calcYp(frequency);
 nr_complex_t y11 = Yp;
 nr_complex_t y12 = -Yp;
 nr_complex_t y21 = -Yp;
 nr_complex_t y22 = Yp;
  setY (NODE_1, NODE_1, y11); setY (NODE_2, NODE_2, y22);
  setY (NODE_1, NODE_2, y12); setY (NODE_2, NODE_1, y21);
}

void capq::calcNoiseAC (nr_double_t) {
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  setMatrixN (4 * celsius2kelvin (T) / T0 * real (getMatrixY ()));
}

// properties
PROP_REQ [] = {
  { "C", PROP_REAL, { 1e-12, PROP_NO_STR }, PROP_POS_RANGE },
  { "Q", PROP_REAL, { 100, PROP_NO_STR }, PROP_POS_RANGE },
  { "f", PROP_REAL, { 100e6, PROP_NO_STR }, PROP_NO_RANGE },
  { "Mode", PROP_STR, { PROP_NO_VAL, "Linear" },
    PROP_RNG_STR3 ("Linear", "SquareRoot", "Constant") },
    PROP_NO_PROP };
PROP_OPT [] = {
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };
struct define_t capq::cirdef =
  { "CAPQ", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
