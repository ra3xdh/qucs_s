/*
 * hbsolver.cpp - harmonic balance solver class implementation
 *
 * Copyright (C) 2005, 2006, 2007, 2008 Stefan Jahn <stefan@lkcc.org>
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

#include<algorithm>

#include <stdio.h>

#include "object.h"
#include "logging.h"
#include "complex.h"
#include "vector.h"
#include "node.h"
#include "circuit.h"
#include "component_id.h"
#include "net.h"
#include "netdefs.h"
#include "strlist.h"
#include "ptrlist.h"
#include "tvector.h"
#include "tmatrix.h"
#include "eqnsys.h"
#include "analysis.h"
#include "dataset.h"
#include "fourier.h"
#include "hbsolver.h"

#define HB_DEBUG 0

namespace qucs {

using namespace fourier;

// Constructor creates an unnamed instance of the hbsolver class.
hbsolver::hbsolver () : analysis () {
  type = ANALYSIS_HBALANCE;
  frequency = 0;
  nlnodes = lnnodes = banodes = nanodes = NULL;
  Y = Z = A = NULL;
  NA = YV = JQ = JG = JF = NULL;
  OM = IR = QR = RH = IG = FQ = VS = VP = FV = IL = IN = IC = IS = NULL;
  vs = x = NULL;
  runs = 0;
  ndfreqs = NULL;
}

// Constructor creates a named instance of the hbsolver class.
hbsolver::hbsolver (char * n) : analysis (n) {
  type = ANALYSIS_HBALANCE;
  frequency = 0;
  nlnodes = lnnodes = banodes = nanodes = NULL;
  Y = Z = A = NULL;
  NA = YV = JQ = JG = JF = NULL;
  OM = IR = QR = RH = IG = FQ = VS = VP = FV = IL = IN = IC = IS = NULL;
  vs = x = NULL;
  runs = 0;
  ndfreqs = NULL;
}

// Destructor deletes the hbsolver class object.
hbsolver::~hbsolver () {
  // delete nodelists
  delete nlnodes;
  delete lnnodes;
  delete banodes;
  delete nanodes;

  // delete temporary matrices
  delete A;
  delete Z;
  delete Y;

  // delete matrices
  delete NA;
  delete YV;
  delete JQ;
  delete JG;
  delete JF;

  // delete vectors
  delete IC;
  delete IS;
  delete FV;
  delete IL;
  delete IN;
  delete IG;
  delete FQ;
  delete VS;
  delete VP;
  delete vs;
  delete OM;
  delete IR;
  delete QR;
  delete RH;

  delete x;
  delete[] ndfreqs;
}

/* The copy constructor creates a new instance of the hbsolver class
   based on the given hbsolver object. */
hbsolver::hbsolver (hbsolver & o) : analysis (o) {
  frequency = o.frequency;
  negfreqs = o.negfreqs;
  posfreqs = o.posfreqs;
  nlnodes = o.nlnodes;
  lnnodes = o.lnnodes;
  banodes = o.banodes;
  nanodes = o.nanodes;
  Y = Z = A = NULL;
  NA = YV = JQ = JG = JF = NULL;
  OM = IR = QR = RH = IG = FQ = VS = VP = FV = IL = IN = IC = IS = NULL;
  vs = x = NULL;
  runs = o.runs;
  ndfreqs = NULL;
}

#define VS_(r) (*VS) (r)
#define OM_(r) (*OM) (r)

/* This is the HB netlist solver.  It prepares the circuit list and
   solves it then. */
int hbsolver::solve (void) {

  int iterations = 0, done = 0;
  int MaxIterations = getPropertyInteger ("MaxIter");

  // collect different parts of the circuit
  splitCircuits ();

  // create frequency array
  collectFrequencies ();

  // find interconnects between the linear and non-linear subcircuit
  getNodeLists ();

  // prepares the linear part --> 0 = IC + [YV] * VS
  prepareLinear ();

  runs++;
  logprint (LOG_STATUS, "NOTIFY: %s: solving for %d frequencies\n",
	    getName (), lnfreqs);

  if (nbanodes > 0) {

    // start balancing
    logprint (LOG_STATUS, "NOTIFY: %s: balancing at %d nodes\n", getName (),
	      nbanodes);

    // prepares the non-linear part
    prepareNonLinear ();

#if HB_DEBUG
      fprintf (stderr, "YV -- transY in f:\n"); YV->print ();
      fprintf (stderr, "IC -- constant current in f:\n"); IC->print ();
#endif

    // start iteration
    do {
      iterations++;

#if HB_DEBUG
      fprintf (stderr, "\n   -- iteration step: %d\n", iterations);
      fprintf (stderr, "vs -- voltage in t:\n"); vs->print ();
#endif

      // evaluate component functionality and fill matrices and vectors
      loadMatrices ();

#if HB_DEBUG
      fprintf (stderr, "FQ -- charge in t:\n"); FQ->print ();
      fprintf (stderr, "IG -- current in t:\n"); IG->print ();
#endif

      // currents into frequency domain
      VectorFFT (IG);

      // charges into frequency domain
      VectorFFT (FQ);

      // right hand side currents and charges into the frequency domain
      VectorFFT (IR);
      VectorFFT (QR);

#if HB_DEBUG
      fprintf (stderr, "VS -- voltage in f:\n"); VS->print ();
      fprintf (stderr, "FQ -- charge in f:\n"); FQ->print ();
      fprintf (stderr, "IG -- current in f:\n"); IG->print ();
      fprintf (stderr, "IR -- corrected Jacobi current in f:\n"); IR->print ();
#endif

      // solve HB equation --> FV = IC + [YV] * VS + j[O] * FQ + IG
      solveHB ();

#if HB_DEBUG
      fprintf (stderr, "FV -- error vector F(V) in f:\n"); FV->print ();
      fprintf (stderr, "IL -- linear currents in f:\n"); IL->print ();
      fprintf (stderr, "IN -- non-linear currents in f:\n"); IN->print ();
      fprintf (stderr, "RH -- right-hand side currents in f:\n"); RH->print ();
#endif

      // termination criteria met
      if (iterations > 1 && checkBalance ()) {
	done = 1;
	break;
      }

#if HB_DEBUG
      fprintf (stderr, "JG -- G-Jacobian in t:\n"); JG->print ();
      fprintf (stderr, "JQ -- C-Jacobian in t:\n"); JQ->print ();
#endif

      // G-Jacobian into frequency domain
      MatrixFFT (JG);

      // C-Jacobian into frequency domain
      MatrixFFT (JQ);

#if HB_DEBUG
      fprintf (stderr, "JQ -- dQ/dV C-Jacobian in f:\n"); JQ->print ();
      fprintf (stderr, "JG -- dI/dV G-Jacobian in f:\n"); JG->print ();
#endif

      // calculate Jacobian --> JF = [YV] + j[O] * JQ + JG
      calcJacobian ();

#if HB_DEBUG
      fprintf (stderr, "JF -- full Jacobian in f:\n"); JF->print ();
#endif

      // solve equation system --> JF * VS(n+1) = JF * VS(n) - FV
      solveVoltages ();

#if HB_DEBUG
      fprintf (stderr, "VS -- next voltage in f:\n"); VS->print ();
#endif

      // inverse FFT of frequency domain voltage vector VS(n+1)
      VectorIFFT (vs);
    }
    // check termination criteria (balanced frequency domain currents)
    while (!done && iterations < MaxIterations);

    if (iterations >= MaxIterations) {
      qucs::exception * e = new qucs::exception (EXCEPTION_NO_CONVERGENCE);
      e->setText ("no convergence in %s analysis after %d iterations",
		  getName (), iterations);
      throw_exception (e);
      logprint (LOG_ERROR, "%s: no convergence after %d iterations\n",
		getName (), iterations);
    }
    else {
      logprint (LOG_STATUS, "%s: convergence reached after %d iterations\n",
		getName (), iterations);
    }
  }
  else {
    // no balancing necessary
    logprint (LOG_STATUS, "NOTIFY: %s: no balancing necessary\n", getName ());
  }

  // print exception stack
  estack.print ();

  // apply AC analysis to the complete network in order to obtain the
  // final results
  finalSolution ();

  // save results into dataset
  saveResults ();
  return 0;
}

/* Goes through the list of circuit objects and runs its calcHB()
   function. */
void hbsolver::calc (hbsolver * self) {
  circuit * root = self->getNet()->getRoot ();
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    c->calcHB (self->frequency);
  }
}

/* Goes through the list of circuit objects and runs its initHB()
   function. */
void hbsolver::initHB (void) {
  circuit * root = subnet->getRoot ();
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    c->initHB ();
  }
}

/* Goes through the list of circuit objects and runs its initDC()
   function. */
void hbsolver::initDC (void) {
  circuit * root = subnet->getRoot ();
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    c->initDC ();
  }
}

// Returns true if circuit is a HB source.
bool hbsolver::isExcitation (circuit * c) {
  int type = c->getType ();
  if (type == CIR_PAC || type == CIR_VAC || type == CIR_VDC)
    return true;
  return false;
}

// Expands the frequency array using the given frequency and the order.
void hbsolver::expandFrequencies (nr_double_t f, int n) {
  auto nfreqs = negfreqs;
  auto pfreqs = posfreqs;
  int i, k, len = nfreqs.size ();
  negfreqs.clear ();
  posfreqs.clear ();
  if (len > 0) {
    // frequency expansion for full frequency sets
    for (i = 0; i <= n + 1; i++) {
      for (k = 0; k < len; k++) {
	negfreqs.push_back (i * f + nfreqs[k]);
      }
    }
    for (i = -n; i < 0; i++) {
      for (k = 0; k < len; k++) {
	negfreqs.push_back (i * f + nfreqs[k]);
      }
    }
    for (i = 0; i <= 2 * n + 1; i++) {
      for (k = 0; k < len; k++) {
	posfreqs.push_back (i * f + pfreqs[k]);
      }
    }
  }
  else {
    // first frequency
    for (i = 0; i <= n + 1; i++) negfreqs.push_back (i * f);
    for (i = -n; i < 0; i++) negfreqs.push_back (i * f);
    for (i = 0; i <= 2 * n + 1; i++) posfreqs.push_back (i * f);
  }
}

// Calculates an order fulfilling the "power of two" requirement.
int hbsolver::calcOrder (int n) {
  int o, order = n * 2;             // current order + DC + negative frequencies
  for (o = 1; o < order; o <<= 1) ; // a power of 2
  return o / 2 - 1;
}

/* The function computes the harmonic frequencies excited in the
   circuit list depending on the maximum number of harmonics per
   excitation and saves its results into the 'negfreqs' vector. */
void hbsolver::collectFrequencies (void) {

  // initialization
  negfreqs.clear ();
  posfreqs.clear ();
  rfreqs.clear ();
  dfreqs.clear ();
  delete[] ndfreqs;

  // obtain order
  int i, n = calcOrder (getPropertyInteger ("n"));

  // expand frequencies for each excitation
  nr_double_t f;
  for (auto * c : excitations) {
    if (c->getType () != CIR_VDC) { // no extra DC sources
      if ((f = c->getPropertyDouble ("f")) != 0.0) {
	const auto epsilon = std::numeric_limits<nr_double_t>::epsilon();
        auto found =
	  std::find_if(dfreqs.cbegin(),dfreqs.cend(),
		       [f,epsilon](decltype(*dfreqs.cbegin()) x) {
			 return (std::abs(x-f) < epsilon);
		       })
	  ;
	if (found == dfreqs.cend()) { // no double frequencies
	  dfreqs.push_back (f);
	  expandFrequencies (f, n);
	}
      }
    }
  }

  // no excitations
  if (negfreqs.size () == 0) {
    // use specified frequency
    f = getPropertyDouble ("f");
    dfreqs.push_back (f);
    expandFrequencies (f, n);
  }

  // build frequency dimension lengths
  ndfreqs = new int[dfreqs.size ()];
  for (i = 0; static_cast<unsigned int>(i) < dfreqs.size (); i++) {
    ndfreqs[i] = (n + 1) * 2;
  }

#if HB_DEBUG
  fprintf (stderr, "%d frequencies: [ ", negfreqs.getSize ());
  for (i = 0; i < negfreqs.getSize (); i++) {
    fprintf (stderr, "%g ", (double) real (negfreqs.get (i)));
  }
  fprintf (stderr, "]\n");
#endif /* HB_DEBUG */

  // build list of positive frequencies including DC
  for (n = 0; static_cast<unsigned int>(n) < negfreqs.size (); n++) {
    if ((f = negfreqs[n]) < 0.0) continue;
    rfreqs.push_back (f);
  }
  lnfreqs = rfreqs.size ();
  nlfreqs = negfreqs.size ();

  // pre-calculate the j[O] vector
  OM = new tvector<nr_complex_t> (nlfreqs);
  for (n = i = 0; n < nlfreqs; n++, i++)
    OM_(n) = nr_complex_t (0, 2 * pi * negfreqs[i]);
}

// Split netlist into excitation, linear and non-linear part.
void hbsolver::splitCircuits (void) {
  circuit * root = subnet->getRoot ();
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    if (c->isNonLinear ()) {
      // non-linear part
      nolcircuits.push_front(c);
    }
    else if (isExcitation (c)) {
      // get sinusoidal sources
      excitations.push_front(c);
    }
    else if (c->getType () != CIR_GROUND) {
      // linear part
      lincircuits.push_front (c);
    }
  }
}

// Creates a nodelist for the given circuit list.
strlist * hbsolver::circuitNodes (ptrlist<circuit> circuits) {
  strlist * nodes = new strlist ();
  for (auto * c : circuits) {
    for (int i = 0; i < c->getSize (); i++) {
      const char * n = c->getNode(i)->getName ();
      if (strcmp (n, "gnd")) {
	if (!nodes->contains (n)) nodes->add (n);
      }
    }
  }
  return nodes;
}

// Obtain node lists for linear and non-linear part.
void hbsolver::getNodeLists (void) {
  // non-linear nodes
  nlnodes = circuitNodes (nolcircuits);
  // linear nodes
  lnnodes = circuitNodes (lincircuits);
  // excitation nodes
  exnodes = circuitNodes (excitations);

#if DEBUG && 0
  fprintf (stderr, "nonlinear nodes: [ %s ]\n", nlnodes->toString ());
  fprintf (stderr, "   linear nodes: [ %s ]\n", lnnodes->toString ());
#endif

  // organization of the nodes for the MNA:
  // --------------------------------------
  // 1.)  balanced nodes: all connected to at least one non-linear device
  // 2.a) the excitation nodes
  // 2.b) all linear nodes not contained in 1. and 2.a
  // 2.c) additional gyrators of linear nodes (built-in voltage sources)
  // please note: excitation nodes also in 2.b; 1. and 2.a are 'ports'

  nanodes = new strlist (*nlnodes); // list 1.
  strlistiterator it;
  // add excitation nodes; list 2.a
  for (it = strlistiterator (exnodes); *it; ++it)
    nanodes->append (*it);
  // add linear nodes; list 2.b
  for (it = strlistiterator (lnnodes); *it; ++it) {
    if (!nanodes->contains (*it))
      nanodes->append (*it);
  }

  banodes = new strlist (*nlnodes);
#if DEBUG && 0
  fprintf (stderr, " balanced nodes: [ %s ]\n", banodes->toString ());
  fprintf (stderr, "  exnodes nodes: [ %s ]\n", exnodes->toString ());
  fprintf (stderr, "  nanodes nodes: [ %s ]\n", nanodes->toString ());
#endif
}

/* The function applies unique voltage source identifiers to each
   built in internal voltage source in the list of circuits. */
int hbsolver::assignVoltageSources (ptrlist<circuit> circuits) {
  int sources = 0;
  for (auto *c: circuits) {
    if (c->getVoltageSources () > 0) {
      c->setVoltageSource (sources);
      sources += c->getVoltageSources ();
    }
  }
  return sources;
}

/* The function assigns unique node identifiers to each circuit node. */
int hbsolver::assignNodes (ptrlist<circuit> circuits, strlist * nodes,
			   int offset) {
  // through all collected nodes
  for (int nr = 0; nr < nodes->length (); nr++) {
    char * nn = nodes->get (nr);
    // through all circuits
    for (auto *c : circuits) {
      // assign current identifier if any of the circuit nodes equals
      // the current one
      for (int i = 0; i < c->getSize (); i++) {
	node * n = c->getNode (i);
	if (!strcmp (n->getName (), nn)) n->setNode (offset + nr + 1);
      }
    }
  }
  return nodes->length ();
}

// Prepares the linear operations.
void hbsolver::prepareLinear (void) {
  for (auto *lc : lincircuits)
    lc->initHB ();
  nlnvsrcs = assignVoltageSources (lincircuits);
  nnlvsrcs = excitations.size ();
  nnanodes = nanodes->length ();
  nexnodes = exnodes->length ();
  nbanodes = banodes->length ();
  assignNodes (lincircuits, nanodes);
  assignNodes (excitations, nanodes);
  createMatrixLinearA ();
  createMatrixLinearY ();
  calcConstantCurrent ();
}

/* The function creates the complex linear network MNA matrix.  It
   contains the MNA entries for all linear components for each
   requested frequency. */
void hbsolver::createMatrixLinearA (void) {
  int M = nlnvsrcs;
  int N = nnanodes;
  int f = 0;
  nr_double_t freq;

  // create new MNA matrix
  A = new tmatrix<nr_complex_t> ((N + M) * lnfreqs);

  // through each frequency
  for (unsigned int i = 0; i < rfreqs.size (); i++) {
    freq = rfreqs[i];
    // calculate components' MNA matrix for the given frequency
    for (auto *lc : lincircuits)
      lc->calcHB (freq);
    // fill in all matrix entries for the given frequency
    fillMatrixLinearA (A, f++);
  }

  // save a copy of the original MNA matrix
  NA = new tmatrix<nr_complex_t> (*A);
}

// some definitions for the linear matrix filler
#undef  A_
#undef  B_
#define A_(r,c) (*A) ((r)*lnfreqs+f,(c)*lnfreqs+f)
#define G_(r,c) A_(r,c)
#define B_(r,c) A_(r,c+N)
#define C_(r,c) A_(r+N,c)
#define D_(r,c) A_(r+N,c+N)

/* This function fills in the MNA matrix entries into the A matrix for
   a given frequency index. */
void hbsolver::fillMatrixLinearA (tmatrix<nr_complex_t> * A, int f) {
  int N = nnanodes;

  // through each linear circuit
  for (auto *cir : lincircuits) {
    int s = cir->getSize ();
    int nr, nc, r, c, v;

    // apply G-matrix entries
    for (r = 0; r < s; r++) {
      if ((nr = cir->getNode(r)->getNode () - 1) < 0) continue;
      for (c = 0; c < s; c++) {
	if ((nc = cir->getNode(c)->getNode () - 1) < 0) continue;
	G_(nr, nc) += cir->getY (r, c);
      }
    }

    // augmented part -- built in voltage sources
    if ((v = cir->getVoltageSources ()) > 0) {

      // apply B-matrix entries
      for (r = 0; r < s; r++) {
	if ((nr = cir->getNode(r)->getNode () - 1) < 0) continue;
	for (c = 0; c < v; c++) {
	  nc = cir->getVoltageSource () + c;
	  B_(nr, nc) += cir->getB (r, nc);
	}
      }

      // apply C-matrix entries
      for (r = 0; r < v; r++) {
	nr = cir->getVoltageSource () + r;
	for (c = 0; c < s; c++) {
	  if ((nc = cir->getNode(c)->getNode () - 1) < 0) continue;
	  C_(nr, nc) += cir->getC (nr, c);
	}
      }

      // apply D-matrix entries
      for (r = 0; r < v; r++) {
	nr = cir->getVoltageSource () + r;
	for (c = 0; c < v; c++) {
	  nc = cir->getVoltageSource () + c;
	  D_(nr, nc) += cir->getD (nr, nc);
	}
      }
    }
  }
}

// The function inverts the given matrix A into the matrix H.
void hbsolver::invertMatrix (tmatrix<nr_complex_t> * A,
			     tmatrix<nr_complex_t> * H) {
  eqnsys<nr_complex_t> eqns;
  int N = A->getCols ();
  tvector<nr_complex_t> * x = new tvector<nr_complex_t> (N);
  tvector<nr_complex_t> * z = new tvector<nr_complex_t> (N);

  try_running () {
    // create LU decomposition of the A matrix
    eqns.setAlgo (ALGO_LU_FACTORIZATION_CROUT);
    eqns.passEquationSys (A, x, z);
    eqns.solve ();
  }
  // appropriate exception handling
  catch_exception () {
  case EXCEPTION_PIVOT:
  default:
    logprint (LOG_ERROR, "WARNING: %s: during TI inversion\n", getName ());
    estack.print ();
  }

  // use the LU decomposition to obtain the inverse H
  eqns.setAlgo (ALGO_LU_SUBSTITUTION_CROUT);
  for (int c = 0; c < N; c++) {
    z->set (0.0);
    z->set (c, 1.0);
    eqns.passEquationSys (A, x, z);
    eqns.solve ();
    for (int r = 0; r < N; r++) H->set (r, c, x->get (r));
  }
  delete x;
  delete z;
}

// Some defines for matrix element access.
#define V_(r) (*V) (r)
#define I_(r) (*I) (r)
#undef  A_
#define A_(r,c) (*A) (r,c)

#define Z_(r,c) (*Z) (r,c)
#define Y_(r,c) (*Y) (r,c)

#define ZVU_(r,c) Z_(r,c)
#define ZVL_(r,c) Z_((r)*lnfreqs+f+sn,c)
#define ZCU_(r,c) Z_(r,(c)*lnfreqs+f+sn)
#define ZCL_(r,c) Z_((r)*lnfreqs+f+sn,(c)*lnfreqs+f+sn)

#define YV_(r,c) (*YV) (r,c)
#define NA_(r,c) (*NA) (r,c)
#define JF_(r,c) (*JF) (r,c)

/* The following function performs the following steps:
   1. form the MNA matrix A including all nodes (linear, non-linear and
      excitations)
   2. compute the variable transimpedance matrix entries for the nodes
      to be balanced
   3. compute the constant transimpedance matrix entries for the constant
      current vector caused by the excitations
   4. invert this overall transimpedance matrix
   5. extract the variable transadmittance matrix entries
*/
void hbsolver::createMatrixLinearY (void) {
  int M = nlnvsrcs;
  int N = nnanodes;
  int c, r, f;

  // size of overall MNA matrix
  int sa = (N + M) * lnfreqs;
  int sv = nbanodes;
  int se = nnlvsrcs;
  int sy = sv + se;

  // allocate new transimpedance matrix
  Z = new tmatrix<nr_complex_t> (sy * lnfreqs);

  // prepare equation system
  eqnsys<nr_complex_t> eqns;
  tvector<nr_complex_t> * V;
  tvector<nr_complex_t> * I;

  // 1. create variable transimpedance matrix entries relating
  // voltages at the balanced nodes to the currents through these
  // nodes into the non-linear part
  int sn = sv * lnfreqs;
  V = new tvector<nr_complex_t> (sa);
  I = new tvector<nr_complex_t> (sa);

  // connect a 100 Ohm resistor (to ground) to balanced node in the MNA matrix
  for (c = 0; c < sv * lnfreqs; c++) A_(c, c) += 0.01;

  // connect a 100 Ohm resistor (in parallel) to each excitation
  for (auto *vs : excitations) {
    // get positive and negative node
    int pnode = vs->getNode(NODE_1)->getNode ();
    int nnode = vs->getNode(NODE_2)->getNode ();
    for (f = 0; f < lnfreqs; f++) { // for each frequency
      int pn = (pnode - 1) * lnfreqs + f;
      int nn = (nnode - 1) * lnfreqs + f;
      if (pnode) A_(pn, pn) += 0.01;
      if (nnode) A_(nn, nn) += 0.01;
      if (pnode && nnode) {
	A_(pn, nn) -= 0.01;
	A_(nn, pn) -= 0.01;
      }
    }
  }

  // LU decompose the MNA matrix
  try_running () {
    eqns.setAlgo (ALGO_LU_FACTORIZATION_CROUT);
    eqns.passEquationSys (A, V, I);
    eqns.solve ();
  }
  // appropriate exception handling
  catch_exception () {
  case EXCEPTION_PIVOT:
  default:
    logprint (LOG_ERROR, "WARNING: %s: during A factorization\n", getName ());
    estack.print ();
  }

  // acquire variable transimpedance matrix entries
  eqns.setAlgo (ALGO_LU_SUBSTITUTION_CROUT);
  for (c = 0; c < sn; c++) {
    I->set (0.0);
    I_(c) = 1.0;
    eqns.passEquationSys (A, V, I);
    eqns.solve ();
    // ZV | ..
    // ---+---
    // .. | ..
    for (r = 0; r < sn; r++) ZVU_(r, c) = V_(r);
    // .. | ..
    // ---+---
    // ZV | ..
    r = 0;
    for (auto ite = excitations.begin(); ite != excitations.end(); ++ite, r++) {
      // lower part entries
      for (f = 0; f < lnfreqs; f++) {
	ZVL_(r, c) = excitationZ (V, *ite, f);
      }
    }
  }

  // create constant transimpedance matrix entries relating the
  // source voltages to the interconnection currents
  int vsrc = 0;
  // acquire constant transadmittance matrix entries
  for (auto it = excitations.begin(); it != excitations.end(); ++it, vsrc++) {
    circuit * vs = *it;
    // get positive and negative node
    int pnode = vs->getNode(NODE_1)->getNode ();
    int nnode = vs->getNode(NODE_2)->getNode ();
    for (f = 0; f < lnfreqs; f++) { // for each frequency
      int pn = (pnode - 1) * lnfreqs + f;
      int nn = (nnode - 1) * lnfreqs + f;
      I->set (0.0);
      if (pnode) I_(pn) = +1.0;
      if (nnode) I_(nn) = -1.0;
      eqns.passEquationSys (A, V, I);
      eqns.solve ();
      // .. | ZC
      // ---+---
      // .. | ..
      for (r = 0; r < sn; r++) {
	// upper part of the entries
	ZCU_(r, vsrc) = V_(r);
      }
      // .. | ..
      // ---+---
      // .. | ZC
      r = 0;
      for (auto ite = excitations.begin(); ite != excitations.end(); ++ite, r++) {
	// lower part entries
	ZCL_(r, vsrc) = excitationZ (V, *ite, f);
      }
    }
  }
  delete I;
  delete V;

  // allocate new transadmittance matrix
  Y = new tmatrix<nr_complex_t> (sy * lnfreqs);

  // invert the Z matrix to a Y matrix
  invertMatrix (Z, Y);

  // substract the 100 Ohm resistor
  for (c = 0; c < sy * lnfreqs; c++) Y_(c, c) -= 0.01;

  // extract the variable transadmittance matrix
  YV = new tmatrix<nr_complex_t> (sv * nlfreqs);

  // variable transadmittance matrix must be continued conjugately
  *YV = expandMatrix (*Y, sv);

  // delete overall temporary MNA matrix
  delete A; A = NULL;
  // delete transimpedance matrix
  delete Z; Z = NULL;
}

/* Little helper function obtaining a transimpedance value for the
   given voltage source (excitation) and for a given frequency
   index. */
nr_complex_t hbsolver::excitationZ (tvector<nr_complex_t> * V, circuit * vs,
				    int f) {
  // get positive and negative node
  int pnode = vs->getNode(NODE_1)->getNode ();
  int nnode = vs->getNode(NODE_2)->getNode ();
  nr_complex_t z = 0.0;
  if (pnode) z += V_((pnode - 1) * lnfreqs + f);
  if (nnode) z -= V_((nnode - 1) * lnfreqs + f);
  return z;
}

/* This function computes the constant current vectors using the
   voltage of the excitations and the transadmittance matrix
   entries. */
void hbsolver::calcConstantCurrent (void) {
  int se = nnlvsrcs * lnfreqs;
  int sn = nbanodes * lnfreqs;
  int r, c, vsrc = 0;

  // collect excitation voltages
  tvector<nr_complex_t> VC (se);
  for (auto it = excitations.begin(); it != excitations.end(); ++it, vsrc++) {
    circuit * vs = *it;
    vs->initHB ();
    vs->setVoltageSource (0);
    for (unsigned int f = 0; f < rfreqs.size (); f++) { // for each frequency
      nr_double_t freq = rfreqs[f];
      vs->calcHB (freq);
      VC (vsrc * lnfreqs + f) = vs->getE (VSRC_1);
    }
  }

  // compute constant current vector for balanced nodes
  IC = new tvector<nr_complex_t> (sn);
  // .. | YC * VC
  // ---+---
  // .. | ..
  for (r = 0; r < sn; r++) {
    nr_complex_t i = 0.0;
    for (c = 0; c < se; c++) {
      i += Y_(r, c + sn) * VC (c);
    }
    int f = r % lnfreqs;
    if (f != 0 && f != lnfreqs - 1) i /= 2;
    IC->set (r, i);
  }
  // expand the constant current conjugate
  *IC = expandVector (*IC, nbanodes);

  // compute constant current vector for sources itself
  IS = new tvector<nr_complex_t> (se);
  // .. | ..
  // ---+---
  // .. | YC * VC
  for (r = 0; r < se; r++) {
    nr_complex_t i = 0.0;
    for (c = 0; c < se; c++) {
      i += Y_(r + sn, c + sn) * VC (c);
    }
    IS->set (r, i);
  }

  // delete overall transadmittance matrix
  delete Y; Y = NULL;
}

/* Checks whether currents through the interconnects of the linear and
   non-linear subcircuit (in the frequency domain) are equal. */
int hbsolver::checkBalance (void) {
  nr_double_t iabstol = getPropertyDouble ("iabstol");
  nr_double_t vabstol = getPropertyDouble ("vabstol");
  nr_double_t reltol = getPropertyDouble ("reltol");
  int n, len = FV->size ();
  for (n = 0; n < len; n++) {
    // check iteration voltages
    nr_double_t v_abs = abs (VS->get (n) - VP->get (n));
    nr_double_t v_rel = abs (VS->get (n));
    if (v_abs >= vabstol + reltol * v_rel) return 0;
    // check balanced currents
    nr_complex_t il = IL->get (n);
    nr_complex_t in = IN->get (n);
    if (il != in) {
      nr_double_t i_abs = abs (il + in);
      nr_double_t i_rel = abs ((il + in) / (il - in));
      if (i_abs >= iabstol && 2.0 * i_rel >= reltol) return 0;
    }
  }
  return 1;
}

// some definitions for the non-linear matrix filler
#undef  G_
#undef  C_
#define G_(r,c) (*jg) ((r)*nlfreqs+f,(c)*nlfreqs+f)
#define C_(r,c) (*jq) ((r)*nlfreqs+f,(c)*nlfreqs+f)
#undef  FI_
#undef  FQ_
#define FI_(r) (*ig) ((r)*nlfreqs+f)
#define FQ_(r) (*fq) ((r)*nlfreqs+f)
#define IR_(r) (*ir) ((r)*nlfreqs+f)
#define QR_(r) (*qr) ((r)*nlfreqs+f)

/* This function fills in the matrix and vector entries for the
   non-linear HB equations for a given frequency index. */
void hbsolver::fillMatrixNonLinear (tmatrix<nr_complex_t> * jg,
				    tmatrix<nr_complex_t> * jq,
				    tvector<nr_complex_t> * ig,
				    tvector<nr_complex_t> * fq,
				    tvector<nr_complex_t> * ir,
				    tvector<nr_complex_t> * qr,
				    int f) {
  // through each linear circuit
  for (auto *cir: nolcircuits) {
    int s = cir->getSize ();
    int nr, nc, r, c;

    for (r = 0; r < s; r++) {
      if ((nr = cir->getNode(r)->getNode () - 1) < 0) continue;
      // apply G- and C-matrix entries
      for (c = 0; c < s; c++) {
	if ((nc = cir->getNode(c)->getNode () - 1) < 0) continue;
	G_(nr, nc) += cir->getY (r, c);
	C_(nr, nc) += cir->getQV (r, c);
      }
      // apply I- and Q-vector entries
      FI_(nr) -= cir->getI (r);
      FQ_(nr) -= cir->getQ (r);
      // ThinkME: positive or negative?
      IR_(nr) += cir->getGV (r) + cir->getI (r);
      QR_(nr) += cir->getCV (r) + cir->getQ (r);
    }
  }
}

/* The function initializes the non-linear part of the HB. */
void hbsolver::prepareNonLinear (void) {
  int N = nbanodes;

  // allocate matrices and vectors
  if (FQ == NULL) {
    FQ = new tvector<nr_complex_t> (N * nlfreqs);
  }
  if (IG == NULL) {
    IG = new tvector<nr_complex_t> (N * nlfreqs);
  }
  if (IR == NULL) {
    IR = new tvector<nr_complex_t> (N * nlfreqs);
  }
  if (QR == NULL) {
    QR = new tvector<nr_complex_t> (N * nlfreqs);
  }
  if (JG == NULL) {
    JG = new tmatrix<nr_complex_t> (N * nlfreqs);
  }
  if (JQ == NULL) {
    JQ = new tmatrix<nr_complex_t> (N * nlfreqs);
  }
  if (JF == NULL) {
    JF = new tmatrix<nr_complex_t> (N * nlfreqs);
  }

  // voltage vector in frequency and time domain
  if (VS == NULL) {
    VS = new tvector<nr_complex_t> (N * nlfreqs);
  }
  if (vs == NULL) {
    vs = new tvector<nr_complex_t> (N * nlfreqs);
  }
  if (VP == NULL) {
    VP = new tvector<nr_complex_t> (N * nlfreqs);
  }

  // error vector
  if (FV == NULL) {
    FV = new tvector<nr_complex_t> (N * nlfreqs);
  }
  // right hand side vector
  if (RH == NULL) {
    RH = new tvector<nr_complex_t> (N * nlfreqs);
  }

  // linear and non-linear current vector
  if (IL == NULL) {
    IL = new tvector<nr_complex_t> (N * nlfreqs);
  }
  if (IN == NULL) {
    IN = new tvector<nr_complex_t> (N * nlfreqs);
  }

  // assign nodes
  assignNodes (nolcircuits, nanodes);

  // initialize circuits
  for (auto *cir : nolcircuits) {
    cir->initHB (nlfreqs);
  }
}

/* Saves the node voltages of the given circuit and for the given
   frequency entry into the circuit voltage vector. */
void hbsolver::saveNodeVoltages (circuit * cir, int f) {
  int r, nr, s = cir->getSize ();
  for (r = 0; r < s; r++) {
    if ((nr = cir->getNode(r)->getNode () - 1) < 0) continue;
    // apply V-vector entries
    cir->setV (r, real (vs->get (nr * nlfreqs + f)));
  }
}

/* The function saves voltages into non-linear circuits, runs each
   non-linear components' HB calculator for each frequency and applies
   the matrix and vector entries appropriately. */
void hbsolver::loadMatrices (void) {
  // clear matrices and vectors before
  IG->set (0.0);
  FQ->set (0.0);
  IR->set (0.0);
  QR->set (0.0);
  JG->set (0.0);
  JQ->set (0.0);
  // through each frequency
  for (int f = 0; f < nlfreqs; f++) {
    // calculate components' HB matrices and vector for the given frequency
    for (auto *cir : nolcircuits) {
      saveNodeVoltages (cir, f); // node voltages
      cir->calcHB (f);         // HB calculator
    }
    // fill in all matrix entries for the given frequency
    fillMatrixNonLinear (JG, JQ, IG, FQ, IR, QR, f);
  }
}

/* The following function transforms a vector using a Fast Fourier
   Transformation from the time domain to the frequency domain. 
   \todo rewrite ugly should die
*/
void hbsolver::VectorFFT (tvector<nr_complex_t> * V, int isign) {
  int i, k, r;
  int n = nlfreqs;
  int nd = dfreqs.size ();
  int nodes = V->size () / n;
  nr_double_t * d = (double *)V->getData ();

  if (nd == 1) {
    // for each node a single 1d-FFT
    for (k = i = 0; i < nodes; i++, k += 2 * n) {
      nr_double_t * dst = &d[k];
      _fft_1d (dst, n, isign);
      if (isign > 0) for (r = 0; r < 2 * n; r++) *dst++ /= n;
    }
  }
  else {
    // for each node a single nd-FFT
    for (k = i = 0; i < nodes; i++, k += 2 * n) {
      nr_double_t * dst = &d[k];
      _fft_nd (dst, ndfreqs, nd, isign);
      if (isign > 0) for (r = 0; r < 2 * n; r++) *dst++ /= ndfreqs[0];
    }
  }
}

/* The following function transforms a vector using an Inverse Fast
   Fourier Transformation from the frequency domain to the domain
   time. */
void hbsolver::VectorIFFT (tvector<nr_complex_t> * V, int isign) {
  VectorFFT (V, -isign);
}

/* The following function transforms a matrix using a Fast Fourier
   Transformation from the time domain to the frequency domain. */
void hbsolver::MatrixFFT (tmatrix<nr_complex_t> * M) {

#if THE_SLO_ALGO
  // each column FFT
  for (int c = 0; c < M->getCols (); c++) {
    tvector<nr_complex_t> V = M->getCol (c);
    VectorFFT (&V);
    M->setCol (c, V);
  }

  // each row IFFT
  for (int r = 0; r < M->getRows (); r++) {
    tvector<nr_complex_t> V = M->getRow (r);
    VectorIFFT (&V);
    M->setRow (r, V);
  }
#else
  int c, r, nc, nr;
  // for each non-linear node block
  for (nc = c = 0; c < nbanodes; c++, nc += nlfreqs) {
    for (nr = r = 0; r < nbanodes; r++, nr += nlfreqs) {
      tvector<nr_complex_t> V (nlfreqs);
      int fr, fc, fi;
      // transform the sub-diagonal only
      for (fc = 0; fc < nlfreqs; fc++) V (fc) = M->get (nr + fc, nc + fc);
      VectorFFT (&V);
      // fill in resulting sub-matrix for the node
      for (fc = 0; fc < nlfreqs; fc++) {
	for (fi = nlfreqs - 1 - fc, fr = 0; fr < nlfreqs; fr++) {
	  if (++fi >= nlfreqs) fi = 0;
	  M->set (nr + fr, nc + fc, V (fi));
	}
      }
    }
  }
#endif
}

/* This function solves the actual HB equation in the frequency domain.
   F(V) = IC + [YV] * VS + j[O] * FQ + IG -> 0
   Care must be taken with indexing here: In the frequency domain only
   real positive frequencies are used and computed, but in the time
   domain we have more values because of the periodic continuation in
   the frequency domain.
   RHS = j[O] * CV + GV - (IC + j[O] * FQ + IG)
   Also the right hand side of the equation system for the new voltage
   vector is computed here. */
void hbsolver::solveHB (void) {
  // for each non-linear node
  for (int r = 0; r < nbanodes * nlfreqs; ) {
    // for each frequency
    for (int f = 0; f < nlfreqs; f++, r++) {
      nr_complex_t il = 0.0, in = 0.0, ir = 0.0;
      // constant current vector due to sources
      il += IC->get (r);
      // part 1 of right hand side vector
      ir -= il;
      // transadmittance matrix multiplied by voltage vector
      for (int c = 0; c < nbanodes * nlfreqs; c++) {
	il += YV_(r, c) * VS_(c);
      }
      // charge vector
      in += OM_(f) * FQ->get (r);
      // current vector
      in += IG->get (r);
      // part 2, 3 and 4 of right hand side vector
      ir += IR->get (r);
      ir += OM_(f) * QR->get (r);
      // put values into result vectors
      RH->set (r, ir);
      FV->set (r, il + in);
      IL->set (r, il);
      IN->set (r, in);
    }
  }
}

/* The function calculates the full Jacobian JF = [YV] + j[O] * JQ + JG */
void hbsolver::calcJacobian (void) {
  int c, r, fc, fr, rt, ct;
  /* add admittances of capacitance matrix JQ and non-linear
     admittances matrix JG into complete Jacobian JF */
  for (c = 0; c < nbanodes; c++) {
    ct = c * nlfreqs;
    for (fc = 0; fc < nlfreqs; fc++, ct++) {
      for (r = 0; r < nbanodes; r++) {
	rt = r * nlfreqs;
	for (fr = 0; fr < nlfreqs; fr++, rt++) {
	  JF_(rt, ct) = JG->get (rt, ct) + JQ->get (rt, ct) * OM_(fr);
	}
      }
    }
  }
  *JF += *YV; // add linear admittance matrix
}

/* The function expands the given vector in the frequency domain to
   make it a real valued signal in the time domain. */
tvector<nr_complex_t> hbsolver::expandVector (tvector<nr_complex_t> V,
					      int nodes) {
  tvector<nr_complex_t> res (nodes * nlfreqs);
  int r, ff, rf, rt;
  for (r = 0; r < nodes; r++) {
    rt = r * nlfreqs;
    rf = r * lnfreqs;
    // copy first part of vector
    for (ff = 0; ff < lnfreqs; ff++, rf++, rt++) {
      res (rt) = V (rf);
    }
    // continue vector conjugated
    for (rf -= 2; ff < nlfreqs; ff++, rf--, rt++) {
      res (rt) = conj (V (rf));
    }
  }
  return res;
}

/* The function expands the given matrix in the frequency domain to
   make it a real valued signal in the time domain. */
tmatrix<nr_complex_t> hbsolver::expandMatrix (tmatrix<nr_complex_t> M,
					      int nodes) {
  tmatrix<nr_complex_t> res (nodes * nlfreqs);
  int r, c, rf, rt, cf, ct, ff;
  for (r = 0; r < nodes; r++) {
    for (c = 0; c < nodes; c++) {
      rf = r * lnfreqs;
      rt = r * nlfreqs;
      cf = c * lnfreqs;
      ct = c * nlfreqs;
      // copy first part of diagonal
      for (ff = 0; ff < lnfreqs; ff++, cf++, ct++, rf++, rt++) {
	res (rt, ct) = M (rf, cf);
      }
      // continue diagonal conjugated
      for (cf -= 2, rf -= 2; ff < nlfreqs; ff++, cf--, ct++, rf--, rt++) {
	res (rt, ct) = conj (M (rf, cf));
      }
    }
  }
  return res;
}

/* This function solves the equation system
   JF * VS(n+1) = JF * VS(n) - FV
   in order to obtains a new voltage vector in the frequency domain. */
void hbsolver::solveVoltages (void) {
  // save previous iteration voltage
  *VP = *VS;

  // setup equation system
  eqnsys<nr_complex_t> eqns;
  try_running () {
    // use LU decomposition for solving
    eqns.setAlgo (ALGO_LU_DECOMPOSITION);
    eqns.passEquationSys (JF, VS, RH);
    eqns.solve ();
  }
  // appropriate exception handling
  catch_exception () {
  case EXCEPTION_PIVOT:
  default:
    logprint (LOG_ERROR, "WARNING: %s: during NR iteration\n", getName ());
    estack.print ();
  }

  // save new voltages in time domain vector
  *vs = *VS;
}

/* The following function extends the existing linear MNA matrix to
   contain the additional rows and columns for the excitation voltage
   sources. */
tmatrix<nr_complex_t> hbsolver::extendMatrixLinear (tmatrix<nr_complex_t> M,
						    int nodes) {
  int no = M.getCols ();
  tmatrix<nr_complex_t> res (no + nodes * lnfreqs);
  // copy the existing part
  for (int r = 0; r < no; r++) {
    for (int c = 0; c < no; c++) {
      res (r, c) = M (r, c);
    }
  }
  return res;
}

/* The function fills in the missing MNA entries for the excitation
   voltage sources into the extended rows and columns as well as the
   actual voltage values into the right hand side vector. */
void hbsolver::fillMatrixLinearExtended (tmatrix<nr_complex_t> * Y,
					 tvector<nr_complex_t> * I) {
  // through each excitation source
  int of = lnfreqs * (nlnvsrcs + nnanodes);
  int sc = of;

  for (auto *vs : excitations) {
    // get positive and negative node
    int pnode = vs->getNode(NODE_1)->getNode ();
    int nnode = vs->getNode(NODE_2)->getNode ();
    for (int f = 0; f < lnfreqs; f++, sc++) { // for each frequency
      // fill right hand side vector
      nr_double_t freq = rfreqs[f];
      vs->calcHB (freq);
      I_(sc) = vs->getE (VSRC_1);
      // fill MNA entries
      int pn = (pnode - 1) * lnfreqs + f;
      int nn = (nnode - 1) * lnfreqs + f;
      if (pnode) {
	Y_(pn, sc) = +1.0;
	Y_(sc, pn) = +1.0;
      }
      if (nnode) {
	Y_(nn, sc) = -1.0;
	Y_(sc, nn) = -1.0;
      }
    }
  }
}

/* The function calculates and saves the final solution. */
void hbsolver::finalSolution (void) {

  // extend the linear MNA matrix
  *NA = extendMatrixLinear (*NA, nnlvsrcs);

  int S = NA->getCols ();
  int N = nnanodes * lnfreqs;

  // right hand side vector
  tvector<nr_complex_t> * I = new tvector<nr_complex_t> (S);
  // temporary solution
  tvector<nr_complex_t> * V = new tvector<nr_complex_t> (S);
  // final solution
  x = new tvector<nr_complex_t> (N);

  // fill in missing MNA entries
  fillMatrixLinearExtended (NA, I);

  // put currents through balanced nodes into right hand side
  for (int n = 0; n < nbanodes; n++) {
    for (int f = 0; f < lnfreqs; f++) {
      nr_complex_t i = IL->get (n * nlfreqs + f);
      if (f != 0 && f != lnfreqs - 1) i *= 2;
      I_(n * lnfreqs + f) = i;
    }
  }

  // use QR decomposition for the final solution
  try_running () {
    eqnsys<nr_complex_t> eqns;
    eqns.setAlgo (ALGO_LU_DECOMPOSITION);
    eqns.passEquationSys (NA, V, I);
    eqns.solve ();
  }
  // appropriate exception handling
  catch_exception () {
  case EXCEPTION_PIVOT:
  default:
    logprint (LOG_ERROR, "WARNING: %s: during final AC analysis\n",
	      getName ());
    estack.print ();
  }
  for (int i = 0; i < N; i++) x->set (i, V_(i));
}

// Saves simulation results.
void hbsolver::saveResults (void) {
  vector * f;
  // add current frequency to the dependency of the output dataset
  if ((f = data->findDependency ("hbfrequency")) == NULL) {
    f = new vector ("hbfrequency");
    data->addDependency (f);
  }
  // save frequency vector
  if (runs == 1) {
    for (int i = 0; i < lnfreqs; i++) f->add (rfreqs[i]);
  }
  // save node voltage vectors
  int nanode = 0;
  for (strlistiterator it (nanodes); *it; ++it, nanode++) {
    int l = strlen (*it);
    char * n = (char *) malloc (l + 4);
    sprintf (n, "%s.Vb", *it);
    for (int i = 0; i < lnfreqs; i++) {
      saveVariable (n, x->get (i + nanode * lnfreqs), f);
    }
  }
}

// properties
PROP_REQ [] = {
  { "n", PROP_INT, { 1, PROP_NO_STR }, PROP_MIN_VAL (1) },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "f", PROP_REAL, { 1e9, PROP_NO_STR }, PROP_POS_RANGEX },
  { "iabstol", PROP_REAL, { 1e-12, PROP_NO_STR }, PROP_RNG_X01I },
  { "vabstol", PROP_REAL, { 1e-6, PROP_NO_STR }, PROP_RNG_X01I },
  { "reltol", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_RNG_X01I },
  { "MaxIter", PROP_INT, { 150, PROP_NO_STR }, PROP_RNGII (2, 10000) },
  PROP_NO_PROP };
struct define_t hbsolver::anadef =
  { "HB", 0, PROP_ACTION, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };

} // namespace qucs

/* TODO list for HB Solver:
   - Take care about nodes with non-linear components only.
   - AC Power Sources (extra Z and open loop voltage).
   - Current sources.
   - Balancing of multi-dimensional non-linear networks.
   - Sources directly connected to non-linear components and no other
     linear component (insert short).
   - Bug: With capacitors at hand there is voltage convergence but no
     current balancing.
   - Output currents through voltage sources.
 */
