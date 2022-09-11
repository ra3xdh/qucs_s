/*
 * wprobe.cpp - AC/DC and transient watt probe class implementation
 *
 * Copyright (C) 2015, Pedro Macedo
 *		 2017, Alberto Silva, 1120196@isep.ipp.pt
 *
 * This file is part of Qucs
 *
 * Qucs is free software; you can redistribute it and/or modify
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
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

//Taking iprobe as basis

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "component.h"
#include "wprobe.h"

using namespace qucs;

wprobe::wprobe () : circuit (4) {
  type = CIR_WPROBE;
  setProbe (true);
  setVSource (true);
  setVoltageSources (1);
}

//NODE 1 --> I+
//NODE 2 --> I-
//NODE 3 --> V+
//NODE 4 --> V-

void wprobe::initDC (void) {
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
}

void wprobe::initAC (void) {
  initDC ();
}

void wprobe::saveOperatingPoints (void) {
  nr_double_t Vr = real (getV (NODE_3) - getV (NODE_4));
  nr_double_t Vi = imag (getV (NODE_3) - getV (NODE_4));
  setOperatingPoint ("Vr", Vr);
  setOperatingPoint ("Vi", Vi); //This section works just like a voltmeter
}

//For specific information regarding The Power triangle and Power factor:
//https://en.wikipedia.org/wiki/Power_factor#Definition_and_calculation

void wprobe::calcOperatingPoints (void) {
//Reading the current and voltage values to calculate power values
  nr_double_t VAr = real  ((getV (NODE_3) - getV (NODE_4)) * getJ (NODE_1));
  nr_double_t VAi = -imag ((getV (NODE_3) - getV (NODE_4)) * getJ (NODE_1));
  setOperatingPoint ("VAr", VAr);
  setOperatingPoint ("VAi", VAi);

  nr_double_t P = VAr;
  setOperatingPoint ("P", P);

  nr_double_t Q = VAi;
  setOperatingPoint ("Q", Q);
//Power Factor calculation
  setOperatingPoint ("PF", P/std::sqrt(P*P+VAi*VAi));
}

void wprobe::initTR (void) {
  initDC ();
}

//properties
PROP_REQ [] = { PROP_NO_PROP };
PROP_OPT [] = {
  PROP_NO_PROP };
struct define_t wprobe::cirdef =
  { "WProbe", 4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
