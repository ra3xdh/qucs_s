/*
 * diode.cpp - diode class implementation
 *
 * Copyright (C) 2004, 2005, 2006, 2007, 2008 Stefan Jahn <stefan@lkcc.org>
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
#include "diode.h"

#define NODE_C 0 /* cathode node */
#define NODE_A 1 /* anode node   */

#define StateVars 1 // state variables

// state variable indices
#define _UdPrev 0

// state variable shortcuts
#define UdPrev deviceVar (_UdPrev)

using namespace qucs;
using namespace qucs::device;

// Constructor for the diode.
diode::diode () : circuit (2) {
  rs = NULL;
  type = CIR_DIODE;
}

// Callback for S-parameter analysis.
void diode::calcSP (nr_double_t frequency) {
  nr_double_t gd = getOperatingPoint ("gd");
  nr_double_t Cd = getOperatingPoint ("Cd");
  nr_complex_t y = 2 * z0 * nr_complex_t (gd, Cd * 2.0 * pi * frequency);
  setS (NODE_C, NODE_C, 1.0 / (1.0 + y));
  setS (NODE_A, NODE_A, 1.0 / (1.0 + y));
  setS (NODE_C, NODE_A, y / (1.0 + y));
  setS (NODE_A, NODE_C, y / (1.0 + y));
}

// Callback for S-parameter noise analysis.
void diode::calcNoiseSP (nr_double_t frequency) {
#if MICHAEL /* shot noise only */
  nr_double_t Id = getOperatingPoint ("Id");
  nr_double_t Is = getPropertyDouble ("Is") + getPropertyDouble ("Isr");

  // adjust shot noise current if necessary
  if (Id < -Is) Id = -Is;

  nr_double_t gd = getOperatingPoint ("gd");
  nr_double_t Cd = getOperatingPoint ("Cd");

  nr_complex_t y = rect (gd, Cd * 2.0 * pi * frequency);
  nr_complex_t f = 2 * z0 * (Id + 2 * Is) / norm (2 * z0 * y + 1) * QoverkB / T0;
  setN (NODE_C, NODE_C, +f); setN (NODE_A, NODE_A, +f);
  setN (NODE_C, NODE_A, -f); setN (NODE_A, NODE_C, -f);
#else
  setMatrixN (cytocs (calcMatrixCy (frequency) * z0, getMatrixS ()));
#endif
}

// Computes noise correlation matrix Cy.
matrix diode::calcMatrixCy (nr_double_t frequency) {
  // fetch computed operating points
  nr_double_t Id = getOperatingPoint ("Id");
  nr_double_t Is = getPropertyDouble ("Is") + getPropertyDouble ("Isr");

  // adjust shot noise current if necessary
  if (Id < -Is) Id = -Is;

  nr_double_t Kf  = getPropertyDouble ("Kf");
  nr_double_t Af  = getPropertyDouble ("Af");
  nr_double_t Ffe = getPropertyDouble ("Ffe");

  // build noise current correlation matrix
  matrix cy (2);
  nr_double_t i = 2 * (Id + 2 * Is) * QoverkB / T0 +    // shot noise
    Kf * qucs::pow (fabs (Id), Af) / qucs::pow (frequency, Ffe) / kB / T0; // flicker noise
  cy.set (NODE_C, NODE_C, +i); cy.set (NODE_A, NODE_A, +i);
  cy.set (NODE_A, NODE_C, -i); cy.set (NODE_C, NODE_A, -i);
  return cy;
}

// Initializes the diode model including temperature and area effects.
void diode::initModel (void) {
  // fetch necessary device properties
  nr_double_t T  = getPropertyDouble ("Temp");
  nr_double_t Tn = getPropertyDouble ("Tnom");
  nr_double_t A  = getPropertyDouble ("Area");

  // compute Is temperature and area dependency
  nr_double_t Is  = getPropertyDouble ("Is");
  nr_double_t N   = getPropertyDouble ("N");
  nr_double_t Xti = getPropertyDouble ("Xti");
  nr_double_t Eg  = getPropertyDouble ("Eg");
  nr_double_t T1, T2;
  T2 = celsius2kelvin (T);
  T1 = celsius2kelvin (Tn);
  Is = pnCurrent_T (T1, T2, Is, Eg, N, Xti);
  setScaledProperty ("Is", Is * A);

  // compute Isr temperature and area dependency
  nr_double_t Isr = getPropertyDouble ("Isr");
  nr_double_t Nr  = getPropertyDouble ("Nr");
  Isr = pnCurrent_T (T1, T2, Isr, Eg, Nr, Xti);
  setScaledProperty ("Isr", Isr * A);

  // check unphysical parameters
  if (Nr < 1.0) {
    logprint (LOG_ERROR, "WARNING: Unphysical model parameter Nr = %g in "
	      "diode `%s'\n", Nr, getName ());
  }
  if (N < 1.0) {
    logprint (LOG_ERROR, "WARNING: Unphysical model parameter N = %g in "
	      "diode `%s'\n", N, getName ());
  }

  // compute Vj temperature dependency
  nr_double_t Vj = getPropertyDouble ("Vj");
  nr_double_t VjT;
  VjT = pnPotential_T (T1,T2, Vj);
  setScaledProperty ("Vj", VjT);

  // compute Cj0 temperature and area dependency
  nr_double_t Cj0 = getPropertyDouble ("Cj0");
  nr_double_t M   = getPropertyDouble ("M");
  Cj0 = pnCapacitance_T (T1, T2, M, VjT / Vj, Cj0);
  setScaledProperty ("Cj0", Cj0 * A);

  // check unphysical parameters
  if (M > 1.0) {
    logprint (LOG_ERROR, "WARNING: Unphysical model parameter M = %g in "
	      "Diode `%s'\n", M, getName ());
  }

  // compute Bv temperature dependency
  nr_double_t Bv  = getPropertyDouble ("Bv");
  nr_double_t Tbv = getPropertyDouble ("Tbv");
  nr_double_t DT  = T2 - T1;
  Bv = Bv - Tbv * DT;
  setScaledProperty ("Bv", Bv);

  // compute Tt temperature dependency
  nr_double_t Tt   = getPropertyDouble ("Tt");
  nr_double_t Ttt1 = getPropertyDouble ("Ttt1");
  nr_double_t Ttt2 = getPropertyDouble ("Ttt2");
  Tt = Tt * (1 + Ttt1 * DT + Ttt2 * DT * DT);
  setScaledProperty ("Tt", Tt);

  // compute M temperature dependency
  nr_double_t Tm1 = getPropertyDouble ("Tm1");
  nr_double_t Tm2 = getPropertyDouble ("Tm2");
  M = M * (1 + Tm1 * DT + Tm2 * DT * DT);
  setScaledProperty ("M", M);

  // compute Rs temperature and area dependency
  nr_double_t Rs  = getPropertyDouble ("Rs");
  nr_double_t Trs = getPropertyDouble ("Trs");
  Rs = Rs * (1 + Trs * DT);
  setScaledProperty ("Rs", Rs / A);
}

// Prepares DC (i.e. HB) analysis.
void diode::prepareDC (void) {
  // allocate MNA matrices
  allocMatrixMNA ();

  // initialize scalability
  initModel ();

  // initialize starting values
  Ud = real (getV (NODE_A) - getV (NODE_C));
  for (int i = 0; i < deviceStates (); i++) {
    deviceState (i);
    UdPrev = Ud;
  }

  // get device temperature
  nr_double_t T = getPropertyDouble ("Temp");

  // possibly insert series resistance
  nr_double_t Rs = getScaledProperty ("Rs");
  if (Rs != 0.0) {
    // create additional circuit if necessary and reassign nodes
    rs = splitResistor (this, rs, "Rs", "anode", NODE_A);
    rs->setProperty ("Temp", T);
    rs->setProperty ("R", Rs);
    rs->setProperty ("Controlled", getName ());
    rs->initDC ();
  }
  // no series resistance
  else {
    disableResistor (this, rs, NODE_A);
  }

  // calculate actual breakdown voltage
  Bv = getScaledProperty ("Bv");
  if (Bv != 0) {
    nr_double_t Ibv, Is, tol, Ut, Xbv, Xibv;
    Ibv = getPropertyDouble ("Ibv");
    Is = getScaledProperty ("Is");
    Ut = celsius2kelvin (T) * kBoverQ;
    // adjust very small breakdown currents
    if (Ibv < Is * Bv / Ut) {
      Ibv = Is * Bv / Ut;
      Xbv = Bv;
      logprint (LOG_ERROR, "WARNING: Increased breakdown current to %g to "
		"match the saturation current %g\n", Ibv, Is);
    }
    // fit reverse and forward regions
    else {
      int good = 0;
      tol = 1e-3 * Ibv;
      Xbv = Bv - Ut * qucs::log (1 + Ibv / Is);
      for (int i = 0; i < 25 ; i++) {
	Xbv = Bv - Ut * qucs::log (Ibv / Is + 1 - Xbv / Ut);
	Xibv = Is * (qucs::exp ((Bv - Xbv) / Ut) - 1 + Xbv / Ut);
	if (fabs (Xibv - Ibv) < tol) {
	  Bv = Xbv;
	  good = 1;
	  break;
	}
      }
      if (!good) {
	logprint (LOG_ERROR, "WARNING: Unable to fit reverse and forward "
		  "diode regions using Bv=%g and Ibv=%g\n", Bv, Ibv);
      }
    }
  }
}

// Callback for initializing the DC analysis.
void diode::initDC (void) {
  deviceStates (StateVars, 1);
  doHB = false;
  prepareDC ();
}

// Callback for restarting the DC analysis.
void diode::restartDC (void) {
  // apply starting value to previous iteration value
  UdPrev = real (getV (NODE_A) - getV (NODE_C));
}

// Callback for DC analysis.
void diode::calcDC (void) {
  // get device properties
  nr_double_t Is  = getScaledProperty ("Is");
  nr_double_t N   = getPropertyDouble ("N");
  nr_double_t Isr = getScaledProperty ("Isr");
  nr_double_t Nr  = getPropertyDouble ("Nr");
  nr_double_t Ikf = getPropertyDouble ("Ikf");
  nr_double_t T   = getPropertyDouble ("Temp");

  nr_double_t Ut, Ieq, Ucrit, gtiny;

  T = celsius2kelvin (T);
  Ut = T * kBoverQ;
  Ud = real (getV (NODE_A) - getV (NODE_C));

  // critical voltage necessary for bad start values
  Ucrit = pnCriticalVoltage (Is, N * Ut);
  if (Bv != 0 && Ud < std::min (0.0, -Bv + 10 * N * Ut)) {
    nr_double_t V = -(Ud + Bv);
    V = pnVoltage (V, -(UdPrev + Bv), Ut * N, Ucrit);
    Ud = -(V + Bv);
  }
  else {
    Ud = pnVoltage (Ud, UdPrev, Ut * N, Ucrit);
  }
  UdPrev = Ud;

  // tiny derivative for little junction voltage
  gtiny = (Ud < - 10 * Ut * N && Bv != 0) ? (Is + Isr) : 0;

  if (Ud >= -3 * N * Ut) { // forward region
    gd = pnConductance (Ud, Is, Ut * N) + pnConductance (Ud, Isr, Ut * Nr);
    Id = pnCurrent (Ud, Is, Ut * N) + pnCurrent (Ud, Isr, Ut * Nr);
  }
  else if (Bv == 0 || Ud >= -Bv) { // reverse region
    nr_double_t a = 3 * N * Ut / (Ud * euler);
    a = cubic (a);
    Id = -Is * (1 + a);
    gd = +Is * 3 * a / Ud;
  }
  else { // middle region
    nr_double_t a = qucs::exp (-(Bv + Ud) / N / Ut);
    Id = -Is * a;
    gd = +Is * a / Ut / N;
  }

  // knee current calculations
  if (Ikf != 0.0) {
    nr_double_t a = Ikf / (Ikf + Id);
    gd *= 0.5 * (2 - Id * a / Ikf) * qucs::sqrt (a);
    Id *= qucs::sqrt (a);
  }

  Id += gtiny * Ud;
  gd += gtiny;

  // HB simulation
  if (doHB) {
    Ieq = Id;
    setGV (NODE_C, -gd * Ud);
    setGV (NODE_A, +gd * Ud);
  }
  // DC and transient simulation
  else {
    Ieq = Id - Ud * gd;
  }

  // fill in I-Vector
  setI (NODE_C, +Ieq);
  setI (NODE_A, -Ieq);

  // fill in G-Matrix
  setY (NODE_C, NODE_C, +gd); setY (NODE_A, NODE_A, +gd);
  setY (NODE_C, NODE_A, -gd); setY (NODE_A, NODE_C, -gd);
}

// Saves operating points (voltages).
void diode::saveOperatingPoints (void) {
  nr_double_t Vd = real (getV (NODE_A) - getV (NODE_C));
  setOperatingPoint ("Vd", Vd);
}

// Loads operating points (voltages).
void diode::loadOperatingPoints (void) {
  Ud = getOperatingPoint ("Vd");
}

// Calculates and saves operating points.
void diode::calcOperatingPoints (void) {

  // load operating points
  loadOperatingPoints ();

  // get necessary properties
  nr_double_t M   = getScaledProperty ("M");
  nr_double_t Cj0 = getScaledProperty ("Cj0");
  nr_double_t Vj  = getScaledProperty ("Vj");
  nr_double_t Fc  = getPropertyDouble ("Fc");
  nr_double_t Cp  = getPropertyDouble ("Cp");
  nr_double_t Tt  = getScaledProperty ("Tt");

  // calculate capacitances and charges
  nr_double_t Cd;
  Cd = pnCapacitance (Ud, Cj0, Vj, M, Fc) + Tt * gd + Cp;
  Qd = pnCharge (Ud, Cj0, Vj, M, Fc) + Tt * Id + Cp * Ud;

  // save operating points
  setOperatingPoint ("gd", gd);
  setOperatingPoint ("Id", Id);
  setOperatingPoint ("Cd", Cd);
}

// Callback for initializing the AC analysis.
void diode::initAC (void) {
  allocMatrixMNA ();
}

// Callback for the AC analysis.
void diode::calcAC (nr_double_t frequency) {
  nr_double_t gd = getOperatingPoint ("gd");
  nr_double_t Cd = getOperatingPoint ("Cd");
  nr_complex_t y = nr_complex_t (gd, Cd * 2.0 * pi * frequency);
  setY (NODE_C, NODE_C, +y); setY (NODE_A, NODE_A, +y);
  setY (NODE_C, NODE_A, -y); setY (NODE_A, NODE_C, -y);
}

// Callback for the AC noise analysis.
void diode::calcNoiseAC (nr_double_t frequency) {
  setMatrixN (calcMatrixCy (frequency));
}

#define qState 0 // charge state
#define cState 1 // current state

// Callback for initializing the TR analysis.
void diode::initTR (void) {
  setStates (2);
  initDC ();
}

// Callback for the TR analysis.
void diode::calcTR (nr_double_t) {
  calcDC ();
  saveOperatingPoints ();
  calcOperatingPoints ();

  nr_double_t Cd = getOperatingPoint ("Cd");

  transientCapacitance (qState, NODE_A, NODE_C, Cd, Ud, Qd);
}

// Callback for initializing the HB analysis.
void diode::initHB (int frequencies) {
  deviceStates (StateVars, frequencies);
  doHB = true;
  prepareDC ();
  allocMatrixHB ();
}

// Callback for the HB analysis.
void diode::calcHB (int frequency) {
  // set current frequency state
  deviceState (frequency);

  // g's (dI/dU) into Y-Matrix and I's into I-Vector
  calcDC ();

  // calculate Q and C
  saveOperatingPoints ();
  calcOperatingPoints ();

  nr_double_t Cd = getOperatingPoint ("Cd");

  // fill in Q's in Q-Vector
  setQ (NODE_C, +Qd);
  setQ (NODE_A, -Qd);

  setCV (NODE_C, -Cd * Ud);
  setCV (NODE_A, +Cd * Ud);

  // fill in C's (dQ/dU) into QV-Matrix
  setQV (NODE_C, NODE_C, +Cd); setQV (NODE_A, NODE_A, +Cd);
  setQV (NODE_C, NODE_A, -Cd); setQV (NODE_A, NODE_C, -Cd);
}

// properties
PROP_REQ [] = {
  { "Is", PROP_REAL, { 1e-15, PROP_NO_STR }, PROP_POS_RANGE },
  { "N", PROP_REAL, { 1, PROP_NO_STR }, PROP_RNGII (1e-6, 100) },
  { "M", PROP_REAL, { 0.5, PROP_NO_STR }, PROP_RNGII (0, 2) },
  { "Cj0", PROP_REAL, { 10e-15, PROP_NO_STR }, PROP_POS_RANGE },
  { "Vj", PROP_REAL, { 0.7, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Rs", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGE },
  { "Isr", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGE },
  { "Nr", PROP_REAL, { 2, PROP_NO_STR }, PROP_RNGII (0.1, 100) },
  { "Bv", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGE },
  { "Ibv", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "Ikf", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGE },
  { "Tt", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGE },
  { "Fc", PROP_REAL, { 0.5, PROP_NO_STR }, PROP_RNGIX (0, 1) },
  { "Cp", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGE },
  { "Kf", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGE },
  { "Af", PROP_REAL, { 1, PROP_NO_STR }, PROP_POS_RANGE },
  { "Ffe", PROP_REAL, { 1, PROP_NO_STR }, PROP_POS_RANGE },
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  { "Xti", PROP_REAL, { 3, PROP_NO_STR }, PROP_POS_RANGE },
  { "Eg", PROP_REAL, { EgSi, PROP_NO_STR }, PROP_POS_RANGE },
  { "Tbv", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGE },
  { "Trs", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Ttt1", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Ttt2", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Tm1", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Tm2", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Tnom", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  { "Area", PROP_REAL, { 1, PROP_NO_STR }, PROP_POS_RANGEX },
  PROP_NO_PROP };
struct define_t diode::cirdef =
  { "Diode", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_NONLINEAR, PROP_DEF };
