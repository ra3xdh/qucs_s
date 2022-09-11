/*
 * nasolver.h - nodal analysis solver class definitions
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

#ifndef __NASOLVER_H__
#define __NASOLVER_H__

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
// BUG
#include "qucs_typedefs.h"
#endif
#include "tvector.h"
#include "tmatrix.h"
#include "eqnsys.h"
#include "nasolution.h"
#include "analysis.h"

// Convergence helper definitions.
#define CONV_None            0
#define CONV_Attenuation     1
#define CONV_LineSearch      2
#define CONV_SteepestDescent 3
#define CONV_GMinStepping    4
#define CONV_SourceStepping  5

namespace qucs {

class analysis;
class circuit;
class nodelist;
class vector;

template <class nr_type_t>
class nasolver : public analysis
{
public:
    nasolver ();
    nasolver (const std::string&);
    nasolver (nasolver &);
    ~nasolver ();
    int  solve_once (void);
    int  solve_nonlinear (void);
    int  solve_nonlinear_continuation_gMin (void);
    int  solve_nonlinear_continuation_Source (void);
    int  solve_linear (void);
    void solve_pre (void);
    void solve_post (void);
    void setDescription (const std::string &n) { desc = n; }
    std::string getDescription (void) const { return desc; }
    void saveResults (const std::string &, const std::string &, int, qucs::vector * f = NULL);
    typedef void (* calculate_func_t) (nasolver<nr_type_t> *);
    void setCalculation (calculate_func_t f) { calculate_func = f; }
    void calculate (void)
    {
        if (calculate_func) (*calculate_func) (this);
    }
    const char * getHelperDescription (void);

    //interface convenience functions
    /// Returns the number of node voltages in the circuit.
    int getN ();
    /// Returns the number of branch currents in the circuit.
    int getM ();

protected:
    void restartNR (void);
    void savePreviousIteration (void);
    void restorePreviousIteration (void);
    int  countNodes (void);
    int  getNodeNr (const std::string &);
    int  findAssignedNode (circuit *, int);
    int  countVoltageSources (void);
    void saveSolution (void);
    circuit * findVoltageSource (int);
    void applyNodeset (bool nokeep = true);
    void createNoiseMatrix (void);
    void runMNA (void);
    void createMatrix (void);
    void storeSolution (void);
    void recallSolution (void);
    int  checkConvergence (void);

private:
    void assignVoltageSources (void);
    void createGMatrix (void);
    void createBMatrix (void);
    void createCMatrix (void);
    void createDMatrix (void);
    void createIVector (void);
    void createEVector (void);
    void createZVector (void);
    void applyAttenuation (void);
    void lineSearch (void);
    void steepestDescent (void);
    std::string createV (int, const std::string&, int);
    std::string createI (int, const std::string&, int);
    std::string createOP (const std::string&, const std::string &);
    void saveNodeVoltages (void);
    void saveBranchCurrents (void);
    nr_type_t MatValX (nr_complex_t, nr_complex_t *);
    nr_type_t MatValX (nr_complex_t, nr_double_t *);

protected:
    tvector<nr_type_t> * z;
    tvector<nr_type_t> * x;
    tvector<nr_type_t> * xprev;
    tvector<nr_type_t> * zprev;
    tmatrix<nr_type_t> * A;
    tmatrix<nr_type_t> * C;
    int iterations;
    int convHelper;
    int fixpoint;
    int eqnAlgo;
    int updateMatrix;
    nr_double_t gMin, srcFactor;
    std::string desc;
    nodelist * nlist;

private:
    eqnsys<nr_type_t> * eqns;
    nr_double_t reltol;
    nr_double_t abstol;
    nr_double_t vntol;
    nasolution<nr_type_t> solution;

private:

    calculate_func_t calculate_func;
};

} // namespace qucs

#include "nasolver.cpp"

#endif /* __NASOLVER_H__ */
