/*
 * analysis.h - analysis class definitions
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

/*! \file analysis.h
 * \brief The analysis class header file.
 *
 * Contains the analysis class definition.
 */

#ifndef __ANALYSIS_H__
#define __ANALYSIS_H__

#include "object.h"
#include "ptrlist.h"

#define SAVE_OPS 1 // save operating points
#define SAVE_ALL 2 // also save subcircuit nodes and operating points
#define SAVE_CVS 4 // save characteristic values

#define ACREATOR(val) \
  val (); \
  static analysis * create (void) { return new val (); } \
  static struct define_t anadef; \
  static struct define_t * definition (void) { return &anadef; }

namespace qucs {

class dataset;
class net;
class environment;
class sweep;
class vector;

/*! \enum analysis_type
 * \brief enumerates the analysis types available.
 *
 * Enumerates the analysis types available.
 *
 */
enum analysis_type
{
    ANALYSIS_UNKNOWN = -1,
    ANALYSIS_SWEEP,
    ANALYSIS_DC,
    ANALYSIS_AC,
    ANALYSIS_HBALANCE,
    ANALYSIS_TRANSIENT,
    ANALYSIS_SPARAMETER,
    ANALYSIS_E_TRANSIENT
};

/*! \class analysis
 * \brief class for performing circuit analyses.
 *
 * This class is used for performing all the anlyses specified in
 * a circuit description. The actual solver classes for specific
 * analysis types inheirit from this class and override it's
 * methods.
 *
 */
class analysis : public object
{
public:

    /*! \fn analysis
    * \brief Constructor (Unnamed)
    *
    * Constructor. Creates an unnamed instance of the analysis class.
    */
    analysis ();

    /*! \fn analysis
    * \brief Constructor (Named)
    *
    * Constructor. Creates a named instance of the analysis class.
    */
    analysis (const std::string &);

    /*! \fn analysis
    * \brief Copy Constructor
    *
    * Constructor. Creates an instance of the analysis class
    * from an existing analysis object.
    */
    analysis (analysis &);

    /*! \fn ~analysis
    * \brief Destructor
    *
    * Destructor. Destroys an analysis object.
    */
    virtual ~analysis ();

    /*! \fn solve
    * \brief placeholder for solution function
    *
    * Virtual solution function intended to be overridden by the
    * inheiriting class's solution function.
    */
    virtual int solve (void)
    {
        return 0;
    }

    /*! \fn initialize
    * \brief placeholder for initialization function
    *
    * Virtual initialize function intended to be overridden by the
    * inheiriting class's initialization function.
    */
    virtual int initialize (void)
    {
        return 0;
    }

    /*! \fn cleanup
    * \brief placeholder for cleanup function
    *
    * Virtual cleanup function intended to be overridden by the
    * inheiriting class's cleanup function.
    */
    virtual int cleanup (void)
    {
        return 0;
    }

    /*! \fn isExternal
    * \brief informs whether this is an external sim
    *
    * External simulations will be ignored by qucsator. This
    * function is used to determine whether the analysis is
    * external or not.
    */
    virtual bool isExternal (void)
    {
        return false;
    }

    dataset * getData (void)
    {
        return data;
    }

    void setData (dataset * d)
    {
        data = d;
    }

    net * getNet (void)
    {
        return subnet;
    }

    void setNet (net * netlist)
    {
        subnet = netlist;
    }

    environment * getEnv (void)
    {
        return env;
    }

    void setEnv (environment * e)
    {
        env = e;
    }

    ptrlist<analysis> * getAnalysis (void)
    {
        return actions;
    }

    void setAnalysis (ptrlist<analysis> * a)
    {
        actions = a;
    }

    /*! \fn addAnalysis
     * \param analysis pointer to existing analysis object
     *
     * Adds the given analysis to the actions being
     * associated with the current analysis object.
     */
    void addAnalysis (analysis *);

    /*! \fn delAnalysis
     * \param analysis pointer to existing analysis object
     *
     * Deletes the given analysis from the actions being associated
     * with the current analysis object.
     */
    void delAnalysis (analysis *);

    int  getType (void)
    {
        return type;
    }

    void setType (int t)
    {
        type = t;
    }

    /*! \fn createSweep
     * \brief create a named sweep object
     * \param n pointer to a character array contining the name
     * of the sweep to be created
     * \param sweep pointer to the created sweep object
     *
     * Creates a named sweep object depending on the analysis's properties.
     * Supported sweep types are: linear, logarithmic, lists and constants.
     *
     */
    sweep * createSweep (const std::string &);

    /*! \fn saveVariable
     * \brief Save variable into analysis dataset.
     * \param n Name of the variable
     * \param z The variable to be added to the dataset
     * \param f
     *
     * Saves the given variable into the dataset associated with the
     * analysis.  Creates the dataset vector if necessary.
     */
    void saveVariable (const std::string &, nr_complex_t, qucs::vector *);

    /*! \fn getProgress
     * \brief get
     * \param progress
     *
     */
    bool getProgress (void)
    {
        return progress;
    }

    /*! \fn setProgress
     * \brief Sets the progress
     * \param p new value of the progress
     *
     * Sets the progress.
     */
    void setProgress (bool p)
    {
        progress = p;
    }

protected:
    int runs;
    int type;
    net * subnet;
    dataset * data;
    environment * env;
    ptrlist<analysis> * actions;
    bool progress;
};

} // namespace qucs

#endif /* __ANALYSIS_H__ */
