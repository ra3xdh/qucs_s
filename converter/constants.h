/*
 * constants.h - global natural constant header file
 *
 * Copyright (C) 2004, 2005 Stefan Jahn <stefan@lkcc.org>
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

/*!\file constants.h
   \brief Global physical constants header file
   @todo Create a material header
   @file constants.h
   @defgroup qucsPhysConstants Qucs Physical Constants
*/

#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include "consts.h"
#include "precision.h"

namespace qucs {

/**
\addtogroup qucsPhysConstants
Qucs physical constants
@{
*/

/*!\brief speed of light in vacuum (\f$c_0\f$) */
static const double C0   = 299792458.0;
/*!\brief magnetic constant of vacuum (\f$\mu_0=4\pi\times10^{-7}\f$) */
static const double  MU0 = 12.566370614e-7;
/*!\brief Electric constant of vacuum \f$\varepsilon_0\f$*/
static const double E0   = 8.854187817e-12;
/*!\brief Wave impedance in vacuum (\f$Z_0=\sqrt{\frac{\mu_0}{\varepsilon_0}}\f$)*/
static const double Z0   = 376.73031346958504364963;
/*!\brief Planck constant (\f$h\f$)
   \todo Add h bar (\f$h/(2\pi)\f$)
*/
static const double Hp   = 6.626069311e-34;
/*!\brief Absolute 0 in centigrade  */
static const double K    = -273.15;
/*!\brief standard temperature      */
static const double T0   = 290;

/*!\brief Boltzmann constant (\f$k_B\f$) */
static const double kB      = 1.380650524e-23;
/*!\brief Elementary charge (\f$q_e\f$)  */
static const double Q_e     = 1.6021765314e-19;
/*!\brief Boltzmann constant over Elementary charge (\f$k_B/q_e\f$)*/
static const double kBoverQ = 0.86173433260414314916e-4;
/*!\brief Elementary charge over Boltzmann constant (\f$q_e/k_B\f$)*/
static const double QoverkB = 1.16045045690360379713e+4;

/*!\brief Relative permittivity of Silicon dioxide (Silica) */
static const double ESiO2     = 3.9;
/*!\brief Relative permittivity of Silicon */
static const double ESi       = 11.7;
/*!\brief relative permittivity of Germanium */
static const double EGe       = 15.8;
/*!\brief Relative permittivity of Gallium(III) arsenide */
static const double EGaAs     = 13.1;
/*!\brief Intrinsic carrier concentration in 1/m^3 of Silicon */
static const double NiSi      = 1.45e16;
/*!\brief Intrinsic carrier concentration in 1/m^3 of Germanium */
static const double NiGe      = 2.40e19;
/*!\brief Intrinsic carrier concentration in 1/m^3 of Gallium(III) arsenide */
static const double NiGaAs    = 9.00e12;
/*!\brief Energy gap at 300K in eV of Silicon */
static const double EgSi      = 1.11;
/*!\brief Energy gap at 300K in eV of Schottky
  \todo What kind of metal (ideal, real, gold)?
*/
static const double EgSchottky = 0.69;
/*!\brief Energy gap at 300K in eV of Germanium */
static const double EgGe       = 0.67;
/*!\brief Energy gap at 300K in eV of Gallium(III) arsenide */
static const double EgGaAs     = 1.43;
/*!\brief Energy gap at 0K in eV of Silicon */
static const double Eg0Si      = 1.16;

/*!\brief Gmin
   \todo Define and document
*/
static const double GMin = NR_TINY;

/**
@}
*/

} // namespace qucs

#endif /* __CONSTANTS_H__ */
