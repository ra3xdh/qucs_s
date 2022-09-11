/*
 * cbesselj.cpp - Bessel function of first kind
 *
 * Copyright (C) 2007 Bastien Roucaries <roucaries.bastien@gmail.com>
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

/*!\file cbesselj.cpp
   \brief compute complex bessel J function

  Bibligraphy:

  [1] Bessel function of the first kind with complex argument
      Yousif, Hashim A.; Melka, Richard
      Computer Physics Communications, vol. 106, Issue 3, pp.199-206
      11/1997, ELSEVIER, doi://10.1016/S0010-4655(97)00087-8

  [2] Handbook of Mathematical Functions with
      Formulas, Graphs, and Mathematical Tables
      Milton Abramowitz and Irene A. Stegun
      public domain (work of US government)
      online http://www.math.sfu.ca/~cbm/aands/

  [3] Mathematica Manual
      Bessel, Airy, Struve Functions> BesselJ[nu,z]
      > General characteristics> Symmetries and periodicities
      http://functions.wolfram.com/BesselAiryStruveFunctions/BesselJ/04/02/01/

  [4] Mathematica Manual
      Bessel, Airy, Struve Functions> BesselJ[nu,z]
      Representations through equivalent functions
      http://functions.wolfram.com/BesselAiryStruveFunctions/BesselJ/27/ShowAll.html

  [5] Algorithms for the evaluation of Bessel functions
      of complex argument and integer orders
      G. D. C. Kuiken
      Applied Mathematics Letters, Volume 2, Issue 4,
      1989,  Pages 353-356
      doi://10.1016/0893-9659(89)90086-4

*/


//#if HAVE_CONFIG_H
//# include <config.h>
//#endif
//
//#include <cmath>
//#include <assert.h>
//#include <errno.h>
//#include <stdio.h>
//#include <stdlib.h>
//
//#include "real.h"
//#include "complex.h"
//#include "constants.h"
//#include "precision.h"
//#include <limits>

#define SMALL_J0_BOUND 1e6

/*!\brief use ascending serie below this magnitude */
#define SMALL_JN_BOUND 5.0

/*!\brief use assymptotic expression above this magnitude */
#define BIG_JN_BOUND 25.0

/*! \brief Arbitrary value for iteration*/
#define MAX_SMALL_ITERATION 2048


/*!\brief Implement bessel J function for small arguments
    according to [5]

    For small argument we use the following formulae:
    \{align}
    J_n(z)&=\sum_0^\infty R_k & & \\
    R_{-1}&=1 & R_k &= a_k R_{k-1} \\
    a_0&=\frac{\left(\frac{1}{2}z\right)^n}{n!} &
    a_{+k}&=\frac{-\frac{1}{4} z^2}{k(n+k)}
    \}

    \todo Not really adapted to high order
          therefore we do not check overflow for n >> 1

    \param[in] n order
    \param[in] arg arguments
*/
static nr_complex_t
cbesselj_smallarg (unsigned int n, nr_complex_t z)
{
  nr_complex_t ak, Rk;
  nr_complex_t R;
  nr_complex_t R0;
  unsigned int k;

  /* a_0 */
  errno = 0;
  ak = pow (0.5 * z, n);
  /* underflow */
  if (errno == ERANGE)
    {
      return n > 0 ? 0.0 : 1;
    }

  ak = ak / (nr_double_t) qucs::factorial (n);

  /* R_0 */
  R0 = ak * 1.0;
  Rk = R0;
  R = R0;

  for (k = 1; k < MAX_SMALL_ITERATION; k++)
    {
      ak = -(z * z) / (4.0 * k * (n + k));
      Rk = ak * Rk;
      if (fabs (real (Rk)) < fabs (real (R) * std::numeric_limits<nr_double_t>::epsilon()) &&
	  fabs (imag (Rk)) < fabs (imag (R) * std::numeric_limits<nr_double_t>::epsilon()))
	return R;

      R += Rk;
    }
  /* impossible */
  assert (k < MAX_SMALL_ITERATION);
  abort ();
}


static nr_complex_t
cbesselj_mediumarg_odd (unsigned int n, nr_complex_t z)
{
  nr_complex_t first, second;
  nr_complex_t ak;

  unsigned int m;
  unsigned int k;
  nr_double_t t;
  nr_double_t m1pna2;

  m = (2 * std::abs (z) + 0.25 * (n + std::abs (imag (z))));

  /* -1^(n/2) */
  m1pna2 = (n / 2) % 2 == 0 ? 1.0 : -1.0;
  first = (1.0 + m1pna2 * cos (z)) / (2.0 * m);

  second = 0.0;
  for (k = 1; k <= m - 1; k++)
    {
      t = (k * M_PI) / (2 * m);
      ak = cos (z * std::sin (t)) * std::cos (n * t);
      second += ak;
    }
  return first + second / (nr_double_t) m;
}

static nr_complex_t
cbesselj_mediumarg_even (unsigned int n, nr_complex_t z)
{
  nr_complex_t first, second;
  nr_complex_t ak;

  unsigned int m;
  unsigned int k;
  nr_double_t t;
  nr_double_t m1pn1a2;

  m = (2 * std::abs (z) + 0.25 * (n + std::abs (imag (z))));

  /* -1^(n/2) */
  m1pn1a2 = ((n - 1) / 2) % 2 == 0 ? 1.0 : -1.0;
  first = (m1pn1a2 * sin (z)) / (2.0 * m);

  second = 0.0;
  for (k = 1; k <= m - 1; k++)
    {
      t = (k * M_PI) / (2 * m);
      ak = std::sin (z * std::sin (t)) * std::sin (n * t);
      second += ak;
    }
  return first + second / (nr_double_t) m;
}


static nr_complex_t
cbesselj_mediumarg (unsigned int n, nr_complex_t z)
{
  if (n % 2 == 0)
    return cbesselj_mediumarg_odd (n, z);
  else
    return cbesselj_mediumarg_even (n, z);
}



/*! \brief num of P(k) (n = 8) will overflow above this value */
#define MAX_LARGE_ITERATION 430

/*!\brief besselj for large argument

    Based on [5] eq (5)
*/
static nr_complex_t
cbesselj_largearg (unsigned int n, nr_complex_t z)
{
  nr_complex_t Pk, P0, P, Qk, Q0, Q_;
  nr_complex_t tmp;
  unsigned long num, denum;
  nr_complex_t m1a8z2;
  unsigned int k;
  nr_double_t l, m;

  /* P0 & Q0 */
  P0 = 1;
  P = P0;
  Pk = P0;

  Q0 = (4.0 * n * n - 1) / (8.0 * z);
  Q_ = Q0;
  Qk = Q0;

  m1a8z2 = (-1.0) / (8.0 * sqr (z));

  /* P */
  for (k = 1;; k++)
    {
      /* Usually converge before overflow */
      assert (k <= MAX_LARGE_ITERATION);

      num = (4 * sqr (n) - sqr (4 * k - 3)) * (4 * sqr (n) - (4 * k - 1));
      denum = 2 * k * (2 * k - 1);
      Pk = Pk * ((nr_double_t) num * m1a8z2) / ((nr_double_t) denum);

      if (real (Pk) < real (P0) * std::numeric_limits<nr_double_t>::epsilon() &&
	  imag (Pk) < imag (P0) * std::numeric_limits<nr_double_t>::epsilon())
	break;

      P += Pk;
    }

  /* P */
  for (k = 1;; k++)
    {
      /* Usually converge before overflow */
      assert (k <= MAX_LARGE_ITERATION);

      num = (4 * sqr (n) - sqr (4 * k - 1)) * (4 * sqr (n) - (4 * k - 1));
      denum = 2 * k * (2 * k - 1);
      Qk = Qk * ((nr_double_t) num * m1a8z2) / ((nr_double_t) denum);

      if (real (Qk) < real (Q0) * std::numeric_limits<nr_double_t>::epsilon() ||
	  imag (Qk) < imag (Q0) * std::numeric_limits<nr_double_t>::epsilon())
	break;

      Q_ += Qk;
    }

  /* l, m cf [5] eq (5) */
  l = (n % 4 == 0) || (n % 4 == 3) ? 1.0 : -1.0;
  m = (n % 4 == 0) || (n % 4 == 1) ? 1.0 : -1.0;


  tmp = (l * P + m * Q_) * cos (z);
  tmp += (m * P - l * Q_) * sin (z);
  return tmp / sqrt (M_PI * z);
}

/*!\brief Main entry point for besselj function

*/
nr_complex_t
cbesselj (unsigned int n, nr_complex_t z)
{
  nr_double_t mul = 1.0;
  nr_complex_t ret;

  /* J_n(-z)=(-1)^n J_n(z) */
  /*
     if(real(z) < 0.0)
     {
     z = -z;
     mul = (n % 2) == 0 ? 1.0 : -1.0 ;
     }
   */
  if (abs (z) < SMALL_JN_BOUND)
    ret = cbesselj_smallarg (n, z);
  else if (abs (z) > BIG_JN_BOUND)
    ret = cbesselj_largearg (n, z);
  else
    ret = cbesselj_mediumarg (n, z);

  return mul * ret;
}
