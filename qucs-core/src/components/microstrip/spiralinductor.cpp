/*
 * spiralinductor.cpp - Spiral inductor class implementation
 *
 * Copyright (C) 2016 Andres Martinez-Mera <andresmartinezmera@gmail.com>
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
#include "spiralinductor.h"
#include "substrate.h"
using namespace qucs;


//------------------------------------------------------------------------
// References:
// [1] I. Bahl, Lumped Elements for RF and Microwave Circuits, Artech House, Norwood, MA, 2003.
// [2] Simple Accurate Expressions for Planar Spiral Inductances. S. S. Mohan. IEEE Journal of Solid State Circuits.
//     Vol 34, No 10, October 1999
// [3] I. Bahl, Fundamentals of RF and Microwave Transistor Amplifiers, John Wiley and Sons, 2009.
//------------------------------------------------------------------------
spiralinductor::spiralinductor () : circuit (2) {
  type = CIR_SPIRALIND;
}
//------------------------------------------------------------------
// This function calculates the ABCD matrix of the spiral inductance
void spiralinductor::calcABCDparams(nr_double_t frequency)
{
 nr_double_t N = getPropertyDouble ("N");//Number of turns
 nr_double_t Di = getPropertyDouble ("Di");//Inner diameter
 nr_double_t W = getPropertyDouble ("W");//Width
 nr_double_t S = getPropertyDouble ("S");//Spacing between turns
 substrate * subst = getSubstrate ();

 nr_double_t Do = Di + 2.*N*W + (2.*N-1)*S;
 nr_double_t chi = (Do - Di) / (Do + Di);//Fill ratio
 nr_double_t a = (Di+Do)/4.;
 nr_double_t Dav = .5*(Do+Di);

 nr_double_t rho = subst->getPropertyDouble ("rho");
 nr_double_t t = subst->getPropertyDouble ("t");

 nr_double_t K = 1.+0.333*qucs::pow(1.+S/W, -1.7);// Crowding effect in the corners. The following relies on the assumption that
// square, hexagonal and octogonal inductors have the same crowding effect as the spiral inductor

 nr_double_t c1, c2, c3, c4;


 if (!strcmp (getPropertyString ("Geometry"), "Circular"))
 {c1 = 1; c2 = 2.46; c3 = 0; c4 = 0.2;}

 if (!strcmp (getPropertyString ("Geometry"), "Square"))
 {c1 = 1.27; c2 = 2.07; c3 = 0.18; c4 = 0.13;}

 if (!strcmp (getPropertyString ("Geometry"), "Hexagonal"))
 {c1 = 1.09; c2 = 2.23; c3 = 0; c4 = 0.17;}

 if (!strcmp (getPropertyString ("Geometry"), "Octogonal"))
 {c1 = 1.07; c2 = 2.29; c3 = 0; c4 = 0.19;}


 nr_double_t L = 2.*pi*1e-7*N*N*Dav*c1*(qucs::log(c2/chi) + c3*chi + c4*chi*chi);
 nr_double_t Rs = rho/t;
 R = K*pi*a*N*Rs/W;//The resistance is a global variable since it is used in the DC analysis too
 nr_double_t C = (3.5e-5*Do+0.06)*1e-12;


 //ABCD matrix
 ABCD = eye(2);
 ABCD.set(0,0, nr_complex_t(1-4*pi*pi*frequency*frequency*L*C, 2*pi*R*C*frequency));
 ABCD.set(0,1, nr_complex_t(R, 2.*pi*L*frequency));
 ABCD.set(1,0, nr_complex_t(-4*pi*pi*C*C*R*frequency*frequency, -8*pi*pi*pi*C*C*L*frequency*frequency*frequency + 4*pi*C*frequency));
 ABCD.set(1,1, nr_complex_t(-4.*pi*pi*C*L*frequency*frequency+1., 2.*pi*C*R*frequency));
}

void spiralinductor::calcSP (nr_double_t frequency) {
  calcABCDparams(frequency);
  matrix Stmp = qucs::atos(ABCD, z0, z0);
  setMatrixS(Stmp);
}

void spiralinductor::calcNoiseSP (nr_double_t) {
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  matrix s = getMatrixS ();
  matrix e = eye (getSize ());
  setMatrixN (celsius2kelvin (T) / T0 * (e - s * transpose (conj (s))));
}

// DC model: The equivalent resistance of the different spiral inductor geometry is assumed to be the same as the series
// resistance of the spiral inductor
void spiralinductor::initDC (void) {
  allocMatrixMNA ();
  if (R != 0.0) {
    nr_double_t g = 1.0 / R;
    setVoltageSources (0);
    allocMatrixMNA ();
    setY (NODE_1, NODE_1, +g); setY (NODE_2, NODE_2, +g);
    setY (NODE_1, NODE_2, -g); setY (NODE_2, NODE_1, -g);
  }
  // for zero resistances create a zero voltage source
  else {
    setVoltageSources (1);
    setInternalVoltageSource (1);
    allocMatrixMNA ();
    voltageSource (VSRC_1, NODE_1, NODE_2);
  }
}

void spiralinductor::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
}


void spiralinductor::initSP(void)
{
  allocMatrixS ();
}

void spiralinductor::calcAC (nr_double_t frequency) {
  calcABCDparams(frequency);
  nr_complex_t y11 = ABCD.get(1,1)/ABCD.get(0,1);
  nr_complex_t y12 = -det(ABCD)/ABCD.get(0,1);
  nr_complex_t y21 = -1./ABCD.get(0,1);
  nr_complex_t y22 = ABCD.get(0,0)/ABCD.get(0,1);
  setY (NODE_1, NODE_1, y11); setY (NODE_2, NODE_2, y22);
  setY (NODE_1, NODE_2, y12); setY (NODE_2, NODE_1, y21);
}

void spiralinductor::calcNoiseAC (nr_double_t) {
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  setMatrixN (4 * celsius2kelvin (T) / T0 * real (getMatrixY ()));
}

// properties
PROP_REQ [] = {
  { "Subst", PROP_STR, { PROP_NO_VAL, "Subst1" }, PROP_NO_RANGE },
  { "Geometry", PROP_STR, { PROP_NO_VAL, "Circular" },
    PROP_RNG_STR4 ("Circular", "Square", "Hexagonal", "Octogonal") },
  { "W", PROP_REAL, { 25e-6, PROP_NO_STR }, PROP_POS_RANGE },
  { "Di", PROP_REAL, { 200e-6, PROP_NO_STR }, PROP_POS_RANGE },
  { "S", PROP_REAL, { 25e-6, PROP_NO_STR }, PROP_NO_RANGE },
  { "N", PROP_REAL, { 3, PROP_NO_STR }, PROP_NO_RANGE },
    PROP_NO_PROP };
PROP_OPT [] = {
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };
struct define_t spiralinductor::cirdef =
  { "SPIRALIND", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
