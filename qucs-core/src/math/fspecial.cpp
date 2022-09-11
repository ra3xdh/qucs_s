/*
 * fspecial.cpp - special functions implementation
 *
 * Copyright (C) 2006, 2008 Stefan Jahn <stefan@lkcc.org>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <float.h>
#include <algorithm>

#include "compat.h"
#include "constants.h"
#include "fspecial.h"

#include <limits>

using qucs::pi_over_2;
using qucs::sqrt_pi;
using qucs::sqrt2;

/* The function computes the complete elliptic integral of first kind
   K() and the second kind E() using the arithmetic-geometric mean
   algorithm (AGM) by Abramowitz and Stegun. */
void fspecial::ellip_ke (nr_double_t arg, nr_double_t &k, nr_double_t &e) {
  int iMax = 16;
  if (arg == 1.0) {
    k = std::numeric_limits<nr_double_t>::infinity();
    e = 0;
  }
  else if (std::isinf (arg) && arg < 0) {
    k = 0;
    e = std::numeric_limits<nr_double_t>::infinity();
  }
  else {
    nr_double_t a, b, c, f, s, fk = 1, fe = 1, t, da = arg;
    int i;
    if (arg < 0) {
      fk = 1 / sqrt (1 - arg);
      fe = sqrt (1 - arg);
      da = -arg / (1 - arg);
    }
    a = 1;
    b = sqrt (1 - da);
    c = sqrt (da);
    f = 0.5;
    s = f * c * c;
    for (i = 0; i < iMax; i++) {
      t = (a + b) / 2;
      c = (a - b) / 2;
      b = sqrt (a * b);
      a = t;
      f *= 2;
      s += f * c * c;
      if (c / a < std::numeric_limits<nr_double_t>::epsilon()) break;
    }
    if (i >= iMax) {
      k = 0; e = 0;
    }
    else {
      k = pi_over_2 / a;
      e = pi_over_2 * (1 - s) / a;
      if (arg < 0) {
	k *= fk;
	e *= fe;
      }
    }
  }
}

const nr_double_t SN_ACC = 1e-5;	// Accuracy of sn(x) is SN_ACC^2
const nr_double_t K_ERR  = 1e-8;	// Accuracy of K(k)

// Computes Carlson's elliptic integral of the first kind.
nr_double_t fspecial::ellip_rf (nr_double_t x, nr_double_t y, nr_double_t z) {
  nr_double_t al, av, dx, dy, dz, e2, e3;
  nr_double_t sx, sy, sz, xt, yt, zt;

  // constants
  const nr_double_t c1 = 1.0 / 24.0;
  const nr_double_t c2 = 0.1;
  const nr_double_t c3 = 3.0 / 44.0;
  const nr_double_t c4 = 1.0 / 14.0;

  xt = x;
  yt = y;
  zt = z;
  do {
    sx = sqrt (xt);
    sy = sqrt (yt);
    sz = sqrt (zt);
    al = sx * (sy + sz) + sy * sz;
    xt = 0.25 * (xt + al);
    yt = 0.25 * (yt + al);
    zt = 0.25 * (zt + al);
    av = (xt + yt + zt) / 3.0;
    dx = (av - xt) / av;
    dy = (av - yt) / av;
    dz = (av - zt) / av;
  }
  while (std::max (std::max (fabs (dx), fabs (dy)), fabs (dz)) > K_ERR);

  e2 = dx * dy - dz * dz;
  e3 = dx * dy * dz;
  return (1 + (c1 * e2 - c2 - c3 * e3) * e2 + c4 * e3) / sqrt (av);
}

// Compute the Jacobian elliptic functions sn (u,k), cn (u,k) and dn (u,k).
nr_double_t fspecial::ellip_sncndn (nr_double_t u, nr_double_t k,
		      nr_double_t& sn, nr_double_t& cn, nr_double_t& dn) {
  nr_double_t a, b, c, d;
  nr_double_t fn[14], en[14];
  int l;
  bool bo;

  d = 1 - k;
  if (k) {
    bo = (k < 0);
    if (bo) {
      k /= -1 / d;
      u *= (d = sqrt (d));
    }
    a = 1;
    dn = 1;
    for (int i = 1; i < 14; i++) {
      l = i;
      fn[i] = a;
      en[i] = (k = sqrt (k));
      c = 0.5 * (a + k);
      if (fabs (a - k) <= SN_ACC * a)
	break;
      k *= a;
      a = c;
    }
    u *= c;
    sn = sin (u);
    cn = cos (u);
    if (sn) {
      a = cn / sn;
      c *= a;
      for (int ii = l; ii > 0; ii--) {
	b = fn[ii];
	a *= c;
	c *= dn;
	dn = (en[ii] + a) / (b + a);
	a = c / b;
      }
      a = 1 / sqrt (c * c + 1);
      sn = (sn >= 0 ? a : -a);
      cn = sn * c;
    }
    if (bo) {
      a = dn;
      dn = cn;
      cn = a;
      sn /= d;
    }
  }
  else {
    cn = 1 / cosh (u);
    dn = cn;
    sn = tanh (u);
  }
  return sn;
}

/* data for a Chebyshev series over a given interval */
struct cheb_series_t {
  nr_double_t * c;   /* coefficients                */
  int order;         /* order of expansion          */
  nr_double_t a;     /* lower interval point        */
  nr_double_t b;     /* upper interval point        */
};
typedef struct cheb_series_t cheb_series;

static nr_double_t cheb_eval (const cheb_series * cs, const nr_double_t x) {
  nr_double_t d  = 0.0;
  nr_double_t dd = 0.0;
  nr_double_t y  = (2.0 * x - cs->a - cs->b) / (cs->b - cs->a);
  nr_double_t y2 = 2.0 * y;
  for (int i = cs->order; i >= 1; i--) {
    nr_double_t t = d;
    d = y2 * d - dd + cs->c[i];
    dd = t;
  }
  d = y * d - dd + 0.5 * cs->c[0];
  return d;
}

#if !defined (HAVE_ERF) || !defined (HAVE_ERFC)

/* Chebyshev fit for erfc ((t+1)/2), -1 < t < 1 */
static nr_double_t erfc_xlt1_data[20] = {
  1.06073416421769980345174155056,
 -0.42582445804381043569204735291,
  0.04955262679620434040357683080,
  0.00449293488768382749558001242,
 -0.00129194104658496953494224761,
 -0.00001836389292149396270416979,
  0.00002211114704099526291538556,
 -5.23337485234257134673693179020e-7,
 -2.78184788833537885382530989578e-7,
  1.41158092748813114560316684249e-8,
  2.72571296330561699984539141865e-9,
 -2.06343904872070629406401492476e-10,
 -2.14273991996785367924201401812e-11,
  2.22990255539358204580285098119e-12,
  1.36250074650698280575807934155e-13,
 -1.95144010922293091898995913038e-14,
 -6.85627169231704599442806370690e-16,
  1.44506492869699938239521607493e-16,
  2.45935306460536488037576200030e-18,
 -9.29599561220523396007359328540e-19
};
static cheb_series erfc_xlt1_cs = {
  erfc_xlt1_data, 19, -1, 1
};

/* Chebyshev fit for erfc (x) * exp (x^2), 1 < x < 5, x = 2t + 3, -1 < t < 1 */
static nr_double_t erfc_x15_data[25] = {
  0.44045832024338111077637466616,
 -0.143958836762168335790826895326,
  0.044786499817939267247056666937,
 -0.013343124200271211203618353102,
  0.003824682739750469767692372556,
 -0.001058699227195126547306482530,
  0.000283859419210073742736310108,
 -0.000073906170662206760483959432,
  0.000018725312521489179015872934,
 -4.62530981164919445131297264430e-6,
  1.11558657244432857487884006422e-6,
 -2.63098662650834130067808832725e-7,
  6.07462122724551777372119408710e-8,
 -1.37460865539865444777251011793e-8,
  3.05157051905475145520096717210e-9,
 -6.65174789720310713757307724790e-10,
  1.42483346273207784489792999706e-10,
 -3.00141127395323902092018744545e-11,
  6.22171792645348091472914001250e-12,
 -1.26994639225668496876152836555e-12,
  2.55385883033257575402681845385e-13,
 -5.06258237507038698392265499770e-14,
  9.89705409478327321641264227110e-15,
 -1.90685978789192181051961024995e-15,
  3.50826648032737849245113757340e-16
};
static cheb_series erfc_x15_cs = {
  erfc_x15_data, 24, -1, 1
};

/* Chebyshev fit for erfc(x) * exp(x^2),
   5 < x < 10, x = (5t + 15)/2, -1 < t <  */
static nr_double_t erfc_x510_data[20] = {
  1.11684990123545698684297865808,
  0.003736240359381998520654927536,
 -0.000916623948045470238763619870,
  0.000199094325044940833965078819,
 -0.000040276384918650072591781859,
  7.76515264697061049477127605790e-6,
 -1.44464794206689070402099225301e-6,
  2.61311930343463958393485241947e-7,
 -4.61833026634844152345304095560e-8,
  8.00253111512943601598732144340e-9,
 -1.36291114862793031395712122089e-9,
  2.28570483090160869607683087722e-10,
 -3.78022521563251805044056974560e-11,
  6.17253683874528285729910462130e-12,
 -9.96019290955316888445830597430e-13,
  1.58953143706980770269506726000e-13,
 -2.51045971047162509999527428316e-14,
  3.92607828989125810013581287560e-15,
 -6.07970619384160374392535453420e-16,
  9.12600607264794717315507477670e-17
};
static cheb_series erfc_x510_cs = {
  erfc_x510_data, 19, -1, 1
};

/* Estimates erfc (x) valid for 8 < x < 100, this is based on index 5725
   in Hart et al. */
static nr_double_t erfc8 (nr_double_t x) {
  static nr_double_t p[] = {
    2.97886562639399288862,
    7.409740605964741794425,
    6.1602098531096305440906,
    5.019049726784267463450058,
    1.275366644729965952479585264,
    0.5641895835477550741253201704
  };
  static nr_double_t q[] = {
    3.3690752069827527677,
    9.608965327192787870698,
    17.08144074746600431571095,
    12.0489519278551290360340491,
    9.396034016235054150430579648,
    2.260528520767326969591866945,
    1.0
  };
  nr_double_t n = 0.0, d = 0.0;
  int i;
  n = p[5];
  for (i = 4; i >= 0; --i) n = x * n + p[i];
  d = q[6];
  for (i = 5; i >= 0; --i) d = x * d + q[i];

  return exp (-x * x) * (n / d);
}

#endif /* !HAVE_ERF || !HAVE_ERFC */

#ifndef HAVE_ERFC

nr_double_t fspecial::erfc (nr_double_t x) {
  const nr_double_t ax = fabs (x);
  nr_double_t val;

  if (ax <= 1.0) {
    nr_double_t t = 2.0 * ax - 1.0;
    val = cheb_eval (&erfc_xlt1_cs, t);
  }
  else if (ax <= 5.0) {
    nr_double_t ex2 = exp (-x * x);
    nr_double_t t = 0.5 * (ax - 3.0);
    val = ex2 * cheb_eval (&erfc_x15_cs, t);
  }
  else if (ax < 10.0) {
    nr_double_t ex = exp(-x * x) / ax;
    nr_double_t t = (2.0 * ax - 15.0) / 5.0;
    val = ex * cheb_eval (&erfc_x510_cs, t);
  }
  else {
    val = erfc8 (ax);
  }
  return (x < 0.0) ? 2.0 - val : val;
}
#else
nr_double_t  fspecial::erfc (nr_double_t d) {
  return ::erfc (d);
}
#endif /* HAVE_ERFC */

#ifndef HAVE_ERF

/* Abramowitz + Stegun, 7.1.5 */
static nr_double_t erfseries (nr_double_t x) {
  nr_double_t c = x;
  nr_double_t e = c;
  nr_double_t d;
  for (int k = 1; k < 30; ++k) {
    c *= -x * x / k;
    d  = c / (2.0 * k + 1.0);
    e += d;
  }
  return 2.0 / sqrt_pi * e;
}

nr_double_t fspecial::erf (nr_double_t x) {
  if (fabs (x) < 1.0) {
    return erfseries (x);
  }
  return 1.0 - erfc (x);
}

#else
nr_double_t  fspecial::erf (nr_double_t d) {
  return ::erf (d);
}
#endif /* HAVE_ERF */

// Inverse of the error function erf().
nr_double_t fspecial::erfinv (nr_double_t y) {
  nr_double_t x = 0.0;  // The output
  nr_double_t z = 0.0;  // Intermediate variable
  nr_double_t y0 = 0.7; // Central range variable

  // Coefficients in rational approximations.
  nr_double_t a[4] = { 0.886226899, -1.645349621,  0.914624893, -0.140543331};
  nr_double_t b[4] = {-2.118377725,  1.442710462, -0.329097515,  0.012229801};
  nr_double_t c[4] = {-1.970840454, -1.624906493,  3.429567803,  1.641345311};
  nr_double_t d[2] = { 3.543889200,  1.637067800};

  if (y < -1.0 || 1.0 < y) {
    x = log (-1.0);
  }
  else if (y == -1.0 || 1.0 == y) {
    x = -y * log(0.0);
  }
  else if (-1.0 < y && y < -y0) {
    z = sqrt(-log((1.0+y)/2.0));
    x = -(((c[3]*z+c[2])*z+c[1])*z+c[0])/((d[1]*z+d[0])*z+1.0);
  }
  else {
    if (-y0 < y && y < y0) {
      z = y * y;
      x = y*(((a[3]*z+a[2])*z+a[1])*z+a[0]) /
	   ((((b[3]*z+b[3])*z+b[1])*z+b[0])*z+1.0);
    }
    else if (y0 < y && y < 1.0) {
      z = sqrt(-log((1.0-y)/2.0));
      x = (((c[3]*z+c[2])*z+c[1])*z+c[0])/((d[1]*z+d[0])*z+1.0);
    }

    // Two steps of Newton-Raphson correction to full accuracy.
    x = x - (erf (x) - y) / (2.0 / sqrt_pi * exp (-x * x));
    x = x - (erf (x) - y) / (2.0 / sqrt_pi * exp (-x * x));
  }
  return x;
}

static nr_double_t bi0_data[12] = {
  -.07660547252839144951,
  1.92733795399380827000,
   .22826445869203013390,
   .01304891466707290428,
   .00043442709008164874,
   .00000942265768600193,
   .00000014340062895106,
   .00000000161384906966,
   .00000000001396650044,
   .00000000000009579451,
   .00000000000000053339,
   .00000000000000000245
};
static cheb_series bi0_cs = {
  bi0_data, 11, -1, 1
};

static nr_double_t ai0_data[21] = {
   .07575994494023796,
   .00759138081082334,
   .00041531313389237,
   .00001070076463439,
  -.00000790117997921,
  -.00000078261435014,
   .00000027838499429,
   .00000000825247260,
  -.00000001204463945,
   .00000000155964859,
   .00000000022925563,
  -.00000000011916228,
   .00000000001757854,
   .00000000000112822,
  -.00000000000114684,
   .00000000000027155,
  -.00000000000002415,
  -.00000000000000608,
   .00000000000000314,
  -.00000000000000071,
   .00000000000000007
};
static cheb_series ai0_cs = {
  ai0_data, 20, -1, 1
};

static nr_double_t ai02_data[22] = {
   .05449041101410882,
   .00336911647825569,
   .00006889758346918,
   .00000289137052082,
   .00000020489185893,
   .00000002266668991,
   .00000000339623203,
   .00000000049406022,
   .00000000001188914,
  -.00000000003149915,
  -.00000000001321580,
  -.00000000000179419,
   .00000000000071801,
   .00000000000038529,
   .00000000000001539,
  -.00000000000004151,
  -.00000000000000954,
   .00000000000000382,
   .00000000000000176,
  -.00000000000000034,
  -.00000000000000027,
   .00000000000000003
};
static cheb_series ai02_cs = {
  ai02_data, 21, -1, 1
};

// Modified Bessel function of order zero.
nr_double_t fspecial::i0 (nr_double_t x) {
  nr_double_t y = fabs (x);
  nr_double_t val;

  if (y < 2.0 * sqrt (std::numeric_limits<nr_double_t>::epsilon())) {
    val = 1.0;
  }
  else if (y <= 3.0) {
    val = 2.75 + cheb_eval (&bi0_cs, y * y / 4.5 - 1.0);
  }
  else if (y <= 8.0) {
    val = cheb_eval (&ai0_cs, (48.0 / y - 11.0) / 5.0);
    val = exp (y) * (0.375 + val) / sqrt (y);
  }
  else {
    val = cheb_eval (&ai02_cs, 16.0 / y - 1.0);
    val = exp (y) * (0.375 + val) / sqrt (y);
  }
  return val;
}

// Lower tail quantile for the standard normal distribution function.
nr_double_t fspecial::ltqnorm (nr_double_t x) {
  nr_double_t q, r, e, u, z = 0.0;
  static nr_double_t a[] = {
    -3.969683028665376e+01,  2.209460984245205e+02,
    -2.759285104469687e+02,  1.383577518672690e+02,
    -3.066479806614716e+01,  2.506628277459239e+00 };
  static nr_double_t b[] = {
    -5.447609879822406e+01,  1.615858368580409e+02,
    -1.556989798598866e+02,  6.680131188771972e+01,
    -1.328068155288572e+01 };
  static nr_double_t c[] = {
    -7.784894002430293e-03, -3.223964580411365e-01,
    -2.400758277161838e+00, -2.549732539343734e+00,
     4.374664141464968e+00,  2.938163982698783e+00 };
  static nr_double_t d[] = {
     7.784695709041462e-03,  3.224671290700398e-01,
     2.445134137142996e+00,  3.754408661907416e+00 };

  // Define break-points.
  nr_double_t pl = 0.02425;
  nr_double_t ph = 1.0 - pl;

  // Rational approximation for central region:
  if (pl <= x && x <= ph) {
    q = x - 0.5;
    r = q * q;
    z = (((((a[0]*r+a[1])*r+a[2])*r+a[3])*r+a[4])*r+a[5])*q/
	(((((b[0]*r+b[1])*r+b[2])*r+b[3])*r+b[4])*r+1);
  }
  // Rational approximation for lower region:
  else if (0.0 < x && x < pl) {
    q = sqrt(-2*log(x));
    z = (((((c[0]*q+c[1])*q+c[2])*q+c[3])*q+c[4])*q+c[5])/
	 ((((d[0]*q+d[1])*q+d[2])*q+d[3])*q+1);
  }
  // Rational approximation for upper region:
  else if (ph < x && x < 1.0) {
    q = sqrt(-2*log(1-x));
    z = -(((((c[0]*q+c[1])*q+c[2])*q+c[3])*q+c[4])*q+c[5])/
	  ((((d[0]*q+d[1])*q+d[2])*q+d[3])*q+1);
  }
  // Case when X = 0:
  else if (x == 0.0) {
    z = -std::numeric_limits<nr_double_t>::infinity();
  }
  // Case when X = 1:
  else if (x == 1.0) {
    z = +std::numeric_limits<nr_double_t>::infinity();
  }
  // Cases when output will be NaN:
  else if (x < 0.0 || x > 1.0 || std::isnan (x)) {
    z = +std::numeric_limits<nr_double_t>::quiet_NaN();
  }

  // The relative error of the approximation has absolute value less
  // than 1.15e-9.  One iteration of Halley's rational method (third
  // order) gives full machine precision.
  if (0.0 < x && x < 1.0) {
    e = 0.5 * erfc (-z / sqrt2) - x;            // error
    u = e * sqrt2 * sqrt_pi * exp (z * z / 2); // f(z)/df(z)
    z = z - u / (1 + z * u / 2);                  // Halley's method
  }
  return z;
}

// Inverse of the error function erfc().
nr_double_t fspecial::erfcinv (nr_double_t x) {
  return -ltqnorm (x / 2.0) / sqrt2;
}
