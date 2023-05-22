/*
 * consts.h - global math constant header file
 *
 * Copyright (C) 2004, 2005, 2007 Stefan Jahn <stefan@lkcc.org>
 * Copyright (C) 2015 Guilherme Brondani Torri <guitorri@gmail.com>
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

/**
 \file consts.h
 \brief Global math constants header file
 @file consts.h
 @defgroup qucsMathConstants Qucs Math Constants
*/

#ifndef __CONSTS_H__
#define __CONSTS_H__

#include <cmath>

namespace qucs {

/**
\addtogroup qucsMathConstants
Qucs mathematical constants.
@{
*/

/*!\brief  Archimedes' constant (\f$\pi\f$) */
static const double pi = 3.1415926535897932384626433832795029;

/*!\brief Half of Archimedes' constant (\f$\pi/2\f$) */
static const double pi_over_2 = 1.5707963267948966192313216916397514;

/*!\brief A quarter of Archimedes' constant (\f$\pi/4\f$) */
static const double pi_over_4 = 0.7853981633974483096156608458198757;

/*!\brief Inverse of Archimedes' constant (\f$1/\pi\f$) */
static const double one_over_pi = 0.3183098861837906715377675267450287;

/*\brief Twice the inverse of Archimedes' constant (\f$2/\pi\f$) */
static const double two_over_pi = 0.6366197723675813430755350534900574;

/*!\brief Square root of Archimedes' constant (\f$\sqrt{\pi}\f$) */
static const double sqrt_pi = 1.77245385090551602729816748334;

/*!\brief  Euler's constant (\f$e=\sum_{n=0}^\infty \frac{1}{n!}\f$) */
static const double euler = 2.7182818284590452353602874713526625;

/*!\brief Decimal logartihm of Euler's constant (\f$\log_{10} (e)\f$) */
static const double log10e = 0.4342944819032518276511289189166051;

/*!\brief Binary logartihm of Euler's constant (\f$\log_{2} (e)\f$)*/
static const double log2e = 1.4426950408889634073599246810018922;

/*!\brief Natural logarithm of 2 (\f$\ln 2\f$) */
static const double ln2 = 0.6931471805599453094172321214581766;

/*!\brief Natural logarithm of 10 (\f$\ln 10\f$) */
static const double ln10 = 2.3025850929940456840179914546843642;

/*!\brief Square root of 2 (\f$\sqrt{2}\f$) */
static const double sqrt2 = 1.4142135623730950488016887242096981;

/*!\brief Inverse of Square root of 2 (\f$1/\sqrt{2}\f$) */
static const double sqrt1_2 = 0.7071067811865475244008443621048490;

/*!\brief Limiting exponetial factor */
static const double limitexp = 80.0;

/**
@}
*/

} // namespace qucs

#endif /* __CONSTS_H__ */
