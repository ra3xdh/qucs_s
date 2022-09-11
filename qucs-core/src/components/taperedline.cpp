/*
 * taperedline.cpp - ideal tapered transmission line class implementation
 *
 * Copyright (C) 2015 Claudio Girardi <in3otd@qsl.net>
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



/* References
[1] Microwave Engineering. David M Pozar. John Wiley and Sons. 4th edition. Pages 261-267
[2] "Conversions between S, Z, Y, h, ABCD and T parameters which are Valid for Complex source and load impedances".
Dean A. Frickey. IEEE Transaction on Microwave Theory and Techniques. Vol 42. No 2. February 1994
[3] "Handbook of Mathematical functions with Formulas, Graphs and Mathematical Tables". Milton Abramowitz. 
US department of commerce (1964). Link: http://people.math.sfu.ca/~cbm/aands/page_375.htm
*/
#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "component.h"
#include "taperedline.h"

using namespace qucs;

taperedline::taperedline () : circuit (2) {
  type = CIR_TAPEREDLINE;
}
//------------------------------------------------------------------
// This function calculates the ABCD matrix of an arbitrarily tapered
// line by diving it into differential slots and calculating their ABCD matrix.
void taperedline::calcABCDparams(nr_double_t frequency)
{
  nr_double_t L = getPropertyDouble ("L");//Length
  nr_double_t alpha = getPropertyDouble ("Alpha");//Loss coefficient
  alpha = std::log(alpha) *.5;//The attenuation coefficient needs to be converted into Neper/m units
  nr_double_t lstep = L/Nsteps; //Size of the differential elements
  nr_double_t beta = 2*pi*frequency/C0; //Propagation constant
  nr_complex_t gamma = nr_complex_t (alpha, beta); //Complex propagation constant
  matrix ABCD_ = eye(2);//Overall ABCD matrix
  matrix ABCDaux = eye(2);//Auxiliary matrix for performing the iterative product
  nr_double_t Zi;
  nr_complex_t a, b, c, d;
  // ABCD coefficients
  a = cosh(gamma*lstep);
  b = sinh(gamma*lstep); // need to be multiplied by Zi
  c = sinh(gamma*lstep); // need to be divided by Zi
  d = cosh(gamma*lstep); 

  nr_double_t l = lstep/2.0; // compute impedance in the middle of the section
  for (int idx = 0 ; idx < Nsteps; idx++)
  {
    // The line is discretized in finite elements. The size of these elements can be considered a differential
    // length since the impedance change across the actual section is small. 
    // Taking into account the cascading property of the ABCD matrix, the overall
    // ABCD matrix can be calculated as the product of the individual ABCD matrices.   
    Zi = Zprofile[idx];
    ABCDaux.set(0,0,a);
    ABCDaux.set(0,1,Zi*b);
    ABCDaux.set(1,0,c/Zi);
    ABCDaux.set(1,1,d);
    //Iterative product
    ABCD_=ABCD_*ABCDaux;
    l += lstep; // move to the next section
  }
  // Overall ABCD coefficients
  ABCD = ABCD_;
}

//------------------------------------------------------------------
// Exponential impedance profile
nr_double_t taperedline::calcExponential(nr_double_t l, nr_double_t L, nr_double_t Z1, nr_double_t Z2)
{
   nr_double_t a = (1/L)*std::log(Z2/Z1);
   return Z1*std::exp(a*l);
}

//------------------------------------------------------------------
// Linear impedance profile
nr_double_t taperedline::calcLinear(nr_double_t l, nr_double_t L, nr_double_t Z1, nr_double_t Z2)
{
   return Z1+l*(Z2-Z1)/L;
}

//------------------------------------------------------------------
// Triangular impedance profile
nr_double_t taperedline::calcTriangular(nr_double_t l, nr_double_t L, nr_double_t Z1, nr_double_t Z2)
{
   if (l < L/2)
   {
     return Z1*std::exp((2*l*l/(L*L))*std::log(Z2/Z1));
   }

   return Z1*std::exp(((4*l/L) - (2*l*l/(L*L)) -1)*std::log(Z2/Z1));
}

//------------------------------------------------------------------
// Klopfenstein impedance profile
nr_double_t taperedline::calcKlopfenstein(nr_double_t l, nr_double_t L, nr_double_t Z1, nr_double_t Z2, nr_double_t gamma_max)
{
   nr_double_t gamma0 = 0.5*std::log(Z2/Z1);
   nr_double_t A = std::acosh(gamma0/gamma_max);
   return (std::exp(0.5*std::log(Z1*Z2) + (gamma0/std::cosh(A))*A*A*phi(2.*(l/L)-1., A)));
}

void taperedline::calcImpedanceProfile()
{
nr_double_t L = getPropertyDouble ("L");//Length
  nr_double_t Z1 = getPropertyDouble ("Z1");//Port 1 impedance
  nr_double_t Z2 = getPropertyDouble ("Z2");//Port 2 impedance

  if (Z1 > Z2)//Handling Z1 > Z2 case
  {
     logprint (LOG_ERROR, "WARNING: The impedance at port 1 is bigger than the impedance at port 2 ((Z1 = %g Ohm ) > (Z2 = %g Ohm))\n", Z1, Z2);
     nr_double_t Zaux = Z2;
     Z2 = Z1;
     Z1 = Zaux;
  } 

  nr_double_t gamma_max = getPropertyDouble ("Gamma_max");;//Maximum ripple (Klopfenstein weighting only)
  nr_double_t lstep = L/Nsteps; //Size of the differential elements

  int idx=0;
  nr_double_t l = lstep/2.0;
  for (idx = 0, l = lstep/2.0 ; idx < Nsteps; idx++, l += lstep)
  {
    // The line is discretized in finite elements. The size of these elements can be considered a differential
    // length since the impedance change across the actual section is small. 
    // Taking into account the cascading property of the ABCD matrix, the overall
    // ABCD matrix can be calculated as the product of the individual ABCD matrices.   
    if (!strcmp (getPropertyString ("Weighting"), "Exponential"))
    {
       Zprofile[idx] = calcExponential(l, L, Z1, Z2);
    }
    else
    {
        if (!strcmp (getPropertyString ("Weighting"), "Linear"))
        {
            Zprofile[idx] = calcLinear(l, L, Z1, Z2);
        }
        else
        {
           if (!strcmp (getPropertyString ("Weighting"), "Triangular"))
           {
              Zprofile[idx] = calcTriangular(l, L, Z1, Z2);
           }
           else//Klopfenstein
           {
              Zprofile[idx] = calcKlopfenstein(l, L, Z1, Z2, gamma_max);
           }
        }
    }
 }
}

//------------------------------------------------------------------
// Auxiliary function for Klopfenstein profile calculation
// The recursive calculation algorithm is from
//   Grossberg, M. A., "Extremely rapid computation of the Klopfenstein
//   impedance taper," IEEE Proc., vol.56, no.9, pp.1629-1630, Sept. 1968
//
// In practice less than 10 iterations are needed in most cases
nr_double_t taperedline::phi(nr_double_t x, nr_double_t A)
{
  nr_double_t ak = 1.0;
  nr_double_t bk = x *.5;
  nr_double_t ck = bk;
  nr_double_t phi = bk;
  for (int k = 1; k < 20; k++) {
    ck *= (1.0 - x*x);
    bk = (ck + 2.0*k*bk) / (2.0*k + 1.0);
    ak = A*A / (4.0*k*(k + 1.0)) * ak;
    phi += ak*bk;
    if (std::abs(ak*bk) < 1e-7) break;
  }
  return phi;
}

void taperedline::calcSP (nr_double_t frequency) {
  nr_double_t Z1 = getPropertyDouble ("Z1");//Port 1 impedance
  nr_double_t Z2 = getPropertyDouble ("Z2");//Port 2 impedance
  calcABCDparams(frequency);
  matrix Stmp = qucs::atos(ABCD, z0, z0);
  if (Z1 > Z2)
  {//Flip S matrix
    Stmp.exchangeRows(0,1); 
    Stmp.exchangeCols(0,1);
  }
  setMatrixS(Stmp);
}

void taperedline::calcNoiseSP (nr_double_t) {
  nr_double_t l = getPropertyDouble ("L");
  if (l < 0) return;
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  matrix s = getMatrixS ();
  matrix e = eye (getSize ());
  setMatrixN (celsius2kelvin (T) / T0 * (e - s * transpose (conj (s))));
}

void taperedline::calcNoiseAC (nr_double_t) {
 nr_double_t l = getPropertyDouble ("L");
  if (l < 0) return;
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  setMatrixN (4.0 * celsius2kelvin (T) / T0 * real (getMatrixY ()));
}

void taperedline::initDC (void) {
  // DC losses not modelled
  setVoltageSources (1);
  setInternalVoltageSource (1);
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
  logprint (LOG_ERROR, "WARNING: The tapered line model does not include DC losses\n");
}

void taperedline::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
  calcImpedanceProfile();
}


void taperedline::initSP(void)
{
  allocMatrixS ();
  calcImpedanceProfile();
}

void taperedline::calcAC (nr_double_t frequency) {
  calcABCDparams(frequency);
  nr_double_t L = getPropertyDouble ("L");
  if (L != 0.0) {
    nr_complex_t y11 = ABCD.get(1,1)/ABCD.get(0,1);
    nr_complex_t y12 = -det(ABCD)/ABCD.get(0,1);
    nr_complex_t y21 = -1./ABCD.get(0,1);
    nr_complex_t y22 = ABCD.get(0,0)/ABCD.get(0,1);
    setY (NODE_1, NODE_1, y11); setY (NODE_2, NODE_2, y22);
    setY (NODE_1, NODE_2, y12); setY (NODE_2, NODE_1, y21);
  }
}


// properties
PROP_REQ [] = {
  { "Z1", PROP_REAL, { 50, PROP_NO_STR }, PROP_POS_RANGE },
  { "Z2", PROP_REAL, { 100, PROP_NO_STR }, PROP_POS_RANGE },
  { "L", PROP_REAL, { 75e-3, PROP_NO_STR }, PROP_NO_RANGE },
  { "Weighting", PROP_STR, { PROP_NO_VAL, "Exponential" },
    PROP_RNG_STR4 ("Exponential", "Linear", "Triangular", "Klopfenstein") },
    PROP_NO_PROP };
PROP_OPT [] = {
  { "Gamma_max", PROP_REAL, { 0.1, PROP_NO_STR }, PROP_POS_RANGEX },
  { "Alpha", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGEX },
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };
struct define_t taperedline::cirdef =
  { "TAPEREDLINE", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
