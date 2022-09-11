/*
 * idc.cpp - DC current source class implementation
 *
 * Copyright (C) 2003, 2004, 2008 Stefan Jahn <stefan@lkcc.org>
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
#include "idc.h"

using namespace qucs;

idc::idc () : circuit (2) {
  type = CIR_IDC;
  setISource (true);
}

void idc::initSP (void) {
  allocMatrixS ();
  setS (NODE_1, NODE_1, 1.0);
  setS (NODE_1, NODE_2, 0.0);
  setS (NODE_2, NODE_1, 0.0);
  setS (NODE_2, NODE_2, 1.0);
}

void idc::initDC (void) {
  nr_double_t i = getPropertyDouble ("I");
  allocMatrixMNA ();
  setI (NODE_1, +i); setI (NODE_2, -i);
}

void idc::calcDC (void) {
  nr_double_t i = getPropertyDouble ("I");
  nr_double_t f = getNet()->getSrcFactor ();
  setI (NODE_1, +i * f); setI (NODE_2, -i * f);
}

void idc::initAC (void) {
  allocMatrixMNA ();
  clearI ();
}

void idc::initTR (void) {
  initDC ();
}

// properties
PROP_REQ [] = {
  { "I", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_NO_RANGE }, PROP_NO_PROP };
PROP_OPT [] = {
  PROP_NO_PROP };
struct define_t idc::cirdef =
  { "Idc", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
