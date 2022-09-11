/*
 * e_trsolver.h - transient solver external interface class definitions
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

/** \file e_trsolver.h
  * \brief The externally controlled trsolver external class header file.
  *
  */

/**
  * \ingroup QucsInterface
  */

#ifndef __E_TRSOLVER_H__
#define __E_TRSOLVER_H__

#include "qucs_interface.h"
#include "trsolver.h"
#include <vector>

namespace qucs {

/**
  * \class e_trsolver
  *
  * \ingroup QucsInterface
  *
  * \brief External interface class for transient simulation
  *
  * This class provides access to the transient solving functionality
  * of Qucs for external software.
  *
  *
  */
class e_trsolver : public trsolver
{
public:
    ACREATOR (e_trsolver);
    //e_trsolver ();
    e_trsolver (char *);
    e_trsolver (e_trsolver &);
    ~e_trsolver ();

    int init (nr_double_t, nr_double_t, int);
//    int finish ();
    bool isExternal() { return true; };

public:

    int stepsolve_sync (nr_double_t synctime);
    void acceptstep_sync (void);
    int stepsolve_async (nr_double_t steptime);
    void acceptstep_async (void);
    void rejectstep_async (void);
    void getsolution (double *);

    /** \brief Sets the voltage of an exterally controlled voltage source.
      * \param ecvsname The name of the voltage source to be set.
      * \param V New value of the voltage.
      * \return Integer flag reporting success or failure.
      *
      * This method searches for the ecvs component with the given
      * name in \a ecvsname. If the ecvs exists, the value of the voltage
      * to be set at the next time step is set to the supplied value in
      * \a V.
      *
      * You can set the voltage of ecvs components in subcircuits created
      * by using a name up of the subcircuit hierarchy it is in, e.g. the
      * name
      *
      * SUBtop.SUBlower.ECVS1
      *
      * to set the voltage of an ecvs named ECVS1 which is located in
      * subcircuit SUBlower, which is itself in subcircuit SUBtop in
      * the top level circuit.
      *
      * Returns 0 if the ecvs was found, and -1 otherwise.
      */
    int setECVSVoltage(char * ecvsname, nr_double_t V);

    /// Returns the number of rows in the Jacobian matrix for the circuit
    int getJacRows ();

    /// Returns the number of columns in the Jacobian matrix for the circuit
    int getJacCols ();

    /** \brief Obtains the data from the Jacobian matrix for the circuit
      * \param r Pointer to character array containing the name of the voltage
      * \param c Reference to nr_double_t in which the node voltage will be returned
      * \return The Jacobian matrix data at the specified location.
      *
      */
    void getJacData (int r, int c, nr_double_t& data);

    /** \brief Obtains the voltage of a node by name
      * \param label Pointer to character array containing the name of the voltage
      * \param nodeV Reference to nr_double_t in which the node voltage will be returned
      * \return Integer flag reporting success or failure
      *
      * This method searches for the node with the given name in \a label.
      * If the node exists, the value of the voltage at that node is
      * copied to \a nodeV. Returns 0 if the node was found, and -1
      * otherwise.
      */
    int getNodeV (char * label, nr_double_t& nodeV);

    /** \brief Obtains the voltage reported by a voltage probe
      * \param probename Pointer to character array containing the name of the probe
      * \param probeV Reference to nr_double_t in which the node voltage will be returned
      * \return Integer flag reporting success or failure
      *
      * This method searches for the voltage probe with the given name
      * in \a probename. If the probe exists, the value of the probe voltage
      * is copied to \a probeV. Returns 0 if the probe with the given name
      * was found and -1 otherwise.
      */
    int getVProbeV (char * probename, nr_double_t& probeV);

    /** \brief Obtains the current reported by a current probe
      * \param probename Pointer to character array containing the name of the probe
      * \param probeI Reference to nr_double_t in which the current will be returned
      * \return Integer flag reporting success or failure
      *
      * This method searches for the current probe with the given name
      * in \a probename. If the probe exists, the value of the probe current
      * is copied to \a probeI. Returns 0 if the probe with the given name
      * was found and -1 otherwise.
      */
    int getIProbeI (char * probename, nr_double_t& probeI);

    // debugging functions
    void debug (void);
    void printx (void);

    /** \var void (*messagefcn)(int level, const char * format, ...);
      * \brief Pointer to function used to print messages during a sim
      * \param level Inter flag indicating the log level
      * \param format Formatting string in the same format as printf
      * \param ... Additional arguments to the printing function
      *
      * By default this points to the standard Qucs logprint function
      * (found in logging.c) on creation of an e_trsolver instance.
      * The log \a level flag indicates whether the message is a status
      * update or an error message. \a format contains the formatting
      * string in the same style as printf (when using logprint). The
      * additional arguments are used by logprint in the supplied
      * formatting string.
      */
    void (*messagefcn)(int level, const char * format, ...);

private:

    int MaxIterations;
    nr_double_t reltol;
    nr_double_t abstol;
    nr_double_t vntol;
    nr_double_t time;
    nr_double_t saveCurrent;
    nr_double_t lastsynctime;
    int running;
    int rejected;
    int convError;

    void initETR (nr_double_t start, nr_double_t, int);
    void truncateHistory (nr_double_t);
    void updateExternalInterpTime(nr_double_t);
    void storeHistoryAges (void);
    void updateHistoryAges(nr_double_t);

//    int solve_nonlinear_step (void);
    void adjustDelta_sync (nr_double_t);

    // Asynchronous specific items

    // For going back in history of a solution after multiple
    // solution steps
    std::vector<nr_double_t> initialhistages;
    tvector<nr_double_t> * lastsolution[8];
    nr_double_t lastasynctime;
    nr_double_t lastdeltas[8];
    nr_double_t lastdelta;
    // restores the solver to an earlier stored state, used
    // when an asynchronous major step is rejected by the
    // parent process
    void restoreSolution (void);
    void copySolution (tvector<nr_double_t> * [8], tvector<nr_double_t> * [8]);
    void fillLastSolution (tvector<nr_double_t> * );
};

} // namespace qucs

#endif /* __e_trsolver_H__ */

