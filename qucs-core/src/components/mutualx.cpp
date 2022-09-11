/*
 * mutualx.cpp - multiple mutual inductors class implementation
 *
 * Copyright (C) 2007, 2008 Stefan Jahn <stefan@lkcc.org>
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
#include "mutualx.h"

using namespace qucs;

mutualx::mutualx () : circuit () {
  type = CIR_MUTUALX;
  setVariableSized (true);
}

void mutualx::calcSP (nr_double_t frequency) {
  setMatrixS (ytos (calcMatrixY (frequency)));
}

matrix mutualx::calcMatrixY (nr_double_t frequency) {
#if 1
  matrix ts = ztos (calcMatrixZ (frequency));
  matrix ty = stoy (ts);
#else
  matrix ty = ztoy (calcMatrixZ (frequency));
#endif
  int r, c;
  int inductors = getSize () / 2;
  matrix y = matrix (inductors * 2);

  for (r = 0; r < inductors; r++) {
    for (c = 0; c < inductors; c++) {
      y.set (2 * r + 0, 2 * c + 0, +ty (r, c));
      y.set (2 * r + 1, 2 * c + 1, +ty (r, c));
      y.set (2 * r + 0, 2 * c + 1, -ty (r, c));
      y.set (2 * r + 1, 2 * c + 0, -ty (r, c));
    }
  }
  return y;
}

matrix mutualx::calcMatrixZ (nr_double_t frequency) {
  int inductors = getSize () / 2;
  int r, c, state;
  qucs::vector * L = getPropertyVector ("L");
  qucs::vector * C = getPropertyVector ("k");
  nr_double_t o = 2 * pi * frequency;
  matrix z = matrix (inductors);

  // fill Z-Matrix entries
  for (state = 0, r = 0; r < inductors; r++) {
    for (c = 0; c < inductors; c++, state++) {
      nr_double_t l1 = real (L->get (r));
      nr_double_t l2 = real (L->get (c));
      nr_double_t k = real (C->get (state)) * std::sqrt (l1 * l2);
      z.set (r, c, nr_complex_t (0.0, k * o));
    }
  }
  return z;
}

void mutualx::initAC (void) {
  initDC ();
}

void mutualx::calcAC (nr_double_t frequency) {
  int inductors = getSize () / 2;
  int r, c, state;
  qucs::vector * L = getPropertyVector ("L");
  qucs::vector * C = getPropertyVector ("k");
  nr_double_t o = 2 * pi * frequency;

  // fill D-Matrix
  for (state = 0, r = 0; r < inductors; r++) {
    for (c = 0; c < inductors; c++, state++) {
      nr_double_t l1 = real (L->get (r));
      nr_double_t l2 = real (L->get (c));
      nr_double_t k = real (C->get (state)) * std::sqrt (l1 * l2);
      setD (VSRC_1 + r, VSRC_1 + c, nr_complex_t (0.0, - k * o));
    }
  }
}

void mutualx::initDC (void) {
  int inductors = getSize () / 2;
  setVoltageSources (inductors);
  allocMatrixMNA ();
  // fill C and B-Matrix entries
  for (int i = 0; i < inductors; i++)
    voltageSource (VSRC_1 + i, NODE_1 + i * 2, NODE_2 + i * 2);
}

void mutualx::initTR (void) {
  int inductors = getSize () / 2;
  initDC ();
  setStates (inductors * inductors * 2);
}

void mutualx::calcTR (nr_double_t) {
  int inductors = getSize () / 2;
  int r, c, state;
  qucs::vector * L = getPropertyVector ("L");
  qucs::vector * C = getPropertyVector ("k");

  nr_double_t * veq = new nr_double_t[inductors * inductors];
  nr_double_t * req = new nr_double_t[inductors * inductors];

  // integration for self and mutual inductances
  for (state = 0, r = 0; r < inductors; r++) {
    for (c = 0; c < inductors; c++, state++) {
      nr_double_t l1 = real (L->get (r));
      nr_double_t l2 = real (L->get (c));
      nr_double_t i = real (getJ (VSRC_1 + c));
      nr_double_t k = real (C->get (state)) * std::sqrt (l1 * l2);
      setState  (2 * state, i * k);
      integrate (2 * state, k, req[state], veq[state]);
    }
  }

  // fill D-Matrix entries and extended RHS
  for (state = 0, r = 0; r < inductors; r++) {
    nr_double_t v = 0;
    for (c = 0; c < inductors; c++, state++) {
      setD (VSRC_1 + r, VSRC_1 + c, -req[state]);
      v += veq[state];
    }
    setE (VSRC_1 + r, v);
  }

  delete[] veq;
  delete[] req;
}

// properties
PROP_REQ [] = {
  { "L", PROP_LIST, { 1e-9, PROP_NO_STR }, PROP_POS_RANGE },
  { "k", PROP_LIST, { 0.9, PROP_NO_STR }, PROP_RNGII (-1, +1) },
  PROP_NO_PROP };
PROP_OPT [] = {
  PROP_NO_PROP };
struct define_t mutualx::cirdef =
  { "MUTX",
    PROP_NODES, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
