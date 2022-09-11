/*
 * cpwline.cpp - coplanar waveguide line class implementation
 *
 * Copyright (C) 2004, 2005 Vincent Habchi, F5RCS <10.50@free.fr>
 * Copyright (C) 2004, 2005, 2006, 2008 Stefan Jahn <stefan@lkcc.org>
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

#include <limits>

#include "component.h"
#include "substrate.h"
#include "cpwline.h"

using namespace qucs;

cpwline::cpwline () : circuit (2) {
  Zl = Er = 0;
  type = CIR_CPWLINE;
}

/*! K(k)

 The function computes the complete elliptic integral of first kind
   K() and the second kind E() using the arithmetic-geometric mean
   algorithm (AGM) by Abramowitz and Stegun.
\todo move to common math
*/
/* The function computes the complete elliptic integral of first kind
   K(k) using the arithmetic-geometric mean algorithm (AGM) found e.g.
   in Abramowitz and Stegun (17.6.1). 
   Note that the argument of the function here is the elliptic modulus k
   and not the parameter m=k^2 . */
/* \todo move to common math */
nr_double_t cpwline::ellipk (nr_double_t k) {
  if ((k < 0.0) || (k >= 1.0))
    // we use only the range from 0 <= k < 1
    return std::numeric_limits<nr_double_t>::quiet_NaN();

  nr_double_t a = 1.0;
  nr_double_t b = qucs::sqrt(1-k*k);
  nr_double_t c = k;

  while (c > std::numeric_limits<nr_double_t>::epsilon()) {
    nr_double_t tmp = (a + b) / 2.0;
    c = (a - b) / 2.0;
    b = qucs::sqrt(a * b);
    a = tmp;
  }
  return (pi_over_2 / a);
}

nr_double_t cpwline::KoverKp(nr_double_t k) {
  if ((k < 0.0) || (k >= 1.0))
    return std::numeric_limits<nr_double_t>::quiet_NaN();

  return (ellipk(k) / ellipk(qucs::sqrt(1-k*k)));
}

/* Approximation of K(k)/K'(k).
   First appeared in
   Hilberg, W., "From Approximations to Exact Relations for Characteristic
   Impedances," IEEE Trans. MTT, May 1969.
   More accurate expressions can be found in the above article and in
   Abbott, J. T., "Modeling the Capacitive Behavior of Coplanar Striplines
   and Coplanar Waveguides using Simple Functions", Rochester Institute of
   Technology, Rochester, New York, June 2011.
   The maximum relative error of the approximation implemented here is
   about 2 ppm, so good enough for any practical purpose.
 */
nr_double_t cpwline::ellipa (nr_double_t k) {
  nr_double_t r, kp;
  if (k < sqrt1_2) {
    kp = qucs::sqrt (1 - k * k);
    r = pi / qucs::log (2 * (1 + qucs::sqrt (kp)) / (1 - qucs::sqrt (kp)));
  }
  else {
    r = qucs::log (2 * (1 + qucs::sqrt (k)) / (1 - qucs::sqrt (k))) / pi;
  }
  return r;
}

void cpwline::initSP (void) {
  // allocate S-parameter matrix
  allocMatrixS ();
  // pre-compute propagation factors
  initPropagation ();
}

void cpwline::initPropagation (void) {
  // get properties of substrate and coplanar line
  nr_double_t W =  getPropertyDouble ("W");
  nr_double_t s =  getPropertyDouble ("S");
  substrate * subst = getSubstrate ();
  nr_double_t er = subst->getPropertyDouble ("er");
  nr_double_t h  = subst->getPropertyDouble ("h");
  nr_double_t t  = subst->getPropertyDouble ("t");
  int backMetal  = !strcmp (getPropertyString ("Backside"), "Metal");
  int approx     = !strcmp (getPropertyString ("Approx"), "yes");

  tand = subst->getPropertyDouble ("tand");
  rho  = subst->getPropertyDouble ("rho");
  len  = getPropertyDouble ("L");

  // other local variables (quasi-static constants)
  nr_double_t k1, kk1, kpk1, k2, k3, q1, q2, q3 = 0, qz, er0 = 0;

  // compute the necessary quasi-static approx. (K1, K3, er(0) and Z(0))
  k1   = W / (W + s + s);
  kk1  = ellipk (k1);
  kpk1 = ellipk (qucs::sqrt (1 - k1 * k1));
  if (approx) {
    q1 = ellipa (k1);
  } else {
    q1 = kk1 / kpk1;
  }

  // backside is metal
  if (backMetal) {
    k3  = qucs::tanh ((pi / 4) * (W / h)) / qucs::tanh ((pi / 4) * (W + s + s) / h);
    if (approx) {
      q3 = ellipa (k3);
    } else {
      q3 = ellipk (k3) / ellipk (qucs::sqrt (1 - k3 * k3));
    }
    qz  = 1 / (q1 + q3);
    er0 = 1 + q3 * qz * (er - 1);
    zl_factor = Z0 / 2 * qz;
  }
  // backside is air
  else {
    k2  = qucs::sinh ((pi / 4) * (W / h)) / qucs::sinh ((pi / 4) * (W + s + s) / h);
    if (approx) {
      q2 = ellipa (k2);
    } else {
      q2 = ellipk (k2) / ellipk (qucs::sqrt (1 - k2 * k2));
    }
    er0 = 1 + (er - 1) / 2 * q2 / q1;
    zl_factor = Z0 / 4 / q1;
  }

  // adds effect of strip thickness
  if (t > 0) {
    nr_double_t d, ke, qe;
    d  = (t * 1.25 / pi) * (1 + qucs::log (4 * pi * W / t));

    // modifies k1 accordingly (k1 = ke)
    ke = k1 + (1 - k1 * k1) * d / 2 / s;
    if (approx) {
      qe = ellipa (ke);
    } else {
      qe = ellipk (ke) / ellipk (qucs::sqrt (1 - ke * ke));
    }
    // backside is metal
    if (backMetal) {
      qz  = 1 / (qe + q3);
      //er0 = 1 + q3 * qz * (er - 1);
      zl_factor = Z0 / 2 * qz;
    }
    // backside is air
    else {
      zl_factor = Z0 / 4 / qe;
    }

    // modifies er0 as well
    er0 = er0 - (0.7 * (er0 - 1) * t / s) / (q1 + (0.7 * t / s));
  }

  // pre-compute square roots
  sr_er = qucs::sqrt (er);
  sr_er0 = qucs::sqrt (er0);

  // cut-off frequency of the TE0 mode
  fte = (C0 / 4) / (h * qucs::sqrt (er - 1));

  // dispersion factor G
  nr_double_t p = qucs::log (W / h);
  nr_double_t u = 0.54 - (0.64 - 0.015 * p) * p;
  nr_double_t v = 0.43 - (0.86 - 0.54 * p) * p;
  G = qucs::exp (u * qucs::log (W / s) + v);

  // loss constant factors (computed only once for efficiency sake)
  nr_double_t ac = 0;
  if (t > 0) {
    // equations by GHIONE
    nr_double_t n  = (1 - k1) * 8 * pi / (t * (1 + k1));
    nr_double_t a  = W / 2;
    nr_double_t b  = a + s;
    ac = (pi + qucs::log (n * a)) / a + (pi + qucs::log (n * b)) / b;
  }
  ac_factor  = ac / (4 * Z0 * kk1 * kpk1 * (1 - k1 * k1));
  ac_factor *= qucs::sqrt (pi * MU0 * rho); // Rs factor
  ad_factor  = (er / (er - 1)) * tand * pi / C0;

  // propagation constant (partial, final value computed in calcAB() )
  bt_factor  = 2 * pi / C0;
}

void cpwline::calcAB (nr_double_t f, nr_double_t& zl, nr_double_t& al,
		      nr_double_t& bt) {
  nr_double_t sr_er_f = sr_er0;
  nr_double_t ac = ac_factor;
  nr_double_t ad = ad_factor;

  // by initializing as much as possible outside this function, the
  // overhead is minimal

  // add the dispersive effects to er0
  sr_er_f += (sr_er - sr_er0) / (1 + G * qucs::pow (f / fte, -1.8));

  // computes impedance
  zl /= sr_er_f;

  // for now, the loss are limited to strip losses (no radiation
  // losses yet) losses in neper/length
  ad *= f * (sr_er_f - 1 / sr_er_f);
  ac *= qucs::sqrt (f) * sr_er0;

  al  = ac + ad;
  bt *= sr_er_f * f;

  Er = sqr (sr_er_f);
  Zl = zl;
}

void cpwline::saveCharacteristics (nr_double_t) {
  setCharacteristic ("Zl", Zl);
  setCharacteristic ("Er", Er);
}

void cpwline::calcSP (nr_double_t frequency) {

  nr_double_t zl = zl_factor;
  nr_double_t beta = bt_factor;
  nr_double_t alpha;

  calcAB (frequency, zl, alpha, beta);

  // calculate and set S-parameters
  nr_double_t z = zl / z0;
  nr_double_t y = 1 / z;
  nr_complex_t g = nr_complex_t (alpha, beta);
  nr_complex_t n = 2.0 * cosh (g * len) + (z + y) * sinh (g * len);
  nr_complex_t s11 = (z - y) * sinh (g * len) / n;
  nr_complex_t s21 = 2.0 / n;

  setS (NODE_1, NODE_1, s11); setS (NODE_2, NODE_2, s11);
  setS (NODE_1, NODE_2, s21); setS (NODE_2, NODE_1, s21);
}

/* FIXME : following function is unused? */
/* The function calculates the quasi-static impedance of a coplanar
   waveguide line and the value of the effective dielectric constant
   for the given coplanar line and substrate properties. */
void cpwline::analyseQuasiStatic (nr_double_t W, nr_double_t s, nr_double_t h,
				  nr_double_t t, nr_double_t er, int backMetal,
				  nr_double_t& ZlEff, nr_double_t& ErEff) {

  // local variables (quasi-static constants)
  nr_double_t k1, k2, k3, q1, q2, q3 = 0, qz;

  ErEff = er;
  ZlEff = 0;

  // compute the necessary quasi-static approx. (K1, K3, er(0) and Z(0))
  k1 = W / (W + s + s);
  q1 = ellipk (k1) / ellipk (qucs::sqrt (1 - k1 * k1));

  // backside is metal
  if (backMetal) {
    k3  = qucs::tanh ((pi / 4) * (W / h)) / qucs::tanh ((pi / 4) * (W + s + s) / h);
    q3 = ellipk (k3) / ellipk (qucs::sqrt (1 - k3 * k3));
    qz  = 1 / (q1 + q3);
    ErEff = 1 + q3 * qz * (er - 1);
    ZlEff = Z0 / 2 * qz;
  }
  // backside is air
  else {
    k2  = qucs::sinh ((pi / 4) * (W / h)) / qucs::sinh ((pi / 4) * (W + s + s) / h);
    q2 = ellipk (k2) / ellipk (qucs::sqrt (1 - k2 * k2));
    ErEff = 1 + (er - 1) / 2 * q2 / q1;
    ZlEff = Z0 / 4 / q1;
  }

  // adds effect of strip thickness
  if (t > 0) {
    nr_double_t d, ke, qe;
    d  = (t * 1.25 / pi) * (1 + qucs::log (4 * pi * W / t));

    // modifies k1 accordingly (k1 = ke)
    ke = k1 + (1 - k1 * k1) * d / 2 / s;
    qe = ellipk (ke) / ellipk (qucs::sqrt (1 - ke * ke));

    // backside is metal
    if (backMetal) {
      qz  = 1 / (qe + q3);
      //ErEff = 1 + q3 * qz * (er - 1);
      ZlEff = Z0 / 2 * qz;
    }
    // backside is air
    else {
      ZlEff = Z0 / 4 / qe;
    }

    // modifies ErEff as well
    ErEff = ErEff - (0.7 * (ErEff - 1) * t / s) / (q1 + (0.7 * t / s));
  }
  ErEff = qucs::sqrt (ErEff);
  ZlEff /= ErEff;
}

/* FIXME : following function is unused? */
/* This function calculates the frequency dependent value of the
   effective dielectric constant and the coplanar line impedance for
   the given frequency. */
void cpwline::analyseDispersion (nr_double_t W, nr_double_t s, nr_double_t h,
				 nr_double_t er, nr_double_t ZlEff,
				 nr_double_t ErEff, nr_double_t frequency,
				 nr_double_t& ZlEffFreq,
				 nr_double_t& ErEffFreq) {
  // local variables
  nr_double_t fte, G;

  ErEffFreq = ErEff;
  ZlEffFreq = ZlEff * ErEff;

  // cut-off frequency of the TE0 mode
  fte = (C0 / 4) / (h * qucs::sqrt (er - 1));

  // dispersion factor G
  nr_double_t p = qucs::log (W / h);
  nr_double_t u = 0.54 - (0.64 - 0.015 * p) * p;
  nr_double_t v = 0.43 - (0.86 - 0.54 * p) * p;
  G = qucs::exp (u * qucs::log (W / s) + v);

  // add the dispersive effects to er0
  ErEffFreq += (qucs::sqrt (er) - ErEff) / (1 + G * qucs::pow (frequency / fte, -1.8));

  // computes impedance
  ZlEffFreq /= ErEffFreq;
}

void cpwline::calcNoiseSP (nr_double_t) {
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  matrix s = getMatrixS ();
  matrix e = eye (getSize ());
  setMatrixN (celsius2kelvin (T) / T0 * (e - s * transpose (conj (s))));
}

void cpwline::initDC (void) {
  // a DC short (voltage source V = 0 volts)
  setVoltageSources (1);
  setInternalVoltageSource (1);
  allocMatrixMNA ();
  clearY ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
}

void cpwline::initTR (void) {
  initDC ();
}

void cpwline::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
  initPropagation ();
}

void cpwline::calcAC (nr_double_t frequency) {

  nr_double_t zl = zl_factor;
  nr_double_t beta = bt_factor;
  nr_double_t alpha;

  calcAB (frequency, zl, alpha, beta);

  // calculate and set Y-parameters
  nr_complex_t g = nr_complex_t (alpha, beta);
  nr_complex_t y11 = coth (g * len) / zl;
  nr_complex_t y21 = -cosech (g * len) / zl;

  setY (NODE_1, NODE_1, y11); setY (NODE_2, NODE_2, y11);
  setY (NODE_1, NODE_2, y21); setY (NODE_2, NODE_1, y21);
}

void cpwline::calcNoiseAC (nr_double_t) {
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  setMatrixN (4 * celsius2kelvin (T) / T0 * real (getMatrixY ()));
}

// properties
PROP_REQ [] = {
  { "W", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "S", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "L", PROP_REAL, { 10e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "Subst", PROP_STR, { PROP_NO_VAL, "Subst1" }, PROP_NO_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  { "Backside", PROP_STR, { PROP_NO_VAL, "Metal" },
    PROP_RNG_STR2 ("Metal", "Air") },
  { "Approx", PROP_STR, { PROP_NO_VAL, "no" }, PROP_RNG_YESNO },
  PROP_NO_PROP };
struct define_t cpwline::cirdef =
  { "CLIN", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
