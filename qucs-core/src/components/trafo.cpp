/*
 * trafo.cpp - trafo class implementation
 *
 * Copyright (C) 2003, 2004, 2005, 2008, 2011 Stefan Jahn <stefan@lkcc.org>
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
#include "trafo.h"

using namespace qucs;

trafo::trafo () : circuit (4) {
  type = CIR_TRAFO;
}

void trafo::initSP (void) {
  nr_double_t  t = getPropertyDouble ("T");
  nr_double_t  d = t * t + 1.0;
  nr_double_t z1 = t * t / d;
  nr_double_t z2 = t / d;
  nr_double_t z3 = 1 / d;
  allocMatrixS ();
  setS (NODE_1, NODE_1,  z1); setS (NODE_1, NODE_2,  z2);
  setS (NODE_1, NODE_3, -z2); setS (NODE_1, NODE_4,  z3);
  setS (NODE_2, NODE_1,  z2); setS (NODE_2, NODE_2,  z3);
  setS (NODE_2, NODE_3,  z1); setS (NODE_2, NODE_4, -z2);
  setS (NODE_3, NODE_1, -z2); setS (NODE_3, NODE_2,  z1);
  setS (NODE_3, NODE_3,  z3); setS (NODE_3, NODE_4,  z2);
  setS (NODE_4, NODE_1,  z3); setS (NODE_4, NODE_2, -z2);
  setS (NODE_4, NODE_3,  z2); setS (NODE_4, NODE_4,  z1);
}

void trafo::initDC (void) {
  setVoltageSources (2);
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_4);
  voltageSource (VSRC_2, NODE_2, NODE_3);
}

void trafo::initAC (void) {
  nr_double_t t = getPropertyDouble ("T");
  setVoltageSources (1);
  allocMatrixMNA ();
  setB (NODE_1, VSRC_1, -1); setB (NODE_2, VSRC_1, +t);
  setB (NODE_3, VSRC_1, -t); setB (NODE_4, VSRC_1, +1);
  setC (VSRC_1, NODE_1, +1); setC (VSRC_1, NODE_2, -t);
  setC (VSRC_1, NODE_3, +t); setC (VSRC_1, NODE_4, -1);
  setD (VSRC_1, VSRC_1, +0);
  setE (VSRC_1, 0);
}

void trafo::initTR (void) {
  initAC ();
}

// properties
PROP_REQ [] = {
  { "T", PROP_REAL, { 1, PROP_NO_STR }, PROP_POS_RANGE }, PROP_NO_PROP };
PROP_OPT [] = {
  PROP_NO_PROP };
struct define_t trafo::cirdef =
  { "Tr", 4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
