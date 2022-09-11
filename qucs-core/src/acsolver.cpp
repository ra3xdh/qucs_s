/*
 * acsolver.cpp - AC solver class implementation
 *
 * Copyright (C) 2004, 2005, 2007, 2008 Stefan Jahn <stefan@lkcc.org>
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
#include <cmath>

#include "object.h"
#include "complex.h"
#include "circuit.h"
#include "sweep.h"
#include "net.h"
#include "netdefs.h"
#include "analysis.h"
#include "nasolver.h"
#include "acsolver.h"

namespace qucs {

// Constructor creates an unnamed instance of the acsolver class.
acsolver::acsolver () : nasolver<nr_complex_t> () {
  swp = NULL;
  type = ANALYSIS_AC;
  setDescription ("AC");
  xn = NULL;
  noise = 0;
}

// Constructor creates a named instance of the acsolver class.
acsolver::acsolver (char * n) : nasolver<nr_complex_t> (n) {
  swp = NULL;
  type = ANALYSIS_AC;
  setDescription ("AC");
  xn = NULL;
  noise = 0;
}

// Destructor deletes the acsolver class object.
acsolver::~acsolver () {
  delete swp;
  delete xn;
}

/* The copy constructor creates a new instance of the acsolver class
   based on the given acsolver object. */
acsolver::acsolver (acsolver & o) : nasolver<nr_complex_t> (o) {
  swp = o.swp ? new sweep (*(o.swp)) : NULL;
  xn = o.xn ? new tvector<nr_double_t> (*(o.xn)) : NULL;
  noise = o.noise;
}

/* This is the AC netlist solver.  It prepares the circuit list for
   each requested frequency and solves it then. */
int acsolver::solve (void) {
  runs++;

  // run additional noise analysis ?
  noise = !strcmp (getPropertyString ("Noise"), "yes") ? 1 : 0;

  // create frequency sweep if necessary
  if (swp == NULL) {
    swp = createSweep ("acfrequency");
  }

  // initialize node voltages, first guess for non-linear circuits and
  // generate extra circuits if necessary
  init ();
  setCalculation ((calculate_func_t) &calc);
  solve_pre ();

  swp->reset ();
  for (int i = 0; i < swp->getSize (); i++) {
    freq = swp->next ();
    if (progress) logprogressbar (i, swp->getSize (), 40);

#if DEBUG && 0
    logprint (LOG_STATUS, "NOTIFY: %s: solving netlist for f = %e\n",
	      getName (), (double) freq);
#endif

    // start the linear solver
    eqnAlgo = ALGO_LU_DECOMPOSITION;
    solve_linear ();

    // compute noise if requested
    if (noise) solve_noise ();

    // save results
    saveAllResults (freq);
  }
  solve_post ();
  if (progress) logprogressclear (40);
  return 0;
}

/* Goes through the list of circuit objects and runs its calcAC()
   function. */
void acsolver::calc (acsolver * self) {
  circuit * root = self->getNet()->getRoot ();
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    c->calcAC (self->freq);
    if (self->noise) c->calcNoiseAC (self->freq);
  }
}

/* Goes through the list of circuit objects and runs its initAC()
   function. */
void acsolver::init (void) {
  circuit * root = subnet->getRoot ();
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    if (c->isNonLinear ()) c->calcOperatingPoints ();
    c->initAC ();
    if (noise) c->initNoiseAC ();
  }
}

/* This function saves the results of a single solve() functionality
   (for the given frequency) into the output dataset. */
void acsolver::saveAllResults (nr_double_t freq) {
  qucs::vector * f;
  // add current frequency to the dependency of the output dataset
  if ((f = data->findDependency ("acfrequency")) == NULL) {
    f = new qucs::vector ("acfrequency");
    data->addDependency (f);
  }
  if (runs == 1) f->add (freq);
  saveResults ("v", "i", 0, f);

  // additionally save noise results if requested
  if (noise) {
    saveNoiseResults (f);
  }
}

/* The function computes the final noise results and puts them into
   the output dataset. */
void acsolver::saveNoiseResults (qucs::vector * f) {
  int N = countNodes ();
  int M = countVoltageSources ();
  for (int r = 0; r < N + M; r++) {
    // renormalise the results
    x->set (r, fabs (xn->get (r) * sqrt (kB * T0)));
  }

  // apply probe data
  circuit * root = subnet->getRoot ();
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    if (!c->isProbe ()) continue;
    int np, nn;
    nr_double_t vp, vn;
    np = getNodeNr (c->getNode (NODE_1)->getName ());
    vp = np > 0 ? xn->get (np - 1) : 0.0;
    nn = getNodeNr (c->getNode (NODE_2)->getName ());
    vn = nn > 0 ? xn->get (nn - 1) : 0.0;
    c->setOperatingPoint ("Vr", fabs ((vp - vn) * sqrt (kB * T0)));
    c->setOperatingPoint ("Vi", 0.0);
  }

  saveResults ("vn", "in", 0, f);
}

/* This function runs the AC noise analysis.  It saves its results in
   the 'xn' vector. */
void acsolver::solve_noise (void) {
  int N = countNodes ();
  int M = countVoltageSources ();

  // save usual AC results
  tvector<nr_complex_t> xsave = *x;

  // create the Cy matrix
  createNoiseMatrix ();
  // create noise result vector if necessary
  if (xn == NULL) xn = new tvector<nr_double_t> (N + M);

  // temporary result vector for transimpedances
  tvector<nr_complex_t> zn = tvector<nr_complex_t> (N + M);

  // create the MNA matrix once again and LU decompose the adjoint matrix
  createMatrix ();
  A->transpose ();
  eqnAlgo = ALGO_LU_FACTORIZATION_CROUT;
  runMNA ();

  // ensure skipping LU decomposition
  updateMatrix = 0;
  convHelper = CONV_None;
  eqnAlgo = ALGO_LU_SUBSTITUTION_CROUT;

  // compute noise voltage for each node (and voltage source)
  for (int i = 0; i < N + M; i++) {
    z->set (0); z->set (i, -1); // modify right hand side appropriately
    runMNA ();                  // solve
    zn = *x;                    // save transimpedance vector

    // compute actual noise voltage
    xn->set (i, sqrt (real (scalar (zn * (*C), conj (zn)))));
  }

  // restore usual AC results
  *x = xsave;
}

// properties
PROP_REQ [] = {
  { "Type", PROP_STR, { PROP_NO_VAL, "lin" }, PROP_RNG_TYP },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Noise", PROP_STR, { PROP_NO_VAL, "no" }, PROP_RNG_YESNO },
  { "Start", PROP_REAL, { 1e9, PROP_NO_STR }, PROP_POS_RANGE },
  { "Stop", PROP_REAL, { 10e9, PROP_NO_STR }, PROP_POS_RANGE },
  { "Points", PROP_INT, { 10, PROP_NO_STR }, PROP_MIN_VAL (2) },
  { "Values", PROP_LIST, { 10, PROP_NO_STR }, PROP_POS_RANGE },
  PROP_NO_PROP };
struct define_t acsolver::anadef =
  { "AC", 0, PROP_ACTION, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };

} // namespace qucs
