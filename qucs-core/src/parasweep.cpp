/*
 * parasweep.cpp - parameter sweep class implementation
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "vector.h"
#include "dataset.h"
#include "net.h"
#include "netdefs.h"
#include "ptrlist.h"
#include "analysis.h"
#include "variable.h"
#include "environment.h"
#include "sweep.h"
#include "parasweep.h"

using namespace qucs::eqn;

namespace qucs {

// Constructor creates an unnamed instance of the parasweep class.
parasweep::parasweep () : analysis () {
  var = NULL;
  swp = NULL;
  eqn = NULL;
  type = ANALYSIS_SWEEP;
}

// Constructor creates a named instance of the parasweep class.
parasweep::parasweep (char * n) : analysis (n) {
  var = NULL;
  swp = NULL;
  eqn = NULL;
  type = ANALYSIS_SWEEP;
}

// Destructor deletes the parasweep class object.
parasweep::~parasweep () {
  delete swp;
}

/* The copy constructor creates a new instance of the parasweep class
   based on the given parasweep object. */
parasweep::parasweep (parasweep & p) : analysis (p) {
  var = new variable (*p.var);
  if (p.swp) swp = new sweep (*p.swp);
}

// Short macro in order to obtain the correct constant value.
#define D(con) ((constant *) (con))->d
#define E(equ) ((eqn::node *) (equ))

/* Initializes the parameter sweep. */
int parasweep::initialize (void) {
  constant * val;

  // get fixed simulation properties
  const char * const n = getPropertyString ("Param");

  // create sweep if necessary
  if (swp == NULL) {
    swp = createSweep (n);
  }

  // get parameter name and the appropriate variable from the current
  // environment, possibly add the variable to the environment if it
  // does not exist yet (which is somehow useless at all)
  if ((var = env->getVariable (n)) == NULL) {
    var = new variable (n);
    val = new constant (TAG_DOUBLE);
    var->setConstant (val);
    env->addVariable (var);
  }
  else val = var->getConstant ();

  // put variable also into equation checker if necessary
  if (!env->getChecker()->containsVariable (n)) {
    eqn = env->getChecker()->addDouble ("#sweep", n, 0);
  }

  // initialize first sweep value in environment and equation checker
  nr_double_t v = swp->get (0);
  env->setDoubleConstant (n, v);
  env->setDouble (n, v);

  // also run initialize functionality for all children
  if (actions != nullptr) {
    for (auto *a : *actions) {
      a->initialize ();
      a->setProgress (false);
    }
  }
  return 0;
}

/* Cleans the parameter sweep up. */
int parasweep::cleanup (void) {

  // remove additional equation from equation checker
  if (eqn) {
    env->getChecker()->dropEquation (E (eqn));
    delete E (eqn);
    eqn = NULL;
  }

  // also run cleanup functionality for all children
  if( actions != nullptr)
    for (auto *a : *actions)
      a->cleanup ();

  return 0;
}

/* This is the parameter sweep solver. */
int parasweep::solve (void) {
  int err = 0;
  runs++;

  // get fixed simulation properties
  const char * const n = getPropertyString ("Param");

  // run the parameter sweep
  swp->reset ();
  for (int i = 0; i < swp->getSize (); i++) {
    // obtain next sweep point
    nr_double_t v = swp->next ();
    // display progress bar if requested
    if (progress) logprogressbar (i, swp->getSize (), 40);
    // update environment and equation checker, then run solver
    env->setDoubleConstant (n, v);
    env->setDouble (n, v);
    env->runSolver ();
    // save results (swept parameter values)
    if (runs == 1) saveResults ();
#if DEBUG
    logprint (LOG_STATUS, "NOTIFY: %s: running netlist for %s = %g\n",
	      getName (), n, v);
#endif
    for (auto *a : *actions) {
      err |= a->solve ();
      // assign variable dataset dependencies to last order analyses
      ptrlist<analysis> * lastorder = subnet->findLastOrderChildren (this);
      for (auto *dep : *lastorder)
	data->assignDependency (dep->getName (), var->getName ());
    }
  }
  // clear progress bar
  if (progress) logprogressclear (40);
  return err;
}

/* This function saves the results of a single solve() functionality
   into the output dataset. */
void parasweep::saveResults (void) {
  qucs::vector * v;

  // add current frequency to the dependencies of the output dataset
  if ((v = data->findDependency (var->getName ())) == NULL) {
    v = new qucs::vector (var->getName ());
    v->setOrigin (getName ());
    data->addDependency (v);
  }
  v->add (D (var->getConstant ()));
}

// properties
PROP_REQ [] = {
  { "Type", PROP_STR, { PROP_NO_VAL, "lin" }, PROP_RNG_TYP },
  { "Param", PROP_STR, { PROP_NO_VAL, "R1" }, PROP_NO_RANGE },
  { "Sim", PROP_STR, { PROP_NO_VAL, "DC1" }, PROP_NO_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Points", PROP_INT, { 5, PROP_NO_STR }, PROP_MIN_VAL (2) },
  { "Stop", PROP_REAL, { 50, PROP_NO_STR }, PROP_NO_RANGE },
  { "Start", PROP_REAL, { 5, PROP_NO_STR }, PROP_NO_RANGE },
  { "Values", PROP_LIST, { 5, PROP_NO_STR }, PROP_NO_RANGE },
  PROP_NO_PROP };
struct define_t parasweep::anadef =
  { "SW", 0, PROP_ACTION, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };

} // namespace qucs
