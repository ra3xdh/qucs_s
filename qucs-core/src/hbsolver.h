/*
 * hbsolver.h - harmonic balance solver class definitions
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

#ifndef __HBSOLVER_H__
#define __HBSOLVER_H__

#include <vector>

#include "ptrlist.h"
#include "tvector.h"

namespace qucs {

class vector;
class strlist;
class circuit;

class hbsolver : public analysis
{
 public:
  ACREATOR (hbsolver);
  hbsolver (char *);
  hbsolver (hbsolver &);
  ~hbsolver ();
  int  solve (void);
  void initHB (void);
  void initDC (void);
  static void calc (hbsolver *);
  void collectFrequencies (void);
  int  checkBalance (void);

  void splitCircuits (void);
  void expandFrequencies (nr_double_t, int);
  bool isExcitation (circuit *);
  strlist * circuitNodes (ptrlist<circuit>);
  void getNodeLists (void);
  int  assignVoltageSources (ptrlist<circuit>);
  int  assignNodes (ptrlist<circuit>, strlist *, int offset = 0);
  void prepareLinear (void);
  void createMatrixLinearA (void);
  void fillMatrixLinearA (tmatrix<nr_complex_t> *, int);
  void invertMatrix (tmatrix<nr_complex_t> *, tmatrix<nr_complex_t> *);
  void createMatrixLinearY (void);
  void saveResults (void);
  void calcConstantCurrent (void);
  nr_complex_t excitationZ (tvector<nr_complex_t> *, circuit *, int);
  void finalSolution (void);
  void fillMatrixNonLinear (tmatrix<nr_complex_t> *, tmatrix<nr_complex_t> *,
			    tvector<nr_complex_t> *, tvector<nr_complex_t> *,
			    tvector<nr_complex_t> *, tvector<nr_complex_t> *,
			    int);
  void prepareNonLinear (void);
  void solveHB (void);
  void loadMatrices (void);
  void VectorFFT (tvector<nr_complex_t> *, int isign = 1);
  void VectorIFFT (tvector<nr_complex_t> *, int isign = 1);
  int  calcOrder (int);
  void MatrixFFT (tmatrix<nr_complex_t> *);
  void calcJacobian (void);
  void solveVoltages (void);
  tvector<nr_complex_t> expandVector (tvector<nr_complex_t>, int);
  tmatrix<nr_complex_t> expandMatrix (tmatrix<nr_complex_t>, int);
  tmatrix<nr_complex_t> extendMatrixLinear (tmatrix<nr_complex_t>, int);
  void fillMatrixLinearExtended (tmatrix<nr_complex_t> *,
				 tvector<nr_complex_t> *);
  void saveNodeVoltages (circuit *, int);

 private:
  std::vector<nr_double_t> negfreqs;    // full frequency set
  std::vector<nr_double_t> posfreqs;    // full frequency set but positive
  std::vector<nr_double_t> rfreqs;      // real positive frequency set
  int * ndfreqs;                    // number of frequencies for each dimension
  std::vector<nr_double_t> dfreqs;      // base frequencies for each dimension
  nr_double_t frequency;
  strlist * nlnodes, * lnnodes, * banodes, * nanodes, * exnodes;
  ptrlist<circuit> excitations;
  ptrlist<circuit> nolcircuits;
  ptrlist<circuit> lincircuits;

  tmatrix<nr_complex_t> * Y;  // transadmittance matrix of linear network
  tmatrix<nr_complex_t> * A;  // MNA-matrix of linear network
  tmatrix<nr_complex_t> * Z;  // transimpedance matrix of linear network

  tmatrix<nr_complex_t> * YV; // linear transadmittance matrix
  tmatrix<nr_complex_t> * NA; // MNA-matrix of complete network

  tmatrix<nr_complex_t> * JQ; // C-Jacobian in t and f
  tmatrix<nr_complex_t> * JG; // G-Jacobian in t and f
  tmatrix<nr_complex_t> * JF; // full Jacobian for non-linear balancing
  tvector<nr_complex_t> * IG; // currents in t and f
  tvector<nr_complex_t> * FQ; // charges in t and f
  tvector<nr_complex_t> * VS;
  tvector<nr_complex_t> * VP;
  tvector<nr_complex_t> * FV; // error vector F(V) of HB equation
  tvector<nr_complex_t> * IL; // currents into linear network
  tvector<nr_complex_t> * IN; // currents into non-linear network

  tvector<nr_complex_t> * IR;
  tvector<nr_complex_t> * QR;
  tvector<nr_complex_t> * RH;
  tvector<nr_complex_t> * OM;

  tvector<nr_complex_t> * IC; // source currents into balanced nodes
  tvector<nr_complex_t> * IS; // currents through sources themselves
  tvector<nr_complex_t> * x;
  tvector<nr_complex_t> * vs;

  int runs;
  int lnfreqs;
  int nlfreqs;
  int nnlvsrcs;
  int nlnvsrcs;
  //int nlnnodes;
  int nnanodes;
  int nexnodes;
  int nbanodes;
};

} // namespace qucs

#endif /* __HBSOLVER_H__ */
