/*
 * vpm.cpp - PM modulated AC voltage source class implementation
 *
 * Copyright (C) 2006, 2008 Stefan Jahn <stefan@lkcc.org>
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
#include "vpm.h"

using namespace qucs;

vpm::vpm () : circuit (3) {
  type = CIR_VPM;
  setVSource (true);
  setVoltageSources (1);
}

void vpm::initSP (void) {
  allocMatrixS ();
  setS (NODE_1, NODE_2, 1.0);
  setS (NODE_2, NODE_1, 1.0);
  setS (NODE_3, NODE_3, 1.0);
}

void vpm::initDC (void) {
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
}

void vpm::initAC (void) {
  initDC ();
  nr_double_t a = getPropertyDouble ("U");
  nr_double_t p = getPropertyDouble ("Phase");
  setE (VSRC_1, qucs::polar (a, deg2rad (p)));
}

void vpm::initTR (void) {
  initDC ();
}

void vpm::calcTR (nr_double_t t) {
  nr_double_t f = getPropertyDouble ("f");
  nr_double_t p = getPropertyDouble ("Phase");
  nr_double_t d = getPropertyDouble ("M") * 2 * pi;
  nr_double_t a = getPropertyDouble ("U");
  nr_double_t v = real (getV  (NODE_3));
  nr_double_t b = 2 * pi * f * t + deg2rad (p) + v * d;
  nr_double_t u = a * std::sin (b);
  nr_double_t g = a * std::cos (b) * d;
  setE (VSRC_1, g * v - u);
  setC (VSRC_1, NODE_3, g);
}

// properties
PROP_REQ [] = {
  { "U", PROP_REAL, { 1, PROP_NO_STR }, PROP_NO_RANGE },
  { "f", PROP_REAL, { 1e9, PROP_NO_STR }, PROP_POS_RANGE },
  { "M", PROP_REAL, { 1, PROP_NO_STR }, PROP_RNGII (0, 1) },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Phase", PROP_REAL, { 0, PROP_NO_STR }, PROP_RNGII (-360, 360) },
  PROP_NO_PROP };
struct define_t vpm::cirdef =
  { "PM_Mod", 3, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
