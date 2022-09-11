/*
 * vprobe.cpp - AC/DC and transient voltage probe class implementation
 *
 * Copyright (C) 2006, 2007, 2008 Stefan Jahn <stefan@lkcc.org>
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
#include "vprobe.h"

using namespace qucs;

vprobe::vprobe () : circuit (2) {
  type = CIR_VPROBE;
  setProbe (true);
}

void vprobe::initSP (void) {
  allocMatrixS ();
  setS (NODE_1, NODE_1, 1.0);
  setS (NODE_1, NODE_2, 0.0);
  setS (NODE_2, NODE_1, 0.0);
  setS (NODE_2, NODE_2, 1.0);
}

void vprobe::initDC (void) {
  allocMatrixMNA ();
}

void vprobe::saveOperatingPoints (void) {
  nr_double_t Vr = real (getV (NODE_1) - getV (NODE_2));
  nr_double_t Vi = imag (getV (NODE_1) - getV (NODE_2));
  setOperatingPoint ("Vr", Vr);
  setOperatingPoint ("Vi", Vi);
}

void vprobe::initAC (void) {
  initDC ();
}

void vprobe::initTR (void) {
  initDC ();
}

// properties
PROP_REQ [] = { PROP_NO_PROP };
PROP_OPT [] = {
  PROP_NO_PROP };
struct define_t vprobe::cirdef =
  { "VProbe", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
