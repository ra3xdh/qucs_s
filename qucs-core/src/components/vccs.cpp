/*
 * vccs.cpp - vccs class implementation
 *
 * Copyright (C) 2003, 2004, 2005, 2006, 2008 Stefan Jahn <stefan@lkcc.org>
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
#include "vccs.h"

using namespace qucs;

vccs::vccs () : circuit (4) {
  type = CIR_VCCS;
}

void vccs::calcSP (nr_double_t frequency) {
  nr_double_t g = getPropertyDouble ("G") * z0;
  nr_double_t t = getPropertyDouble ("T");

  nr_complex_t z1 = qucs::polar (2.0 * g, pi - 2.0 * pi * frequency * t);
  nr_complex_t z2 = qucs::polar (2.0 * g, - 2.0 * pi * frequency * t);

  setS (NODE_1, NODE_1, 1.0); setS (NODE_1, NODE_2, 0.0);
  setS (NODE_1, NODE_3, 0.0); setS (NODE_1, NODE_4, 0.0);
  setS (NODE_2, NODE_1, z1);  setS (NODE_2, NODE_2, 1.0);
  setS (NODE_2, NODE_3, 0.0); setS (NODE_2, NODE_4, z2);
  setS (NODE_3, NODE_1, z2);  setS (NODE_3, NODE_2, 0.0);
  setS (NODE_3, NODE_3, 1.0); setS (NODE_3, NODE_4, z1);
  setS (NODE_4, NODE_1, 0.0); setS (NODE_4, NODE_2, 0.0);
  setS (NODE_4, NODE_3, 0.0); setS (NODE_4, NODE_4, 1.0);
}

void vccs::initDC (void) {
  setISource (false);
  allocMatrixMNA ();
  nr_double_t g = getPropertyDouble ("G");
  setY (NODE_2, NODE_1, +g); setY (NODE_3, NODE_4, +g);
  setY (NODE_3, NODE_1, -g); setY (NODE_2, NODE_4, -g);
}

void vccs::initAC (void) {
  initDC ();
}

void vccs::calcAC (nr_double_t frequency) {
  nr_double_t t = getPropertyDouble ("T");
  nr_complex_t g = qucs::polar (getPropertyDouble ("G"),
			  - 2.0 * pi * frequency * t);
  setY (NODE_2, NODE_1, +g); setY (NODE_3, NODE_4, +g);
  setY (NODE_3, NODE_1, -g); setY (NODE_2, NODE_4, -g);
}

void vccs::initTR (void) {
  nr_double_t t = getPropertyDouble ("T");
  initDC ();
  deleteHistory ();
  if (t > 0.0) {
    setISource (true);
    setHistory (true);
    initHistory (t);
    clearY ();
  }
}

void vccs::calcTR (nr_double_t t) {
  nr_double_t T = getPropertyDouble ("T");
  if (T > 0.0) {
    T = t - T;
    nr_double_t g = getPropertyDouble ("G");
    nr_double_t v = getV (NODE_1, T) - getV (NODE_4, T);
    setI (NODE_2, -g * v);
    setI (NODE_3, +g * v);
  }
}

// properties
PROP_REQ [] = {
  { "G", PROP_REAL, { 1, PROP_NO_STR }, PROP_NO_RANGE }, PROP_NO_PROP };
PROP_OPT [] = {
  { "T", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGE }, PROP_NO_PROP };
struct define_t vccs::cirdef =
  { "VCCS", 4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
