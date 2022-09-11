/*
 * gyrator.cpp - gyrator class implementation
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
#include "gyrator.h"

using namespace qucs;

gyrator::gyrator () : circuit (4) {
  type = CIR_GYRATOR;
#if AUGMENTED
  setVoltageSources (2);
#endif
}

void gyrator::initSP (void) {
  nr_double_t R = getPropertyDouble ("R");
  nr_double_t z = getPropertyDouble ("Zref");
  nr_double_t r = R / z;
  nr_double_t s1 = r * r / (r * r + 4.0);
  nr_double_t s2 = 2.0 * r / (r * r + 4.0);
  allocMatrixS ();
  setS (NODE_1, NODE_1, s1); setS (NODE_2, NODE_2, s1);
  setS (NODE_3, NODE_3, s1); setS (NODE_4, NODE_4, s1);
  setS (NODE_1, NODE_4, 1.0 - s1);
  setS (NODE_2, NODE_3, 1.0 - s1);
  setS (NODE_3, NODE_2, 1.0 - s1);
  setS (NODE_4, NODE_1, 1.0 - s1);
  setS (NODE_1, NODE_2, +s2); setS (NODE_2, NODE_4, +s2);
  setS (NODE_3, NODE_1, +s2); setS (NODE_4, NODE_3, +s2);
  setS (NODE_1, NODE_3, -s2); setS (NODE_2, NODE_1, -s2);
  setS (NODE_3, NODE_4, -s2); setS (NODE_4, NODE_2, -s2);
}

void gyrator::initDC (void) {
  nr_double_t r = getPropertyDouble ("R");
  allocMatrixMNA ();
#if AUGMENTED
  setB (NODE_1, VSRC_1, +1.0); setB (NODE_2, VSRC_1, +0.0);
  setB (NODE_3, VSRC_1, +0.0); setB (NODE_4, VSRC_1, -1.0);
  setB (NODE_2, VSRC_1, +0.0); setB (NODE_2, VSRC_2, +1.0);
  setB (NODE_3, VSRC_2, -1.0); setB (NODE_4, VSRC_2, +0.0);
  setC (VSRC_1, NODE_1, +0.0); setC (VSRC_1, NODE_2, +1/r);
  setC (VSRC_1, NODE_3, -1/r); setC (VSRC_1, NODE_4, +0.0);
  setC (VSRC_2, NODE_1, -1/r); setC (VSRC_2, NODE_2, +0.0);
  setC (VSRC_2, NODE_3, +0.0); setC (VSRC_2, NODE_4, +1/r);
  setD (VSRC_1, VSRC_1, -1.0); setD (VSRC_2, VSRC_2, -1.0);
  setD (VSRC_1, VSRC_2, +0.0); setD (VSRC_2, VSRC_1, +0.0);
  setE (VSRC_1, +0.0);
  setE (VSRC_2, +0.0);
#else
  setY (NODE_1, NODE_2, +1/r); setY (NODE_1, NODE_3, -1/r);
  setY (NODE_2, NODE_1, -1/r); setY (NODE_2, NODE_4, +1/r);
  setY (NODE_3, NODE_1, +1/r); setY (NODE_3, NODE_4, -1/r);
  setY (NODE_4, NODE_2, -1/r); setY (NODE_4, NODE_3, +1/r);
#endif
}

void gyrator::initAC (void) {
  initDC ();
}

void gyrator::initTR (void) {
  initDC ();
}

// properties
PROP_REQ [] = {
  { "R", PROP_REAL, { 50, PROP_NO_STR }, PROP_NO_RANGE }, PROP_NO_PROP };
PROP_OPT [] = {
  { "Zref", PROP_REAL, { 50, PROP_NO_STR }, PROP_POS_RANGE },
  PROP_NO_PROP };
struct define_t gyrator::cirdef =
  { "Gyrator", 4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
