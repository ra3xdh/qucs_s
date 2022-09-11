/*
 * eqndefined.cpp - equation defined device class implementation
 *
 * Copyright (C) 2007 Stefan Jahn <stefan@lkcc.org>
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
#include "device.h"
#include "eqndefined.h"

using namespace qucs;
using namespace qucs::eqn;

// Constructor for the equation defined device.
eqndefined::eqndefined () : circuit () {
  type = CIR_EQNDEFINED;
  setVariableSized (true);
  veqn = NULL;
  ieqn = NULL;
  qeqn = NULL;
  geqn = NULL;
  ceqn = NULL;
  _jstat = NULL;
  _jdyna = NULL;
  _charges = NULL;
}

// Destructor deletes equation defined device object from memory.
eqndefined::~eqndefined () {
  free (veqn);
  free (ieqn);
  free (geqn);
  free (qeqn);
  free (ceqn);
  free (_jstat);
  free (_jdyna);
  free (_charges);
}

// Callback for initializing the DC analysis.
void eqndefined::initDC (void) {
  allocMatrixMNA ();
  if (ieqn == NULL) initModel ();
  doHB = false;
}

// Some help macros.
#define A(a)  ((assignment *) (a))
#define C(c)  ((constant *) (c))
#define BP(n) real (getV (n * 2 + 0) - getV (n * 2 + 1))

// Creates a variable name from the given arguments.
char * eqndefined::createVariable (const char * base, int n, bool pfx) {
  const char * str = strchr (getName (), '.');
  if (str != NULL)
    str = strrchr (str, '.') + 1;
  else
    str = getName ();
  char * txt = (char *) malloc (strlen (str) + strlen (base) + 3);
  if (pfx)
    sprintf (txt, "%s.%s%d", str, base, n);
  else
    sprintf (txt, "%s%d", base, n);
  return txt;
}

// Creates also a variable name from the given arguments.
char * eqndefined::createVariable (const char * base, int r, int c, bool pfx) {
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

// Saves the given value into the equation result.
void eqndefined::setResult (void * eqn, nr_double_t val) {
  A(eqn)->evaluate ();
  constant * c = A(eqn)->getResult ();
  c->d = val;
}

// Returns the result of the equation.
nr_double_t eqndefined::getResult (void * eqn) {
  A(eqn)->evaluate ();
  return A(eqn)->getResultDouble ();
}

// Initializes the equation defined device.
void eqndefined::initModel (void) {
  int i, j, k, branches = getSize () / 2;
  char * in, * qn, * vn, * gn, * cn, * vnold, * inold;
  eqn::node * ivalue, * qvalue, * diff;

  // allocate space for equation pointers
  veqn = (void **) malloc (sizeof (assignment *) * branches);
  ieqn = (void **) malloc (sizeof (assignment *) * branches);
  geqn = (void **) malloc (sizeof (assignment *) * branches * branches);
  qeqn = (void **) malloc (sizeof (assignment *) * branches);
  ceqn = (void **) malloc (sizeof (assignment *) * branches * branches);

  // allocate space for Jacobians and charges
  _jstat = (nr_double_t *) malloc (sizeof (nr_double_t) * branches * branches);
  _jdyna = (nr_double_t *) malloc (sizeof (nr_double_t) * branches * branches);
  _charges = (nr_double_t *) malloc (sizeof (nr_double_t) * branches);

  // first create voltage variables
  for (i = 0; i < branches; i++) {
    vn = createVariable ("V", i + 1);
    if ((veqn[i] = getEnv()->getChecker()->findEquation (vn)) == NULL) {
      veqn[i] = getEnv()->getChecker()->addDouble ("#voltage", vn, 0);
      A(veqn[i])->evalType ();
      A(veqn[i])->skip = 1;
    }
    free (vn);
  }

  // prepare current and charge equations
  for (i = 0; i < branches; i++) {

    // fetch current and charge equations
    in = createVariable ("I", i + 1);
    ivalue = getEnv()->getChecker()->findEquation (in);
    if (!ivalue) {
      logprint (LOG_ERROR, "ERROR: current equation `%s' not found for "
		"EDD `%s'\n", in, getName ());
    }
    qn = createVariable ("Q", i + 1);
    qvalue = getEnv()->getChecker()->findEquation (qn);
    if (!qvalue) {
      logprint (LOG_ERROR, "ERROR: charge equation `%s' not found for "
		"EDD `%s'\n", qn, getName ());
    }
    free (in);
    free (qn);

    // replace voltage and current references
    for (j = 0; j < branches; j++) {
      in = createVariable ("I", j + 1);
      inold = createVariable ("I", j + 1, false);
      vn = createVariable ("V", j + 1);
      vnold = createVariable ("V", j + 1, false);
      if (ivalue) {
	ivalue->replace (vnold, vn);
	ivalue->replace (inold, in);
      }
      if (qvalue) {
	qvalue->replace (vnold, vn);
	qvalue->replace (inold, in);
      }
      free (vnold); free (vn);
      free (inold); free (in);
    }

    // setup and save equations for currents and charges
    ieqn[i] = ivalue;
    qeqn[i] = qvalue;
  }

  // evaluate types of currents and charges
  for (i = 0; i < branches; i++) {
    if (ieqn[i]) { A(ieqn[i])->evalType (); A(ieqn[i])->skip = 1; }
    if (qeqn[i]) { A(qeqn[i])->evalType (); A(qeqn[i])->skip = 1; }
  }

  // create derivatives of currents
  for (k = 0, i = 0; i < branches; i++) {
    ivalue = A(ieqn[i]);

    // create "static" differentiations
    for (j = 0; j < branches; j++, k++) {
      vn = createVariable ("V", j + 1);

      // create conductance equations
      if (ivalue) {
	gn = createVariable ("G", i + 1, j + 1);
	if ((geqn[k] = getEnv()->getChecker()->findEquation (gn)) == NULL) {
	  diff = ivalue->differentiate (vn);
	  getEnv()->getChecker()->addEquation (diff);
	  diff->evalType ();
	  diff->skip = 1;
	  geqn[k] = diff;
	  A(diff)->rename (gn);
	}
	free (gn);
#if DEBUG
	logprint (LOG_STATUS, "DEBUG: %s\n", A(geqn[k])->toString ());
#endif
      }
      else geqn[k] = NULL;

      free (vn);
    }
  }

  // create derivatives of charges
  for (k = 0, i = 0; i < branches; i++) {
    qvalue = A(qeqn[i]);

    // create "dynamic" differentiations
    for (j = 0; j < branches; j++, k++) {
      vn = createVariable ("V", j + 1);

      // create capacitance equations
      if (qvalue) {
	cn = createVariable ("C", i + 1, j + 1);
	if ((ceqn[k] = getEnv()->getChecker()->findEquation (cn)) == NULL) {
	  diff = qvalue->differentiate (vn);
	  getEnv()->getChecker()->addEquation (diff);
	  diff->evalType ();
	  ceqn[k] = diff;
	  A(diff)->rename (cn);

	  // apply dQ/dI * dI/dV => dQ/dV derivatives
	  for (int l = 0; l < branches; l++) {
	    in = createVariable ("I", l + 1);
	    diff = qvalue->differentiate (in);
	    A(diff)->mul (A(geqn[l * branches + j]));
	    A(ceqn[k])->add (A(diff));
	    delete diff;
	    free (in);
	  }
	  A(ceqn[k])->evalType ();
	  A(ceqn[k])->skip = 1;
	}
	free (cn);
#if DEBUG
	logprint (LOG_STATUS, "DEBUG: %s\n", A(ceqn[k])->toString ());
#endif
      }
      else ceqn[k] = NULL;

      free (vn);
    }
  }
}

// Update local variable equations.
void eqndefined::updateLocals (void) {
  int i, branches = getSize () / 2;

  // update voltages for equations
  for (i = 0; i < branches; i++) {
    setResult (veqn[i], BP (i));
  }
  // get local subcircuit values
  getEnv()->passConstants ();
  getEnv()->equationSolver ();
}

// Callback for DC analysis.
void eqndefined::calcDC (void) {
  int i, j, k, branches = getSize () / 2;

  // update local equations
  updateLocals ();

  // calculate currents and put into right-hand side
  for (i = 0; i < branches; i++) {
    nr_double_t c = getResult (ieqn[i]);
    setI (i * 2 + 0, -c);
    setI (i * 2 + 1, +c);
  }

  // calculate derivatives and put into Jacobian and right-hand side
  for (k = 0, i = 0; i < branches; i++) {
    nr_double_t gv = 0;
    // usual G (dI/dV) entries
    for (j = 0; j < branches; j++, k++) {
      nr_double_t g = getResult (geqn[k]);
      setY (i * 2 + 0, j * 2 + 0, +g);
      setY (i * 2 + 1, j * 2 + 1, +g);
      setY (i * 2 + 0, j * 2 + 1, -g);
      setY (i * 2 + 1, j * 2 + 0, -g);
      gv += g * BP (j);
    }
    // during HB
    if (doHB) {
      setGV (i * 2 + 0, +gv);
      setGV (i * 2 + 1, -gv);
    }
    // DC and transient analysis
    else {
      addI (i * 2 + 0, +gv);
      addI (i * 2 + 1, -gv);
    }
  }
}

// Evaluate operating points.
void eqndefined::evalOperatingPoints (void) {
  int i, j, k, branches = getSize () / 2;

  // save values for charges, conductances and capacitances
  for (k = 0, i = 0; i < branches; i++) {
    nr_double_t q = getResult (qeqn[i]);
    _charges[i] = q;
    for (j = 0; j < branches; j++, k++) {
      nr_double_t g = getResult (geqn[k]);
      _jstat[k] = g;
      nr_double_t c = getResult (ceqn[k]);
      _jdyna[k] = c;
    }
  }
}

// Saves operating points.
void eqndefined::saveOperatingPoints (void) {

  // update local equations
  updateLocals ();

  // save values for charges, conductances and capacitances
  evalOperatingPoints ();
}

// Callback for initializing the AC analysis.
void eqndefined::initAC (void) {
  allocMatrixMNA ();
  doHB = false;
}

// Callback for AC analysis.
void eqndefined::calcAC (nr_double_t frequency) {
  setMatrixY (calcMatrixY (frequency));
}

// Computes Y-matrix for AC analysis.
matrix eqndefined::calcMatrixY (nr_double_t frequency) {
  int i, j, k, branches = getSize () / 2;
  matrix y (2 * branches);
  nr_double_t o = 2 * pi * frequency;

  // merge conductances and capacitances
  for (k = 0, i = 0; i < branches; i++) {
    for (j = 0; j < branches; j++, k++) {
      int r = i * 2;
      int c = j * 2;
      nr_complex_t val = nr_complex_t (_jstat[k], o * _jdyna[k]);
      y (r + 0, c + 0) = +val;
      y (r + 1, c + 1) = +val;
      y (r + 0, c + 1) = -val;
      y (r + 1, c + 0) = -val;
    }
  }

  return y;
}

// Callback for initializing the TR analysis.
void eqndefined::initTR (void) {
  int branches = getSize () / 2;
  setStates (2 * branches);
  initDC ();
}

// Callback for the TR analysis.
void eqndefined::calcTR (nr_double_t) {
  int state, i, j, k, branches = getSize () / 2;

  // run usual DC iteration, then save operating points
  calcDC ();

  // calculate Q and C
  evalOperatingPoints ();

  // charge integrations
  for (i = 0; i < branches; i++) {
    int r = i * 2;
    state = 2 * i;
    transientCapacitanceQ (state, r + 0, r + 1, _charges[i]);
  }

  // charge: 2-node, voltage: 2-node
  for (k = 0, i = 0; i < branches; i++) {
    for (j = 0; j < branches; j++, k++) {
      int r = i * 2;
      int c = j * 2;
      nr_double_t v = BP (j);
      transientCapacitanceC (r + 0, r + 1, c + 0, c + 1, _jdyna[k], v);
    }
  }
}

// Callback for initializing the S-parameter analysis.
void eqndefined::initSP (void) {
  allocMatrixS ();
  doHB = false;
}

// Callback for S-parameter analysis.
void eqndefined::calcSP (nr_double_t frequency) {
  setMatrixS (ytos (calcMatrixY (frequency)));
}

// Callback for initializing the HB analysis.
void eqndefined::initHB (int) {
  allocMatrixHB ();
  if (ieqn == NULL) initModel ();
  doHB = true;
}

// Callback for HB analysis.
void eqndefined::calcHB (int) {
  int i, j, k, branches = getSize () / 2;

  // G's (dI/dV) into Y-Matrix and I's into I-Vector
  calcDC ();

  // calculate Q and C
  evalOperatingPoints ();

  // setup additional charge right-hand side
  for (i = 0; i < branches; i++) {
    setQ (i * 2 + 0, -_charges[i]);
    setQ (i * 2 + 1, +_charges[i]);
  }

  // fill in C's (dQ/dV) in QV-Matrix and CV into right hand side
  for (k = 0, i = 0; i < branches; i++) {
    nr_double_t cv = 0;
    for (j = 0; j < branches; j++, k++) {
      int r = i * 2;
      int c = j * 2;
      nr_double_t val = _jdyna[k];
      setQV (r + 0, c + 0, +val);
      setQV (r + 1, c + 1, +val);
      setQV (r + 0, c + 1, -val);
      setQV (r + 1, c + 0, -val);
      cv += val * BP (j);
    }
    setCV (i * 2 + 0, +cv);
    setCV (i * 2 + 1, -cv);
  }
}

// properties
PROP_REQ [] = {
  { "I1", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Q1", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "I2", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Q2", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "I3", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Q3", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "I4", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Q4", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "I5", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Q5", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "I6", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Q6", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "I7", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Q7", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "I8", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Q8", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "I9", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Q9", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "I10", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Q10", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "I11", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Q11", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "I12", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Q12", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "I13", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Q13", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "I14", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Q14", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "I15", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Q15", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "I16", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Q16", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "I17", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Q17", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "I18", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Q18", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "I19", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Q19", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "I20", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Q20", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  PROP_NO_PROP };
struct define_t eqndefined::cirdef =
  { "EDD",
    PROP_NODES, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_NONLINEAR, PROP_DEF };
