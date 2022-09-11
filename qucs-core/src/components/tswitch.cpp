/*
 * tswitch.cpp - time controlled switch class implementation
 *
 * Copyright (C) 2006, 2008 Stefan Jahn <stefan@lkcc.org>
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

/*!
 * \file tswitch.cpp
 * \brief time controlled switch class implementation
 *
 * \todo add a property to allow choosing the resistance profile
 * (cubic spline, linear, abrupt)
 */


#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <iostream>
#include <cmath>
#include "component.h"
#include "tswitch.h"

using namespace qucs;

tswitch::tswitch () : circuit (2) {
  type = CIR_TSWITCH;
  setVoltageSources (1);
}

nr_double_t tswitch::initState (void) {
  const char * const init = getPropertyString ("init");
  bool on = !strcmp (init, "on");
  return on ? getPropertyDouble ("Ron") : getPropertyDouble ("Roff");
}

void tswitch::initSP (void) {
  nr_double_t r = initState ();
  allocMatrixS ();
  setS (NODE_1, NODE_1, r / (r + 2));
  setS (NODE_2, NODE_2, r / (r + 2));
  setS (NODE_1, NODE_2, 2 / (r + 2));
  setS (NODE_2, NODE_1, 2 / (r + 2));
}

void tswitch::calcNoiseSP (nr_double_t) {
  nr_double_t T = getPropertyDouble ("Temp");
  nr_double_t r = initState ();
  nr_double_t f = celsius2kelvin (T) * 4.0 * r * z0 / sqr (2.0 * z0 + r) / T0;
  setN (NODE_1, NODE_1, +f); setN (NODE_2, NODE_2, +f);
  setN (NODE_1, NODE_2, -f); setN (NODE_2, NODE_1, -f);
}

void tswitch::calcNoiseAC (nr_double_t) {
  nr_double_t r = initState ();
  if (r > 0.0 || r < 0.0) {
    nr_double_t T = getPropertyDouble ("Temp");
    nr_double_t f = celsius2kelvin (T) / T0 * 4.0 / r;
    setN (NODE_1, NODE_1, +f); setN (NODE_2, NODE_2, +f);
    setN (NODE_1, NODE_2, -f); setN (NODE_2, NODE_1, -f);
  }
}

void tswitch::initDC (void) {
  nr_double_t r = initState ();
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
  setD (VSRC_1, VSRC_1, -r);
}

void tswitch::initAC (void) {
  initDC ();
}

void tswitch::initTR (void) {
  qucs::vector * values = getPropertyVector ("time");
  // Find the total time of the switching periods
  T = real (sum (*values));
  // if the user enters an even number of switching times
  // the pattern is repeated continuously
  repeat = (values->getSize () % 2) == 0 ? true : false;
  // make the time taken to go from fully on to fully off
  // the smallest switching time / 100, or the smallest possible
  // number, but no bigger than the max specified duration
  nr_double_t maxduration = getPropertyDouble("MaxDuration");
  duration = std::min ( std::max (10*NR_TINY, values->minimum() / 100),
                        maxduration );
  initDC ();
}

void tswitch::calcTR (nr_double_t t) {
  const char * const init = getPropertyString ("init");
  nr_double_t ron = getPropertyDouble ("Ron");
  nr_double_t roff = getPropertyDouble ("Roff");
  const char * const trans_type = getPropertyString ("Transition");
  nr_double_t r = 0;
  nr_double_t rdiff = 0;
  //nr_double_t s_i = 0;
  nr_double_t r_0 = 0;
  qucs::vector * values = getPropertyVector ("time");
  bool on = !strcmp (init, "on");
  nr_double_t ti = 0;

  if (repeat) {
    // if the user enters an even number of switching times
    // the pattern is repeated continuously. This is achieved by
    // subtracting an integer number of total switching periods
    // from the real time
    t = t - T * qucs::floor (t / T);
  }

  // Initialise the last switching time to be well in the past
  // to avoid having the switch even partially in a transition
  // state (due to inaccurately computed time differences)
  nr_double_t ts = -2*duration;

  // here we determine whether a switching event should occur
  // by looping through the list of switching times and comparing
  // to the current time
  for (int i = 0; i < values->getSize (); i++) {
    // add the current value from the list of switching times
    // to a counter
    ti += real (values->get (i));

    if (t >= ti)
    {
      // the current time is greater than or equal to the current
      // sum of switching times so the switch state changes
      on = !on;
      // store the current switching time
      ts = ti;
    }
    else {
      // the current sum of switching times is in the future
      // so exit the loop
      break;
    }
  }

  if (!strcmp(trans_type, "abrupt")) {
    r = (on ? ron : roff);
  } else {
    // calculate the time since the last switch occurred
    nr_double_t tdiff = std::max(NR_TINY, t - ts);
    
    // set the time difference to be no more than the max switch
    // duration so when we interpolate below we only get the max
    // or min function value if we are past a switching time
    if (tdiff > duration) {
      tdiff = duration;
    }
    // Set the appropriate resistance. 
    if (on) {
      r_0 = roff;
      rdiff = ron - roff;
      //    s_i = (rdiff) / (duration);
    } else {
      r_0 = ron;
      rdiff = roff - ron;
      //  s_i = (rdiff) / (duration);
    }
    if (!strcmp(trans_type, "linear")) {
      // simple linear transition over the transition time
      r = r_0 + rdiff * tdiff / duration;
    } else { // assume trans_type is "spline"
	// the resistance is interpolated along a constrained cubic spline
	// with zero derivative at the start and end points to ensure a 
	// smooth derivative
	//r = r_0 + ((3. * s_i * qucs::pow (tdiff,2.0)) / (duration))
	//        + ((-2. * s_i * qucs::pow (tdiff,3.0)) / qucs::pow (duration, 2.0));
	// use Horner's rule to reduce the numerical errors
	r = r_0 + (((-2. * rdiff * tdiff / duration) + 3. * rdiff) * qucs::pow(tdiff/duration, 2.0));
      }
  }

  // check for (numerical) errors
  assert(r >= ron);
  assert(r <= roff);

  setD (VSRC_1, VSRC_1, -r);
}

// properties
PROP_REQ [] = {
  { "init", PROP_STR, { PROP_NO_VAL, "off" }, PROP_RNG_STR2 ("on", "off") },
  { "time", PROP_LIST, { 1e-9, PROP_NO_STR }, PROP_POS_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Ron", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGE },
  { "Roff", PROP_REAL, { 1e12, PROP_NO_STR }, PROP_POS_RANGE },
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  { "MaxDuration", PROP_REAL, { 1e-6, PROP_NO_STR }, PROP_MIN_VAL (10*NR_TINY) },
  { "Transition", PROP_STR, { PROP_NO_VAL, "spline" }, PROP_RNG_STR3 ("abrupt", "linear", "spline") },
  PROP_NO_PROP };
struct define_t tswitch::cirdef =
  { "Switch", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
