/*
 * iinoise.cpp - correlated noise current sources class implementation
 *
 * Copyright (C) 2005, 2008 Stefan Jahn <stefan@lkcc.org>
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
#include "iinoise.h"

#define NODE_I1P 0
#define NODE_I1N 3
#define NODE_I2P 1
#define NODE_I2N 2

using namespace qucs;

iinoise::iinoise () : circuit (4) {
  type = CIR_IINOISE;
}

void iinoise::initSP (void) {
  allocMatrixS ();
  setS (NODE_I1P, NODE_I1P, 1.0);
  setS (NODE_I1N, NODE_I1N, 1.0);
  setS (NODE_I2P, NODE_I2P, 1.0);
  setS (NODE_I2N, NODE_I2N, 1.0);
}

void iinoise::calcNoiseSP (nr_double_t frequency) {
  setMatrixN (cytocs (calcMatrixCy (frequency) * z0, getMatrixS ()));
}

matrix iinoise::calcMatrixCy (nr_double_t frequency) {
  nr_double_t C = getPropertyDouble ("C");
  nr_double_t e = getPropertyDouble ("e");
  nr_double_t c = getPropertyDouble ("c");
  nr_double_t a = getPropertyDouble ("a");
  nr_double_t k = a + c * qucs::pow (frequency, e);
  nr_double_t i1 = getPropertyDouble ("i1") / k / kB / T0;
  nr_double_t i2 = getPropertyDouble ("i2") / k / kB / T0;
  nr_double_t ci = C * std::sqrt (i1 * i2);

  matrix cy = matrix (4);
  // entries of source 1
  cy.set (NODE_I1P, NODE_I1P, +i1); cy.set (NODE_I1N, NODE_I1N, +i1);
  cy.set (NODE_I1P, NODE_I1N, -i1); cy.set (NODE_I1N, NODE_I1P, -i1);
  // entries of source 2
  cy.set (NODE_I2P, NODE_I2P, +i2); cy.set (NODE_I2N, NODE_I2N, +i2);
  cy.set (NODE_I2P, NODE_I2N, -i2); cy.set (NODE_I2N, NODE_I2P, -i2);
  // correlation entries
  cy.set (NODE_I1P, NODE_I2P, +ci); cy.set (NODE_I1N, NODE_I2N, +ci);
  cy.set (NODE_I1P, NODE_I2N, -ci); cy.set (NODE_I1N, NODE_I2P, -ci);
  cy.set (NODE_I2P, NODE_I1P, +ci); cy.set (NODE_I2N, NODE_I1N, +ci);
  cy.set (NODE_I2P, NODE_I1N, -ci); cy.set (NODE_I2N, NODE_I1P, -ci);
  return cy;
}

void iinoise::calcNoiseAC (nr_double_t frequency) {
  setMatrixN (calcMatrixCy (frequency));
}

// properties
PROP_REQ [] = {
  { "i1", PROP_REAL, { 1e-6, PROP_NO_STR }, PROP_POS_RANGE },
  { "i2", PROP_REAL, { 1e-6, PROP_NO_STR }, PROP_POS_RANGE },
  { "C", PROP_REAL, { 0.5, PROP_NO_STR }, PROP_RNGII (-1, 1) },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "a", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGE },
  { "c", PROP_REAL, { 1, PROP_NO_STR }, PROP_POS_RANGE },
  { "e", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGE },
  PROP_NO_PROP };
struct define_t iinoise::cirdef =
  { "IInoise", 4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
