/*
 * relais.cpp - relais class implementation
 *
 * Copyright (C) 2006, 2008, 2009 Stefan Jahn <stefan@lkcc.org>
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
#include "relais.h"

using namespace qucs;

relais::relais () : circuit (4) {
  type = CIR_RELAIS;
  setVoltageSources (1);
}

void relais::initSP (void) {
  allocMatrixS ();
  setS (NODE_1, NODE_1, 1.0); setS (NODE_4, NODE_4, 1.0);
  setS (NODE_2, NODE_2, r / (r + 2));
  setS (NODE_3, NODE_3, r / (r + 2));
  setS (NODE_2, NODE_3, 2 / (r + 2));
  setS (NODE_3, NODE_2, 2 / (r + 2));
}

void relais::calcNoiseSP (nr_double_t) {
  nr_double_t T = getPropertyDouble ("Temp");
  nr_double_t f = celsius2kelvin (T) * 4.0 * r * z0 / qucs::sqr (2.0 * z0 + r) / T0;
  setN (NODE_2, NODE_2, +f); setN (NODE_3, NODE_3, +f);
  setN (NODE_2, NODE_3, -f); setN (NODE_3, NODE_2, -f);
}

void relais::calcNoiseAC (nr_double_t) {
  if (r > 0.0 || r < 0.0) {
    nr_double_t T = getPropertyDouble ("Temp");
    nr_double_t f = celsius2kelvin (T) / T0 * 4.0 / r;
    setN (NODE_2, NODE_2, +f); setN (NODE_3, NODE_3, +f);
    setN (NODE_2, NODE_3, -f); setN (NODE_3, NODE_2, -f);
  }
}

void relais::initDC (void) {
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_2, NODE_3);
  state = 0;
  r = 0.0;
}

#define REAL_OFF 0
#define REAL_ON  1
#define HYST_OFF 2
#define HYST_ON  3

void relais::calcDC (void) {
  nr_double_t vt   = getPropertyDouble ("Vt");
  nr_double_t vh   = getPropertyDouble ("Vh");
  nr_double_t von  = vt + vh;
  nr_double_t voff = vt - vh;
  nr_double_t ron  = getPropertyDouble ("Ron");
  nr_double_t roff = getPropertyDouble ("Roff");
  nr_double_t v = real (getV (NODE_1) - getV (NODE_4));
  if (state == REAL_OFF) {
    if (v >= von) {
      state = REAL_ON;
    }
  } else if (state == REAL_ON) {
    if (v <= voff) {
      state = REAL_OFF;
    }
  }
  if (state == REAL_ON) {
    r = ron;
  } else if (state == REAL_OFF) {
    r = roff;
  }
  setD (VSRC_1, VSRC_1, -r);
}

void relais::saveOperatingPoints (void) {
  setOperatingPoint ("R", r);
}

void relais::initAC (void) {
  initDC ();
  setD (VSRC_1, VSRC_1, -r);
}

void relais::initTR (void) {
  initDC ();
}

void relais::calcTR (nr_double_t) {
  calcDC ();
}

// properties
PROP_REQ [] = {
  { "Vt", PROP_REAL, { 0.5, PROP_NO_STR }, PROP_NO_RANGE },
  { "Vh", PROP_REAL, { 0.1, PROP_NO_STR }, PROP_POS_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Ron", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGE },
  { "Roff", PROP_REAL, { 1e12, PROP_NO_STR }, PROP_POS_RANGE },
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };
struct define_t relais::cirdef =
  { "Relais", 4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_NONLINEAR, PROP_DEF };
