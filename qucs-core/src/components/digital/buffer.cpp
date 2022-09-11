/*
 * buffer.cpp - logical buffer class implementation
 *
 * Copyright (C) 2008, 2009 Stefan Jahn <stefan@lkcc.org>
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
#include "buffer.h"

using namespace qucs;

buffer::buffer () : digital () {
  type = CIR_BUFFER;
  setSize (2);
}

void buffer::calcOutput (void) {
  nr_double_t v = getPropertyDouble ("V");
  Vout = v / 2 * (1 + calcTransferX (0));
}

void buffer::calcDerivatives (void) {
  g[0] = 0.5 * calcDerivativeX (0);
}

// properties
PROP_REQ [] = {
  { "V", PROP_REAL, { 1, PROP_NO_STR }, PROP_POS_RANGE }, PROP_NO_PROP };
PROP_OPT [] = {
  { "t", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGE },
  { "TR", PROP_REAL, { 10, PROP_NO_STR }, PROP_RNGII (1, 100) },
  PROP_NO_PROP };
struct define_t buffer::cirdef =
  { "Buf", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_NONLINEAR, PROP_DEF };
