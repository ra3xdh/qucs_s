/*
 * ctline.cpp - ideal coupled transmission line class implementation
 *
 * Copyright (C) 2011 Michael Margraf <michael.margraf@alumni.tu-berlin.de>
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
 * $Id: ctline.cpp 1876 2013-03-11 08:00:11Z fransschreuder $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "component.h"
#include "ctline.h"

using namespace qucs;

ctline::ctline () : circuit (4) {
  type = CIR_CTLINE;
}

void ctline::calcSP (nr_double_t frequency) {
  nr_double_t l   = getPropertyDouble ("L");
  nr_double_t ze  = getPropertyDouble ("Ze");
  nr_double_t zo  = getPropertyDouble ("Zo");
  nr_double_t ere = getPropertyDouble ("Ere");
  nr_double_t ero = getPropertyDouble ("Ero");
  nr_double_t ae  = getPropertyDouble ("Ae");
  nr_double_t ao  = getPropertyDouble ("Ao");
  nr_double_t o   = 2.0 * pi * frequency;

  nr_complex_t ge = nr_complex_t (std::log (ae) / 2, o / C0 * std::sqrt (ere)) * l;
  nr_complex_t go = nr_complex_t (std::log (ao) / 2, o / C0 * std::sqrt (ero)) * l;
  nr_complex_t xe = 2.0 * ze * z0 * std::cosh (ge) + (ze*ze + z0*z0) * std::sinh (ge);
  nr_complex_t xo = 2.0 * zo * z0 * std::cosh (go) + (zo*zo + z0*z0) * std::sinh (go);
  nr_complex_t ye = ze * z0 / xe;
  nr_complex_t yo = zo * z0 / xo;
  xe = (ze*ze - z0*z0) * std::sinh (ge) / 2.0 / xe;
  xo = (zo*zo - z0*z0) * std::sinh (go) / 2.0 / xo;

  setS (NODE_1, NODE_1, xe+xo); setS (NODE_2, NODE_2, xe+xo);
  setS (NODE_3, NODE_3, xe+xo); setS (NODE_4, NODE_4, xe+xo);
  setS (NODE_1, NODE_4, xe-xo); setS (NODE_4, NODE_1, xe-xo);
  setS (NODE_2, NODE_3, xe-xo); setS (NODE_3, NODE_2, xe-xo);
  setS (NODE_1, NODE_2, ye+yo); setS (NODE_2, NODE_1, ye+yo);
  setS (NODE_3, NODE_4, ye+yo); setS (NODE_4, NODE_3, ye+yo);
  setS (NODE_1, NODE_3, ye-yo); setS (NODE_3, NODE_1, ye-yo);
  setS (NODE_2, NODE_4, ye-yo); setS (NODE_4, NODE_2, ye-yo);
}

void ctline::calcNoiseSP (nr_double_t) {
  nr_double_t l = getPropertyDouble ("L");
  if (l < 0) return;
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  matrix s = getMatrixS ();
  matrix e = eye (getSize ());
  setMatrixN (celsius2kelvin (T) / T0 * (e - s * transpose (conj (s))));
}

void ctline::calcNoiseAC (nr_double_t) {
  nr_double_t l = getPropertyDouble ("L");
  if (l < 0) return;
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  setMatrixN (4 * celsius2kelvin (T) / T0 * real (getMatrixY ()));
}

void ctline::initDC (void) {
  setVoltageSources (2);
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
  voltageSource (VSRC_2, NODE_3, NODE_4);
}

void ctline::initAC (void) {
  nr_double_t l = getPropertyDouble ("L");
  if (l != 0.0) {
    setVoltageSources (0);
    allocMatrixMNA ();
  } else {
    setVoltageSources (2);
    allocMatrixMNA ();
    voltageSource (VSRC_1, NODE_1, NODE_2);
    voltageSource (VSRC_2, NODE_3, NODE_4);
  }
}

void ctline::calcAC (nr_double_t frequency) {
  nr_double_t l   = getPropertyDouble ("L");
  nr_double_t ze  = getPropertyDouble ("Ze");
  nr_double_t zo  = getPropertyDouble ("Zo");
  nr_double_t ere = getPropertyDouble ("Ere");
  nr_double_t ero = getPropertyDouble ("Ero");
  nr_double_t ae  = getPropertyDouble ("Ae");
  nr_double_t ao  = getPropertyDouble ("Ao");
  nr_double_t o   = 2.0 * pi * frequency;

  if (l != 0.0) {
    nr_complex_t y11, y12, y13, y14;
    nr_complex_t arg_e = nr_complex_t (std::log (ae) / 2.0, o / C0 * std::sqrt (ere)) * l;
    nr_complex_t arg_o = nr_complex_t (std::log (ao) / 2.0, o / C0 * std::sqrt (ero)) * l;

    y12   =  0.5 / sinh (arg_e) / ze;
    y13   = -0.5 / sinh (arg_o) / zo;
    arg_e = std::cosh (arg_e) * y12;
    arg_o = std::cosh (arg_o) * y13;
    y11   = arg_e - arg_o;
    y14   = arg_e + arg_o;
    arg_e = y12;
    y12   =  y13 - y12;
    y13   = -y13 - arg_e;

    setY (NODE_1, NODE_1, +y11); setY (NODE_2, NODE_2, +y11);
    setY (NODE_3, NODE_3, +y11); setY (NODE_4, NODE_4, +y11);
    setY (NODE_1, NODE_2, +y12); setY (NODE_2, NODE_1, +y12);
    setY (NODE_3, NODE_4, +y12); setY (NODE_4, NODE_3, +y12);
    setY (NODE_1, NODE_3, +y13); setY (NODE_3, NODE_1, +y13);
    setY (NODE_2, NODE_4, +y13); setY (NODE_4, NODE_2, +y13);
    setY (NODE_1, NODE_4, +y14); setY (NODE_4, NODE_1, +y14);
    setY (NODE_2, NODE_3, +y14); setY (NODE_3, NODE_2, +y14);
  }
}

// properties
PROP_REQ [] = {
  { "Ze", PROP_REAL, { 50, PROP_NO_STR }, PROP_POS_RANGE },
  { "Zo", PROP_REAL, { 50, PROP_NO_STR }, PROP_POS_RANGE },
  { "L", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_NO_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Ere", PROP_REAL, { 1, PROP_NO_STR }, PROP_POS_RANGE },
  { "Ero", PROP_REAL, { 1, PROP_NO_STR }, PROP_POS_RANGE },
  { "Ae", PROP_REAL, { 1, PROP_NO_STR }, PROP_POS_RANGEX },
  { "Ao", PROP_REAL, { 1, PROP_NO_STR }, PROP_POS_RANGEX },
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };
struct define_t ctline::cirdef =
  { "CTLIN", 4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
