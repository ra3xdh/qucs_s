/*
 * cpwopen.cpp - coplanar waveguide open end class implementation
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
#include "substrate.h"
#include "cpwline.h"
#include "cpwopen.h"

using namespace qucs;

cpwopen::cpwopen () : circuit (1) {
  type = CIR_CPWOPEN;
}

// Returns the coplanar open end capacitance.
nr_double_t cpwopen::calcCend (nr_double_t frequency) {

  // get properties of substrate and coplanar open
  nr_double_t W =  getPropertyDouble ("W");
  nr_double_t s =  getPropertyDouble ("S");
  substrate * subst = getSubstrate ();
  nr_double_t er = subst->getPropertyDouble ("er");
  nr_double_t h  = subst->getPropertyDouble ("h");
  nr_double_t t  = subst->getPropertyDouble ("t");
  int backMetal  = !strcmp (getPropertyString ("Backside"), "Metal");

  nr_double_t ZlEff, ErEff, ZlEffFreq, ErEffFreq;
  cpwline::analyseQuasiStatic (W, s, h, t, er, backMetal, ZlEff, ErEff);
  cpwline::analyseDispersion  (W, s, h, er, ZlEff, ErEff, frequency,
			       ZlEffFreq, ErEffFreq);
  nr_double_t dl = (W / 2 + s) / 2;
  return dl * ErEffFreq / C0 / ZlEffFreq;
}

void cpwopen::initSP (void) {
  allocMatrixS ();
  checkProperties ();
}

void cpwopen::calcSP (nr_double_t frequency) {
  setS (NODE_1, NODE_1, ztor (1.0 / calcY (frequency)));
}

void cpwopen::checkProperties (void) {
  nr_double_t W = getPropertyDouble ("W");
  nr_double_t s = getPropertyDouble ("S");
  nr_double_t g = getPropertyDouble ("G");
  if (g <= W + s + s) {
    logprint (LOG_ERROR, "WARNING: Model for coplanar open end valid for "
	      "g > 2b (2b = %g)\n", W + s + s);
  }
  nr_double_t ab = W / (W + s + s);
  if (ab < 0.2 || ab > 0.8) {
    logprint (LOG_ERROR, "WARNING: Model for coplanar open end valid for "
	      "0.2 < a/b < 0.8 (a/b = %g)\n", ab);
  }
}

nr_complex_t cpwopen::calcY (nr_double_t frequency) {
  nr_double_t o = 2 * pi * frequency;
  nr_double_t c = calcCend (frequency);
  return nr_complex_t (0, c * o);
}

void cpwopen::initDC (void) {
  allocMatrixMNA ();
  setY (NODE_1, NODE_1, 0);
}

void cpwopen::initAC (void) {
  allocMatrixMNA ();
  checkProperties ();
}

void cpwopen::calcAC (nr_double_t frequency) {
  setY (NODE_1, NODE_1, calcY (frequency));
}

// properties
PROP_REQ [] = {
  { "W", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "S", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "G", PROP_REAL, { 5e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "Subst", PROP_STR, { PROP_NO_VAL, "Subst1" }, PROP_NO_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Backside", PROP_STR, { PROP_NO_VAL, "Metal" },
    PROP_RNG_STR2 ("Metal", "Air") },
  PROP_NO_PROP };
struct define_t cpwopen::cirdef =
  { "COPEN", 1, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
