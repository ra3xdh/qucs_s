/*
 * rectline.cpp - rectangular waveguide class implementation
 *
 * Copyright (C) 2008 Bastien ROUCARIES <roucaries.bastien@gmail.com>
 * Copyright (C) 2008 Andrea Zonca <andrea.zonca@gmail.com>
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
#include "rectline.h"

/*!\file rectangular.cpp
   A TE10 rectangular waveguide

   References:
   [1] Microwave engineering
       David M. Pozar
       John Wiley & sons
       2d edition
       1998
       isbn 0-471-17096-8

   [2] Foundation for Microwave Engineering
       The {IEEE} press serie on Electromagnetics Wave Theory
       Robert E. Collin
       IEEE Press,
       2d edition
       1992
       isbn 0-7803-6331-1

*/


using namespace qucs;

rectline::rectline () : circuit (2) {
  alpha = beta = fc_low = fc_high = 0.0;
  zl = 0.0;
  type = CIR_RECTANGULAR;
}

void rectline::calcResistivity (const char * const Mat, nr_double_t T) {
  if (!strcmp (Mat, "Copper")) {
    if (T < 7) {
      rho = 2e-11;
    }
    else if (T < 15) {
      rho = 6.66667e-17 * qucs::pow (T, 5.0) - 3.88549e-15 * qucs::pow (T, 4.0) +
	9.82267e-14 * qucs::pow (T, 3.0) - 1.29684e-12 * qucs::pow (T, 2.0) +
	8.68341e-12 * T - 2.72120e-12;
    }
    else if (T < 45) {
      rho = 6.60731e-15 * qucs::pow (T, 3.0) - 1.14812e-13 * qucs::pow (T, 2.0) -
	1.11681e-12 * T + 4.23709e-11;
    }
    else if (T < 100) {
      rho = -6.53059e-15 * qucs::pow (T, 3.0) +	1.73783e-12 * qucs::pow (T, 2.0) -
	8.73888e-11 * T + 1.37016e-9;
    }
    else if (T < 350) {
      rho = 1.00018e-17 * qucs::pow (T, 3.0) - 8.72408e-15 * qucs::pow (T, 2.0) +
	7.06020e-11 * T - 3.51125e-9;
    }
    else {
      rho = 0.000000020628;
    }
    // in ADS iT_K is forced T_Ko Cu_300K:
    //rho = 1.7e-8;
  }
  else if (!strcmp (Mat, "StainlessSteel")) {
    rho = 7.4121e-17 * qucs::pow (T, 4.0) - 5.3504e-14 * qucs::pow (T, 3.0) +
      1.2902e-11 * qucs::pow (T, 2.0) - 2.9186e-10 * T +4.9320e-7;
  }
  else if (!strcmp (Mat, "Gold")) {
    if (T < 20) {
      rho = 0.00000000024;
    }
    else if (T < 65) {
      rho = 2e-12 * qucs::pow (T, 2.0) - 8e-11 * T + 1e-9;
    }
    else if (T < 80) {
      rho = 5e-13 * qucs::pow (T, 3.0) - 1e-10 * qucs::pow (T, 2.0) + 9e-9 * T - 2e-7;
    }
    else if (T < 300) {
      rho = 8e-11 * T - 1e-10;
    }
    else {
      rho = 2.4e-8;
    }
  }
}

/*! Compute propagation constant

    According to [1] table 3.2 p 128
    Wawe number in vacuum is:
    \f[
    k=\omega\sqrt{\mu\varepsilon}=\omega\sqrt{\mu_r \varepsilon_r \mu_0 \varepsilon_0}
    \f]
    Where \f$\omega=2\pi f\f$, \f$f\f$ is the frequency, $\f\mu\f$ the magnetic permeability,
    \f$\varepsilon_r\f$ the permitivity. Using well known formula \f$c^2\mu_0\varepsilon_0=1\f$, we found:
    \f[
    k=\frac{\omega}{c} std::sqrt{\mu_r \varepsilon_r}
    \f]
    In general case \f$k_c\f$ is:
    \f[
    k_c = \sqrt{\left(\frac{m\pi}{a}\right)^2+\left(\frac{n\pi}{b}\right)^2}
    \f]
    Where \f$a\f$ is the wider dimension of the guide and \f$b\f$ the smaller and \f$(n,m)\in\mathbb{N}^2\f$.
    In the TE10 case it simplifies to:
    \f[
    k_c = \sqrt{\left(\frac{\pi}{a}\right)^2} = \frac{\pi}{a}
    \f]
    The propagation constant is:
    \f[
    \beta = \sqrt{k^2 - k_c^2}
    \f]
    Loss could be divised in dielectric and resistive loss.
    Dielectric loss are computed using:
    \f[
    \alpha_d = \frac{k^2 \tan \delta}{2\beta}
    \f]
    Resistive using [1] eq (3.96) p 125 (valid only for TE10 mode)
    \f[
    \alpha_c=\frac{R_s}{a^3 b \beta k Z_0} \left( 2 b \pi^2 + a^3  k^2 \right)
    \f]
    Wave impedance is for TE10:
    \f[
    Z = \frac{k Z_0}{\beta}
    \f]
*/
void rectline::calcPropagation (nr_double_t frequency) {
  nr_double_t er   = getPropertyDouble ("er");
  nr_double_t mur  = getPropertyDouble ("mur");
  nr_double_t tand = getPropertyDouble ("tand");
  nr_double_t a    = getPropertyDouble ("a");
  nr_double_t b    = getPropertyDouble ("b");

  /* wave number */
  nr_double_t k0;
  nr_double_t kc;
  /* dielectric loss */
  nr_double_t ad, ac, rs;

  // check cutoff frequency
  if (frequency >= fc_high) {
    logprint (LOG_ERROR, "WARNING: Operating frequency (%g) outside TE10 "
	      "band (%g <= TE10 <= %g) or outside non propagative mode "
	      "<= %g\n", frequency, fc_low, fc_high, fc_low);
  }
  // calculate wave number
  k0 = (2.0 * pi * frequency * std::sqrt (er * mur)) / C0;
  kc = pi / a;

  // calculate losses only for propagative mode
  if (frequency >= fc_low) {
    // calculate beta
    beta = std::sqrt (sqr (k0) - sqr (kc));

    // dielectric
    ad = (sqr(k0) * tand) / (2.0 * beta);
    // resistive
    rs = std::sqrt (pi * frequency * mur * MU0 * rho);
    ac = rs * (2.0 * b * sqr (pi) + cubic (a) * sqr (k0)) /
      (cubic (a) * b * beta * k0 *  Z0  * std::sqrt(mur/er));
    alpha = (ad + ac);

    // wave impedance
    zl = (k0 * Z0  * std::sqrt(mur/er)) / beta;

  } else {
    /* according to [2] eq 3.207 */
    beta = 0;
    alpha = -std::sqrt (- (sqr (k0) - sqr (kc)));
    // wave impedance
    zl = (k0 * Z0 * std::sqrt(mur/er)) / nr_complex_t (0, -alpha) ;
  }
}

/*! Compute noise parameter */
void rectline::calcNoiseSP (nr_double_t) {
  nr_double_t l = getPropertyDouble ("L");
  if (l < 0) return;
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  matrix s = getMatrixS ();
  matrix e = eye (getSize ());
  setMatrixN (celsius2kelvin (T) / T0 * (e - s * transpose (conj (s))));
}

/*! Check validity of parameter and compute cutoff frequencies
    \note do not check validity of epsr or mur because some research stuff could use epsr < 1 (plasma)
*/
void rectline::initCheck (void) {
  nr_double_t a = getPropertyDouble ("a");
  nr_double_t b = getPropertyDouble ("b");
  nr_double_t epsr = getPropertyDouble ("er");
  nr_double_t mur = getPropertyDouble ("mur");
  // check validity
  if (a < b) {
    logprint (LOG_ERROR, "ERROR: a < b should be a >= b.\n");
  }
  nr_double_t c = std::sqrt (epsr * mur);
  fc_low =  C0 / (2.0 * a * c);
  /* min of second TE mode and first TM mode */
  fc_high = std::min (C0 / (a * c),  C0 / (2.0  * b * c));
  // calculation of resistivity
  rho  = getPropertyDouble ("rho");
  nr_double_t T = getPropertyDouble ("Temp");
  calcResistivity (getPropertyString ("Material"), celsius2kelvin (T));
}

void rectline::saveCharacteristics (nr_complex_t) {
  setCharacteristic ("Zl", real (zl));
}

/*! initialize S-parameters */
void rectline::initSP (void) {
  // allocate S-parameter matrix
  allocMatrixS ();
  initCheck ();
}

/*! Compute S parameter use generic transmission line formulae */
void rectline::calcSP (nr_double_t frequency) {
  nr_double_t l = getPropertyDouble ("L");

  // calculate propagation constants
  calcPropagation (frequency);

  // calculate S-parameters
  nr_complex_t z = zl / z0;
  nr_complex_t y = 1.0 / z;
  nr_complex_t g = nr_complex_t (alpha, beta);
  nr_complex_t n = 2.0 * cosh (g * l) + (z + y) * sinh (g * l);
  nr_complex_t s11 = (z - y) * sinh (g * l) / n;
  nr_complex_t s21 = 2.0 / n;
  setS (NODE_1, NODE_1, s11); setS (NODE_2, NODE_2, s11);
  setS (NODE_1, NODE_2, s21); setS (NODE_2, NODE_1, s21);
}

/* ! Compute DC
     \note below cut off it is an open circuit
*/
void rectline::initDC (void) {
  allocMatrixMNA ();
  // open circuit
  clearY ();
}

/*! init AC */
void rectline::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
  initCheck ();
}

/*! calc propagation using classical transmission line formulae */
void rectline::calcAC (nr_double_t frequency) {
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

/*! Compute noise */
void rectline::calcNoiseAC (nr_double_t) {
  nr_double_t l = getPropertyDouble ("L");
  if (l < 0) return;
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  setMatrixN (4.0 * celsius2kelvin (T) / T0 * real (getMatrixY ()));
}

// properties
PROP_REQ [] = {
  { "a", PROP_REAL, { 2.86e-2, PROP_NO_STR }, PROP_POS_RANGEX },
  { "b", PROP_REAL, { 1.016e-2, PROP_NO_STR }, PROP_POS_RANGEX },
  { "L", PROP_REAL, { 1500e-3, PROP_NO_STR }, PROP_NO_RANGE },
  { "er", PROP_REAL, { 1, PROP_NO_STR }, PROP_RNGII (1, 100) },
  { "mur", PROP_REAL, { 1, PROP_NO_STR }, PROP_RNGII (1, 100) },
  { "tand", PROP_REAL, { 4e-4, PROP_NO_STR }, PROP_POS_RANGE },
  { "rho", PROP_REAL, { 0.022e-6, PROP_NO_STR }, PROP_POS_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  { "Material", PROP_STR, { PROP_NO_VAL, "unspecified" },
    PROP_RNG_STR4 ("unspecified", "Copper", "StainlessSteel", "Gold") },
  PROP_NO_PROP };
struct define_t rectline::cirdef =
  { "RECTLINE", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
