/*
 * vdc.cpp - DC voltage source class implementation
 *
 * Copyright (C) 2003, 2004, 2006, 2008 Stefan Jahn <stefan@lkcc.org>
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
#include "vdc.h"

using namespace qucs;

vdc::vdc () : circuit (2) {
  type = CIR_VDC;
  setVSource (true);
  setVoltageSources (1);
}

void vdc::initSP (void) {
  allocMatrixS ();
  setS (NODE_1, NODE_1, 0.0);
  setS (NODE_1, NODE_2, 1.0);
  setS (NODE_2, NODE_1, 1.0);
  setS (NODE_2, NODE_2, 0.0);
}

void vdc::initDC (void) {
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2, getPropertyDouble ("U"));
}

void vdc::calcDC (void) {
  nr_double_t f = getNet()->getSrcFactor ();
  setE (VSRC_1, f * getPropertyDouble ("U"));
}

void vdc::initTR (void) {
  initDC ();
}

void vdc::initAC (void) {
  initDC ();
  setE (VSRC_1, 0.0);
}

void vdc::initHB (void) {
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
}

void vdc::calcHB (nr_double_t frequency) {
  if (frequency == 0.0) {
    setE (VSRC_1, getPropertyDouble ("U"));
  } else {
    setE (VSRC_1, 0);
  }
}

// properties
PROP_REQ [] = {
  { "U", PROP_REAL, { 1, PROP_NO_STR }, PROP_NO_RANGE }, PROP_NO_PROP };
PROP_OPT [] = {
  PROP_NO_PROP };
struct define_t vdc::cirdef =
  { "Vdc", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
