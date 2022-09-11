/*
 * irect.cpp - rectangular pulse current source class implementation
 *
 * Copyright (C) 2004, 2006, 2008 Stefan Jahn <stefan@lkcc.org>
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
#include "irect.h"

using namespace qucs;

irect::irect () : circuit (2) {
  type = CIR_IRECT;
  setISource (true);
}

void irect::initSP (void) {
  allocMatrixS ();
  setS (NODE_1, NODE_1, 1.0);
  setS (NODE_1, NODE_2, 0.0);
  setS (NODE_2, NODE_1, 0.0);
  setS (NODE_2, NODE_2, 1.0);
}

void irect::initDC (void) {
  nr_double_t th = getPropertyDouble ("TH");
  nr_double_t tl = getPropertyDouble ("TL");
  nr_double_t tr = getPropertyDouble ("Tr");
  nr_double_t tf = getPropertyDouble ("Tf");
  if (tr > th) tr = th;
  if (tf > tl) tf = tl;
  nr_double_t a  = (th + (tf - tr) / 2) / (th + tl);
  nr_double_t i  = getPropertyDouble ("I") * a;
  allocMatrixMNA ();
  setI (NODE_1, +i); setI (NODE_2, -i);
}

void irect::initAC (void) {
  allocMatrixMNA ();
  clearI ();
}

void irect::initTR (void) {
  initDC ();
}

void irect::calcTR (nr_double_t t) {
  nr_double_t i  = getPropertyDouble ("I");
  nr_double_t th = getPropertyDouble ("TH");
  nr_double_t tl = getPropertyDouble ("TL");
  nr_double_t tr = getPropertyDouble ("Tr");
  nr_double_t tf = getPropertyDouble ("Tf");
  nr_double_t td = getPropertyDouble ("Td");
  nr_double_t it = 0;
  nr_double_t s  = getNet()->getSrcFactor ();

  if (tr > th) tr = th;
  if (tf > tl) tf = tl;

  if (t > td) { // after delay
    t = t - td;
    t = t - (th + tl) * qucs::floor (t / (th + tl));
    if (t < tr) { // rising edge
      it = + i / tr * t;
    }
    else if (t < th) { // high pulse
      it = i;
    }
    else if (t < th + tf) { // falling edge
      it = - i / tf * (t - (th + tf));
    }
  }
  setI (NODE_1, +it * s); setI (NODE_2, -it * s);
}

// properties
PROP_REQ [] = {
  { "I", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_NO_RANGE },
  { "TH", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "TL", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_POS_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Tr", PROP_REAL, { 1e-9, PROP_NO_STR }, PROP_POS_RANGE },
  { "Tf", PROP_REAL, { 1e-9, PROP_NO_STR }, PROP_POS_RANGE },
  { "Td", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  PROP_NO_PROP };
struct define_t irect::cirdef =
  { "Irect", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
