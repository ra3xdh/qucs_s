/*
 * units.h - some conversion definitions
 *
 * Copyright (C) 2001 Gopal Narayanan <gopal@astro.umass.edu>
 * Copyright (C) 2005, 2006 Stefan Jahn <stefan@lkcc.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.  
 *
 */

#ifndef __UNITS_H
#define __UNITS_H

#include <cmath>


static const double pi        = 3.1415926535897932384626433832795029;  /* pi   */
static const double pi_over_2 = 1.5707963267948966192313216916397514;  /* pi/2 */
static const double e         = 2.7182818284590452353602874713526625;  /* e    */

static const double MU0 = 12.566370614e-7;          /* magnetic constant         */
static const double C0  = 299792458.0;              /* speed of light in vacuum  */
static const double ZF0 = 376.73031346958504364963; /* wave resistance in vacuum */

#ifndef INFINITY
const double INFINITY = -log (0.0);
#endif

static const double NR_EPSI   = 2.2204460492503131e-16;
static const double MAX_ERROR = 0.000001;

// Types of units.
#define UNIT_FREQ   0
#define UNIT_LENGTH 1
#define UNIT_RES    2
#define UNIT_ANG    3

// Frequency units.
#define FREQ_GHZ 0
#define FREQ_HZ  1
#define FREQ_KHZ 2
#define FREQ_MHZ 3

// Length units.
#define LENGTH_MIL 0
#define LENGTH_CM  1
#define LENGTH_MM  2
#define LENGTH_M   3
#define LENGTH_UM  4
#define LENGTH_IN  5
#define LENGTH_FT  6

// Resistance units.
#define RES_OHM  0
#define RES_KOHM 1

// Angle units.
#define ANG_DEG 0
#define ANG_RAD 1


#endif /* __UNITS_H */
