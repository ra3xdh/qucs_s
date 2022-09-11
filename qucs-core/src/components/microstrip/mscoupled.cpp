/*
 * mscoupled.cpp - parallel coupled microstrip lines class implementation
 *
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

#include "component.h"
#include "substrate.h"
#include "msline.h"
#include "mscoupled.h"

using namespace qucs;

mscoupled::mscoupled () : circuit (4) {
  type = CIR_MSCOUPLED;
}

void mscoupled::calcPropagation (nr_double_t frequency) {

  // fetch line properties
  nr_double_t W = getPropertyDouble ("W");
  nr_double_t s = getPropertyDouble ("S");
  const char * SModel = getPropertyString ("Model");
  const char * DModel = getPropertyString ("DispModel");

  // fetch substrate properties
  substrate * subst = getSubstrate ();
  nr_double_t er    = subst->getPropertyDouble ("er");
  nr_double_t h     = subst->getPropertyDouble ("h");
  nr_double_t t     = subst->getPropertyDouble ("t");
  nr_double_t tand  = subst->getPropertyDouble ("tand");
  nr_double_t rho   = subst->getPropertyDouble ("rho");
  nr_double_t D     = subst->getPropertyDouble ("D");

  // quasi-static analysis
  nr_double_t Zle, ErEffe, Zlo, ErEffo;
  analysQuasiStatic (W, h, s, t, er, SModel, Zle, Zlo, ErEffe, ErEffo);

  // analyse dispersion of Zl and Er
  nr_double_t ZleFreq, ErEffeFreq, ZloFreq, ErEffoFreq;
  analyseDispersion (W, h, s, er, Zle, Zlo, ErEffe, ErEffo, frequency, DModel,
		     ZleFreq, ZloFreq, ErEffeFreq, ErEffoFreq);

  // analyse losses of line
  nr_double_t ace, aco, ade, ado;
  msline::analyseLoss (W, t, er, rho, D, tand, Zle, Zlo, ErEffe,
		       frequency, "Hammerstad", ace, ade);
  msline::analyseLoss (W, t, er, rho, D, tand, Zlo, Zle, ErEffo,
		       frequency, "Hammerstad", aco, ado);

  // compute propagation constants for even and odd mode
  nr_double_t k0 = 2 * pi * frequency / C0;
  ae = ace + ade;
  ao = aco + ado;
  be = qucs::sqrt (ErEffeFreq) * k0;
  bo = qucs::sqrt (ErEffoFreq) * k0;
  ze = ZleFreq;
  zo = ZloFreq;
  ee = ErEffeFreq;
  eo = ErEffoFreq;
}

void mscoupled::saveCharacteristics (nr_double_t) {
  setCharacteristic ("ZlEven", ze);
  setCharacteristic ("ErEven", ee);
  setCharacteristic ("ZlOdd", zo);
  setCharacteristic ("ErOdd", eo);
}

void mscoupled::calcSP (nr_double_t frequency) {
  // fetch line properties
  nr_double_t l = getPropertyDouble ("L");

  // compute propagation constants for even and odd mode
  calcPropagation (frequency);
  nr_complex_t ge = nr_complex_t (ae, be);
  nr_complex_t go = nr_complex_t (ao, bo);

  // compute abbreviations
  nr_complex_t Ee, Eo, De, Do, Xe, Xo, Ye, Yo;
  Ee = (sqr (ze) + sqr (z0)) * qucs::sinh (ge * l);
  Eo = (sqr (zo) + sqr (z0)) * qucs::sinh (go * l);
  De = 2 * ze * z0 * cosh (ge * l) + Ee;
  Do = 2 * zo * z0 * cosh (go * l) + Eo;
  Xe = (sqr (ze) - sqr (z0)) * qucs::sinh (ge * l) / 2.0 / De;
  Xo = (sqr (zo) - sqr (z0)) * qucs::sinh (go * l) / 2.0 / Do;
  Ye = ze * z0 / De;
  Yo = zo * z0 / Do;

  // reflexion coefficients
  setS (NODE_1, NODE_1, Xe + Xo); setS (NODE_2, NODE_2, Xe + Xo);
  setS (NODE_3, NODE_3, Xe + Xo); setS (NODE_4, NODE_4, Xe + Xo);
  // through paths
  setS (NODE_1, NODE_2, Ye + Yo); setS (NODE_2, NODE_1, Ye + Yo);
  setS (NODE_3, NODE_4, Ye + Yo); setS (NODE_4, NODE_3, Ye + Yo);
  // coupled paths
  setS (NODE_1, NODE_4, Xe - Xo); setS (NODE_4, NODE_1, Xe - Xo);
  setS (NODE_2, NODE_3, Xe - Xo); setS (NODE_3, NODE_2, Xe - Xo);
  // isolated paths
  setS (NODE_1, NODE_3, Ye - Yo); setS (NODE_3, NODE_1, Ye - Yo);
  setS (NODE_2, NODE_4, Ye - Yo); setS (NODE_4, NODE_2, Ye - Yo);
}

void mscoupled::calcNoiseSP (nr_double_t) {
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  matrix s = getMatrixS ();
  matrix e = eye (getSize ());
  setMatrixN (celsius2kelvin (T) / T0 * (e - s * transpose (conj (s))));
}

/* The function calculates the quasi-static dielectric constants and
   characteristic impedances for the even and odd mode based upon the
   given line and substrate properties for parallel coupled microstrip
   lines. */
void mscoupled::analysQuasiStatic (nr_double_t W, nr_double_t h, nr_double_t s,
				   nr_double_t t, nr_double_t er,
				   const char * const SModel, nr_double_t& Zle,
				   nr_double_t& Zlo, nr_double_t& ErEffe,
				   nr_double_t& ErEffo) {
  // initialize default return values
  ErEffe = ErEffo = er;
  Zlo = 42.2; Zle = 55.7;

  // normalized width and gap
  nr_double_t u = W / h;
  nr_double_t g = s / h;

  // HAMMERSTAD and JENSEN
  if (!strcmp (SModel, "Hammerstad")) {
    nr_double_t Zl1, Fe, Fo, a, b, fo, Mu, Alpha, Beta, ErEff;
    nr_double_t Pe, Po, r, fo1, q, p, n, Psi, Phi, m, Theta;

    // modifying equations for even mode
    m = 0.2175 + qucs::pow (4.113 + qucs::pow (20.36 / g, 6.), -0.251) +
      qucs::log (qucs::pow (g, 10.) / (1 + qucs::pow (g / 13.8, 10.))) / 323;
    Alpha = 0.5 * qucs::exp (-g);
    Psi = 1 + g / 1.45 + qucs::pow (g, 2.09) / 3.95;
    Phi = 0.8645 * qucs::pow (u, 0.172);
    Pe = Phi / (Psi * (Alpha * qucs::pow (u, m) + (1 - Alpha) * qucs::pow (u, -m)));
    // TODO: is this ... Psi * (Alpha ... or ... Psi / (Alpha ... ?

    // modifying equations for odd mode
    n = (1 / 17.7 + qucs::exp (-6.424 - 0.76 * qucs::log (g) - qucs::pow (g / 0.23, 5.))) *
      qucs::log ((10 + 68.3 * sqr (g)) / (1 + 32.5 * qucs::pow (g, 3.093)));
    Beta = 0.2306 + qucs::log (qucs::pow (g, 10.) / (1 + qucs::pow (g / 3.73, 10.))) / 301.8 +
      qucs::log (1 + 0.646 * qucs::pow (g, 1.175)) / 5.3;
    Theta = 1.729 + 1.175 * qucs::log (1 + 0.627 / (g + 0.327 * qucs::pow (g, 2.17)));
    Po = Pe - Theta / Psi * qucs::exp (Beta * qucs::pow (u, -n) * qucs::log (u));

    // further modifying equations
    r = 1 + 0.15 * (1 - qucs::exp (1 - sqr (er - 1) / 8.2) / (1 + qucs::pow (g, -6.)));
    fo1 = 1 - qucs::exp (-0.179 * qucs::pow (g, 0.15) -
		   0.328 * qucs::pow (g, r) / qucs::log (euler + qucs::pow (g / 7, 2.8)));
    q = qucs::exp (-1.366 - g);
    p = qucs::exp (-0.745 * qucs::pow (g, 0.295)) / qucs::cosh (qucs::pow (g, 0.68));
    fo = fo1 * qucs::exp (p * qucs::log (u) + q * qucs::sin (pi * qucs::log10 (u)));

    Mu = g * qucs::exp (-g) + u * (20 + sqr (g)) / (10 + sqr (g));
    msline::Hammerstad_ab (Mu, er, a, b);
    Fe = qucs::pow (1 + 10 / Mu, -a * b);
    msline::Hammerstad_ab (u, er, a, b);
    Fo = fo * qucs::pow (1 + 10 / u, -a * b);

    // finally compute effective dielectric constants and impedances
    ErEffe = (er + 1) / 2 + (er - 1) / 2 * Fe;
    ErEffo = (er + 1) / 2 + (er - 1) / 2 * Fo;

    msline::Hammerstad_er (u, er, a, b, ErEff);  // single microstrip

    // first variant
    Zl1 = Z0 / (u + 1.98 * qucs::pow (u, 0.172));
    Zl1 /= qucs::sqrt (ErEff);

    // second variant
    msline::Hammerstad_zl (u, Zl1);
    Zl1 /= qucs::sqrt (ErEff);

    Zle = Zl1 / (1 - Zl1 * Pe / Z0);
    Zlo = Zl1 / (1 - Zl1 * Po / Z0);
  }
  // KIRSCHNING and JANSEN
  else if (!strcmp (SModel, "Kirschning")) {
    nr_double_t a, b, ae, be, ao, bo, v, co, d, ErEff, Zl1;
    nr_double_t q1, q2, q3, q4, q5, q6, q7, q8, q9, q10;

    // consider effect of finite strip thickness (JANSEN only)
    nr_double_t ue = u;
    nr_double_t uo = u;
    if (t != 0 && s > 10 * (2 * t)) {
      nr_double_t dW = 0;
      // SCHNEIDER, referred by JANSEN
      if (u >= one_over_pi / 2 && one_over_pi / 2 > 2 * t / h)
	dW = t * (1 + qucs::log (2 * h / t)) / pi;
      else if (W > 2 * t)
	dW = t * (1 + qucs::log (4 * pi * W / t)) / pi;
      // JANSEN
      nr_double_t dt = 2 * t * h / s / er;
      nr_double_t We = W + dW * (1 - 0.5 * qucs::exp (-0.69 * dW / dt));
      nr_double_t Wo = We + dt;
      ue = We / h;
      uo = Wo / h;
    }

    // even relative dielectric constant
    v = ue * (20 + sqr (g)) / (10 + sqr (g)) + g * qucs::exp (-g);
    msline::Hammerstad_ab (v, er, ae, be);
    msline::Hammerstad_er (v, er, ae, be, ErEffe);

    // odd relative dielectric constant
    msline::Hammerstad_ab (uo, er, a, b);
    msline::Hammerstad_er (uo, er, a, b, ErEff);
    d = 0.593 + 0.694 * qucs::exp (-0.562 * uo);
    bo = 0.747 * er / (0.15 + er);
    co = bo - (bo - 0.207) * qucs::exp (-0.414 * uo);
    ao = 0.7287 * (ErEff - (er + 1) / 2) * (1 - qucs::exp (-0.179 * uo));
    ErEffo = ((er + 1) / 2 + ao - ErEff) * qucs::exp (-co * qucs::pow (g, d)) + ErEff;

    // characteristic impedance of single line
    msline::Hammerstad_zl (u, Zl1);
    Zl1 /= qucs::sqrt (ErEff);

    // even characteristic impedance
    q1 = 0.8695 * qucs::pow (ue, 0.194);
    q2 = 1 + 0.7519 * g + 0.189 * qucs::pow (g, 2.31);
    q3 = 0.1975 + qucs::pow (16.6 + qucs::pow (8.4 / g, 6.), -0.387) +
      qucs::log (qucs::pow (g, 10.) / (1 + qucs::pow (g / 3.4, 10.))) / 241;
    q4 = q1 / q2 * 2 /
      (qucs::exp (-g) * qucs::pow (ue, q3) + (2 - qucs::exp (-g)) * qucs::pow (ue, -q3));
    Zle = qucs::sqrt (ErEff / ErEffe) * Zl1 / (1 - Zl1 * qucs::sqrt (ErEff) * q4 / Z0);

    // odd characteristic impedance
    q5 = 1.794 + 1.14 * qucs::log (1 + 0.638 / (g + 0.517 * qucs::pow (g, 2.43)));
    q6 = 0.2305 + qucs::log (qucs::pow (g, 10.) / (1 + qucs::pow (g / 5.8, 10.))) / 281.3 +
      qucs::log (1 + 0.598 * qucs::pow (g, 1.154)) / 5.1;
    q7 = (10 + 190 * sqr (g)) / (1 + 82.3 * cubic (g));
    q8 = qucs::exp (-6.5 - 0.95 * qucs::log (g) - qucs::pow (g / 0.15, 5.));
    q9 = qucs::log (q7) * (q8 + 1 / 16.5);
    q10 = (q2 * q4 - q5 * qucs::exp (qucs::log (uo) * q6 * qucs::pow (uo, -q9))) / q2;
    Zlo = qucs::sqrt (ErEff / ErEffo) * Zl1 / (1 - Zl1 * qucs::sqrt (ErEff) * q10 / Z0);
  }
}

/* The function computes the dispersion effects on the dielectric
   constants and characteristic impedances for the even and odd mode
   of parallel coupled microstrip lines. */
void mscoupled::analyseDispersion (nr_double_t W, nr_double_t h, nr_double_t s,
				   nr_double_t er, nr_double_t Zle,
				   nr_double_t Zlo, nr_double_t ErEffe,
				   nr_double_t ErEffo, nr_double_t frequency,
				   const char * const DModel, nr_double_t& ZleFreq,
				   nr_double_t& ZloFreq,
				   nr_double_t& ErEffeFreq,
				   nr_double_t& ErEffoFreq) {

  // initialize default return values
  ZleFreq = Zle;
  ErEffeFreq = ErEffe;
  ZloFreq = Zlo;
  ErEffoFreq = ErEffo;

  // normalized width and gap
  nr_double_t u = W / h;
  nr_double_t g = s / h;

  // GETSINGER
  if (!strcmp (DModel, "Getsinger")) {
    // even mode dispersion
    msline::Getsinger_disp (h, er, ErEffe, Zle / 2,
			    frequency, ErEffeFreq, ZleFreq);
    ZleFreq *= 2;
    // odd mode dispersion
    msline::Getsinger_disp (h, er, ErEffo, Zlo * 2,
			    frequency, ErEffoFreq, ZloFreq);
    ZloFreq /= 2;
  }
  // KIRSCHNING and JANSEN
  else if (!strcmp (DModel, "Kirschning")) {
    nr_double_t p1, p2, p3, p4, p5, p6, p7, Fe;
    nr_double_t fn = frequency * h * 1e-6;

    // even relative dielectric constant dispersion
    p1 = 0.27488 * (0.6315 + 0.525 / qucs::pow (1 + 0.0157 * fn, 20.)) * u -
      0.065683 * qucs::exp (-8.7513 * u);
    p2 = 0.33622 * (1 - qucs::exp (-0.03442 * er));
    p3 = 0.0363 * qucs::exp (-4.6 * u) * (1 - qucs::exp (- qucs::pow (fn / 38.7, 4.97)));
    p4 = 1 + 2.751 * (1 - qucs::exp (- qucs::pow (er / 15.916, 8.)));
    p5 = 0.334 * qucs::exp (-3.3 * cubic (er / 15)) + 0.746;
    p6 = p5 * qucs::exp (- qucs::pow (fn / 18, 0.368));
    p7 = 1 + 4.069 * p6 * qucs::pow (g, 0.479) *
      qucs::exp (-1.347 * qucs::pow (g, 0.595) - 0.17 * qucs::pow (g, 2.5));
    Fe = p1 * p2 * qucs::pow ((p3 * p4 + 0.1844 * p7) * fn, 1.5763);
    ErEffeFreq = er - (er - ErEffe) / (1 + Fe);

    // odd relative dielectric constant dispersion
    nr_double_t p8, p9, p10, p11, p12, p13, p14, p15, Fo;
    p8 = 0.7168 * (1 + 1.076 / (1 + 0.0576 * (er - 1)));
    p9 = p8 - 0.7913 * (1 - qucs::exp (- qucs::pow (fn / 20, 1.424))) *
      qucs::atan (2.481 * qucs::pow (er / 8, 0.946));
    p10 = 0.242 * qucs::pow (er - 1, 0.55);
    p11 = 0.6366 * (qucs::exp (-0.3401 * fn) - 1) *
      qucs::atan (1.263 * qucs::pow (u / 3, 1.629));
    p12 = p9 + (1 - p9) / (1 + 1.183 * qucs::pow (u, 1.376));
    p13 = 1.695 * p10 / (0.414 + 1.605 * p10);
    p14 = 0.8928 + 0.1072 * (1 - qucs::exp (-0.42 * qucs::pow (fn / 20, 3.215)));
    p15 = fabs (1 - 0.8928 * (1 + p11) *
		qucs::exp (-p13 * qucs::pow (g, 1.092)) * p12 / p14);
    Fo = p1 * p2 * qucs::pow ((p3 * p4 + 0.1844) * fn * p15, 1.5763);
    ErEffoFreq = er - (er - ErEffo) / (1 + Fo);

    // dispersion of even characteristic impedance
    nr_double_t t, q11, q12, q13, q14, q15, q16, q17, q18, q19, q20, q21;
    q11 = 0.893 * (1 - 0.3 / (1 + 0.7 * (er - 1)));
    t = qucs::pow (fn / 20, 4.91);
    q12 = 2.121 * t / (1 + q11 * t) * qucs::exp (-2.87 * g) * qucs::pow (g, 0.902);
    q13 = 1 + 0.038 * qucs::pow (er / 8, 5.1);
    t = quadr (er / 15);
    q14 = 1 + 1.203 * t / (1 + t);
    q15 = 1.887 * qucs::exp (-1.5 * qucs::pow (g, 0.84)) * qucs::pow (g, q14) /
      (1 + 0.41 * qucs::pow (fn / 15, 3.) *
       qucs::pow (u, 2 / q13) / (0.125 + qucs::pow (u, 1.626 / q13)));
    q16 = q15 * (1 + 9 / (1 + 0.403 * sqr (er - 1)));
    q17 = 0.394 * (1 - qucs::exp (-1.47 * qucs::pow (u / 7, 0.672))) *
      (1 - qucs::exp (-4.25 * qucs::pow (fn / 20, 1.87)));
    q18 = 0.61 * (1 - qucs::exp (-2.31 * qucs::pow (u / 8, 1.593))) /
      (1 + 6.544 * qucs::pow (g, 4.17));
    q19 = 0.21 * quadr (g) / (1 + 0.18 * qucs::pow (g, 4.9)) / (1 + 0.1 * sqr (u)) /
      (1 + qucs::pow (fn / 24, 3.));
    q20 = q19 * (0.09 + 1 / (1 + 0.1 * qucs::pow (er - 1, 2.7)));
    t = qucs::pow (u, 2.5);
    q21 = fabs (1 - 42.54 * qucs::pow (g, 0.133) * qucs::exp (-0.812 * g) * t /
		(1 + 0.033 * t));

    nr_double_t re, qe, pe, de, Ce, q0, ZlFreq, ErEffFreq;
    msline::Kirschning_er (u, fn, er, ErEffe, ErEffFreq);
    msline::Kirschning_zl (u, fn, er, ErEffe, ErEffFreq, Zle, q0, ZlFreq);
    re = qucs::pow (fn / 28.843, 12.);
    qe = 0.016 + qucs::pow (0.0514 * er * q21, 4.524);
    pe = 4.766 * qucs::exp (-3.228 * qucs::pow (u, 0.641));
    t = qucs::pow (er - 1, 6.);
    de = 5.086 * qe * re / (0.3838 + 0.386 * qe) *
      qucs::exp (-22.2 * qucs::pow (u, 1.92)) / (1 + 1.2992 * re) * t / (1 + 10 * t);
    Ce = 1 + 1.275 * (1 - qucs::exp (-0.004625 * pe * qucs::pow (er, 1.674) *
	 qucs::pow (fn / 18.365, 2.745))) - q12 + q16 - q17 + q18 + q20;
    ZleFreq = Zle * qucs::pow ((0.9408 * qucs::pow (ErEffFreq, Ce) - 0.9603) /
			 ((0.9408 - de) * qucs::pow (ErEffe, Ce) - 0.9603), q0);

    // dispersion of odd characteristic impedance
    nr_double_t q22, q23, q24, q25, q26, q27, q28, q29;
    msline::Kirschning_er (u, fn, er, ErEffo, ErEffFreq);
    msline::Kirschning_zl (u, fn, er, ErEffo, ErEffFreq, Zlo, q0, ZlFreq);
    q29 = 15.16 / (1 + 0.196 * sqr (er - 1));
    t = sqr (er - 1);
    q25 = 0.3 * sqr (fn) / (10 + sqr (fn)) * (1 + 2.333 * t / (5 + t));
    t = qucs::pow ((er - 1) / 13, 12.);
    q26 = 30 - 22.2 * t / (1 + 3 * t) - q29;
    t = qucs::pow (er - 1, 1.5);
    q27 = 0.4 * qucs::pow (g, 0.84) * (1 + 2.5 * t / (5 + t));
    t = qucs::pow (er - 1, 3.);
    q28 = 0.149 * t / (94.5 + 0.038 * t);
    q22 = 0.925 * qucs::pow (fn / q26, 1.536) / (1 + 0.3 * qucs::pow (fn / 30, 1.536));
    q23 = 1 + 0.005 * fn * q27 / (1 + 0.812 * qucs::pow (fn / 15, 1.9)) /
      (1 + 0.025 * sqr (u));
    t = qucs::pow (u, 0.894);
    q24 = 2.506 * q28 * t / (3.575 + t) *
      qucs::pow ((1 + 1.3 * u) * fn / 99.25, 4.29);
    ZloFreq = ZlFreq + (Zlo * qucs::pow (ErEffoFreq / ErEffo, q22) - ZlFreq * q23) /
      (1 + q24 + qucs::pow (0.46 * g, 2.2) * q25);

  }
}

void mscoupled::initDC (void) {
  nr_double_t l     = getPropertyDouble ("L");
  nr_double_t W     = getPropertyDouble ("W");
  substrate * subst = getSubstrate ();
  nr_double_t t     = subst->getPropertyDouble ("t");
  nr_double_t rho   = subst->getPropertyDouble ("rho");

  if (t != 0.0 && rho != 0.0) {
    // tiny resistances
    nr_double_t g = t * W / rho / l;
    setVoltageSources (0);
    allocMatrixMNA ();
    setY (NODE_1, NODE_1, +g); setY (NODE_2, NODE_2, +g);
    setY (NODE_1, NODE_2, -g); setY (NODE_2, NODE_1, -g);
    setY (NODE_3, NODE_3, +g); setY (NODE_4, NODE_4, +g);
    setY (NODE_3, NODE_4, -g); setY (NODE_4, NODE_3, -g);
  }
  else {
    // DC shorts (voltage sources V = 0 volts)
    setVoltageSources (2);
    setInternalVoltageSource (1);
    allocMatrixMNA ();
    clearY ();
    voltageSource (VSRC_1, NODE_1, NODE_2);
    voltageSource (VSRC_2, NODE_3, NODE_4);
    setD (VSRC_1, VSRC_2, 0.0); setD (VSRC_2, VSRC_1, 0.0);
  }
}

void mscoupled::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
}

void mscoupled::calcAC (nr_double_t frequency) {
  // fetch line properties
  nr_double_t l = getPropertyDouble ("L");

  // compute propagation constants for even and odd mode
  calcPropagation (frequency);
  nr_complex_t ge = nr_complex_t (ae, be);
  nr_complex_t go = nr_complex_t (ao, bo);

  // compute abbreviations
  nr_complex_t De, Do, y1, y2, y3, y4;
  De = 0.5 / (ze * qucs::sinh (ge * l));
  Do = 0.5 / (zo * qucs::sinh (go * l));
  y2 = -De - Do;
  y3 = -De + Do;
  De *= cosh (ge * l);
  Do *= cosh (go * l);
  y1 = De + Do;
  y4 = De - Do;

  // store Y-parameters
  setY (NODE_1, NODE_1, y1); setY (NODE_2, NODE_2, y1);
  setY (NODE_3, NODE_3, y1); setY (NODE_4, NODE_4, y1);
  setY (NODE_1, NODE_2, y2); setY (NODE_2, NODE_1, y2);
  setY (NODE_3, NODE_4, y2); setY (NODE_4, NODE_3, y2);
  setY (NODE_1, NODE_3, y3); setY (NODE_2, NODE_4, y3);
  setY (NODE_3, NODE_1, y3); setY (NODE_4, NODE_2, y3);
  setY (NODE_1, NODE_4, y4); setY (NODE_2, NODE_3, y4);
  setY (NODE_3, NODE_2, y4); setY (NODE_4, NODE_1, y4);
}

void mscoupled::calcNoiseAC (nr_double_t) {
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  setMatrixN (4 * celsius2kelvin (T) / T0 * real (getMatrixY ()));
}

// properties
PROP_REQ [] = {
  { "W", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "L", PROP_REAL, { 10e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "S", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "Subst", PROP_STR, { PROP_NO_VAL, "Subst1" }, PROP_NO_RANGE },
  { "Model", PROP_STR, { PROP_NO_VAL, "Kirschning" },
    PROP_RNG_STR2 ("Kirschning", "Hammerstad") },
  { "DispModel", PROP_STR, { PROP_NO_VAL, "Kirschning" },
    PROP_RNG_STR2 ("Kirschning", "Getsinger") },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };
struct define_t mscoupled::cirdef =
  { "MCOUPLED", 4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
