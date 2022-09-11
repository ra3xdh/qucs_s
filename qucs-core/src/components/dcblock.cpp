/*
 * dcblock.cpp - DC block class implementation
 *
 * Copyright (C) 2003, 2004, 2005, 2008 Stefan Jahn <stefan@lkcc.org>
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
#include "dcblock.h"

using namespace qucs;

dcblock::dcblock () : circuit (2) {
  type = CIR_DCBLOCK;
}

void dcblock::initSP (void) {
  allocMatrixS ();
  setS (NODE_1, NODE_1, 0.0);
  setS (NODE_2, NODE_2, 0.0);
  setS (NODE_1, NODE_2, 1.0);
  setS (NODE_2, NODE_1, 1.0);
}

void dcblock::initDC (void) {
  setISource (false);
  setVoltageSources (0);
  allocMatrixMNA ();
}

void dcblock::initAC (void) {
  setISource (false);
  setVoltageSources (1);
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
}

#define qState 0 // charge state
#define cState 1 // current state

void dcblock::initTR (void) {
  setStates (2);
  initDC ();
  setISource (true);
}

void dcblock::calcTR (nr_double_t) {
  nr_double_t c = getPropertyDouble ("C");
  nr_double_t g, i;
  nr_double_t v = real (getV (NODE_1) - getV (NODE_2));

  setState (qState, c * v);
  integrate (qState, c, g, i);
  setY (NODE_1, NODE_1, +g); setY (NODE_2, NODE_2, +g);
  setY (NODE_1, NODE_2, -g); setY (NODE_2, NODE_1, -g);
  setI (NODE_1 , -i);
  setI (NODE_2 , +i);
}

// properties
PROP_REQ [] = {
  PROP_NO_PROP };
PROP_OPT [] = {
  { "C", PROP_REAL, { 1e-6, PROP_NO_STR }, PROP_POS_RANGE },
  PROP_NO_PROP };
struct define_t dcblock::cirdef =
  { "DCBlock", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
