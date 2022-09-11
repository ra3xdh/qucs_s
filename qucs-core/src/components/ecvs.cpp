/*
 * ecvs.cpp - ecvs class implementation
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

/** \file ecvs.h
  * \brief The externally controlled voltage source component implementation file.
  *
  */

/**
  * \ingroup QucsInterface
  */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "component.h"
#include "ecvs.h"

using namespace qucs;

ecvs::ecvs () : circuit (2) {
  type = CIR_ECVS;
  setVSource (true);
  setVoltageSources (1);
}

void ecvs::initSP (void) {
  allocMatrixS ();
  setS (NODE_1, NODE_1, 0.0);
  setS (NODE_1, NODE_2, 1.0);
  setS (NODE_2, NODE_1, 1.0);
  setS (NODE_2, NODE_2, 0.0);
}

void ecvs::initDC (void) {
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
  setE (VSRC_1, 0);
}

void ecvs::initAC (void) {
  initDC ();
  setE (VSRC_1, 0);
}

void ecvs::initTR (void) {
  initDC ();
  // we need to store a history of time values
  // for interpolation, initialise the history here
  deleteHistory ();
  setHistory (true);
  initHistory (0);
}

void ecvs::calcTR (nr_double_t t) {
  nr_double_t V = 0;
  nr_double_t y0 = 0;
  nr_double_t Tlast;
  int hsize = getHistorySize ();
  // choose the voltage at the current time by interpolating
  // at the current time, using the previous values of the
  // voltage and the value of the voltage at the specified
  // next time in Tnext
  nr_double_t y1 = getPropertyDouble ("U");
  if (hsize < 1)
  {
    Tlast = t;
    y0 = y1;
  }
  else
  {
    Tlast = getHistoryTFromIndex (hsize-1);
    y0 = (getV (NODE_1, hsize-1) - getV (NODE_2, hsize-1));
  }
  nr_double_t Tnext = getPropertyDouble ("Tnext");
  // do the interpolation
  nr_double_t tdiff = t - Tlast;
  if (tdiff <= 0)
  {
    V = y0;
  }
  else
  {
    V =  y0 + (y1 - y0) * (tdiff / (Tnext - Tlast));
  }
#ifdef DEBUG
  printf ("ECVS -- t: %e, V: %e\n", t, V);
#endif
  // set the voltage source value
  setE (VSRC_1, V);
}

// properties
PROP_REQ [] = {
  { "U", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE }, PROP_NO_PROP };
PROP_OPT [] = {
  { "Interpolator", PROP_STR, { PROP_NO_VAL, "linear" },
    PROP_RNG_STR3 ("hold", "linear", "cubic") },
  { "Tnext", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  PROP_NO_PROP };
struct define_t ecvs::cirdef =
  { "ECVS", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
