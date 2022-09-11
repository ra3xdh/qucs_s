/*
 * nor.cpp - logical nor class implementation
 *
 * Copyright (C) 2005, 2008, 2009 Stefan Jahn <stefan@lkcc.org>
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
#include "digital.h"
#include "nor.h"

using namespace qucs;

logicnor::logicnor () : digital () {
  type = CIR_NOR;
  setVariableSized (true);
}

void logicnor::calcOutput (void) {
  nr_double_t v = getPropertyDouble ("V");
  nr_double_t n = getSize () - 1;
  nr_double_t x;
  for (x = 0, i = 0; i < n; i++) {
    x += 2 / (1 - calcTransfer (i));
  }
  Vout = v * n / x;
}

void logicnor::calcDerivatives (void) {
  nr_double_t n = getSize () - 1;
  nr_double_t x;
  for (int k = 0; k < n; k++) {
    for (x = 0, i = 0; i < n; i++) {
      x += 2 / (1 - calcTransfer (i));
    }
    x *= (1 - calcTransfer (k));
    g[k] = -2 * n * calcDerivative (k) / x / x;
  }
}

// properties
PROP_REQ [] = {
  { "V", PROP_REAL, { 1, PROP_NO_STR }, PROP_POS_RANGE }, PROP_NO_PROP };
PROP_OPT [] = {
  { "t", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGE },
  { "TR", PROP_REAL, { 10, PROP_NO_STR }, PROP_RNGII (1, 100) },
  PROP_NO_PROP };
struct define_t logicnor::cirdef =
  { "NOR",
    PROP_NODES, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_NONLINEAR, PROP_DEF };
