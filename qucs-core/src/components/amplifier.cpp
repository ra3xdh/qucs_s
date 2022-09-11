/*
 * amplifier.cpp - amplifier class implementation
 *
 * Copyright (C) 2004, 2008, 2010 Stefan Jahn <stefan@lkcc.org>
 * Copyright (C) 2008 Michael Margraf <Michael.Margraf@alumni.TU-Berlin.DE>
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

/*! \file amplifier.cpp
    \brief amplifier class implementation

    An ideal amplifier increases signal strength from input
    to output and blocks all signals flowing into the output.
*/

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "component.h"
#include "amplifier.h"

using namespace qucs;

/*! Constructor */
amplifier::amplifier () : circuit (2) {
  type = CIR_AMPLIFIER;
}

/*! Initialize S-parameter simulation.
    An ideal amplifier is characterized by the following
    S-matrix
    \f[
      S=\begin{pmatrix}
         \dfrac{Z_1-Z_0}{Z_1+Z_0} & 0 \\
	  \dfrac{4\cdot Z_0\cdot\sqrt{Z_1\cdot Z_2}\cdot G}{(Z_1+Z_0)\cdot(Z_2+Z_0)}
          & \dfrac{Z_2-Z_0}{Z_2+Z_0}
        \end{pmatrix}
    \f]
    With \f$Z_1\f$ and \f$Z_2\f$ the impedance of the port 1 and 2 and
    \f$G\f$ the gain.
*/
void amplifier::initSP (void) {
  nr_double_t g = getPropertyDouble ("G");
  nr_double_t z1 = getPropertyDouble ("Z1");
  nr_double_t z2 = getPropertyDouble ("Z2");

  allocMatrixS ();

  setS (NODE_1, NODE_1, (z1 - z0) / (z1 + z0));
  setS (NODE_1, NODE_2, 0);
  setS (NODE_2, NODE_2, (z2 - z0) / (z2 + z0));
  setS (NODE_2, NODE_1, 4 * z0 * std::sqrt (z1 * z2) * g / (z1 + z0) / (z2 + z0));
}

void amplifier::calcNoiseSP (nr_double_t) {
  nr_double_t g = getPropertyDouble ("G");
  nr_double_t z2 = getPropertyDouble ("Z2");
  nr_double_t NF = getPropertyDouble ("NF");
  setN (NODE_1, NODE_1, 0);
  setN (NODE_2, NODE_2, 4 * z0 * z2 * sqr (g) * (NF - 1) / sqr (z2 + z0));
  setN (NODE_1, NODE_2, 0);
  setN (NODE_2, NODE_1, 0);
}

/*! DC model initialization.
    An ideal amplifier is characterized by the following
    Y-matrix:
    \f[
    \begin{pmatrix}
    \dfrac{1}{Z_1} & 0 \\
    \dfrac{-2}{\sqrt{Z_1\cdot Z_2}} & \dfrac{1}{Z_2}
    \end{pmatrix}
    \f]
    With \f$Z_1\f$ and \f$Z_2\f$ the impedance of the port 1 and 2 and
    \f$G\f$ the gain.
*/
void amplifier::initDC (void) {
  nr_double_t g = getPropertyDouble ("G");
  nr_double_t z1 = getPropertyDouble ("Z1");
  nr_double_t z2 = getPropertyDouble ("Z2");

  allocMatrixMNA ();

  setY (NODE_1, NODE_1, 1 / z1);
  setY (NODE_1, NODE_2, 0);
  setY (NODE_2, NODE_1, -2 * g / std::sqrt (z1 * z2));
  setY (NODE_2, NODE_2, 1 / z2);
}

/*! AC model initialization.

    Idem than DC model.
*/
void amplifier::initAC (void) {
  initDC ();
}

void amplifier::calcNoiseAC (nr_double_t) {
  nr_double_t g = getPropertyDouble ("G");
  nr_double_t z2 = getPropertyDouble ("Z2");
  nr_double_t NF = getPropertyDouble ("NF");
  setN (NODE_1, NODE_1, 0);
  setN (NODE_2, NODE_2, 4 * sqr (g) * (NF - 1) / z2);
  setN (NODE_1, NODE_2, 0);
  setN (NODE_2, NODE_1, 0);
}

/*! Transient model initialization.

    Idem than DC model.
*/
void amplifier::initTR (void) {
  initDC ();
}

// properties
PROP_REQ [] = {
  { "G", PROP_REAL, { 10, PROP_NO_STR }, PROP_MIN_VAL (1) },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Z1", PROP_REAL, { 50, PROP_NO_STR }, PROP_POS_RANGE },
  { "Z2", PROP_REAL, { 50, PROP_NO_STR }, PROP_POS_RANGE },
  { "NF", PROP_REAL, { 1, PROP_NO_STR }, PROP_MIN_VAL (1) },
  PROP_NO_PROP };
struct define_t amplifier::cirdef =
  { "Amp", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
