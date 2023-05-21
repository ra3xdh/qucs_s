/*
 * real.h - some real valued function definitions
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

#ifndef __REAL_H__
#define __REAL_H__

#include <cmath>

#include <constants.h>

/**It is prefered to add all used funcions into the qucs namespace.
 * Doing so one is forced do think about compatibility instead of using std directly.
 * Inline is optional at this moment
 * \todo test if inline indeed performace improves (optimization flags should inline them anyway)
 */

namespace qucs {

//
// trigonometric
//
nr_double_t    cos (const nr_double_t);
nr_double_t    sin (const nr_double_t);
nr_double_t    tan (const nr_double_t);
nr_double_t   acos (const nr_double_t);
nr_double_t   asin (const nr_double_t);
nr_double_t   atan (const nr_double_t);
nr_double_t  atan2 (const nr_double_t, const nr_double_t); //not used?, only for complex


//
// hyperbolic
//
nr_double_t   cosh (const nr_double_t);
nr_double_t   sinh (const nr_double_t);
nr_double_t   tanh (const nr_double_t);
nr_double_t  acosh (const nr_double_t); // c++11
nr_double_t  asinh (const nr_double_t); // c++11
nr_double_t  atanh (const nr_double_t); // c++11, not used?, only for complex


//
// exponential and logarithmic functions
//
nr_double_t exp (const nr_double_t);
nr_double_t log (const nr_double_t);
nr_double_t log10 (const nr_double_t);


//
// power functions
//
nr_double_t pow (const nr_double_t, const nr_double_t );
nr_double_t sqrt (const nr_double_t );
nr_double_t xhypot (const nr_double_t, const nr_double_t ); // same hypot in c++11?


//
// error functions
//
nr_double_t erf(const nr_double_t );


//
// rounding and remainder functions
//
nr_double_t ceil(const nr_double_t );
nr_double_t floor(const nr_double_t );
nr_double_t fmod(const nr_double_t ); //FIXME
nr_double_t trunc(const nr_double_t ); // c++11
nr_double_t round(const nr_double_t ); // c++11

//
// Qucs extra trigonometric helper
//
nr_double_t coth (const nr_double_t );
nr_double_t sech (const nr_double_t );
nr_double_t cosech (const nr_double_t );


//
// Qucs extra math functions
//
nr_double_t  sqr (const nr_double_t );
unsigned int sqr (unsigned int);
nr_double_t  quadr (const nr_double_t );

nr_double_t rad2deg (const nr_double_t );
nr_double_t deg2rad (const nr_double_t x );

/*!\brief Compute the third power of input */
static inline nr_double_t cubic (const nr_double_t x)  { return (x * x * x); }

/*!\brief Convert Celsius to Kelvin */
static inline nr_double_t celsius2kelvin (const nr_double_t x)  { return (x - K); }

/*!\brief Convert Kelvin to Celsius */
static inline nr_double_t kelvin2celsius (const nr_double_t x)  { return (x + K); }


//
// extra math functions
//
nr_double_t limexp (const nr_double_t);
nr_double_t signum (const nr_double_t);
nr_double_t   sign (const nr_double_t);
nr_double_t   sinc (const nr_double_t);
nr_double_t    fix (const nr_double_t);
nr_double_t   step (const nr_double_t);
unsigned int factorial (unsigned int);


//
// overload complex manipulations on reals
//
nr_double_t   real (const nr_double_t);
nr_double_t   imag (const nr_double_t);
nr_double_t   norm (const nr_double_t);
nr_double_t   conj (const nr_double_t);
nr_double_t   abs (const nr_double_t);

} // namespace qucs

#endif /* __REAL_H__ */
