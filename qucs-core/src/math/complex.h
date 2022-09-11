/*
 * complex.h - complex number class definitions
 *
 * Copyright (C) 2008 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __COMPLEX_H__
#define __COMPLEX_H__

#include <complex>
#include "real.h"

typedef std::complex<nr_double_t> nr_complex_t;

// undefine this macro if it is defined already
#ifdef log2
#undef log2
#endif

namespace qucs {

// see http://www.cplusplus.com/reference/complex/

//
// trigonometric complex
//
nr_complex_t     cos (const nr_complex_t);
nr_complex_t     sin (const nr_complex_t);
nr_complex_t     tan (const nr_complex_t);
nr_complex_t    acos (const nr_complex_t); //c++11
nr_complex_t    asin (const nr_complex_t); //c++11
nr_complex_t    atan (const nr_complex_t); //c++11

//
// hyperbolic complex
//
nr_complex_t    cosh (const nr_complex_t);
nr_complex_t    sinh (const nr_complex_t);
nr_complex_t    tanh (const nr_complex_t);
nr_complex_t   acosh (const nr_complex_t); //c++11
nr_complex_t   asinh (const nr_complex_t); //c++11
nr_complex_t   atanh (const nr_complex_t); //c++11

//
// transcendentals overloads
//
nr_complex_t     exp (const nr_complex_t);
nr_complex_t     log (const nr_complex_t);
nr_complex_t   log10 (const nr_complex_t);
nr_complex_t     pow (const nr_complex_t, const nr_double_t);
nr_complex_t     pow (const nr_double_t, const nr_complex_t);
nr_complex_t     pow (const nr_complex_t, const nr_complex_t);
nr_complex_t    sqrt (const nr_complex_t);

nr_double_t     norm (const nr_complex_t);


//
//  Qucs extra trancendental functions
//
nr_complex_t     cot (const nr_complex_t);
nr_complex_t    acot (const nr_complex_t);
nr_complex_t    coth (const nr_complex_t);
nr_complex_t   acoth (const nr_complex_t);
nr_complex_t    sech (const nr_complex_t);
nr_complex_t   asech (const nr_complex_t);
nr_complex_t  cosech (const nr_complex_t);
nr_complex_t   atan2 (const nr_complex_t, const nr_complex_t);


//
// extra math
//
nr_complex_t    log2 (const nr_complex_t);
nr_complex_t  signum (const nr_complex_t);
nr_complex_t    sign (const nr_complex_t);
nr_complex_t    sinc (const nr_complex_t);
nr_double_t   xhypot (const nr_complex_t, const nr_complex_t);
nr_double_t   xhypot (const nr_double_t, const nr_complex_t);
nr_double_t   xhypot (const nr_complex_t, const nr_double_t);

nr_complex_t round (const nr_complex_t);
nr_complex_t trunc (const nr_complex_t);


nr_double_t dB (const nr_complex_t);

nr_complex_t limexp (const nr_complex_t);

nr_complex_t   polar (const nr_double_t mag, const nr_double_t theta = 0.0);
/// \bug are these needed/used?
//nr_complex_t   polar (const nr_double_t a, const nr_complex_t p);
//nr_complex_t   polar (const nr_complex_t a, const nr_double_t p = 0.0);
nr_complex_t   polar (const nr_complex_t a, const nr_complex_t p = 0.0);


nr_complex_t    ztor (const nr_complex_t, const nr_complex_t zref = 50.0);
nr_complex_t    rtoz (const nr_complex_t, const nr_complex_t zref = 50.0);
nr_complex_t    ytor (const nr_complex_t, const nr_complex_t zref = 50.0);
nr_complex_t    rtoy (const nr_complex_t, const nr_complex_t zref = 50.0);


nr_complex_t floor (const nr_complex_t );
nr_complex_t  ceil (const nr_complex_t );
nr_complex_t   fix (const nr_complex_t );

/// \todo add fmod to manual
nr_complex_t  fmod (const nr_complex_t x, const nr_complex_t y);
nr_complex_t  sqr (const nr_complex_t z);

nr_complex_t step (const nr_complex_t);


// bessel functions
nr_complex_t      jn (const int, const nr_complex_t);
nr_complex_t      yn (const int, const nr_complex_t);
nr_complex_t      i0 (const nr_complex_t);


// error functions
nr_complex_t     erf (const nr_complex_t);
nr_complex_t    erfc (const nr_complex_t);
nr_complex_t  erfinv (const nr_complex_t); //see fspecial
nr_complex_t erfcinv (const nr_complex_t); //see fspecial


nr_double_t rad2deg (const nr_complex_t );
nr_double_t deg2rad (const nr_complex_t );

// modulo operators
nr_complex_t operator % (const nr_complex_t, const nr_complex_t);
nr_complex_t operator % (const nr_complex_t, const nr_double_t);
nr_complex_t operator % (const nr_double_t, const nr_complex_t);

// comparisons
bool operator == (const nr_complex_t, const nr_complex_t);
bool operator != (const nr_complex_t, const nr_complex_t);
bool operator >= (const nr_complex_t, const nr_complex_t);
bool operator <= (const nr_complex_t, const nr_complex_t);
bool operator >  (const nr_complex_t, const nr_complex_t);
bool operator <  (const nr_complex_t, const nr_complex_t);

} // namespace qucs

#endif /* __COMPLEX_H__ */
