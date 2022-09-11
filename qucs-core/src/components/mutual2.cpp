/*
 * mutual2.cpp - three mutual inductors class implementation
 *
 * Copyright (C) 2005, 2008 Stefan Jahn <stefan@lkcc.org>
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
#include "mutual2.h"

using namespace qucs;

mutual2::mutual2 () : circuit (6) {
  type = CIR_MUTUAL2;
}

void mutual2::calcSP (nr_double_t frequency) {
  setMatrixS (ytos (calcMatrixY (frequency)));
}

matrix mutual2::calcMatrixY (nr_double_t frequency) {
  nr_double_t k12 = getPropertyDouble ("k12");
  nr_double_t k13 = getPropertyDouble ("k13");
  nr_double_t k23 = getPropertyDouble ("k23");
  nr_double_t l1 = getPropertyDouble ("L1");
  nr_double_t l2 = getPropertyDouble ("L2");
  nr_double_t l3 = getPropertyDouble ("L3");
  nr_double_t o = 2 * pi * frequency;
  nr_double_t a = 1 - k12 * k12 - k13 * k13 - k23 * k23 + 2 * k12 * k13 * k23;
  nr_complex_t y11 = nr_complex_t (0, (k23 * k23 - 1) / l1 / a / o);
  nr_complex_t y22 = nr_complex_t (0, (k12 * k12 - 1) / l3 / a / o);
  nr_complex_t y44 = nr_complex_t (0, (k13 * k13 - 1) / l2 / a / o);
  nr_complex_t y12 = nr_complex_t (0, (k13 - k12 * k23) / std::sqrt (l1 * l3) / a / o);
  nr_complex_t y15 = nr_complex_t (0, (k12 - k13 * k23) / std::sqrt (l1 * l2) / a / o);
  nr_complex_t y25 = nr_complex_t (0, (k23 - k12 * k13) / std::sqrt (l2 * l3) / a / o);

  matrix y = matrix (6);
  y.set (NODE_1, NODE_1, +y11); y.set (NODE_6, NODE_6, +y11);
  y.set (NODE_1, NODE_6, -y11); y.set (NODE_6, NODE_1, -y11);
  y.set (NODE_2, NODE_2, +y22); y.set (NODE_3, NODE_3, +y22);
  y.set (NODE_2, NODE_3, -y22); y.set (NODE_3, NODE_2, -y22);
  y.set (NODE_4, NODE_4, +y44); y.set (NODE_5, NODE_5, +y44);
  y.set (NODE_4, NODE_5, -y44); y.set (NODE_5, NODE_4, -y44);
  y.set (NODE_1, NODE_2, +y12); y.set (NODE_2, NODE_1, +y12);
  y.set (NODE_3, NODE_6, +y12); y.set (NODE_6, NODE_3, +y12);
  y.set (NODE_1, NODE_3, -y12); y.set (NODE_3, NODE_1, -y12);
  y.set (NODE_2, NODE_6, -y12); y.set (NODE_6, NODE_2, -y12);
  y.set (NODE_1, NODE_5, +y15); y.set (NODE_5, NODE_1, +y15);
  y.set (NODE_4, NODE_6, +y15); y.set (NODE_6, NODE_4, +y15);
  y.set (NODE_1, NODE_4, -y15); y.set (NODE_4, NODE_1, -y15);
  y.set (NODE_5, NODE_6, -y15); y.set (NODE_6, NODE_5, -y15);
  y.set (NODE_2, NODE_5, +y25); y.set (NODE_5, NODE_2, +y25);
  y.set (NODE_4, NODE_3, +y25); y.set (NODE_3, NODE_4, +y25);
  y.set (NODE_2, NODE_4, -y25); y.set (NODE_4, NODE_2, -y25);
  y.set (NODE_5, NODE_3, -y25); y.set (NODE_3, NODE_5, -y25);
  return y;
}

void mutual2::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
}

void mutual2::calcAC (nr_double_t frequency) {
  setMatrixY (calcMatrixY (frequency));
}

void mutual2::initDC (void) {
  setVoltageSources (3);
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_6);
  voltageSource (VSRC_2, NODE_5, NODE_4);
  voltageSource (VSRC_3, NODE_2, NODE_3);
}

void mutual2::initTR (void) {
  initDC ();
  setStates (18);
}

#define fState11  0 // flux state
#define vState11  1 // voltage state
#define fState12  2
#define vState12  3
#define fState13  4
#define vState13  5
#define fState21  6
#define vState21  7
#define fState22  8
#define vState22  9
#define fState23 10
#define vState23 11
#define fState31 12
#define vState31 13
#define fState32 14
#define vState32 15
#define fState33 16
#define vState33 17

void mutual2::calcTR (nr_double_t) {
  nr_double_t k12 = getPropertyDouble ("k12");
  nr_double_t k13 = getPropertyDouble ("k13");
  nr_double_t k23 = getPropertyDouble ("k23");
  nr_double_t l1  = getPropertyDouble ("L1");
  nr_double_t l2  = getPropertyDouble ("L2");
  nr_double_t l3  = getPropertyDouble ("L3");
  nr_double_t M12 = k12 * std::sqrt (l1 * l2);
  nr_double_t M13 = k13 * std::sqrt (l1 * l3);
  nr_double_t M23 = k23 * std::sqrt (l2 * l3);
  nr_double_t r11, r12, r13, r21, r22, r23, r31, r32, r33;
  nr_double_t v11, v12, v13, v21, v22, v23, v31, v32, v33;
  nr_double_t i1 = real (getJ (VSRC_1));
  nr_double_t i2 = real (getJ (VSRC_2));
  nr_double_t i3 = real (getJ (VSRC_3));

  setState  (fState11, i1 * l1);
  integrate (fState11, l1, r11, v11);
  setState  (fState22, i2 * l2);
  integrate (fState22, l2, r22, v22);
  setState  (fState33, i3 * l3);
  integrate (fState33, l3, r33, v33);

  setState  (fState12, i2 * M12);
  integrate (fState12, M12, r12, v12);
  setState  (fState13, i3 * M13);
  integrate (fState13, M13, r13, v13);
  setState  (fState21, i1 * M12);
  integrate (fState21, M12, r21, v21);
  setState  (fState23, i3 * M23);
  integrate (fState23, M23, r23, v23);
  setState  (fState31, i1 * M13);
  integrate (fState31, M13, r31, v31);
  setState  (fState32, i2 * M23);
  integrate (fState32, M23, r32, v32);

  setD (VSRC_1, VSRC_1, -r11);
  setD (VSRC_1, VSRC_2, -r12);
  setD (VSRC_1, VSRC_3, -r13);
  setD (VSRC_2, VSRC_1, -r21);
  setD (VSRC_2, VSRC_2, -r22);
  setD (VSRC_2, VSRC_3, -r23);
  setD (VSRC_3, VSRC_1, -r31);
  setD (VSRC_3, VSRC_2, -r32);
  setD (VSRC_3, VSRC_3, -r33);
  setE (VSRC_1, v11 + v12 + v13);
  setE (VSRC_2, v21 + v22 + v23);
  setE (VSRC_3, v31 + v32 + v33);
}

// properties
PROP_REQ [] = {
  { "L1", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_POS_RANGEX },
  { "L2", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_POS_RANGEX },
  { "L3", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_POS_RANGEX },
  { "k12", PROP_REAL, { 0.9, PROP_NO_STR }, PROP_RNGXX (-1, 1) },
  { "k13", PROP_REAL, { 0.9, PROP_NO_STR }, PROP_RNGXX (-1, 1) },
  { "k23", PROP_REAL, { 0.9, PROP_NO_STR }, PROP_RNGXX (-1, 1) },
  PROP_NO_PROP };
PROP_OPT [] = {
  PROP_NO_PROP };
struct define_t mutual2::cirdef =
  { "MUT2", 6, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
