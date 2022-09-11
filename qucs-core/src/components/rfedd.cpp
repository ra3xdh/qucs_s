/*
 * rfedd.cpp - equation defined RF device class implementation
 *
 * Copyright (C) 2008 Stefan Jahn <stefan@lkcc.org>
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
#include "equation.h"
#include "environment.h"
#include "rfedd.h"

using namespace qucs;
using namespace qucs::eqn;

// Constructor for the equation defined RF device.
rfedd::rfedd () : circuit () {
  type = CIR_RFEDD;
  setVariableSized (true);
  peqn = NULL;
}

// Destructor deletes equation defined RF device object from memory.
rfedd::~rfedd () {
  free (peqn);
}

// Callback for initializing the DC analysis.
void rfedd::initDC (void) {

  // get appropriate property value
  const char * const dc = getPropertyString ("duringDC");

  // a short during DC
  if (!strcmp (dc, "short")) {
    int v, n, lastnode = getSize () - 1;
    setVoltageSources (lastnode);
    allocMatrixMNA ();
    // place zero voltage sources
    for (v = VSRC_1, n = NODE_1; n < lastnode; n++, v++) {
      voltageSource (v, n, lastnode);
    }
    return;
  }
  // an open during DC
  else if (!strcmp (dc, "open")) {
    setVoltageSources (0);
    allocMatrixMNA ();
    return;
  }
  // zero frequency evaluation
  else if (!strcmp (dc, "zerofrequency")) {
    prepareModel ();
    initMNA ();
    calcMNA (0);
    return;
  }
  // none specified, DC value of IDFT ?
  else {
    setVoltageSources (0);
    allocMatrixMNA ();
  }
}

// Some help macros.
#define A(a)  ((assignment *) (a))

// Creates a parameter variable name from the given arguments.
char * rfedd::createVariable (const char * base, int r, int c, bool pfx) {
  const char * str = strchr (getName (), '.');
  if (str != NULL)
    str = strrchr (str, '.') + 1;
  else
    str = getName ();
  char * txt = (char *) malloc (strlen (str) + strlen (base) + 4);
  if (pfx)
    sprintf (txt, "%s.%s%d%d", str, base, r, c);
  else
    sprintf (txt, "%s%d%d", base, r, c);
  return txt;
}

// Creates a variable name from the given arguments.
char * rfedd::createVariable (const char * base, bool pfx) {
  const char * str = strchr (getName (), '.');
  if (str != NULL)
    str = strrchr (str, '.') + 1;
  else
    str = getName ();
  char * txt = (char *) malloc (strlen (str) + strlen (base) + 2);
  if (pfx)
    sprintf (txt, "%s.%s", str, base);
  else
    sprintf (txt, "%s", base);
  return txt;
}

// Saves the given real value into the equation result.
void rfedd::setResult (void * eqn, nr_double_t val) {
  A(eqn)->evaluate ();
  constant * c = A(eqn)->getResult ();
  c->d = val;
}

// Saves the given complex value into the equation result.
void rfedd::setResult (void * eqn, nr_complex_t val) {
  A(eqn)->evaluate ();
  constant * c = A(eqn)->getResult ();
  *(c->c) = val;
}

// Returns the result of the equation.
nr_complex_t rfedd::getResult (void * eqn) {
  A(eqn)->evaluate ();
  return A(eqn)->getResultComplex ();
}

// Initializes the equation defined device.
void rfedd::initModel (void) {
  int i, j, k, ports = getSize ();
  char * pn, * sn, * snold, * fn, * fnold;
  const char * vr;
  eqn::node * pvalue;

  // allocate space for equation pointers
  peqn = (void **) malloc (sizeof (assignment *) * ports * ports);

  // first create frequency variables
  sn = createVariable ("S");
  snold = createVariable ("S", false);
  fn = createVariable ("F");
  fnold = createVariable ("F", false);
  seqn = getEnv()->getChecker()->addComplex ("#laplace", sn, nr_complex_t (0, 0));
  feqn = getEnv()->getChecker()->addDouble ("#frequency", fn, 0);
  A(seqn)->evalType (); A(seqn)->skip = 1;
  A(feqn)->evalType (); A(feqn)->skip = 1;

  // obtain type of parameters
  const char * const type = getPropertyString ("Type");

  // prepare device equations
  for (k = 0, i = 0; i < ports; i++) {
    for (j = 0; j < ports; j++, k++) {
      // find equation referenced in property
      pn = createVariable ("P", i + 1, j + 1, false);
      vr = getPropertyReference (pn);
      pvalue = getEnv()->getChecker()->findEquation (vr);
      if (!pvalue) {
	logprint (LOG_ERROR, "ERROR: %s-parameter equation `%s' not found for "
		  "RFEDD `%s'\n", type, vr, getName ());
      }
      else {
	// replace references to S and F by local references
	pvalue->replace (snold, sn);
	pvalue->replace (fnold, fn);
	// evaluate types of parameters
	A(pvalue)->evalType ();
	A(pvalue)->skip = 1;
      }
      // save equations for parameters
      peqn[k] = pvalue;
      free (pn);
    }
  }

  free (sn); free (snold);
  free (fn); free (fnold);
}

// Prepares model equations if necessary.
void rfedd::prepareModel (void) {
  if (peqn == NULL) initModel ();
}

// Update local variable equations.
void rfedd::updateLocals (nr_double_t frequency) {

  // update frequency variables for equations
  setResult (seqn, nr_complex_t (0, 2 * pi * frequency));
  setResult (feqn, frequency);

  // get local subcircuit values
  getEnv()->passConstants ();
  getEnv()->equationSolver ();
}

// Callback for DC analysis.
void rfedd::calcDC (void) {
}

// Initializes MNA representation depending on parameter type.
void rfedd::initMNA (void) {
  int i, ports = getSize ();
  const char * const type = getPropertyString ("Type");
  switch (type[0]) {
  case 'Y':
    setVoltageSources (0);
    allocMatrixMNA ();
    break;
  case 'Z':
    setVoltageSources (ports);
    allocMatrixMNA ();
    for (i = 0; i < ports; i++) setC (i, i, -1);
    for (i = 0; i < ports; i++) setB (i, i, +1);
    break;
  case 'S':
    setVoltageSources (ports);
    allocMatrixMNA ();
    for (i = 0; i < ports; i++) setB (i, i, +1);
    break;
  case 'H':
    setVoltageSources (1);
    allocMatrixMNA ();
    setB (0, 0, +1); setC (0, 0, -1);
    break;
  case 'G':
    setVoltageSources (1);
    allocMatrixMNA ();
    setB (1, 0, +1); setC (0, 1, -1);
    break;
  case 'A':
    setVoltageSources (1);
    allocMatrixMNA ();
    setB (1, 0, -1); setC (0, 0, -1);
    break;
  case 'T':
    setVoltageSources (2);
    allocMatrixMNA ();
    setB (0, 0, +1); setB (1, 1, +1);
    setC (0, 0, -1); setC (1, 0, -1);
    break;
  }
}

// Calculates MNA representation depending on parameter type.
void rfedd::calcMNA (nr_double_t frequency) {
  const char * const type = getPropertyString ("Type");
  int r, c, ports = getSize ();
  matrix p = calcMatrix (frequency);
  switch (type[0]) {
  case 'Y':
    setMatrixY (p);
    break;
  case 'Z':
    for (r = 0; r < ports; r++)
      for (c = 0; c < ports; c++)
	setD (r, c, p (r, c));
    break;
  case 'S':
    for (r = 0; r < ports; r++)
      for (c = 0; c < ports; c++) {
	if (r == c) {
	  setC (r, c, p (r, c) - 1.0);
	  setD (r, c, z0 * (p (r, c) + 1.0));
	}
	else {
	  setC (r, c, p (r, c));
	  setD (r, c, z0 * p (r, c));
	}
      }
    break;
  case 'H':
    setY (1, 1, p (1, 1)); setB (1, 0, p (1, 0));
    setC (0, 1, p (0, 1)); setD (0, 0, p (0, 0));
    break;
  case 'G':
    setY (0, 0, p (0, 0)); setB (0, 0, p (0, 1));
    setC (0, 0, p (1, 0)); setD (0, 0, p (1, 1));
    break;
  case 'A':
    setY (0, 1, p (1, 0)); setB (0, 0, p (1, 1));
    setC (0, 1, p (0, 0)); setD (0, 0, p (0, 1));
    break;
  case 'T':
    setC (0, 1, p (0, 1) + p (0, 0));
    setC (1, 1, p (1, 1) + p (1, 0));
    setD (0, 0, -z0);
    setD (1, 0, +z0);
    setD (0, 1, z0 * (p (0, 1) - p (0, 0)));
    setD (1, 1, z0 * (p (1, 1) - p (1, 0)));
    break;
  }
}

// Callback for initializing the AC analysis.
void rfedd::initAC (void) {
  initMNA ();
  prepareModel ();
}

// Callback for AC analysis.
void rfedd::calcAC (nr_double_t frequency) {
  calcMNA (frequency);
}

// Computes parameter matrix.
matrix rfedd::calcMatrix (nr_double_t frequency) {
  int i, j, k, ports = getSize ();
  matrix p (ports);

  // update local equations
  updateLocals (frequency);

  // calculate parameters and put into Jacobian
  for (k = 0, i = 0; i < ports; i++) {
    for (j = 0; j < ports; j++, k++) {
      p (i, j) = getResult (peqn[k]);
    }
  }

  return p;
}

// Callback for initializing the TR analysis.
void rfedd::initTR (void) {
  initDC ();
}

// Callback for the TR analysis.
void rfedd::calcTR (nr_double_t) {
  calcDC ();
}

// Callback for initializing the S-parameter analysis.
void rfedd::initSP (void) {
  allocMatrixS ();
  prepareModel ();
}

// Callback for S-parameter analysis.
void rfedd::calcSP (nr_double_t frequency) {
  const char * const type = getPropertyString ("Type");
  matrix p = calcMatrix (frequency);
  switch (type[0]) {
  case 'Y':
    setMatrixS (ytos (p));
    break;
  case 'Z':
    setMatrixS (ztos (p));
    break;
  case 'S':
    setMatrixS (p);
    break;
  case 'H':
    setMatrixS (twoport (p, 'H', 'S'));
    break;
  case 'G':
    setMatrixS (twoport (p, 'G', 'S'));
    break;
  case 'A':
    setMatrixS (twoport (p, 'A', 'S'));
    break;
  case 'T':
    setMatrixS (twoport (p, 'T', 'S'));
    break;
  }
}

// properties
PROP_REQ [] = {
  { "Type", PROP_STR, { PROP_NO_VAL, "Y" },
    PROP_RNG_STR7 ("Y", "Z", "S", "H", "G", "A", "T") },
  { "duringDC", PROP_STR, { PROP_NO_VAL, "open" },
    PROP_RNG_STR4  ("open", "short", "unspecified", "zerofrequency") },
  { "P11", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "P12", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P13", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P14", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P15", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P16", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P17", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P18", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P21", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P22", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P23", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P24", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P25", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P26", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P27", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P28", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P31", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P32", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P33", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P34", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P35", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P36", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P37", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P38", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P41", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P42", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P43", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P44", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P45", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P46", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P47", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P48", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P51", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P52", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P53", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P54", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P55", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P56", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P57", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P58", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P61", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P62", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P63", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P64", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P65", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P66", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P67", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P68", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P71", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P72", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P73", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P74", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P75", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P76", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P77", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P78", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P81", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P82", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P83", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P84", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P85", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P86", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P87", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "P88", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  PROP_NO_PROP };
struct define_t rfedd::cirdef =
  { "RFEDD",
    PROP_NODES, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
