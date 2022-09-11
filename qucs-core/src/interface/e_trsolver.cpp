/*
 * e_trsolver.cpp - external transient solver interface class implementation
 *
 * Copyright (C) 2004, 2005, 2006, 2007, 2009 Stefan Jahn <stefan@lkcc.org>
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

/** \file ecvs.h
  * \brief The externally controlled transient solver implementation file.
  *
  */

/**
  * \ingroup QucsInterface
  */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <float.h>

#include "compat.h"
#include "object.h"
#include "logging.h"
#include "complex.h"
#include "circuit.h"
#include "sweep.h"
#include "net.h"
#include "netdefs.h"
#include "analysis.h"
#include "nasolver.h"
#include "history.h"
#include "transient.h"
#include "exception.h"
#include "exceptionstack.h"
#include "environment.h"
#include "e_trsolver.h"
#include "component_id.h"
#include "ecvs.h"

#define STEPDEBUG   0 // set to zero for release
#define BREAKPOINTS 0 // exact breakpoint calculation

#ifndef dState
#define dState 0 // delta T state
#endif

#ifndef sState
#define sState 1 // solution state
#endif

// Macro for the n-th state of the solution vector history.
#ifndef SOL
#define SOL(state) (solution[(int) getState (sState, (state))])
#endif

namespace qucs {

using namespace transient;

// Constructor creates an unnamed instance of the trsolver class.
e_trsolver::e_trsolver ()
    : trsolver ()
{
    //swp = NULL;
    type = ANALYSIS_E_TRANSIENT;
    //setDescription ("m-transient");
    //for (int i = 0; i < 8; i++) solution[i] = NULL;
    //tHistory = NULL;
    //relaxTSR = false;
    //initialDC = true;

    // initialise the message function pointer to
    // point to the PrintWarningMsg function
    messagefcn = &logprint;
#if DEBUG
    //loginit ();
    // produce an actual log file
//    file_error = file_status = fopen("e_trsolver.log", "w+");
//    precinit ();
//    ::srand (::time (NULL));
#endif // DEBUG
}

// Constructor creates a named instance of the e_trsolver class.
e_trsolver::e_trsolver (char * n)
    : trsolver (n)
{
    //swp = NULL;
    type = ANALYSIS_E_TRANSIENT;
    //setDescription ("m-transient");
    //for (int i = 0; i < 8; i++) solution[i] = NULL;
    //tHistory = NULL;
    //relaxTSR = false;
    //initialDC = true;

    // initialise the message function pointer to
    // point to the PrintWarningMsg function
    messagefcn = &logprint;
}

// Destructor deletes the e_trsolver class object.
e_trsolver::~e_trsolver ()
{

    solve_post ();
    if (progress) logprogressclear (40);

    deinitTR ();

    delete swp;

    for (int i = 0; i < 8; i++)
    {
        if (solution[i] != NULL)
        {
            delete solution[i];
        }
        if (lastsolution[i] != NULL)
        {
            delete lastsolution[i];
        }
    }
    delete tHistory;

}

/* The copy constructor creates a new instance of the e_trsolver class
   based on the given e_trsolver object. */
e_trsolver::e_trsolver (e_trsolver & o)
    : trsolver (o)
{
    swp = o.swp ? new sweep (*o.swp) : NULL;
    for (int i = 0; i < 8; i++) solution[i] = NULL;
    tHistory = o.tHistory ? new history (*o.tHistory) : NULL;
    relaxTSR = o.relaxTSR;
    initialDC = o.initialDC;
}

void e_trsolver::debug()
{
    circuit * root = subnet->getRoot ();

    for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ())
    {
        messagefcn (0, c->getName() );

        if (!c->getSubcircuit ().empty()) {
	  printf ("subcircuit Name %s\n", c->getSubcircuit ().c_str());
        }
    }

    //netlist_list();
}


/* This is the initialisation function for the slaved transient
   netlist solver. It prepares the circuit for simulation. */
int e_trsolver::init (nr_double_t start, nr_double_t firstdelta, int mode)
{
    // run the equation solver for the netlist
    this->getEnv()->runSolver();

    int error = 0;
    const char * const solver = getPropertyString ("Solver");
    relaxTSR = !strcmp (getPropertyString ("relaxTSR"), "yes") ? true : false;
    initialDC = !strcmp (getPropertyString ("initialDC"), "yes") ? true : false;
    // fetch simulation properties
    MaxIterations = getPropertyInteger ("MaxIter");
    reltol = getPropertyDouble ("reltol");
    abstol = getPropertyDouble ("abstol");
    vntol = getPropertyDouble ("vntol");

    runs++;
    saveCurrent = current = 0;
    stepDelta = -1;
    converged = 0;
    fixpoint = 0;
    lastsynctime = 0.0;
    statRejected = statSteps = statIterations = statConvergence = 0;

    // Choose a solver.
    if (!strcmp (solver, "CroutLU"))
        eqnAlgo = ALGO_LU_DECOMPOSITION;
    else if (!strcmp (solver, "DoolittleLU"))
        eqnAlgo = ALGO_LU_DECOMPOSITION_DOOLITTLE;
    else if (!strcmp (solver, "HouseholderQR"))
        eqnAlgo = ALGO_QR_DECOMPOSITION;
    else if (!strcmp (solver, "HouseholderLQ"))
        eqnAlgo = ALGO_QR_DECOMPOSITION_LS;
    else if (!strcmp (solver, "GolubSVD"))
        eqnAlgo = ALGO_SV_DECOMPOSITION;

    // Perform initial DC analysis.
    if (initialDC)
    {
        error = dcAnalysis ();
        if (error)
            return -1;
    }

    // Initialize transient analysis.
    setDescription ("transient");
    initETR (start, firstdelta, mode);
    setCalculation ((calculate_func_t) &calcTR);
    solve_pre ();

    // Recall the DC solution.
    recallSolution ();

    // Apply the nodesets and adjust previous solutions.
    applyNodeset (false);
    fillSolution (x);
    fillLastSolution (x);

    // Tell integrators to be initialized.
    setMode (MODE_INIT);

    // reset the circuit status to not running so the histories
    // etc will be reinitialised on the first time step
    running = 0;
    rejected = 0;
    if (mode == ETR_MODE_ASYNC)
    {
        delta /= 10;

    }
    else if (mode == ETR_MODE_SYNC)
    {
        //lastsynctime = start - delta;
    }
    else
    {
        qucs::exception * e = new qucs::exception (EXCEPTION_UNKNOWN_ETR_MODE);
        e->setText ("Unknown ETR mode.");
        throw_exception (e);
        return -2;
    }
    fillState (dState, delta);
    adjustOrder (1);

    storeHistoryAges ();

    return 0;

}

/* Stores all the initial history lengths requested by the circuit
   elements for later use (to make sure we don't set the histories
   to be less than these initial requested values) */
void e_trsolver::storeHistoryAges (void)
{
    circuit * root = subnet->getRoot ();
    for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ())
    {
        // get the a
        if (c->hasHistory ())
        {
            initialhistages.push_back (c->getHistoryAge ());
        }
    }
}

void e_trsolver::fillLastSolution (tvector<nr_double_t> * s)
{
    for (int i = 0; i < 8; i++) * lastsolution[(int) getState (sState, (i))] = *s;
}

/* Goes through the list of circuit objects and runs its initTR()
   function. */
void e_trsolver::initETR (nr_double_t start, nr_double_t firstdelta, int mode)
{
    const char * const IMethod = getPropertyString ("IntegrationMethod");
    //nr_double_t start = getPropertyDouble ("Start");
    //nr_double_t stop = start + firstdelta;
    //nr_double_t points = 1.0;

    // fetch corrector integration method and determine predicor method
    corrMaxOrder = getPropertyInteger ("Order");
    corrType = CMethod = correctorType (IMethod, corrMaxOrder);
    predType = PMethod = predictorType (CMethod, corrMaxOrder, predMaxOrder);
    corrOrder = corrMaxOrder;
    predOrder = predMaxOrder;

    // initialize step values
    if (mode == ETR_MODE_ASYNC){
        delta = getPropertyDouble ("InitialStep");
        deltaMin = getPropertyDouble ("MinStep");
        deltaMax = getPropertyDouble ("MaxStep");
        if (deltaMax == 0.0)
            deltaMax = firstdelta; // MIN ((stop - start) / (points - 1), stop / 200);
        if (deltaMin == 0.0)
            deltaMin = NR_TINY * 10 * deltaMax;
        if (delta == 0.0)
            delta = firstdelta; // MIN (stop / 200, deltaMax) / 10;
        if (delta < deltaMin) delta = deltaMin;
        if (delta > deltaMax) delta = deltaMax;
    }
    else if (mode == ETR_MODE_SYNC)
    {
        delta = firstdelta;
        deltaMin = NR_TINY * 10;
        deltaMax =  std::numeric_limits<nr_double_t>::max() / 10;
    }

    // initialize step history
    setStates (2);
    initStates ();
    // initialise the history of states, setting them all to 'delta'
    fillState (dState, delta);

    // copy the initialised states to the 'deltas' array
    saveState (dState, deltas);
    // copy the deltas to all the circuits
    setDelta ();
    // set the initial corrector and predictor coefficients
    calcCorrectorCoeff (corrType, corrOrder, corrCoeff, deltas);
    calcPredictorCoeff (predType, predOrder, predCoeff, deltas);

    // initialize history of solution vectors (solutions)
    for (int i = 0; i < 8; i++)
    {
        // solution contains the last sets of node voltages and branch
        // currents at each of the last 8 'deltas'.
        // Note for convenience the definition:
        //   #define SOL(state) (solution[(int) getState (sState, (state))])
        // is provided and used elsewhere to update the solutions
        solution[i] = new tvector<nr_double_t>;
        setState (sState, (nr_double_t) i, i);
        lastsolution[i] = new tvector<nr_double_t>;
    }

    // Initialise history tracking for asynchronous solvers
    // See acceptstep_async and rejectstep_async for more
    // information
    lastasynctime = start;
    saveState (dState, lastdeltas);
    lastdelta = delta;

    // tell circuit elements about the transient analysis
    circuit *c, * root = subnet->getRoot ();
    for (c = root; c != NULL; c = (circuit *) c->getNext ())
        initCircuitTR (c);
    // also initialize the created circuit elements
    for (c = root; c != NULL; c = (circuit *) c->getPrev ())
        initCircuitTR (c);
}

void e_trsolver::printx()
{
    char buf [1024];

    for (unsigned int r = 0; r < x->size(); r++) {
        buf[0] = '\0';
        //sprintf (buf, "%+.2e%+.2ei", (double) real (x->get (r)), (double) imag (x->get (r)));

        if (r == 2)
        {
            // print just the currents
//            SOL (0)->get->(r)
//            solution[0]->get(r);
            sprintf (buf, "%f\t%18.17f\t%6.5f %6.5f %6.5f %6.5f %6.5f %6.5f %6.5f %6.5f",
                     current,
                     (double) real (x->get (r)),
                     solution[0]->get(r) ,
                     solution[1]->get(r) ,
                     solution[2]->get(r) ,
                     solution[3]->get(r) ,
                     solution[4]->get(r) ,
                     solution[5]->get(r) ,
                     solution[6]->get(r) ,
                     solution[7]->get(r) );

            messagefcn(0, buf);
        }
    }
}

/* synchronous step solver for external ode routine
 *
 * This function solves the circuit for a single time delta provided
 * by an external source. Convergence issues etc. are expected to
 * be handled by the external solver, as it is in full control of the
 * time stepping.
 */
int e_trsolver::stepsolve_sync(nr_double_t synctime)
{

    int error = 0;
    convError = 0;

    time = synctime;
    // update the interpolation time of any externally controlled
    // components which require it.
    updateExternalInterpTime(time);

    // copy the externally chosen time step to delta
    delta = time - lastsynctime;

    // get the current solution time
    //current += delta;

    // updates the integrator coefficients, and updates the array of prev
    // 8 deltas with the new delta for this step
    updateCoefficients (delta);

    // Run predictor to get a start value for the solution vector for
    // the successive iterative corrector process
    error += predictor ();

    // restart Newton iteration
    restartNR (); // restart non-linear devices

    // Attempt to solve the circuit with the given delta
    try_running () // #defined as:    do {
    {
        //error += solve_nonlinear_step ();
        error += corrector ();
    }
    catch_exception () // #defined as:   } while (0); if (estack.top ()) switch (estack.top()->getCode ())
    {
    case EXCEPTION_NO_CONVERGENCE:
        pop_exception ();

        // Retry using damped Newton-Raphson.
        this->convHelper = CONV_SteepestDescent;
        convError = 2;
#if DEBUG
        messagefcn (LOG_ERROR, "WARNING: delta rejected at t = %.3e, h = %.3e "
                  "(no convergence)\n", (double) saveCurrent, (double) delta);
#endif

        try_running () // #defined as:    do {
        {
//            error += solve_nonlinear_step ();
            error += solve_nonlinear ();
        }
        catch_exception () // #defined as:   } while (0); if (estack.top ()) switch (estack.top()->getCode ())
        {
        case EXCEPTION_NO_CONVERGENCE:
            pop_exception ();

            // Update statistics.
            statRejected++;
            statConvergence++;
            rejected++;
            converged = 0;
            error = 0;

            break;
        default:
            // Otherwise return.
            estack.print ();
            error++;
            break;
        }

        // Update statistics and no more damped Newton-Raphson.
//        statIterations += iterations;
//        if (--convError < 0) this->convHelper = 0;


        break;
    default:
        // Otherwise return.
        estack.print ();
        error++;
        break;
    }
    // if there was an error other than non-convergence, return -1
    if (error) return -1;

    // check whether Jacobian matrix is still non-singular
    if (!A->isFinite ())
    {
//        messagefcn (LOG_ERROR, "ERROR: %s: Jacobian singular at t = %.3e, "
//                  "aborting %s analysis\n", getName (), (double) current,
//                  getDescription ());
        return -1;
    }

    return 0;
}

// accept a time step into the solution history
void e_trsolver::acceptstep_sync()
{
    statIterations += iterations;
    if (--convError < 0) convHelper = 0;

    // Now advance in time or not...
    if (running > 1)
    {
        adjustDelta_sync (current);
//        deltaOld = delta;
//        stepDelta = deltaOld;
//        nextStates ();
//        rejected = 0;
        adjustOrder ();
    }
    else
    {
        fillStates ();
        nextStates ();
        rejected = 0;
    }

    saveCurrent = current;
    current += delta;
    running++;
    converged++;

    // Tell integrators to be running.
    setMode (MODE_NONE);

    // Initialize or update history.
    if (running > 1)
    {
        // update the solution history with the new results
        updateHistory (current);
    }
    else
    {
        // we have just solved the first transient state
        initHistory (current);
    }

    // store the current time
    lastsynctime = current;
}

/* This function tries to adapt the current time-step according to the
   global truncation error. */
void e_trsolver::adjustDelta_sync (nr_double_t t)
{
    deltaOld = delta;

    // makes a new delta based on truncation error
//    delta = checkDelta ();

    if (delta > deltaMax)
    {
        delta = deltaMax;
    }

    if (delta < deltaMin)
    {
        delta = deltaMin;
    }

    // delta correction in order to hit exact breakpoint
    int good = 0;
//    if (!relaxTSR)   // relaxed step raster?
//    {
//        if (!statConvergence || converged > 64)   /* Is this a good guess? */
//        {
//            // check next breakpoint
//            if (stepDelta > 0.0)
//            {
//                // restore last valid delta
//                delta = stepDelta;
//                stepDelta = -1.0;
//            }
//            else
//            {
//                if (delta > (t - current) && t > current)
//                {
//                    // save last valid delta and set exact step
//                    stepDelta = deltaOld;
//                    delta = t - current;
//                    good = 1;
//                }
//                else
//                {
//                    stepDelta = -1.0;
//                }
//            }
//            if (delta > deltaMax) delta = deltaMax;
//            if (delta < deltaMin) delta = deltaMin;
//        }
//    }

    stepDelta = -1;
    good = 1;

    // usual delta correction
    if (delta > 0.9 * deltaOld || good)   // accept current delta
    {
        nextStates ();
        rejected = 0;
#if STEPDEBUG
        logprint (LOG_STATUS,
                  "DEBUG: delta accepted at t = %.3e, h = %.3e\n",
                  (double) current, (double) delta);
#endif
    }
    else if (deltaOld > delta)   // reject current delta
    {
        rejected++;
        statRejected++;
#if STEPDEBUG
        logprint (LOG_STATUS,
                  "DEBUG: delta rejected at t = %.3e, h = %.3e\n",
                  (double) current, (double) delta);
#endif
        if (current > 0) current -= deltaOld;
    }
    else
    {
        nextStates ();
        rejected = 0;
    }
}

// asynchronous step solver
int e_trsolver::stepsolve_async(nr_double_t steptime)
{
    // Start to sweep through time.
    int error = 0;
    convError = 0;

    time = steptime;
    // update the interpolation time of any externally controlled
    // components which require it.
    updateExternalInterpTime(time);
    // make the stored histories for all ircuits that have
    // requested them at least as long as the next major time
    // step so we can reject the step later if needed and
    // restore all the histories to their previous state
    updateHistoryAges (time - lastasynctime);

    //delta = (steptime - time) / 10;
    //if (progress) logprogressbar (i, swp->getSize (), 40);
#if DEBUG && 0
    messagefcn (LOG_STATUS, "NOTIFY: %s: solving netlist for t = %e\n",
              getName (), (double) time);
#endif

    do
    {
#if STEPDEBUG
        if (delta == deltaMin)
        {
            messagefcn (LOG_ERROR,
                      "WARNING: %s: minimum delta h = %.3e at t = %.3e\n",
                      getName (), (double) delta, (double) current);
        }
#endif
        // update the integration coefficients
        updateCoefficients (delta);

        // Run predictor to get a start value for the solution vector for
        // the successive iterative corrector process
        error += predictor ();

        // restart Newton iteration
        if (rejected)
        {
            restartNR ();      // restart non-linear devices
            rejected = 0;
        }

        // Run corrector process with appropriate exception handling.
        // The corrector iterates through the solutions of the integration
        // process until a certain error tolerance has been reached.
        try_running () // #defined as:    do {
        {
            error += corrector ();
        }
        catch_exception () // #defined as:   } while (0); if (estack.top ()) switch (estack.top()->getCode ())
        {
        case EXCEPTION_NO_CONVERGENCE:
            pop_exception ();

            // Reduce step-size (by half) if failed to converge.
            if (current > 0) current -= delta;
            delta /= 2;
            if (delta <= deltaMin)
            {
                delta = deltaMin;
                adjustOrder (1);
            }
            if (current > 0) current += delta;

            // Update statistics.
            statRejected++;
            statConvergence++;
            rejected++;
            converged = 0;
            error = 0;

            // Start using damped Newton-Raphson.
            convHelper = CONV_SteepestDescent;
            convError = 2;
#if DEBUG
            messagefcn (LOG_ERROR, "WARNING: delta rejected at t = %.3e, h = %.3e "
                      "(no convergence)\n", (double) saveCurrent, (double) delta);
#endif
            break;
        default:
            // Otherwise return.
            estack.print ();
            error++;
            break;
        }
        if (error) return -1;
        if (rejected) continue;

        // check whether Jacobian matrix is still non-singular
        if (!A->isFinite ())
        {
            messagefcn (LOG_ERROR, "ERROR: %s: Jacobian singular at t = %.3e, "
                      "aborting %s analysis\n", getName (), (double) current,
			getDescription ().c_str());
            return -1;
        }

        // Update statistics and no more damped Newton-Raphson.
        statIterations += iterations;
        if (--convError < 0) convHelper = 0;

        // Now advance in time or not...
        if (running > 1)
        {
            adjustDelta (time);
            adjustOrder ();
        }
        else
        {
            fillStates ();
            nextStates ();
            rejected = 0;
        }

        saveCurrent = current;
        current += delta;
        running++;
        converged++;

        // Tell integrators to be running.
        setMode (MODE_NONE);

        // Initialize or update history.
        if (running > 1)
        {
            updateHistory (saveCurrent);
        }
        else
        {
            initHistory (saveCurrent);
        }
    }
    while (saveCurrent < time); // Hit a requested time point?

    return 0;
}

// accept an asynchronous step into the solution history
void e_trsolver::acceptstep_async(void)
{
    // copy the solution in case we wish to step back to this
    // point later
    copySolution (solution, lastsolution);

    // Store the time
    lastasynctime = time;

    // Store the deltas history
    saveState (dState, lastdeltas);

    // Store the current delta
    lastdelta = delta;
}

// reject the last asynchronous step and restore the history
// of solutions to the last major step
void e_trsolver::rejectstep_async(void)
{
    // restore the solution (node voltages and branch currents) from
    // the previously stored solution
    copySolution (lastsolution, solution);

    // Restore the circuit histories to their previous states
    truncateHistory (lastasynctime);

    // Restore the time deltas
    inputState (dState, lastdeltas);

    for (int i = 0; i < 8; i++)
    {
        deltas[i] = lastdeltas[i];
    }

    delta = lastdelta;

    // copy the deltas to all the circuit elements
    setDelta ();

    // reset the corrector and predictor coefficients
    calcCorrectorCoeff (corrType, corrOrder, corrCoeff, deltas);
    calcPredictorCoeff (predType, predOrder, predCoeff, deltas);
}

/* Makes a copy of a set of solution vectors */
void e_trsolver::copySolution (tvector<nr_double_t> * src[8], tvector<nr_double_t> * dest[8])
{
    for (int i = 0; i < 8; i++)
    {
        // check sizes are the same
        assert (src[i]->size () == dest[i]->size ());
        // copy over the data values
        for (unsigned int j = 0; j < src[i]->size (); j++)
        {
            dest[i]->set (j, src[i]->get (j));
        }
    }
}

void e_trsolver::updateHistoryAges (nr_double_t newage)
{
    int i = 0;
    circuit * root = subnet->getRoot ();
    for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ())
    {
        // set the history length to retain to be at least
        // the length of the supplied age
        if (c->hasHistory ())
        {
            c->setHistoryAge (std::max (initialhistages[i], newage));
            i++;
        }
    }
}

//int e_trsolver::finish()
//{
//    solve_post ();
//
//    if (progress) logprogressclear (40);
//
//    messagefcn (LOG_STATUS, "NOTIFY: %s: average time-step %g, %d rejections\n",
//              getName (), (double) (saveCurrent / statSteps), statRejected);
//    messagefcn (LOG_STATUS, "NOTIFY: %s: average NR-iterations %g, "
//              "%d non-convergences\n", getName (),
//              (double) statIterations / statSteps, statConvergence);
//
//    // cleanup
//    return 0;
//}


void e_trsolver::getsolution (double * lastsol)
{
    int N = countNodes ();
    int M = countVoltageSources ();

    // copy solution
    for (int r = 0; r < N + M; r++)
    {
        lastsol[r]  = real(x->get(r));
    }
}

/* getNodeV attempts to get the voltage of the node with a
   given name. returns -1 if the node name was not found */
int e_trsolver::getNodeV (char * label, nr_double_t& nodeV)
{
    int r = nlist->getNodeNr (label);

    if (r == -1)
    {
      return r;
    }
    else
    {
      nodeV = x->get(r);
      return 0;
    }
}

/* Get the voltage reported by a voltage probe */
int e_trsolver::getVProbeV (char * probename, nr_double_t& probeV)
{
    // string to hold the full name of the circuit
    std::string fullname;

    // check for NULL name
    if (probename)
    {
        circuit * root = subnet->getRoot ();
        for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ())
        {
            if (c->getType () == CIR_VPROBE) {

                fullname.clear ();

                // Subcircuit elements top level name is the
                // subcircuit type (the base name of the subcircuit
                // file)
                if (!c->getSubcircuit ().empty())
                {
                    fullname.append (c->getSubcircuit ());
                    fullname.append (".");
                }

                // append the user supplied name to search for
                fullname.append (probename);

                // Check if it is the desired voltage source
                if (strcmp (fullname.c_str(), c->getName ()) == 0)
                {
                    // Saves the real and imaginary voltages in the probe to the
                    // named variables Vr and Vi
                    c->saveOperatingPoints ();
                    // We are only interested in the real part for transient
                    // analysis
                    probeV = c->getOperatingPoint ("Vr");

                    return 0;
                }
            }
        }
    }
    return -1;
}

/* Get the current reported by a current probe */
int e_trsolver::getIProbeI (char * probename, nr_double_t& probeI)
{
    // string to hold the full name of the circuit
    std::string fullname;

    // check for NULL name
    if (probename)
    {
        circuit * root = subnet->getRoot ();
        for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ())
        {
            if (c->getType () == CIR_IPROBE) {

                fullname.clear ();

                // Subcircuit elements top level name is the
                // subcircuit type (the base name of the subcircuit
                // file)
                if (!c->getSubcircuit ().empty())
                {
                    fullname.append (c->getSubcircuit ());
                    fullname.append (".");
                }

                // append the user supplied name to search for
                fullname.append (probename);

                // Check if it is the desired voltage source
                if (strcmp (fullname.c_str(), c->getName ()) == 0)
                {
                    // Get the current reported by the probe
                    probeI = real (x->get (c->getVoltageSource () + getN ()));

                    return 0;
                }
            }
        }
    }
    return -1;
}

int e_trsolver::setECVSVoltage(char * ecvsname, nr_double_t V)
{
    // string to hold the full name of the circuit
    std::string fullname;

    // check for NULL name
    if (ecvsname)
    {
        circuit * root = subnet->getRoot ();
        for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ())
        {
            // examine only ECVS elements
            if (c->getType () == CIR_ECVS) {

                fullname.clear ();

                // Subcircuit elements top level name is the
                // subcircuit type (the base name of the subcircuit
                // file)
                if (!c->getSubcircuit ().empty())
                {
                    fullname.append (c->getSubcircuit ());
                    fullname.append (".");
                }

                // append the user supplied name to search for
                fullname.append (ecvsname);

                // Check if it is the desired voltage source
                if (strcmp (fullname.c_str(), c->getName ()) == 0)
                {
                    // Set the voltage to the desired value
                    c->setProperty("U", V);
                    return 0;
                }
            }
        }
    }
    return -1;
}

void e_trsolver::updateExternalInterpTime(nr_double_t t)
{
    circuit * root = subnet->getRoot ();
    for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ())
    {
        // examine only external elements that have interpolation,
        // such as ECVS elements
        if (c->getType () == CIR_ECVS) {
            // Set the voltage to the desired value
            c->setProperty ("Tnext", t);
            if (tHistory != NULL && tHistory->size () > 0)
            {
              c->setHistoryAge ( t - tHistory->last () + 0.1 * (t - tHistory->last ()) );
            }
        }
    }
}

/* The following function removed stored times newer than a specified time
   from all the circuit element histories */
void e_trsolver::truncateHistory (nr_double_t t)
{
    // truncate all the circuit element histories
    circuit * root = subnet->getRoot ();
    for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ())
    {
        if (c->hasHistory ()) c->truncateHistory (t);
    }
}

int e_trsolver::getJacRows()
{
    return A->getRows();
}

int e_trsolver::getJacCols()
{
    return A->getCols();
}

void e_trsolver::getJacData(int r, int c, nr_double_t& data)
{
    data = A->get(r,c);
}

// properties
PROP_REQ [] =
{
    PROP_NO_PROP
};
PROP_OPT [] =
{
    {
        "IntegrationMethod", PROP_STR, { PROP_NO_VAL, "Trapezoidal" },
        PROP_RNG_STR4 ("Euler", "Trapezoidal", "Gear", "AdamsMoulton")
    },
    { "Order", PROP_INT, { 2, PROP_NO_STR }, PROP_RNGII (1, 6) },
    { "InitialStep", PROP_REAL, { 1e-9, PROP_NO_STR }, PROP_POS_RANGE },
    { "MinStep", PROP_REAL, { 1e-16, PROP_NO_STR }, PROP_POS_RANGE },
    { "MaxStep", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGE },
    { "MaxIter", PROP_INT, { 150, PROP_NO_STR }, PROP_RNGII (2, 10000) },
    { "abstol", PROP_REAL, { 1e-12, PROP_NO_STR }, PROP_RNG_X01I },
    { "vntol", PROP_REAL, { 1e-6, PROP_NO_STR }, PROP_RNG_X01I },
    { "reltol", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_RNG_X01I },
    { "LTEabstol", PROP_REAL, { 1e-6, PROP_NO_STR }, PROP_RNG_X01I },
    { "LTEreltol", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_RNG_X01I },
    { "LTEfactor", PROP_REAL, { 1, PROP_NO_STR }, PROP_RNGII (1, 16) },
    { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
    { "Solver", PROP_STR, { PROP_NO_VAL, "CroutLU" }, PROP_RNG_SOL },
    { "relaxTSR", PROP_STR, { PROP_NO_VAL, "no" }, PROP_RNG_YESNO },
    { "initialDC", PROP_STR, { PROP_NO_VAL, "yes" }, PROP_RNG_YESNO },
    PROP_NO_PROP
};
struct define_t e_trsolver::anadef =
    { "ETR", 0, PROP_ACTION, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };


} // namespace qucs
