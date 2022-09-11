/*
 * iac.cpp - AC current source class implementation
 *
 * Copyright (C) 2004, 2006, 2007, 2008 Stefan Jahn <stefan@lkcc.org>
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
#include "iac.h"

using namespace qucs;

iac::iac () : circuit (2) {
  type = CIR_IAC;
  setISource (true);
}

void iac::initSP (void) {
  allocMatrixS ();
  setS (NODE_1, NODE_1, 1.0);
  setS (NODE_1, NODE_2, 0.0);
  setS (NODE_2, NODE_1, 0.0);
  setS (NODE_2, NODE_2, 1.0);
}

void iac::initDC (void) {
  allocMatrixMNA ();
  clearI ();
}

void iac::initAC (void) {
  nr_double_t a = getPropertyDouble ("I");
  nr_double_t p = getPropertyDouble ("Phase");
  nr_complex_t i = qucs::polar (a, deg2rad (p));
  allocMatrixMNA ();
  setI (NODE_1, +i); setI (NODE_2, -i);
}

void iac::calcTR (nr_double_t t) {
  nr_double_t f = getPropertyDouble ("f");
  nr_double_t p = getPropertyDouble ("Phase");
  nr_double_t d = getPropertyDouble ("Theta");
  nr_double_t a = getPropertyDouble ("I");
  nr_double_t s = getNet()->getSrcFactor ();
  nr_double_t o = 2 * pi * f;
  nr_double_t T = p / f / 360;
  nr_double_t i = s * a * std::exp (-(t + T) * d * f) * std::sin (o * t + deg2rad (p));
  setI (NODE_1, +i); setI (NODE_2, -i);
}

// properties
PROP_REQ [] = {
  { "I", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_NO_RANGE }, PROP_NO_PROP };
PROP_OPT [] = {
  { "Phase", PROP_REAL, { 0, PROP_NO_STR }, PROP_RNGII (-360, 360) },
  { "Theta", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGE },
  { "f", PROP_REAL, { 1e9, PROP_NO_STR }, PROP_POS_RANGE },
  PROP_NO_PROP };
struct define_t iac::cirdef =
  { "Iac", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
