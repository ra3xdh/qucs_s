/*
 * triac.cpp - triac class implementation
 *
 * Copyright (C) 2008 Stefan Jahn <stefan@lkcc.org>
 * Copyright (C) 2008 Michael Margraf <Michael.Margraf@alumni.TU-Berlin.DE>
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
#include "device.h"
#include "devstates.h"
#include "triac.h"

#define NODE_A1 0 /* anode 1 */
#define NODE_A2 1 /* anode 2 (cathode) */
#define NODE_GA 2 /* gate */
#define NODE_IN 3 /* internal node */

using namespace qucs;
using namespace qucs::device;

// Constructor for the triac.
triac::triac () : circuit (4) {
  type = CIR_TRIAC;
}

// Callback for initializing the DC analysis.
void triac::initDC (void) {
  Ud_last = 0.0;
  // allocate MNA matrices
  allocMatrixMNA ();
  // create internal node
  setInternalNode (NODE_IN, "int");
}

// Callback for DC analysis.
void triac::calcDC (void) {
  calcTheModel (false);
}

void triac::calcTheModel (bool last) {
  // get device properties
  nr_double_t Ubo = getPropertyDouble ("Vbo");
  nr_double_t Ibo = getPropertyDouble ("Igt");
  nr_double_t Is  = getPropertyDouble ("Is");
  nr_double_t N   = getPropertyDouble ("N");
  nr_double_t Gg  = 1.0 / getPropertyDouble ("Rg");
  nr_double_t T   = getPropertyDouble ("Temp");
  gi = 1.0 / getPropertyDouble ("Ri");

  nr_double_t Ut, Ud_bo, Ieq, Vd;

  Ut = N * celsius2kelvin (T) * kBoverQ;
  Ud_bo = std::log (Ibo / Is + 1.0);

  Vd = Ud = real (getV (NODE_IN) - getV (NODE_A2));
  Id = sign (Ud) * Is;
  Ud = fabs (Ud) / Ut;

  bool isOn;
  if (last)
    isOn = (Ud_last / Ut) > Ud_bo;
  else
    isOn = Ud > Ud_bo;

  if (Ud >= 80.0) {
    Id *= std::exp (80.0) * (1.0 + Ud - 80.0) - 1.0;
    Ud  = 80.0;
  }
  else
    Id *= std::exp (Ud) - 1.0;

  gd  = Is / Ut * std::exp (Ud);
  Ieq = Id - Vd * gd;

  // fill in I-Vector
  setI (NODE_A2, +Ieq);
  setI (NODE_IN, -Ieq);
  setI (NODE_A1, +0.0);
  setI (NODE_GA, +0.0);

  if (!isOn) {
    Ut = Ubo / std::log (Ibo / Is);
    Vd = Ud = real (getV (NODE_A1) - getV (NODE_IN));
    Id = sign (Ud) * Is;
    Ud = fabs (Ud) / Ut;

    if (Ud >= 80.0) {
      Id *= std::exp (80.0) * (1.0 + Ud - 80.0) - 1.0;
      Ud  = 80.0;
    }
    else
      Id *= std::exp (Ud) - 1.0;

    gi  = Is / Ut * std::exp (Ud);
    Ieq = Id - Vd * gi;
    addI (NODE_A1, -Ieq);
    addI (NODE_IN, +Ieq);
  }

  // fill in G-Matrix
  setY (NODE_A2, NODE_A2, +gd); setY (NODE_IN, NODE_IN, +gd);
  setY (NODE_A2, NODE_IN, -gd); setY (NODE_IN, NODE_A2, -gd);
  setY (NODE_A1, NODE_A1, +gi); addY (NODE_IN, NODE_IN, +gi);
  setY (NODE_A1, NODE_IN, -gi); setY (NODE_IN, NODE_A1, -gi);
  setY (NODE_GA, NODE_GA, +Gg); addY (NODE_IN, NODE_IN, +Gg);
  setY (NODE_GA, NODE_IN, -Gg); setY (NODE_IN, NODE_GA, -Gg);
}

// Saves operating points (voltages).
void triac::saveOperatingPoints (void) {
  nr_double_t Vd = real (getV (NODE_IN) - getV (NODE_A2));
  nr_double_t Vi = real (getV (NODE_A1) - getV (NODE_IN));
  setOperatingPoint ("Vd", Vd);
  setOperatingPoint ("Vi", Vi);
}

// Loads operating points (voltages).
void triac::loadOperatingPoints (void) {
  Ud = getOperatingPoint ("Vd");
  Ui = getOperatingPoint ("Vi");
}

// Calculates and saves operating points.
void triac::calcOperatingPoints (void) {
  nr_double_t Cj0 = getPropertyDouble ("Cj0");
  // calculate capacitances and charges
  nr_double_t Ci;
  Ci = Cj0;
  Qi = Cj0 * Ui;
  // save operating points
  setOperatingPoint ("gi", gi);
  setOperatingPoint ("gd", gd);
  setOperatingPoint ("Id", Id);
  setOperatingPoint ("Ci", Ci);
}

// Callback for initializing the AC analysis.
void triac::initAC (void) {
  initDC ();
}

// Build admittance matrix for AC and SP analysis.
matrix triac::calcMatrixY (nr_double_t frequency) {
  nr_double_t gd = getOperatingPoint ("gd");
  nr_double_t gi = getOperatingPoint ("gi");
  nr_double_t gg = 1.0 / getPropertyDouble ("Rg");
  nr_double_t Ci = getOperatingPoint ("Ci");
  nr_complex_t yi = nr_complex_t (gi, Ci * 2.0 * pi * frequency);
  matrix y (4);
  y.set (NODE_A2, NODE_A2, +gd);
  y.set (NODE_IN, NODE_IN, +gd +yi +gg);
  y.set (NODE_A2, NODE_IN, -gd);
  y.set (NODE_IN, NODE_A2, -gd);
  y.set (NODE_A1, NODE_A1, +yi);
  y.set (NODE_A1, NODE_IN, -yi);
  y.set (NODE_IN, NODE_A1, -yi);
  y.set (NODE_GA, NODE_GA, +gg);
  y.set (NODE_GA, NODE_IN, -gg);
  y.set (NODE_IN, NODE_GA, -gg);
  return y;
}

// Callback for the AC analysis.
void triac::calcAC (nr_double_t frequency) {
  setMatrixY (calcMatrixY (frequency));
}

// Callback for S-parameter analysis.
void triac::calcSP (nr_double_t frequency) {
  setMatrixS (ytos (calcMatrixY (frequency)));
}

#define qState 0 // charge state
#define cState 1 // current state

// Callback for initializing the TR analysis.
void triac::initTR (void) {
  setStates (2);
  initDC ();
  time_prev = -1.0;
}

// Callback for the TR analysis.
void triac::calcTR (nr_double_t time) {
  if (time_prev < time) {
    time_prev = time;
    Ud_last = fabs (real (getV (NODE_IN) - getV (NODE_A2)));
  }
  calcTheModel (true);

  saveOperatingPoints ();
  loadOperatingPoints ();
  calcOperatingPoints ();

  nr_double_t Ci = getOperatingPoint ("Ci");
  transientCapacitance (qState, NODE_A1, NODE_IN, Ci, Ui, Qi);
}

// properties
PROP_REQ [] = {
  { "Igt", PROP_REAL, { 50e-6, PROP_NO_STR }, PROP_POS_RANGEX },
  { "Vbo", PROP_REAL, { 30, PROP_NO_STR }, PROP_POS_RANGEX },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Cj0", PROP_REAL, { 10e-12, PROP_NO_STR }, PROP_POS_RANGE },
  { "Is", PROP_REAL, { 1e-10, PROP_NO_STR }, PROP_POS_RANGE },
  { "N", PROP_REAL, { 2.0, PROP_NO_STR }, PROP_RNGII (0.1, 100) },
  { "Ri", PROP_REAL, { 10.0, PROP_NO_STR }, PROP_POS_RANGEX },
  { "Rg", PROP_REAL, { 5.0, PROP_NO_STR }, PROP_POS_RANGEX },
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };
struct define_t triac::cirdef =
  { "Triac", 3, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_NONLINEAR, PROP_DEF };
