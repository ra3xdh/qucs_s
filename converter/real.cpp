/*
 * real.cpp - some real valued function implementations
 *
 * Copyright (C) 2008 Stefan Jahn <stefan@lkcc.org>
 * Copyright (C) 2014 Guilheme Brondani Torri <guitorri@gmail.com>
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

#include <complex>
#include "qucs_typedefs.h"

//#include <cstdlib>
#include <cmath>
#include <cassert>

#include "consts.h"
#include "real.h"

namespace qucs {

//
// trigonometric
//

/*! \brief Compute cosine of an angle
    \param[in] z angle in radians
    \return cosine of z
*/
nr_double_t cos (const nr_double_t arg) {
  return std::cos (arg);
}

/*! \brief Compute sine of an angle
    \param[in] z angle in radians
    \return sine of z
*/
nr_double_t sin (const nr_double_t arg) {
  return std::sin (arg);
}

/*! \brief Compute tangent of an angle
    \param[in] z angle in radians
    \return tangent of z
*/
nr_double_t  tan (const nr_double_t arg) {
  return std::tan (arg);
}

/*! \brief Compute arc cosine
    \param[in] z arc
    \return arc cosine of z
*/
nr_double_t  acos (const nr_double_t arg) {
  return std::acos (arg);
}

/*! \brief Compute arc sine
    \param[in] z arc
    \return arc sine of z
*/
nr_double_t  asin (const nr_double_t arg) {
  return std::asin (arg);
}

/*! \brief Compute arc tangent
    \param[in] z arc
    \return arc tangent of z
*/
nr_double_t  atan (const nr_double_t arg) {
  return std::atan (arg);
}

/*! \brief Compute arc tangent with two parameters (fortran like function)
    \param[in] x proportion of x-coordinate
    \param[in] y proportion of y-coordinate
    \return principal value of the arc tangent of y/x, expressed in radians.
*/
nr_double_t  atan2 (const nr_double_t x, const nr_double_t y) {
  return std::atan2 (x,y);
}

//
// hyperbolic
//

/*! \brief Compute hyperbolic cosine
    \param[in] z arc
    \return hyperbolic cosine of z
*/
nr_double_t  cosh (const nr_double_t arg) {
  return std::cosh (arg);
}

/*! \brief Compute hyperbolic sine
    \param[in] z arc
    \return hyperbolic sine of z
*/
nr_double_t  sinh (const nr_double_t arg) {
  return std::sinh (arg);
}

/*! \brief Compute hyperbolic tangent
    \param[in] z arc
    \return hyperbolic tangent of z
*/
nr_double_t  tanh (const nr_double_t arg) {
  return std::tanh (arg);
}

/*! \brief Compute arc hyperbolic cosine
    \param[in] z arc
    \return arc hyperbolic cosine of z
*/
nr_double_t  acosh (const nr_double_t arg) {
#ifdef HAVE_STD_ACOSH
  // c++11
  return std::acosh (arg);
#elif HAVE_ACOSH
  return ::acosh (arg);
#else
  return log (arg + sqrt (arg * arg - 1.0));
#endif
}

/*! \brief Compute arc hyperbolic sine
    \param[in] z arc
    \return arc hyperbolic sine of z
*/
nr_double_t asinh (const nr_double_t arg)
{
#ifdef HAVE_STD_ASINH
  // c++11
  return std::asinh (arg);
#elif HAVE_ASINH
  return ::asinh (arg);
#else
  return log (arg + sqrt (arg * arg + 1.0));
#endif
}

/*! \brief Compute arc hyperbolic tangent
    \param[in] z arc
    \return arc hyperbolic tangent of z
*/
nr_double_t atanh (const nr_double_t arg)
{
#ifdef HAVE_STD_ATANH
  // c++11
  return std::atanh (arg);
#elif HAVE_ATANH
  return ::atanh (arg);
#else
  return 0.5 * log ( 2.0 / (1.0 - arg) - 1.0);
#endif
}


//
// exponential and logarithmic functions
//
nr_double_t exp (const nr_double_t arg) {
  return std::exp (arg);
}
nr_double_t log (const nr_double_t arg) {
  return std::log (arg);
}
nr_double_t log10 (const nr_double_t arg) {
  return std::log10 (arg);
}

//
// power functions
//

nr_double_t pow (const nr_double_t a, const nr_double_t b)
{
  return std::pow (a,b);
}

nr_double_t sqrt (const nr_double_t d) {
  return std::sqrt (d);
}

/*!\brief Euclidean distance function

   The xhypot() function returns \f$\sqrt{a^2+b^2}\f$.
   This is the length of the hypotenuse of a right-angle triangle with sides
   of length a and b, or the distance
   of the point (a,b) from the origin.

   \param[in] a first length
   \param[in] b second length
   \return Euclidean distance from (0,0) to (a,b): \f$\sqrt{a^2+b^2}\f$
*/
nr_double_t xhypot (const nr_double_t a, const nr_double_t b) {
#ifdef HAVE_STD_HYPOT
  return std::hypot(a,b) // c++11
#else
  nr_double_t c = fabs (a);
  nr_double_t d = fabs (b);
  if (c > d) {
    nr_double_t e = d / c;
    return c * sqrt (1 + e * e);
  }
  else if (d == 0)
    return 0;
  else {
    nr_double_t e = c / d;
    return d * sqrt (1 + e * e);
  }
#endif
}

//
// error functions
//

nr_double_t erf( nr_double_t arg) {
  return std::erf (arg); // c++11
}


//
// rounding and remainder functions
//
nr_double_t ceil( nr_double_t arg) {
  return std::ceil(arg);
}

nr_double_t floor( nr_double_t arg) {
  return std::floor(arg);
}

nr_double_t fmod( nr_double_t arg) {
#ifdef HAVE_STD_TRUNC
  return std::fmod(arg);
#else
  return fmod(arg);
#endif
}

nr_double_t trunc( nr_double_t arg) {
#ifdef HAVE_STD_TRUNC
  return std::trunc(arg);
#elif HAVE_TRUNC
  return ::trunc (arg);
#else
  return arg > 0 ? floor (arg) : floor (arg + 1);
#endif
}
nr_double_t round( nr_double_t arg) {
#ifdef HAVE_STD_ROUND
  return std::round(arg);
#elif HAVE_ROUND
  return ::round (arg);
#else
  return (arg > 0) ? floor (arg + 0.5) : ceil (arg - 0.5);
#endif
}


//
// Qucs extra trigonometric helper
//
nr_double_t coth (const nr_double_t d) {
  return 1.0 / std::tanh (d);
}

nr_double_t sech (const nr_double_t d) {
  return  (1.0 / std::cosh (d));
}

nr_double_t cosech (const nr_double_t d) {
  return  (1.0 / std::sinh (d));
}


//
// Qucs extra math functions
//

/*!\brief Square a value

   \param[in] r Real number
   \return \f$x^2\f$
*/
nr_double_t  sqr (const nr_double_t r) {
  return r * r;
}

unsigned int sqr (unsigned int r) {
  return r * r;
}



/*!\brief Quartic function

   \param[in] r Real number
   \return \f$x^4\f$
*/
nr_double_t  quadr (const nr_double_t r) {
  return r * r * r * r;
}


//
//  extra math functions
//

/*!\brief Compute limited exponential

   Compute limited exponential:
   \f[
   \begin{cases}
   \exp r & \text{if } r < \text{M\_LIMEXP} \\
   \exp (\text{M\_LIMEXP})\left[1.0 + (r - \text{M\_LIMEXP})\right] &
        \text{else}
   \end{cases}
   \f]

   #limitexp is a constant
   \param[in] r real number
   \return limited exponential of r
   \todo Change limexp(real) limexp(complex) file order
   \todo Document #limitexp
*/
nr_double_t limexp (const nr_double_t r) {
  return r < limitexp ? exp (r) : exp (limitexp) * (1.0 + (r - limitexp));
}

/*!\brief real signum function

    compute \f[
    \mathrm{signum}\;d=
                     = \begin{cases}
		       O & \text{if } d=0 \\
		       1 & \text{if } d>0 \\
		       -1 & \text{if } d<0
		       \end{cases}
	    \f]
   \param[in] d real number
   \return signum of d
   \todo Move near complex signum
*/
nr_double_t signum (const nr_double_t d) {
  if (d == 0) return 0;
  return d < 0 ? -1 : 1;
}

/*!\brief real sign function

    compute \f[
    \mathrm{sign}\;d=
                     = \begin{cases}
		       1 & \text{if } d\ge 0 \\
		       -1 & \text{if } d<0
		       \end{cases}
	    \f]
   \param[in] d real number
   \return sign of d
   \todo Move near complex sign
*/
nr_double_t sign (const nr_double_t d) {
  return d < 0 ? -1 : 1;
}

/*!\brief Real cardinal sinus

   Compute \f$\mathrm{sinc}\;d=\frac{\sin d}{d}\f$
   \param[in] d real number
   \return cardianal sinus of s
   \todo Why not inline
*/
nr_double_t sinc (const nr_double_t d) {
  if (d == 0) return 1;
  return sin (d) / d;
}

/*!\brief Fix function

    Fix is nearest integral value in direction of 0,
    \f[
    \operatorname{fix} d=\begin{cases}
    \operatorname{floor} d & \text{if } d > 0 \\
    \operatorname{ceil} d  & \text{else}
    \end{cases}
    \f]

    \param[in] d real number
    \return fixed complex number
    \todo Why not inline?
*/
nr_double_t fix (const nr_double_t d) {
  return (d > 0) ? floor (d) : ceil (d);
}

/*!\brief Heaviside step function

   The Heaviside step function, H, also called unit step function,
   is a discontinuous function whose value is zero for negative argument and
   one for positive argument. For zero by convention, H(0)=0.5
   \param[in] d Heaviside argument
   \return Heaviside step
   \todo Create Heaviside alias
*/
nr_double_t step (const nr_double_t d) {
  nr_double_t x = d;
  if (x < 0.0)
    x = 0.0;
  else if (x > 0.0)
    x = 1.0;
  else
    x = 0.5;
  return x;
}

/*!\brief Compute factorial n ie \$n!\$

*/
unsigned int
factorial (unsigned int n) {
  unsigned int result = 1;

  /* 13! > 2^32 */
  assert (n < 13);

  if (n == 0)
    return 1;

  for (; n > 1; n--)
    result = result * n;

  return result;
}


//
// overload complex manipulations on reals
//


/*!\brief Real part of real number

   \param[in] r Real number
   \return Real part of r ie r
*/
nr_double_t real (const nr_double_t r) {
  return r;
}

/*!\brief Imaginary part of complex number

   \param[in] r Real number
   \return Imaginary part of r
*/
nr_double_t imag (const nr_double_t r) {
  (void) r; // supress unused warning
  return 0.0;
}

/*!\brief Compute euclidian norm of real number

   Compute \f$r^2\f$
   \param[in] r Real number
   \return Euclidian norm of r
*/
nr_double_t norm (const nr_double_t r) {
  return r * r;
}

/*!\brief Compute complex modulus of real number

   \param[in] r Real number
   \return Modulus of r
*/
nr_double_t abs (const nr_double_t r) {
  return std::abs (r);
}

/*!\brief Conjugate of real number

   \param[in] r Real number
   \return Conjugate of real r ie r
*/
nr_double_t conj (const nr_double_t r) {
  return r;
}

/*!
 * \brief rad2deg Convert radian to degree
 * \param x input
 * \return input in degree (x)*180/pi
 */
nr_double_t rad2deg (const nr_double_t x) {
  return (180.0 * (x) / pi);
}

/*!
 * \brief deg2rad Convert radian to degree
 * \param x input
 * \return input in radian (x)*pi/180
 */
nr_double_t deg2rad (const nr_double_t x) {
  return (pi * (x) / 180.0);
}



} // namespace qucs
