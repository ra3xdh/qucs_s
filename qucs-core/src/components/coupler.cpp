/*
 * coupler.cpp - ideal coupler class implementation
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
#include "coupler.h"

using namespace qucs;

coupler::coupler () : circuit (4) {
  type = CIR_COUPLER;
}

void coupler::initSP (void) {
  allocMatrixS ();
  nr_double_t k = getPropertyDouble ("k");
  nr_double_t z = getPropertyDouble ("Z");
  nr_double_t p = deg2rad (getPropertyDouble ("phi"));
  nr_double_t r = (z0 - z) / (z0 + z);
  nr_double_t k2 = k * k;
  nr_double_t r2 = r * r;
  nr_complex_t a = k2 * (qucs::polar (1.0, 2 * p) + 1.0);
  nr_complex_t b = r2 * (1.0 - a);
  nr_complex_t c = k2 * (qucs::polar (1.0, 2 * p) - 1.0);
  nr_complex_t d = 1.0 - 2.0 * r2 * (1.0 + c) + b * b;
  nr_complex_t s = r * (a * b + c + qucs::polar (2 * r2 * k2, 2 * p)) / d;
  setS (NODE_1, NODE_1, s); setS (NODE_2, NODE_2, s);
  setS (NODE_3, NODE_3, s); setS (NODE_4, NODE_4, s);
  s = std::sqrt (1 - k2) * (1.0 - r2) * (1.0 - b) / d;
  setS (NODE_1, NODE_2, s); setS (NODE_2, NODE_1, s);
  setS (NODE_3, NODE_4, s); setS (NODE_4, NODE_3, s);
  s = qucs::polar (k, p) * (1.0 - r2) * (1.0 + b) / d;
  setS (NODE_1, NODE_3, s); setS (NODE_3, NODE_1, s);
  setS (NODE_2, NODE_4, s); setS (NODE_4, NODE_2, s);
  s = 2 * std::sqrt (1.0 - k2) * qucs::polar (k, p) * r * (1.0 - r2) / d;
  setS (NODE_1, NODE_4, s); setS (NODE_4, NODE_1, s);
  setS (NODE_2, NODE_3, s); setS (NODE_3, NODE_2, s);
}

void coupler::initDC (void) {
  setVoltageSources (2);
  setInternalVoltageSource (1);
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
  voltageSource (VSRC_2, NODE_3, NODE_4);
}

void coupler::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
  nr_double_t k = getPropertyDouble ("k");
  nr_double_t z = getPropertyDouble ("Z");
  nr_double_t p = deg2rad (getPropertyDouble ("phi"));
  nr_double_t b = 2 * std::sqrt (1 - k * k);
  nr_complex_t a = k * k * (qucs::polar (1.0, 2 * p) + 1.0);
  nr_complex_t c = qucs::polar (2 * k, p);
  nr_complex_t d = z * (a * a - c * c);
  nr_complex_t y;
  y = a * (2.0 - a) / d;
  setY (NODE_1, NODE_1, y); setY (NODE_2, NODE_2, y);
  setY (NODE_3, NODE_3, y); setY (NODE_4, NODE_4, y);
  y = -a * b / d;
  setY (NODE_1, NODE_2, y); setY (NODE_2, NODE_1, y);
  setY (NODE_3, NODE_4, y); setY (NODE_4, NODE_3, y);
  y = c * (a - 2.0) / d;
  setY (NODE_1, NODE_3, y); setY (NODE_3, NODE_1, y);
  setY (NODE_2, NODE_4, y); setY (NODE_4, NODE_2, y);
  y = b * c / d;
  setY (NODE_1, NODE_4, y); setY (NODE_4, NODE_1, y);
  setY (NODE_2, NODE_3, y); setY (NODE_3, NODE_2, y);
}

void coupler::initTR (void) {
  initDC ();
}

// properties
PROP_REQ [] = {
  { "k", PROP_REAL, { sqrt1_2, PROP_NO_STR }, PROP_NO_RANGE },
  { "phi", PROP_REAL, { 0, PROP_NO_STR }, PROP_RNGII (-180, +180) },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Z", PROP_REAL, { 50, PROP_NO_STR }, PROP_POS_RANGE },
  PROP_NO_PROP };
struct define_t coupler::cirdef =
  { "Coupler", 4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
