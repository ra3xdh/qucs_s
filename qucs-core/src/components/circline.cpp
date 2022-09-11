/*
 * circline.cpp - Circular waveguide TE11 mode implementation
 *
 * copyright (C) 2015 Andres Martinez-Mera <andresmartinezmera@gmail.com>
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
 *
 */
#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "component.h"
#include "circline.h"

/*
References:
Microwave Engineering. David M Pozar. John Wiley and Sons. 4th Edition. Pg 121-130
Foundation for Microwave Engineering. IEEE Press. Robert E. Collin. 2d edition. Pg 194-198
       1992
*/


using namespace qucs;

circline::circline () : circuit (2) {
  alpha = beta = fc_low = fc_high = 0.0;
  zl = 0.0;
  type = CIR_CIRCULAR;
}

void circline::calcResistivity (const char * const Mat, nr_double_t T) {
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
    k_c = p_{nm}/a
    \f]
    where p_{nm} are given in Table 3.3 (Pg 123) and in table 3.4 (126) for the TE and TM modes, respectively.
    The propagation constant is:
    \f[
    \beta = \sqrt{k^2 - k_c^2}
    \f]
    Loss could be divised in dielectric and resistive loss.
    Dielectric loss are computed using:
    \f[
    \alpha_d = \frac{k^2 \tan \delta}{2\beta}
    \f]
    The attenuation constant for he TE11
    \f[
    \alpha_c= \frac{rs}{ak\eta\beta} \left( k_c^2 + \frac{k^2}{p'_{11}}\right)
    \f]
    Wave impedance is for TE11:
    \f[
    Z = \frac{k Z_0}{\beta}
    where Z_0 = \sqrt{\frac{\mu_0 \mu_r}{\epsilon_0 \epsilon_r}}
    \f]
*/
void circline::calcPropagation (nr_double_t frequency) {
  nr_double_t er   = getPropertyDouble ("er");//Dielectric permittivity
  nr_double_t mur  = getPropertyDouble ("mur");//Dielectric permeability
  nr_double_t tand = getPropertyDouble ("tand");//Dielectric loss
  nr_double_t a    = getPropertyDouble ("a");//Radius

  /* wave number */
  nr_double_t k0;
  nr_double_t kc;

  nr_double_t ad, ac, rs;

  // check cutoff frequency

  if (frequency >= fc_high) {
    logprint (LOG_ERROR, "WARNING: Operating frequency (%g) outside TE11 "
	      "band (%g <= TE11 <= %g) or outside non propagative mode "
	      "<= %g\n", frequency, fc_low, fc_high, fc_low);
  }
  // calculate wave number
  k0 = (2.0 * pi * frequency * std::sqrt (E0 * er * MU0*mur));
  kc = pe11 / a;//TE11 mode

  // calculate losses only for propagative mode
  if (frequency >= fc_low) {
    // calculate beta
    beta = std::sqrt (sqr (k0) - sqr (kc));//Propagation constant

    // dielectric
    ad = (sqr(k0) * tand) / (2.0 * beta);
    // resistive
    rs = std::sqrt (pi * frequency * mur * MU0 * rho);
    ac = (rs/(a*k0*Z0*std::sqrt(mur/er)*beta))*(kc*kc + (k0*k0)/(pe11*pe11 - 1));//Conductor attenuation for the TE11 mode
    alpha = (ad + ac);

    // wave impedance for TE11
    zl = (k0 * Z0* std::sqrt(mur/er)) / beta;

  } else {//Evanescent modes
    beta = 0;
    alpha = std::sqrt (- (sqr (k0) - sqr (kc)));
    // wave impedance
    zl = (k0 * Z0 * std::sqrt(mur/er)) / nr_complex_t (0, alpha) ;
    //Evanescent modes have an imaginary propagation constant, so they do not carry energy (they vanish). Eq. 203 Collins
  }
}

/*! Compute noise parameter */
void circline::calcNoiseSP (nr_double_t) {
  nr_double_t l = getPropertyDouble ("L");//Length
  if (l < 0) return;
  // calculate noise using Bosma's theore
  nr_double_t T = getPropertyDouble ("Temp");
  matrix s = getMatrixS ();
  matrix e = eye (getSize ());
  setMatrixN (celsius2kelvin (T) / T0 * (e - s * transpose (conj (s))));
}

/*! Check validity of parameter and compute cutoff frequencies
    \note do not check validity of epsr or mur because some research stuff could use epsr < 1 (plasma)
*/
void circline::initCheck (void) {
  nr_double_t a = getPropertyDouble ("a");//Radius
  nr_double_t epsr = getPropertyDouble ("er");//Relative permittivity
  nr_double_t mur = getPropertyDouble ("mur");//Relative permeability

  if (a < 0) logprint (LOG_ERROR, "ERROR: The radius, 'a', must be positive!.\n");// checking if the radius is > 0
  fc_low =  (pe11*C0)/(2*pi*a*std::sqrt(mur*epsr));// Below this frequency, the modes do not propagate (evanescent modes)
  fc_high = (pm01*C0)/(2*pi*a*std::sqrt(mur*epsr));//In most cases, the circular waveguide operation is limited to TE11.
                                                   //So, it is of interest to determine the range in which only TE11 is propagating.
                                                   //When using circular waveguides, the upper cutoff frequency is given by the TM01 mode
  rho  = getPropertyDouble ("rho");//Resitivity
  nr_double_t T = getPropertyDouble ("Temp");//Temperature
  calcResistivity (getPropertyString ("Material"), celsius2kelvin (T));//Temperature calculation
}

void circline::saveCharacteristics (nr_complex_t) {
  setCharacteristic ("Zl", real (zl));
}

/*! initialize S-parameters */
void circline::initSP (void) {
  // allocate S-parameter matrix
  allocMatrixS ();
  initCheck ();
}

/*! Compute S parameter use generic transmission line formulae */
void circline::calcSP (nr_double_t frequency) {
  nr_double_t l = getPropertyDouble ("L");

  // calculate propagation constants
  calcPropagation (frequency);

  // calculate S-parameters Reference: http://qucs.sourceforge.net/tech/node61.html
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
void circline::initDC (void) {
  allocMatrixMNA ();
  // open circuit
  clearY ();
}

/*! init AC */
void circline::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
  initCheck ();
}

/*! calc propagation using classical transmission line formulae */
void circline::calcAC (nr_double_t frequency) {
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
void circline::calcNoiseAC (nr_double_t) {
  nr_double_t l = getPropertyDouble ("L");
  if (l < 0) return;
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  setMatrixN (4.0 * celsius2kelvin (T) / T0 * real (getMatrixY ()));
}

// properties
PROP_REQ [] = {
  { "a", PROP_REAL, { 2.86e-2, PROP_NO_STR }, PROP_POS_RANGEX },
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
struct define_t circline::cirdef =
  { "CIRCLINE", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
