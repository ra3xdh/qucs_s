/*
 * nasolver.cpp - nodal analysis solver class implementation
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
// BUG
#include "qucs_typedefs.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <float.h>
#include <assert.h>
#include <limits>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "vector.h"
#include "dataset.h"
#include "net.h"
#include "analysis.h"
#include "nodelist.h"
#include "nodeset.h"
#include "strlist.h"
#include "tvector.h"
#include "tmatrix.h"
#include "eqnsys.h"
#include "precision.h"
#include "operatingpoint.h"
#include "exception.h"
#include "exceptionstack.h"
#include "nasolver.h"
#include "constants.h"

namespace qucs {

// Constructor creates an unnamed instance of the nasolver class.
template <class nr_type_t>
nasolver<nr_type_t>::nasolver () : analysis ()
{
    nlist = NULL;
    A = C = NULL;
    z = x = xprev = zprev = NULL;
    reltol = abstol = vntol = 0;
    calculate_func = NULL;
    convHelper = fixpoint = 0;
    eqnAlgo = ALGO_LU_DECOMPOSITION;
    updateMatrix = 1;
    gMin = srcFactor = 0;
    eqns = new eqnsys<nr_type_t> ();
}

// Constructor creates a named instance of the nasolver class.
template <class nr_type_t>
nasolver<nr_type_t>::nasolver (const std::string &n) : analysis (n)
{
    nlist = NULL;
    A = C = NULL;
    z = x = xprev = zprev = NULL;
    reltol = abstol = vntol = 0;
    calculate_func = NULL;
    convHelper = fixpoint = 0;
    eqnAlgo = ALGO_LU_DECOMPOSITION;
    updateMatrix = 1;
    gMin = srcFactor = 0;
    eqns = new eqnsys<nr_type_t> ();
}

// Destructor deletes the nasolver class object.
template <class nr_type_t>
nasolver<nr_type_t>::~nasolver ()
{
    delete nlist;
    delete C;
    delete A;
    delete z;
    delete x;
    delete xprev;
    delete zprev;
    delete eqns;
}

/* The copy constructor creates a new instance of the nasolver class
   based on the given nasolver object. */
template <class nr_type_t>
nasolver<nr_type_t>::nasolver (nasolver & o) : analysis (o)
{
    nlist = o.nlist ? new nodelist (*(o.nlist)) : NULL;
    A = o.A ? new tmatrix<nr_type_t> (*(o.A)) : NULL;
    C = o.C ? new tmatrix<nr_type_t> (*(o.C)) : NULL;
    z = o.z ? new tvector<nr_type_t> (*(o.z)) : NULL;
    x = o.x ? new tvector<nr_type_t> (*(o.x)) : NULL;
    xprev = zprev = NULL;
    reltol = o.reltol;
    abstol = o.abstol;
    vntol = o.vntol;
    desc = o.desc;
    calculate_func = o.calculate_func;
    convHelper = o.convHelper;
    eqnAlgo = o.eqnAlgo;
    updateMatrix = o.updateMatrix;
    fixpoint = o.fixpoint;
    gMin = o.gMin;
    srcFactor = o.srcFactor;
    eqns = new eqnsys<nr_type_t> (*(o.eqns));
    solution = nasolution<nr_type_t> (o.solution);
}

/* The function runs the nodal analysis solver once, reports errors if
   any and save the results into each circuit. */
template <class nr_type_t>
int nasolver<nr_type_t>::solve_once (void)
{
    qucs::exception * e;
    int error = 0, d;

    // run the calculation function for each circuit
    calculate ();

    // generate A matrix and z vector
    createMatrix ();

    // solve equation system
    try_running ()
    {
        runMNA ();
    }
    // appropriate exception handling
    catch_exception ()
    {
    case EXCEPTION_PIVOT:
    case EXCEPTION_WRONG_VOLTAGE:
        e = new qucs::exception (EXCEPTION_NA_FAILED);
        d = top_exception()->getData ();
        pop_exception ();
        if (d >= countNodes ())
        {
            d -= countNodes ();
            e->setText ("voltage source `%s' conflicts with some other voltage "
                        "source", findVoltageSource(d)->getName ());
        }
        else
        {
            e->setText ("circuit admittance matrix in %s solver is singular at "
                        "node `%s' connected to [%s]", desc.c_str(), nlist->get (d).c_str(),
                        nlist->getNodeString (d).c_str());
        }
        throw_exception (e);
        error++;
        break;
    case EXCEPTION_SINGULAR:
        do
        {
            d = top_exception()->getData ();
            pop_exception ();
            if (d < countNodes ())
            {
                logprint (LOG_ERROR, "WARNING: %s: inserted virtual resistance at "
                          "node `%s' connected to [%s]\n", getName (), nlist->get (d).c_str(),
                          nlist->getNodeString (d).c_str());
            }
        }
        while (top_exception() != NULL &&
                top_exception()->getCode () == EXCEPTION_SINGULAR);
        break;
    default:
        estack.print ();
        break;
    }

    // save results into circuits
    if (!error) saveSolution ();
    return error;
}

/* Run this function after the actual solver run and before evaluating
   the results. */
template <class nr_type_t>
void nasolver<nr_type_t>::solve_post (void)
{
    delete nlist;
    nlist = NULL;
}

/* Run this function before the actual solver. */
template <class nr_type_t>
void nasolver<nr_type_t>::solve_pre (void)
{
    // create node list, enumerate nodes and voltage sources
#if DEBUG
    logprint (LOG_STATUS, "NOTIFY: %s: creating node list for %s analysis\n",
              getName (), desc.c_str());
#endif
    nlist = new nodelist (subnet);
    nlist->assignNodes ();
    assignVoltageSources ();
#if DEBUG && 0
    nlist->print ();
#endif

    // create matrix, solution vector and right hand side vector
    int M = countVoltageSources ();
    int N = countNodes ();
    delete A;
    A = new tmatrix<nr_type_t> (M + N);
    delete z;
    z = new tvector<nr_type_t> (N + M);
    delete x;
    x = new tvector<nr_type_t> (N + M);

#if DEBUG
    logprint (LOG_STATUS, "NOTIFY: %s: solving %s netlist\n", getName (), desc.c_str());
#endif
}

/* This function goes through the nodeset list of the current netlist
   and applies the stored values to the current solution vector.  Then
   the function saves the solution vector back into the actual
   component nodes. */
template <class nr_type_t>
void nasolver<nr_type_t>::applyNodeset (bool nokeep)
{
    if (x == NULL || nlist == NULL) return;

    // set each solution to zero
    if (nokeep) for (unsigned int i = 0; i < x->size (); i++) x->set (i, 0);

    // then apply the nodeset itself
    for (nodeset * n = subnet->getNodeset (); n; n = n->getNext ())
    {
        struct nodelist_t * nl = nlist->getNode (n->getName ());
        if (nl != NULL)
        {
            x->set (nl->n, n->getValue ());
        }
        else
        {
            logprint (LOG_ERROR, "WARNING: %s: no such node `%s' found, cannot "
                      "initialize node\n", getName (), n->getName ());
        }
    }
    if (xprev != NULL) *xprev = *x;
    saveSolution ();
    // propagate the solution to the non-linear circuits
    restartNR ();
}

/* The following function uses the gMin-stepping algorithm in order to
   solve the given non-linear netlist by continuous iterations. */
template <class nr_type_t>
int nasolver<nr_type_t>::solve_nonlinear_continuation_gMin (void)
{
    qucs::exception * e;
    int convergence, run = 0, MaxIterations, error = 0;
    nr_double_t gStep, gPrev;

    // fetch simulation properties
    MaxIterations = getPropertyInteger ("MaxIter") / 4 + 1;
    updateMatrix = 1;
    fixpoint = 0;

    // initialize the stepper
    gPrev = gMin = 0.01;
    gStep = gMin / 100;
    gMin -= gStep;

    do
    {
        // run solving loop until convergence is reached
        run = 0;
        do
        {
            error = solve_once ();
            if (!error)
            {
                // convergence check
                convergence = (run > 0) ? checkConvergence () : 0;
                savePreviousIteration ();
                run++;
            }
            else break;
        }
        while (!convergence && run < MaxIterations);
        iterations += run;

        // not yet converged, so decreased the gMin-step
        if (run >= MaxIterations || error)
        {
            gStep /= 2;
            // here the absolute minimum step checker
            if (gStep < std::numeric_limits<nr_double_t>::epsilon())
            {
                error = 1;
                e = new qucs::exception (EXCEPTION_NO_CONVERGENCE);
                e->setText ("no convergence in %s analysis after %d gMinStepping "
                            "iterations", desc.c_str(), iterations);
                throw_exception (e);
                break;
            }
            gMin = MAX (gPrev - gStep, 0);
        }
        // converged, increased the gMin-step
        else
        {
            gPrev = gMin;
            gMin = MAX (gMin - gStep, 0);
            gStep *= 2;
        }
    }
    // continue until no additional resistances is necessary
    while (gPrev > 0);

    return error;
}

/* The following function uses the source-stepping algorithm in order
   to solve the given non-linear netlist by continuous iterations. */
template <class nr_type_t>
int nasolver<nr_type_t>::solve_nonlinear_continuation_Source (void)
{
    qucs::exception * e;
    int convergence, run = 0, MaxIterations, error = 0;
    nr_double_t sStep, sPrev;

    // fetch simulation properties
    MaxIterations = getPropertyInteger ("MaxIter") / 4 + 1;
    updateMatrix = 1;
    fixpoint = 0;

    // initialize the stepper
    sPrev = srcFactor = 0;
    sStep = 0.01;
    srcFactor += sStep;

    do
    {
        // run solving loop until convergence is reached
        run = 0;
        do
        {
            subnet->setSrcFactor (srcFactor);
            error = solve_once ();
            if (!error)
            {
                // convergence check
                convergence = (run > 0) ? checkConvergence () : 0;
                savePreviousIteration ();
                run++;
            }
            else break;
        }
        while (!convergence && run < MaxIterations);
        iterations += run;

        // not yet converged, so decreased the source-step
        if (run >= MaxIterations || error)
        {
            if (error)
                sStep *= 0.1;
            else
                sStep *= 0.5;
            restorePreviousIteration ();
            saveSolution ();
            // here the absolute minimum step checker
            if (sStep < std::numeric_limits<nr_double_t>::epsilon())
            {
                error = 1;
                e = new qucs::exception (EXCEPTION_NO_CONVERGENCE);
                e->setText ("no convergence in %s analysis after %d sourceStepping "
                            "iterations", desc.c_str(), iterations);
                throw_exception (e);
                break;
            }
            srcFactor = std::min (sPrev + sStep, 1.0);
        }
        // converged, increased the source-step
        else if (run < MaxIterations / 4)
        {
            sPrev = srcFactor;
            srcFactor = std::min (srcFactor + sStep, 1.0);
            sStep *= 1.5;
        }
        else
        {
            srcFactor = std::min (srcFactor + sStep, 1.0);
        }
    }
    // continue until no source factor is necessary
    while (sPrev < 1);

    subnet->setSrcFactor (1);
    return error;
}

/* The function returns an appropriate text representation for the
   currently used convergence helper algorithm. */
template <class nr_type_t>
const char * nasolver<nr_type_t>::getHelperDescription (void)
{
    if (convHelper == CONV_Attenuation)
    {
        return "RHS attenuation";
    }
    else if  (convHelper == CONV_LineSearch)
    {
        return "line search";
    }
    else if  (convHelper == CONV_SteepestDescent)
    {
        return "steepest descent";
    }
    else if  (convHelper == CONV_GMinStepping)
    {
        return "gMin stepping";
    }
    else if  (convHelper == CONV_SourceStepping)
    {
        return "source stepping";
    }
    return "none";
}

/* This is the non-linear iterative nodal analysis netlist solver. */
template <class nr_type_t>
int nasolver<nr_type_t>::solve_nonlinear (void)
{
    qucs::exception * e;
    int convergence, run = 0, MaxIterations, error = 0;

    // fetch simulation properties
    MaxIterations = getPropertyInteger ("MaxIter");
    reltol = getPropertyDouble ("reltol");
    abstol = getPropertyDouble ("abstol");
    vntol = getPropertyDouble ("vntol");
    updateMatrix = 1;

    if (convHelper == CONV_GMinStepping)
    {
        // use the alternative non-linear solver solve_nonlinear_continuation_gMin
        // instead of the basic solver provided by this function
        iterations = 0;
        error = solve_nonlinear_continuation_gMin ();
        return error;
    }
    else if (convHelper == CONV_SourceStepping)
    {
        // use the alternative non-linear solver solve_nonlinear_continuation_Source
        // instead of the basic solver provided by this function
        iterations = 0;
        error = solve_nonlinear_continuation_Source ();
        return error;
    }

    // run solving loop until convergence is reached
    do
    {
        error = solve_once ();
        if (!error)
        {
            // convergence check
            convergence = (run > 0) ? checkConvergence () : 0;
            savePreviousIteration ();
            run++;
            // control fixpoint iterations
            if (fixpoint)
            {
                if (convergence && !updateMatrix)
                {
                    updateMatrix = 1;
                    convergence = 0;
                }
                else
                {
                    updateMatrix = 0;
                }
            }
        }
        else
        {
            break;
        }
    }
    while (!convergence &&
            run < MaxIterations * (1 + convHelper ? 1 : 0));

    if (run >= MaxIterations || error)
    {
        e = new qucs::exception (EXCEPTION_NO_CONVERGENCE);
        e->setText ("no convergence in %s analysis after %d iterations",
                    desc.c_str(), run);
        throw_exception (e);
        error++;
    }

    iterations = run;
    return error;
}

/* This is the linear nodal analysis netlist solver. */
template <class nr_type_t>
int nasolver<nr_type_t>::solve_linear (void)
{
    updateMatrix = 1;
    return solve_once ();
}

/* Applying the MNA (Modified Nodal Analysis) to a circuit with
   passive elements and independent current and voltage sources
   results in a matrix equation of the form Ax = z.  This function
   generates the A and z matrix. */
template <class nr_type_t>
void nasolver<nr_type_t>::createMatrix (void)
{

    /* Generate the A matrix.  The A matrix consists of four (4) minor
       matrices in the form     +-   -+
                            A = | G B |
                                | C D |
    		      +-   -+.
       Each of these minor matrices is going to be generated here. */
    if (updateMatrix)
    {
        createGMatrix ();
        createBMatrix ();
        createCMatrix ();
        createDMatrix ();
    }

    /* Adjust G matrix if requested. */
    if (convHelper == CONV_GMinStepping)
    {
        int N = countNodes ();
        int M = countVoltageSources ();
        for (int n = 0; n < N + M; n++)
        {
            A->set (n, n, A->get (n, n) + gMin);
        }
    }

    /* Generate the z Matrix.  The z Matrix consists of two (2) minor
       matrices in the form     +- -+
                            z = | i |
                                | e |
    		      +- -+.
       Each of these minor matrices is going to be generated here. */
    createZVector ();
}

/* This MatVal() functionality is just helper to get the correct
   values from the circuit's matrices.  The additional (unused)
   argument is used to differentiate between the two possible
   types. */
#define MatVal(x) MatValX (x, (nr_type_t *) 0)

template <class nr_type_t>
nr_type_t nasolver<nr_type_t>::MatValX (nr_complex_t z, nr_complex_t *)
{
    return z;
}

template <class nr_type_t>
nr_type_t nasolver<nr_type_t>::MatValX (nr_complex_t z, nr_double_t *)
{
    return real (z);
}

/* The B matrix is an MxN matrix with only 0, 1 and -1 elements.  Each
   location in the matrix corresponds to a particular voltage source
   (first dimension) or a node (second dimension).  If the positive
   terminal of the ith voltage source is connected to node k, then the
   element (i,k) in the B matrix is a 1.  If the negative terminal of
   the ith voltage source is connected to node k, then the element
   (i,k) in the B matrix is a -1.  Otherwise, elements of the B matrix
   are zero. */
template <class nr_type_t>
void nasolver<nr_type_t>::createBMatrix (void)
{
    int N = countNodes ();
    int M = countVoltageSources ();
    circuit * vs;
    struct nodelist_t * n;
    nr_type_t val;

    // go through each voltage sources (first dimension)
    for (int c = 0; c < M; c++)
    {
        vs = findVoltageSource (c);
        // go through each node (second dimension)
        for (int r = 0; r < N; r++)
        {
            val = 0.0;
            n = nlist->getNode (r);
            for (auto &current : *n)
            {
                // is voltage source connected to node ?
	      if (current->getCircuit () == vs)
                {
		  val += MatVal (vs->getB (current->getPort (), c));
                }
            }
            // put value into B matrix
            A->set (r, c + N, val);
        }
    }
}

/* The C matrix is an NxM matrix with only 0, 1 and -1 elements.  Each
   location in the matrix corresponds to a particular node (first
   dimension) or a voltage source (first dimension).  If the positive
   terminal of the ith voltage source is connected to node k, then the
   element (k,i) in the C matrix is a 1.  If the negative terminal of
   the ith voltage source is connected to node k, then the element
   (k,i) in the C matrix is a -1.  Otherwise, elements of the C matrix
   are zero. */
template <class nr_type_t>
void nasolver<nr_type_t>::createCMatrix (void)
{
    int N = countNodes ();
    int M = countVoltageSources ();
    circuit * vs;
    struct nodelist_t * n;
    nr_type_t val;

    // go through each voltage sources (second dimension)
    for (int r = 0; r < M; r++)
    {
        vs = findVoltageSource (r);
        // go through each node (first dimension)
        for (int c = 0; c < N; c++)
        {
            val = 0.0;
            n = nlist->getNode (c);
            for (auto &current: *n)
            {
                // is voltage source connected to node ?
	      if (current->getCircuit () == vs)
                {
		  val += MatVal (vs->getC (r, current->getPort ()));
                }
            }
            // put value into C matrix
            A->set (r + N, c, val);
        }
    }
}

/* The D matrix is an MxM matrix that is composed entirely of zeros.
   It can be non-zero if dependent sources are considered. */
template <class nr_type_t>
void nasolver<nr_type_t>::createDMatrix (void)
{
    int M = countVoltageSources ();
    int N = countNodes ();
    circuit * vsr, * vsc;
    nr_type_t val;
    for (int r = 0; r < M; r++)
    {
        vsr = findVoltageSource (r);
        for (int c = 0; c < M; c++)
        {
            vsc = findVoltageSource (c);
            val = 0.0;
            if (vsr == vsc)
            {
                val = MatVal (vsr->getD (r, c));
            }
            A->set (r + N, c + N, val);
        }
    }
}

/* The G matrix is an NxN matrix formed in two steps.
   1. Each element in the diagonal matrix is equal to the sum of the
   conductance of each element connected to the corresponding node.
   2. The off diagonal elements are the negative conductance of the
   element connected to the pair of corresponding nodes.  Therefore a
   resistor between nodes 1 and 2 goes into the G matrix at location
   (1,2) and location (2,1).  If an element is grounded, it will only
   have contribute to one entry in the G matrix -- at the appropriate
   location on the diagonal. */
template <class nr_type_t>
void nasolver<nr_type_t>::createGMatrix (void)
{
    int pr, pc, N = countNodes ();
    nr_type_t g;
    struct nodelist_t * nr, * nc;
    circuit * ct;

    // go through each column of the G matrix
    for (int c = 0; c < N; c++)
    {
        nc = nlist->getNode (c);
        // go through each row of the G matrix
        for (int r = 0; r < N; r++)
        {
            nr = nlist->getNode (r);
            g = 0.0;
            // sum up the conductance of each connected circuit
            for (auto & currentnc  : *nc)
	      for (auto & currentnr: *nr)
		if (currentnc->getCircuit () == currentnr->getCircuit ())
		  {
		    ct = currentnc->getCircuit ();
		    pc = currentnc->getPort ();
		    pr = currentnr->getPort ();
		    g += MatVal (ct->getY (pr, pc));
		  }
            // put value into G matrix
            A->set (r, c, g);
        }
    }
}

/* The following function creates the (N+M)x(N+M) noise current
   correlation matrix used during the AC noise computations.  */
template <class nr_type_t>
void nasolver<nr_type_t>::createNoiseMatrix (void)
{
    int pr, pc, N = countNodes ();
    int M = countVoltageSources ();
    struct nodelist_t * n;
    nr_type_t val;
    int r, c, ri, ci;
    struct nodelist_t * nr, * nc;
    circuit * ct;

    // create new Cy matrix if necessary
    delete C;
    C = new tmatrix<nr_type_t> (N + M);

    // go through each column of the Cy matrix
    for (c = 0; c < N; c++)
    {
        nc = nlist->getNode (c);
        // go through each row of the Cy matrix
        for (r = 0; r < N; r++)
        {
            nr = nlist->getNode (r);
            val = 0.0;
            // sum up the noise-correlation of each connected circuit
            for (auto & currentnc: *nc)
		/* a = 0; a < nc->size(); a++ */
	      for (auto &currentnr : *nr)
		/* b = 0; b < nr->size(); b++) */
                    if (currentnc->getCircuit () == currentnr->getCircuit ())
                    {
                        ct = currentnc->getCircuit ();
                        pc = currentnc->getPort ();
                        pr = currentnr->getPort ();
                        val += MatVal (ct->getN (pr, pc));
                    }
            // put value into Cy matrix
            C->set (r, c, val);
        }
    }

    // go through each additional voltage source and put coefficients into
    // the noise current correlation matrix
    circuit * vsr, * vsc;
    for (r = 0; r < M; r++)
    {
        vsr = findVoltageSource (r);
        for (c = 0; c < M; c++)
        {
            vsc = findVoltageSource (c);
            val = 0.0;
            if (vsr == vsc)
            {
                ri = vsr->getSize () + r - vsr->getVoltageSource ();
                ci = vsc->getSize () + c - vsc->getVoltageSource ();
                val = MatVal (vsr->getN (ri, ci));
            }
            C->set (r + N, c + N, val);
        }
    }

    // go through each additional voltage source
    for (r = 0; r < M; r++)
    {
        vsr = findVoltageSource (r);
        // go through each node
        for (c = 0; c < N; c++)
        {
            val = 0.0;
            n = nlist->getNode (c);
            for (auto &currentn: *n)
	      /*i = 0; i < n->size(); i++ )*/
            {
                // is voltage source connected to node ?
                if (currentn->getCircuit () == vsr)
                {
                    ri = vsr->getSize () + r - vsr->getVoltageSource ();
                    ci = currentn->getPort ();
                    val += MatVal (vsr->getN (ri, ci));
                }
            }
            // put value into Cy matrix
            C->set (r + N, c, val);
        }
    }

    // go through each voltage source
    for (c = 0; c < M; c++)
    {
        vsc = findVoltageSource (c);
        // go through each node
        for (r = 0; r < N; r++)
        {
            val = 0.0;
            n = nlist->getNode (r);
            for (auto & currentn: *n)/*i = 0; i < n->size(); i++)*/
            {
                // is voltage source connected to node ?
                if (currentn->getCircuit () == vsc)
                {
                    ci = vsc->getSize () + c - vsc->getVoltageSource ();
                    ri = currentn->getPort ();
                    val += MatVal (vsc->getN (ri, ci));
                }
            }
            // put value into Cy matrix
            C->set (r, c + N, val);
        }
    }

}

/* The i matrix is an 1xN matrix with each element of the matrix
   corresponding to a particular node.  The value of each element of i
   is determined by the sum of current sources into the corresponding
   node.  If there are no current sources connected to the node, the
   value is zero. */
template <class nr_type_t>
void nasolver<nr_type_t>::createIVector (void)
{
    int N = countNodes ();
    nr_type_t val;
    struct nodelist_t * n;
    circuit * is;

    // go through each node
    for (int r = 0; r < N; r++)
    {
        val = 0.0;
        n = nlist->getNode (r);
        // go through each circuit connected to the node
        for (auto &currentn: *n)/* int i = 0; i < n->size(); i++)*/
        {
	  is = currentn->getCircuit ();
	  // is this a current source ?
	  if (is->isISource () || is->isNonLinear ())
	    {
	      val += MatVal (is->getI (currentn->getPort ()));
            }
        }
        // put value into i vector
        z->set (r, val);
    }
}

/* The e matrix is a 1xM matrix with each element of the matrix equal
   in value to the corresponding independent voltage source. */
template <class nr_type_t>
void nasolver<nr_type_t>::createEVector (void)
{
    int N = countNodes ();
    int M = countVoltageSources ();
    nr_type_t val;
    circuit * vs;

    // go through each voltage source
    for (int r = 0; r < M; r++)
    {
        vs = findVoltageSource (r);
        val = MatVal (vs->getE (r));
        // put value into e vector
        z->set (r + N, val);
    }
}

// The function loads the right hand side vector.
template <class nr_type_t>
void nasolver<nr_type_t>::createZVector (void)
{
    createIVector ();
    createEVector ();
}

// Returns the number of nodes in the nodelist, excluding the ground node.
template <class nr_type_t>
int nasolver<nr_type_t>::countNodes (void)
{
    return nlist->length () - 1;
}

// Returns the node number of the give node name.
template <class nr_type_t>
int nasolver<nr_type_t>::getNodeNr (const std::string &str)
{
    return nlist->getNodeNr (str);
}

/* The function returns the assigned node number for the port of the
   given circuits.  It returns -1 if there is no such node. */
template <class nr_type_t>
int nasolver<nr_type_t>::findAssignedNode (circuit * c, int port)
{
    int N = countNodes ();
    struct nodelist_t * n;
    for (int r = 0; r < N; r++)
    {
        n = nlist->getNode (r);
        for (auto &currentn : *n) /*int i = 0; i < n->size(); i++)*/
            if (c == currentn->getCircuit ())
                if (port == currentn->getPort ())
                    return r;
    }
    return -1;
}

// Returns the number of voltage sources in the nodelist.
template <class nr_type_t>
int nasolver<nr_type_t>::countVoltageSources (void)
{
    return subnet->getVoltageSources ();
}

/* The function returns the voltage source circuit object
   corresponding to the given number.  If there is no such voltage
   source it returns NULL. */
template <class nr_type_t>
circuit * nasolver<nr_type_t>::findVoltageSource (int n)
{
    circuit * root = subnet->getRoot ();
    for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ())
    {
        if (n >= c->getVoltageSource () &&
                n <= c->getVoltageSource () + c->getVoltageSources () - 1)
            return c;
    }
    return NULL;
}

/* The function applies unique voltage source identifiers to each
   voltage source (explicit and built in internal ones) in the list of
   registered circuits. */
template <class nr_type_t>
void nasolver<nr_type_t>::assignVoltageSources (void)
{
    circuit * root = subnet->getRoot ();
    int nSources = 0;
    for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ())
    {
        if (c->getVoltageSources () > 0)
        {
            c->setVoltageSource (nSources);
            nSources += c->getVoltageSources ();
        }
    }
    subnet->setVoltageSources (nSources);
}

/* The matrix equation Ax = z is solved by x = A^-1*z.  The function
   applies the operation to the previously generated matrices. */
template <class nr_type_t>
void nasolver<nr_type_t>::runMNA (void)
{

    // just solve the equation system here
    eqns->setAlgo (eqnAlgo);
    eqns->passEquationSys (updateMatrix ? A : NULL, x, z);
    eqns->solve ();

    // if damped Newton-Raphson is requested
    if (xprev != NULL && top_exception () == NULL)
    {
        if (convHelper == CONV_Attenuation)
        {
            applyAttenuation ();
        }
        else if (convHelper == CONV_LineSearch)
        {
            lineSearch ();
        }
        else if (convHelper == CONV_SteepestDescent)
        {
            steepestDescent ();
        }
    }
}

/* This function applies a damped Newton-Raphson (limiting scheme) to
   the current solution vector in the form x1 = x0 + a * (x1 - x0).  This
   convergence helper is heuristic and does not ensure global convergence. */
template <class nr_type_t>
void nasolver<nr_type_t>::applyAttenuation (void)
{
    nr_double_t alpha = 1.0, nMax;

    // create solution difference vector and find maximum deviation
    tvector<nr_type_t> dx = *x - *xprev;
    nMax = maxnorm (dx);

    // compute appropriate damping factor
    if (nMax > 0.0)
    {
        nr_double_t g = 1.0;
        alpha = std::min (0.9, g / nMax);
        if (alpha < 0.1) alpha = 0.1;
    }

    // apply damped solution vector
    *x = *xprev + alpha * dx;
}

/* This is damped Newton-Raphson using nested iterations in order to
   find a better damping factor.  It identifies a damping factor in
   the interval [0,1] which minimizes the right hand side vector.  The
   algorithm actually ensures global convergence but pushes the
   solution to local minimums, i.e. where the Jacobian matrix A may be
   singular. */
template <class nr_type_t>
void nasolver<nr_type_t>::lineSearch (void)
{
    nr_double_t alpha = 0.5, n, nMin, aprev = 1.0, astep = 0.5, adiff;
    int dir = -1;

    // compute solution deviation vector
    tvector<nr_type_t> dx = *x - *xprev;
    nMin = std::numeric_limits<nr_double_t>::max();

    do
    {
        // apply current damping factor and see what happens
        *x = *xprev + alpha * dx;

        // recalculate Jacobian and right hand side
        saveSolution ();
        calculate ();
        createZVector ();

        // calculate norm of right hand side vector
        n = norm (*z);

        // TODO: this is not perfect, but usable
        astep /= 2;
        adiff = fabs (alpha - aprev);
        if (adiff > 0.005)
        {
            aprev = alpha;
            if (n < nMin)
            {
                nMin = n;
                if (alpha == 1) dir = -dir;
                alpha += astep * dir;
            }
            else
            {
                dir = -dir;
                alpha += 1.5 * astep * dir;
            }
        }
    }
    while (adiff > 0.005);

    // apply final damping factor
    assert (alpha > 0 && alpha <= 1);
    *x = *xprev + alpha * dx;
}

/* The function looks for the optimal gradient for the right hand side
   vector using the so-called 'steepest descent' method.  Though
   better than the one-dimensional linesearch (it doesn't push
   iterations into local minimums) it converges painfully slow. */
template <class nr_type_t>
void nasolver<nr_type_t>::steepestDescent (void)
{
    nr_double_t alpha = 1.0, sl, n;

    // compute solution deviation vector
    tvector<nr_type_t> dx = *x - *xprev;
    tvector<nr_type_t> dz = *z - *zprev;
    n = norm (*zprev);

    do
    {
        // apply current damping factor and see what happens
        *x = *xprev + alpha * dx;

        // recalculate Jacobian and right hand side
        saveSolution ();
        calculate ();
        createZVector ();

        // check gradient criteria, ThinkME: Is this correct?
        dz = *z - *zprev;
        sl = real (sum (dz * -dz));
        if (norm (*z) < n + alpha * sl) break;
        alpha *= 0.7;
    }
    while (alpha > 0.001);

    // apply final damping factor
    *x = *xprev + alpha * dx;
}

/* The function checks whether the iterative algorithm for linearizing
   the non-linear components in the network shows convergence.  It
   returns non-zero if it converges and zero otherwise. */
template <class nr_type_t>
int nasolver<nr_type_t>::checkConvergence (void)
{

    int N = countNodes ();
    int M = countVoltageSources ();
    nr_double_t v_abs, v_rel, i_abs, i_rel;
    int r;

    // check the nodal voltage changes against the allowed absolute
    // and relative tolerance values
    for (r = 0; r < N; r++)
    {
        v_abs = abs (x->get (r) - xprev->get (r));
        v_rel = abs (x->get (r));
        if (v_abs >= vntol + reltol * v_rel) return 0;
        if (!convHelper)
        {
            i_abs = abs (z->get (r) - zprev->get (r));
            i_rel = abs (z->get (r));
            if (i_abs >= abstol + reltol * i_rel) return 0;
        }
    }

    for (r = 0; r < M; r++)
    {
        i_abs = abs (x->get (r + N) - xprev->get (r + N));
        i_rel = abs (x->get (r + N));
        if (i_abs >= abstol + reltol * i_rel) return 0;
        if (!convHelper)
        {
            v_abs = abs (z->get (r + N) - zprev->get (r + N));
            v_rel = abs (z->get (r + N));
            if (v_abs >= vntol + reltol * v_rel) return 0;
        }
    }
    return 1;
}

/* The function saves the solution and right hand vector of the previous
   iteration. */
template <class nr_type_t>
void nasolver<nr_type_t>::savePreviousIteration (void)
{
    if (xprev != NULL)
        *xprev = *x;
    else
        xprev = new tvector<nr_type_t> (*x);
    if (zprev != NULL)
        *zprev = *z;
    else
        zprev = new tvector<nr_type_t> (*z);
}

/* The function restores the solution and right hand vector of the
   previous (successful) iteration. */
template <class nr_type_t>
void nasolver<nr_type_t>::restorePreviousIteration (void)
{
    if (xprev != NULL) *x = *xprev;
    if (zprev != NULL) *z = *zprev;
}

/* The function restarts the NR iteration for each non-linear
   circuit. */
template <class nr_type_t>
void nasolver<nr_type_t>::restartNR (void)
{
    circuit * root = subnet->getRoot ();
    for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ())
    {
        if (c->isNonLinear ()) c->restartDC ();
    }
}

/* This function goes through solution (the x vector) and saves the
   node voltages of the last iteration into each non-linear
   circuit. */
template <class nr_type_t>
void nasolver<nr_type_t>::saveNodeVoltages (void)
{
    int N = countNodes ();
    struct nodelist_t * n;
    // save all nodes except reference node
    for (int r = 0; r < N; r++)
    {
        n = nlist->getNode (r);
        /* for (int i = 0; i < n->size(); i++)*/
	for(auto &currentn: *n)
        {
	  currentn->getCircuit()->setV (currentn->getPort (), x->get (r));
        }
    }
    // save reference node
    n = nlist->getNode (-1);
    for(auto &currentn: *n)
      currentn->getCircuit()->setV (currentn->getPort (), 0.0);
}

/* This function goes through solution (the x vector) and saves the
   branch currents through the voltage sources of the last iteration
   into each circuit. */
template <class nr_type_t>
void nasolver<nr_type_t>::saveBranchCurrents (void)
{
    int N = countNodes ();
    int M = countVoltageSources ();
    circuit * vs;
    // save all branch currents of voltage sources
    for (int r = 0; r < M; r++)
    {
        vs = findVoltageSource (r);
        vs->setJ (r, x->get (r + N));
    }
}

// The function saves the solution vector into each circuit.
template <class nr_type_t>
void nasolver<nr_type_t>::saveSolution (void)
{
    saveNodeVoltages ();
    saveBranchCurrents ();
}

// This function stores the solution (node voltages and branch currents).
template <class nr_type_t>
void nasolver<nr_type_t>::storeSolution (void)
{
    // cleanup solution previously
    solution.clear ();
    int r;
    int N = countNodes ();
    int M = countVoltageSources ();
    // store all nodes except reference node
    for (r = 0; r < N; r++)
    {
        struct nodelist_t * n = nlist->getNode (r);
	nr_type_t gr = x->get (r);
	qucs::naentry<nr_type_t> entry(gr, 0);
        solution.insert({{n->name, entry }});
    }
    // store all branch currents of voltage sources
    for (r = 0; r < M; r++)
    {
        circuit * vs = findVoltageSource (r);
        int vn = r - vs->getVoltageSource () + 1;
	nr_type_t xg = x->get (r + N);
	qucs::naentry<nr_type_t> entry(xg, vn);
        solution.insert({{vs->getName (), entry}});
    }
}

// This function recalls the solution (node voltages and branch currents).
template <class nr_type_t>
void nasolver<nr_type_t>::recallSolution (void)
{
    int r;
    int N = countNodes ();
    int M = countVoltageSources ();
    // store all nodes except reference node
    for (r = 0; r < N; r++)
    {
        struct nodelist_t * n = nlist->getNode (r);
	auto na = solution.find(n->name);
	if (na != solution.end())
	  if ((*na).second.current == 0)
            x->set (r, (*na).second.value);
    }
    // store all branch currents of voltage sources
    for (r = 0; r < M; r++)
    {
        circuit * vs = findVoltageSource (r);
        int vn = r - vs->getVoltageSource () + 1;
	auto na = solution.find(vs->getName ());
	if (na != solution.end())
	  if ((*na).second.current == vn)
            x->set (r + N, (*na).second.value);
    }
}

/* This function saves the results of a single solve() functionality
   into the output dataset. */
template <class nr_type_t>
void nasolver<nr_type_t>::saveResults (const std::string &volts, const std::string &amps,
                                       int saveOPs, qucs::vector * f)
{
    int N = countNodes ();
    int M = countVoltageSources ();

    // add node voltage variables
    if (!volts.empty())
    {
        for (int r = 0; r < N; r++)
        {
	  std::string n = createV (r, volts, saveOPs);
	  if(!n.empty())
	    {
                saveVariable (n, x->get (r), f);
            }
        }
    }

    // add branch current variables
    if (!amps.empty())
    {
        for (int r = 0; r < M; r++)
        {
	  std::string n = createI (r, amps, saveOPs);
	  if (!n.empty())
            {
                saveVariable (n, x->get (r + N), f);
            }
        }
    }

    // add voltage probe data
    if (!volts.empty())
    {
        circuit * root = subnet->getRoot ();
        for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ())
        {
            if (!c->isProbe ()) continue;
            if (!c->getSubcircuit().empty() && !(saveOPs & SAVE_ALL)) continue;
            if (volts != "vn")
                c->saveOperatingPoints ();
	    std::string n = createOP (c->getName (), volts);
            saveVariable (n, nr_complex_t (c->getOperatingPoint ("Vr"),
            c->getOperatingPoint ("Vi")), f);

	    //add watt probe data
	    c->calcOperatingPoints ();
	    for (auto ops: c->getOperatingPoints ())
            {
		//It will only get values if none of the strings are 0
		//Once again most of this is adapted from Vprobe and Iprobe
                operatingpoint &p = ops.second;
	        if (strcmp(p.getName(), "Vi") == 0) continue;
                if (strcmp(p.getName(), "VAi") == 0) continue;
	        if (strcmp(p.getName(), "Vr") == 0) continue;
		if (strcmp(p.getName(), "VAr") == 0)
		{
              	    std::string n = createOP(c->getName(), "S");
              	    saveVariable (n, nr_complex_t (c->getOperatingPoint ("VAr"),
                    c->getOperatingPoint ("VAi")), f);
		   continue;
		}
           	
           	std::string n = createOP(c->getName(), p.getName());
           	saveVariable(n, p.getValue(), f);
       	    }	    
	    	    
        }
    }

    // save operating points of non-linear circuits if requested
    if (saveOPs & SAVE_OPS)
    {
        circuit * root = subnet->getRoot ();
        for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ())
        {
            if (!c->isNonLinear ()) continue;
            if (!c->getSubcircuit ().empty() && !(saveOPs & SAVE_ALL)) continue;
            c->calcOperatingPoints ();
            for (auto ops: c->getOperatingPoints ())
            {
                operatingpoint &p = ops.second;
		std::string n = createOP (c->getName (), p.getName ());
                saveVariable (n, p.getValue (), f);
            }
        }
    }
}

/* Create an appropriate variable name for operating points.  The
   caller is responsible to free() the returned string. */
template <class nr_type_t>
std::string nasolver<nr_type_t>::createOP (const std::string &c, const std::string &n)
{
    return c+"."+n;
}

/* Creates an appropriate variable name for voltages.  The caller is
   responsible to free() the returned string. */
template <class nr_type_t>
std::string nasolver<nr_type_t>::createV (int n, const std::string &volts, int saveOPs)
{
    if (nlist->isInternal (n))
      return std::string();
    std::string node = nlist->get (n);
    if(node.find('.')!=std::string::npos && !(saveOPs & SAVE_ALL))
      return std::string();
    std::string ret = node+"."+volts;
    return ret;
}

/* Create an appropriate variable name for currents.  The caller is
   responsible to free() the returned string. */
template <class nr_type_t>
std::string nasolver<nr_type_t>::createI (int n, const std::string &amps, int saveOPs)
{
    circuit * vs = findVoltageSource (n);

    // don't output internal (helper) voltage sources
    if (vs->isInternalVoltageSource ())
      return std::string();

    /* save only current through real voltage sources and explicit
       current probes */
    if (!vs->isVSource () && !(saveOPs & SAVE_OPS))
      return std::string();

    // don't output subcircuit components if not requested
    if (!vs->getSubcircuit ().empty() && !(saveOPs & SAVE_ALL))
      return std::string();

    // create appropriate current name for single/multiple voltage sources
    std::string name = vs->getName ();
    if (vs->getVoltageSources () > 1)
      return name+"."+amps+std::to_string(n - vs->getVoltageSource () + 1);
    else
      return name+"."+amps;
}


/* Alternative to countNodes () */
template <class nr_type_t>
int nasolver<nr_type_t>::getN()
{
    return countNodes ();
}

/* Alternative to countVoltageSources () */
template <class nr_type_t>
int nasolver<nr_type_t>::getM()
{
    return countVoltageSources ();
}

} // namespace qucs
