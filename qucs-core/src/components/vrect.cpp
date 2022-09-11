/*
 * vrect.cpp - rectangular pulse voltage source class implementation
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
#include "vrect.h"

using namespace qucs;

vrect::vrect () : circuit (2) {
  type = CIR_VRECT;
  setVSource (true);
  setVoltageSources (1);
}

void vrect::initSP (void) {
  allocMatrixS ();
  setS (NODE_1, NODE_1, 0.0);
  setS (NODE_1, NODE_2, 1.0);
  setS (NODE_2, NODE_1, 1.0);
  setS (NODE_2, NODE_2, 0.0);
}

void vrect::initDC (void) {
  nr_double_t th = getPropertyDouble ("TH");
  nr_double_t tl = getPropertyDouble ("TL");
  nr_double_t tr = getPropertyDouble ("Tr");
  nr_double_t tf = getPropertyDouble ("Tf");
  if (tr > th) tr = th;
  if (tf > tl) tf = tl;
  // DC value defined as 0.0 instead of
  // (th + (tf - tr) / 2) / (th + tl) previously used
  // so that the transient starting value will also be 0, 
  // otherwise a discontinuity occurs
  nr_double_t a  = 0.0;
  nr_double_t u  = getPropertyDouble ("U") * a;
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2, u);
}

void vrect::initAC (void) {
  initDC ();
  setE (VSRC_1, 0);
}

void vrect::initTR (void) {
  initDC ();
}

void vrect::calcTR (nr_double_t t) {
  nr_double_t u  = getPropertyDouble ("U");
  nr_double_t th = getPropertyDouble ("TH");
  nr_double_t tl = getPropertyDouble ("TL");
  nr_double_t tr = getPropertyDouble ("Tr");
  nr_double_t tf = getPropertyDouble ("Tf");
  nr_double_t td = getPropertyDouble ("Td");
  nr_double_t ut = 0;
  nr_double_t s  = getNet()->getSrcFactor ();

  if (tr > th) tr = th;
  if (tf > tl) tf = tl;

  if (t > td) { // after delay
    t = t - td;
    t = t - (th + tl) * qucs::floor (t / (th + tl));
    if (t < tr) { // rising edge
      ut = + u / tr * t;
    }
    else if (t < th) { // high pulse
      ut = u;
    }
    else if (t < th + tf) { // falling edge
      ut = - u / tf * (t - (th + tf));
    }
  }
  setE (VSRC_1, ut * s);
}

// properties
PROP_REQ [] = {
  { "U", PROP_REAL, { 1, PROP_NO_STR }, PROP_NO_RANGE },
  { "TH", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "TL", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_POS_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Tr", PROP_REAL, { 1e-9, PROP_NO_STR }, PROP_POS_RANGE },
  { "Tf", PROP_REAL, { 1e-9, PROP_NO_STR }, PROP_POS_RANGE },
  { "Td", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  PROP_NO_PROP };
struct define_t vrect::cirdef =
  { "Vrect", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
