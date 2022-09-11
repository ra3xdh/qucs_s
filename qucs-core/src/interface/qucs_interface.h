/*
 * qucs_interface.h - m-code interface class definitions
 *
 * Copyright (C) 2003, 2004, 2005, 2006, 2007 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: analysis.h 1869 2013-03-06 12:50:21Z crobarcro $
 *
 */

/** \file qucs_interface.h
  * \brief The generic external interface class header file.
  *
  * Contains the m-code interface class definition.
  */

/**
  * \defgroup QucsInterface
  */

#ifndef __QUCS_INTERFACE_H__
#define __QUCS_INTERFACE_H__


namespace qucs
{


enum QUCS_ERROR_CODES { NETLIST_OK,
                        NETLIST_FILE_NOT_FOUND,
                        NETLIST_FAILED_CHECK };

// forward declarations of some classes to avoid including
// header files and speed compilation
class net;
class input;
class circuit;
class dataset;
class environment;
class e_trsolver;

/** \class qucsint
  * \brief superclass for interfacing to the Qucs circuit solvers.
  *
  * \ingroup QucsInterface
  *
  * This class is used for interfacing qucs with external software
  *
  */
class qucsint
{

public:
    qucsint ();
    qucsint (char* infile);
    ~qucsint ();

    int prepare_netlist (char* infile);
    int evaluate ();
    int output (char* outfile);

protected:

    qucs::net * subnet;
    qucs::input * in;
    qucs::circuit * gnd;
    qucs::dataset * out;
    qucs::environment * root;
    int err;
    int ret;

};


/*/////////////////////////////////////////////////////////////////////////////

                            trsolver_interface

/////////////////////////////////////////////////////////////////////////////*/


enum ETR_MODE { ETR_MODE_ASYNC, ETR_MODE_SYNC };

/** \class trsolver_interface
  * \brief subclass for interfacing to the Qucs transient circuit solvers.
  *
  * \ingroup QucsInterface
  *
  * This class is used for interfacing the qucs transient circuit
  * solver with external software.
  *
  */
class trsolver_interface : public qucsint
{
public:
    trsolver_interface ();
    trsolver_interface (char* infile);
//    ~trsolver_interface ();

    void getETR(void);
    int init (double, double, int);
    bool getIsInitialised() { return isInitialised; };
    int stepsolve_sync (double synctime);
    void acceptstep_sync (void);
    int stepsolve_async (double steptime);
    void acceptstep_async (void);
    void rejectstep_async (void);
    void getsolution (double *);

    /// Returns the number of node voltages in the circuit.
    int getN ();

    /// Returns the number of branch currents in the circuit.
    int getM ();

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
    int setECVSVoltage(char * ecvsname, double V);

    /// Returns the number of rows in the Jacobian matrix for the circuit
    int getJacRows ();

    /// Returns the number of columns in the Jacobian matrix for the circuit
    int getJacCols ();

    /** \brief Obtains the data from the Jacobian matrix for the circuit
      * \param r Pointer to character array containing the name of the voltage
      * \param c Reference to double in which the node voltage will be returned
      * \return The Jacobian matrix data at the specified location.
      *
      */
    int getJacData (int r, int c, double& data);

    /** \brief Obtains the voltage of a node by name
      * \param label Pointer to character array containing the name of the voltage
      * \param nodeV Reference to double in which the node voltage will be returned
      * \return Integer flag reporting success or failure
      *
      * This method searches for the node with the given name in \a label.
      * If the node exists, the value of the voltage at that node is
      * copied to \a nodeV. Returns 0 if the node was found, and -1
      * otherwise.
      */
    int getNodeV (char * label, double& nodeV);

    /** \brief Obtains the voltage reported by a voltage probe
      * \param probename Pointer to character array containing the name of the probe
      * \param probeV Reference to double in which the node voltage will be returned
      * \return Integer flag reporting success or failure
      *
      * This method searches for the voltage probe with the given name
      * in \a probename. If the probe exists, the value of the probe voltage
      * is copied to \a probeV. Returns 0 if the probe with the given name
      * was found and -1 otherwise.
      */
    int getVProbeV (char * probename, double& probeV);

    /** \brief Obtains the current reported by a current probe
      * \param probename Pointer to character array containing the name of the probe
      * \param probeI Reference to double in which the current will be returned
      * \return Integer flag reporting success or failure
      *
      * This method searches for the current probe with the given name
      * in \a probename. If the probe exists, the value of the probe current
      * is copied to \a probeI. Returns 0 if the probe with the given name
      * was found and -1 otherwise.
      */
    int getIProbeI (char * probename, double& probeI);

    /** \brief Sets pointer to function used to print messages during a sim
      * \param printing function to be used by e_trsolver
      *
      * The printing function can be replace with a function with
      * signature
      *
      *   (*messagefcn)(int level, const char * format, ...);
      *
      * By default this points to the standard Qucs logprint function
      * (found in logging.c) on creation of an e_trsolver instance.
      * The log \a level flag indicates whether the message is a status
      * update or an error message. \a format contains the formatting
      * string in the same style as printf (when using logprint). The
      * additional arguments are used by logprint in the supplied
      * formatting string.
      */
    void setMessageFcn (void (*newmessagefcn)(int level, const char * format, ...));

    // debugging functions
//    void debug (void);
    /** \brief Prints the histor of solutions available to the integrators at the current time step
      *
      * The history of soliutions used in the integration are printed
      * to the command line by this function. Primarily useful for
      * debugging purposes.
      */
    void printSolution (void);

private:

    bool isInitialised;
    /// Pointer to etr sim
    qucs::e_trsolver * etr;

};

} // namespace qucs

#endif /* __QUCS_INTERFACE_H__ */
