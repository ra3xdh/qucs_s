/*
 * pac.cpp - AC power source class implementation
 *
 * Copyright (C) 2003, 2004, 2005, 2006 Stefan Jahn <stefan@lkcc.org>
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
#include "pac.h"

using namespace qucs;

pac::pac () : circuit (2) {
  type = CIR_PAC;
  setISource (true);
}

void pac::calcSP (nr_double_t) {
  nr_double_t z = getPropertyDouble ("Z") / z0;
  setS (NODE_1, NODE_1, z / (z + 2));
  setS (NODE_2, NODE_2, z / (z + 2));
  setS (NODE_1, NODE_2, 2 / (z + 2));
  setS (NODE_2, NODE_1, 2 / (z + 2));
}

void pac::calcNoiseSP (nr_double_t) {
  nr_double_t r = getPropertyDouble ("Z");
  nr_double_t T = getPropertyDouble ("Temp");
  nr_double_t f = celsius2kelvin (T) * 4.0 * r * z0 / qucs::sqr (2.0 * z0 + r) / T0;
  setN (NODE_1, NODE_1, +f); setN (NODE_2, NODE_2, +f);
  setN (NODE_1, NODE_2, -f); setN (NODE_2, NODE_1, -f);
}

void pac::calcDC (void) {
  nr_double_t g = 1.0 / getPropertyDouble ("Z");
  clearI ();
  setY (NODE_1, NODE_1, +g); setY (NODE_2, NODE_2, +g);
  setY (NODE_1, NODE_2, -g); setY (NODE_2, NODE_1, -g);
}

void pac::calcAC (nr_double_t) {
  nr_double_t p = getPropertyDouble ("P");
  nr_double_t r = getPropertyDouble ("Z");
  nr_double_t i = std::sqrt (8 * p / r);
  calcDC ();
  setI (NODE_1, +i); setI (NODE_2, -i);
}

void pac::calcNoiseAC (nr_double_t) {
  nr_double_t r = getPropertyDouble ("Z");
  nr_double_t T = getPropertyDouble ("Temp");
  nr_double_t f = celsius2kelvin (T) / T0 * 4.0 / r;
  setN (NODE_1, NODE_1, +f); setN (NODE_2, NODE_2, +f);
  setN (NODE_1, NODE_2, -f); setN (NODE_2, NODE_1, -f);
}

void pac::calcTR (nr_double_t t) {
  nr_double_t p = getPropertyDouble ("P");
  nr_double_t r = getPropertyDouble ("Z");
  nr_double_t f = getPropertyDouble ("f");
  nr_double_t i = std::sqrt (8 * p / r) * std::sin (2 * pi * f * t);
  calcDC ();
  setI (NODE_1, +i); setI (NODE_2, -i);
}

void pac::initHB (void) {
  setVoltageSources (1);
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
  nr_double_t g = 1.0 / getPropertyDouble ("Z");
  setY (NODE_1, NODE_1, +g); setY (NODE_2, NODE_2, +g);
  setY (NODE_1, NODE_2, -g); setY (NODE_2, NODE_1, -g);
}

void pac::calcHB (nr_double_t frequency) {
  nr_double_t f = getPropertyDouble ("f");
  if (f == frequency) {
    nr_double_t p = getPropertyDouble ("P");
    nr_double_t r = getPropertyDouble ("Z");
    nr_double_t u = std::sqrt (4 * p * r);
    setE (VSRC_1, u);
  }
  else {
    setE (VSRC_1, 0);
  }
}

// properties
PROP_REQ [] = {
  { "f", PROP_REAL, { 1e9, PROP_NO_STR }, PROP_POS_RANGE },
  { "Z", PROP_REAL, { 50, PROP_NO_STR }, PROP_POS_RANGEX },
  { "Num", PROP_INT, { 1, PROP_NO_STR }, PROP_RNGII (1, MAX_PORTS) },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "P", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGE },
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };
struct define_t pac::cirdef =
  { "Pac", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
