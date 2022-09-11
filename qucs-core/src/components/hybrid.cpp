/*
 * hybrid.cpp - hybrid class implementation
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
 * $Id$
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "component.h"
#include "hybrid.h"

using namespace qucs;

hybrid::hybrid () : circuit (4) {
  type = CIR_HYBRID;
}


//All functions below are obtained from
//http://qucs.sourceforge.net/tech/node56.html

void hybrid::initSP (void) {

  nr_complex_t p = qucs::polar (1.0, deg2rad (getPropertyDouble ("phi")));
  //nr_double_t  k = std::sqrt(1-pow((1/sqrt2),2));
  nr_double_t  k = (1/sqrt2); //last line reduces to this for 1/std::sqrt(2)

  allocMatrixS ();
//S11 = S22 = S33 = S44 = 0
  setS (NODE_1, NODE_1, 0.0); setS (NODE_2, NODE_2, 0.0);
  setS (NODE_3, NODE_3, 0.0); setS (NODE_4, NODE_4, 0.0);
//S14 = S23 = S32 = S41 = 0
  setS (NODE_1, NODE_4, 0.0); setS (NODE_2, NODE_3, 0.0);
  setS (NODE_3, NODE_2, 0.0); setS (NODE_4, NODE_1, 0.0);
//S12 = S21 = S34 = S43 = std::sqrt(1-k^2)
  setS (NODE_1, NODE_2, k); setS (NODE_2, NODE_1, k);
  setS (NODE_3, NODE_4, k); setS (NODE_4, NODE_3, k);
//S13 = S31 = S24 = S42 = k*exp(j * p)
  setS (NODE_1, NODE_3, k * p);  setS (NODE_3, NODE_1, k * p);
  setS (NODE_2, NODE_4, k * p);  setS (NODE_4, NODE_2, k * p);
}

void hybrid::initDC (void) {
  setVoltageSources (2);
  setInternalVoltageSource (1);
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_3);
  voltageSource (VSRC_2, NODE_2, NODE_4);
}

void hybrid::initAC (void) {

    nr_double_t  k = 1 / sqrt2;
    nr_complex_t y;
    nr_complex_t A = k*k*(nr_complex_t(1,0)+qucs::polar(1.0, 2.0*deg2rad (getPropertyDouble ("phi"))));
    nr_double_t B  = 2 * std::sqrt(1-(k*k));
    nr_complex_t C = qucs::polar (2*k, deg2rad (getPropertyDouble ("phi")));
    nr_complex_t D = getPropertyDouble ("Zref") * ((A*A)-(C*C));

    setVoltageSources (0);
    allocMatrixMNA ();
//    d *= getPropertyDouble ("Zref");

    y = (A*(nr_complex_t(2,0)-A))/D;
    setY (NODE_1, NODE_1, y); setY (NODE_2, NODE_2, y);
    setY (NODE_3, NODE_3, y); setY (NODE_4, NODE_4, y);

    y = (nr_complex_t(-1,0)*A*B)/D;
    setY (NODE_1, NODE_2, y); setY (NODE_2, NODE_1, y);
    setY (NODE_3, NODE_4, y); setY (NODE_4, NODE_3, y);

    y = (C*(nr_complex_t(-2,0)+A))/D;
    setY (NODE_1, NODE_3, y); setY (NODE_3, NODE_1, y);
    setY (NODE_2, NODE_4, y); setY (NODE_4, NODE_2, y);

    y= (B*C)/D;
    setY (NODE_1, NODE_4, y); setY (NODE_4, NODE_1, y);
    setY (NODE_2, NODE_3, y); setY (NODE_3, NODE_2, y);



}

void hybrid::initTR (void) {
  initDC ();
}

// properties
PROP_REQ [] = {
  { "phi", PROP_REAL, { 0, PROP_NO_STR },  PROP_RNGII (-180, +180) },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Zref", PROP_REAL, { 50, PROP_NO_STR }, PROP_POS_RANGE },
  PROP_NO_PROP };
struct define_t hybrid::cirdef =
  { "Hybrid", 4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };

