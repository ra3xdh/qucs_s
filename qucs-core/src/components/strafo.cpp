/*
 * strafo.cpp - symmetrical trafo class implementation
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
#include "strafo.h"

using namespace qucs;

strafo::strafo () : circuit (6) {
  type = CIR_STRAFO;
}

void strafo::initSP (void) {

  nr_double_t t1 = getPropertyDouble ("T1");
  nr_double_t t2 = getPropertyDouble ("T2");

  nr_double_t  d = t1 * t1 + t2 * t2 + 1.0;
  nr_double_t z1 = t1 * t1 / d;
  nr_double_t z2 = t2 * t2 / d;
  nr_double_t z3 = 1.0 / d;
  nr_double_t z4 = t1 / d;
  nr_double_t z5 = t2 / d;
  nr_double_t z6 = t1 * t2 / d;

  allocMatrixS ();

  setS (NODE_1, NODE_1,  z1);    setS (NODE_1, NODE_2, z4);
  setS (NODE_1, NODE_3, -z4);    setS (NODE_1, NODE_4, -z6);
  setS (NODE_1, NODE_5, z6);     setS (NODE_1, NODE_6, 1 - z1);
  setS (NODE_2, NODE_1,  z4);    setS (NODE_2, NODE_2, z3);
  setS (NODE_2, NODE_3, 1 - z3); setS (NODE_2, NODE_4, -z5);
  setS (NODE_2, NODE_5, z5);     setS (NODE_2, NODE_6, -z4);
  setS (NODE_3, NODE_1, -z4);    setS (NODE_3, NODE_2, 1 - z3);
  setS (NODE_3, NODE_3, z3);     setS (NODE_3, NODE_4,  z5);
  setS (NODE_3, NODE_5, -z5);    setS (NODE_3, NODE_6, z4);
  setS (NODE_4, NODE_1, -z6);    setS (NODE_4, NODE_2, -z5);
  setS (NODE_4, NODE_3, z5);     setS (NODE_4, NODE_4,  z2);
  setS (NODE_4, NODE_5, 1 - z2); setS (NODE_4, NODE_6, z6);
  setS (NODE_5, NODE_1,  z6);    setS (NODE_5, NODE_2, z5);
  setS (NODE_5, NODE_3, -z5);    setS (NODE_5, NODE_4, 1 - z2);
  setS (NODE_5, NODE_5, z2);     setS (NODE_5, NODE_6, -z6);
  setS (NODE_6, NODE_1, 1 - z1); setS (NODE_6, NODE_2, -z4);
  setS (NODE_6, NODE_3, z4);     setS (NODE_6, NODE_4, z6);
  setS (NODE_6, NODE_5, -z6);    setS (NODE_6, NODE_6, z1);
}

void strafo::initDC (void) {
  setVoltageSources (3);
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_6);
  voltageSource (VSRC_2, NODE_5, NODE_4);
  voltageSource (VSRC_3, NODE_2, NODE_3);
}

void strafo::initAC (void) {
  nr_double_t t1 = getPropertyDouble ("T1");
  nr_double_t t2 = getPropertyDouble ("T2");

  setVoltageSources (2);
  allocMatrixMNA ();

  setB (NODE_1, VSRC_1, -1.0); setB (NODE_2, VSRC_1, + t1);
  setB (NODE_3, VSRC_1, - t1); setB (NODE_4, VSRC_1, +0.0);
  setB (NODE_5, VSRC_1, +0.0); setB (NODE_6, VSRC_1, +1.0);
  setB (NODE_1, VSRC_2, +0.0); setB (NODE_2, VSRC_2, + t2);
  setB (NODE_3, VSRC_2, - t2); setB (NODE_4, VSRC_2, +1.0);
  setB (NODE_5, VSRC_2, -1.0); setB (NODE_6, VSRC_2, +0.0);

  setC (VSRC_1, NODE_1, +1.0); setC (VSRC_1, NODE_2, - t1);
  setC (VSRC_1, NODE_3, + t1); setC (VSRC_1, NODE_4, +0.0);
  setC (VSRC_1, NODE_5, +0.0); setC (VSRC_1, NODE_6, -1.0);
  setC (VSRC_2, NODE_1, +0.0); setC (VSRC_2, NODE_2, - t2);
  setC (VSRC_2, NODE_3, + t2); setC (VSRC_2, NODE_4, -1.0);
  setC (VSRC_2, NODE_5, +1.0); setC (VSRC_2, NODE_6, +0.0);

  setD (VSRC_1, VSRC_1, 0); setD (VSRC_2, VSRC_2, 0);
  setD (VSRC_1, VSRC_2, 0); setD (VSRC_2, VSRC_1, 0);
  setE (VSRC_1, 0.0);
  setE (VSRC_2, 0.0);
}

void strafo::initTR (void) {
  initAC ();
}

// properties
PROP_REQ [] = {
  { "T1", PROP_REAL, { 1, PROP_NO_STR }, PROP_POS_RANGE },
  { "T2", PROP_REAL, { 1, PROP_NO_STR }, PROP_POS_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  PROP_NO_PROP };
struct define_t strafo::cirdef =
  { "sTr", 6, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
