/*
 * vnoise.cpp - noise voltage source class implementation
 *
 * Copyright (C) 2004, 2005, 2008 Stefan Jahn <stefan@lkcc.org>
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
#include "vnoise.h"

using namespace qucs;

vnoise::vnoise () : circuit (2) {
  type = CIR_VNOISE;
  setVoltageSources (1);
}

void vnoise::initSP (void) {
  allocMatrixS ();
  setS (NODE_1, NODE_1, 0.0);
  setS (NODE_1, NODE_2, 1.0);
  setS (NODE_2, NODE_1, 1.0);
  setS (NODE_2, NODE_2, 0.0);
}

void vnoise::calcNoiseSP (nr_double_t frequency) {
  nr_double_t u = getPropertyDouble ("u");
  nr_double_t e = getPropertyDouble ("e");
  nr_double_t c = getPropertyDouble ("c");
  nr_double_t a = getPropertyDouble ("a");
  nr_double_t vpsd = u / (a + c * qucs::pow (frequency, e)) / kB / T0 / 4 / z0;
  setN (NODE_1, NODE_1, +vpsd); setN (NODE_2, NODE_2, +vpsd);
  setN (NODE_1, NODE_2, -vpsd); setN (NODE_2, NODE_1, -vpsd);
}

void vnoise::initDC (void) {
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
}

void vnoise::initTR (void) {
  initDC ();
}

void vnoise::initAC (void) {
  initDC ();
}

void vnoise::calcNoiseAC (nr_double_t frequency) {
  nr_double_t u = getPropertyDouble ("u");
  nr_double_t e = getPropertyDouble ("e");
  nr_double_t c = getPropertyDouble ("c");
  nr_double_t a = getPropertyDouble ("a");
  nr_double_t ipsd = u / (a + c * qucs::pow (frequency, e)) / kB / T0;
  setN (NODE_3, NODE_3, +ipsd);
}

// properties
PROP_REQ [] = {
  { "u", PROP_REAL, { 1e-6, PROP_NO_STR }, PROP_POS_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "a", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGE },
  { "c", PROP_REAL, { 1, PROP_NO_STR }, PROP_POS_RANGE },
  { "e", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGE },
  PROP_NO_PROP };
struct define_t vnoise::cirdef =
  { "Vnoise", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
