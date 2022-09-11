/*
 * digisource.cpp - digital source class implementation
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
#include "digisource.h"

using namespace qucs;

digisource::digisource () : circuit (1) {
  type = CIR_DIGISOURCE;
  setVSource (true);
  setVoltageSources (1);
}

void digisource::initSP (void) {
  allocMatrixS ();
  setS (NODE_1, NODE_1, -1.0);
}

void digisource::initDC (void) {
  const char * const init = getPropertyString ("init");
  nr_double_t v = getPropertyDouble ("V");
  bool lo = !strcmp (init, "low");
  allocMatrixMNA ();
  setC (VSRC_1, NODE_1, 1.0);
  setB (NODE_1, VSRC_1, 1.0);
  setD (VSRC_1, VSRC_1, 0.0);
  setE (VSRC_1, lo ? 0 : v);
}

void digisource::initAC (void) {
  initDC ();
  setE (VSRC_1, 0);
}

void digisource::initTR (void) {
  qucs::vector * values = getPropertyVector ("times");
  T = real (sum (*values));
  initDC ();
}

void digisource::calcTR (nr_double_t t) {
  const char * const init = getPropertyString ("init");
  nr_double_t v = getPropertyDouble ("V");
  qucs::vector * values = getPropertyVector ("times");
  bool lo = !strcmp (init, "low");
  nr_double_t ti = 0;

  t = t - T * qucs::floor (t / T);
  for (int i = 0; i < values->getSize (); i++) {
    ti += real (values->get (i));
    if (t >= ti) lo = !lo; else break;
  }

  setE (VSRC_1, lo ? 0 : v);
}

// properties
PROP_REQ [] = {
  { "init", PROP_STR, { PROP_NO_VAL, "low" }, PROP_RNG_STR2 ("low", "high") },
  { "times", PROP_LIST, { 1e-9, PROP_NO_STR }, PROP_POS_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "V", PROP_REAL, { 1, PROP_NO_STR }, PROP_POS_RANGE }, PROP_NO_PROP };
struct define_t digisource::cirdef =
  { "DigiSource",
    1, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
