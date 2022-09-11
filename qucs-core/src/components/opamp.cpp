/*
 * opamp.cpp - operational amplifier class implementation
 *
 * Copyright (C) 2004, 2008 Stefan Jahn <stefan@lkcc.org>
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
#include "opamp.h"

#define NODE_INM 0
#define NODE_INP 1
#define NODE_OUT 2

using namespace qucs;

opamp::opamp () : circuit (3) {
  type = CIR_OPAMP;
  setVoltageSources (1);
}

void opamp::initSP (void) {
  allocMatrixS ();
  setS (NODE_INP, NODE_INP, 1);
  setS (NODE_INP, NODE_OUT, 0);
  setS (NODE_INP, NODE_INM, 0);
  setS (NODE_INM, NODE_INP, 0);
  setS (NODE_INM, NODE_OUT, 0);
  setS (NODE_INM, NODE_INM, 1);
  setS (NODE_OUT, NODE_INP, +4 * gv);
  setS (NODE_OUT, NODE_OUT, -1);
  setS (NODE_OUT, NODE_INM, -4 * gv);
}

void opamp::initDC (void) {
  allocMatrixMNA ();
  setB (NODE_INP, VSRC_1, 0);
  setB (NODE_OUT, VSRC_1, 1);
  setB (NODE_INM, VSRC_1, 0);
  setC (VSRC_1, NODE_OUT, -1); setD (VSRC_1, VSRC_1, 0); setE (VSRC_1, 0);
}

void opamp::calcDC (void) {
  nr_double_t g    = getPropertyDouble ("G");
  nr_double_t uMax = getPropertyDouble ("Umax");
  nr_double_t Uin  = real (getV (NODE_INP) - getV (NODE_INM));
  nr_double_t Uout = uMax * two_over_pi * qucs::atan (Uin * g * pi_over_2 / uMax);
  gv = g / (1 + sqr (pi_over_2 / uMax * g * Uin)) + GMin;
  setC (VSRC_1, NODE_INP, +gv);
  setC (VSRC_1, NODE_INM, -gv);
  setE (VSRC_1, Uin * gv - Uout);
}

void opamp::calcOperatingPoints (void) {
  setOperatingPoint ("g", gv);
}

void opamp::initAC (void) {
  initDC ();
  setC (VSRC_1, NODE_INP, +gv);
  setC (VSRC_1, NODE_INM, -gv);
}

void opamp::initTR (void) {
  initDC ();
}

void opamp::calcTR (nr_double_t) {
  calcDC ();
}

// properties
PROP_REQ [] = {
  { "G", PROP_REAL, { 1e6, PROP_NO_STR }, PROP_MIN_VAL (1) },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Umax", PROP_REAL, { 15, PROP_NO_STR }, PROP_POS_RANGE },
  PROP_NO_PROP };
struct define_t opamp::cirdef =
  { "OpAmp", 3, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_NONLINEAR, PROP_DEF };
