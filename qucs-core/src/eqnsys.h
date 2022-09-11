/*
 * eqnsys.h - equations system solver class definitions
 *
 * Copyright (C) 2004, 2005, 2006 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __EQNSYS_H__
#define __EQNSYS_H__

#include <limits>

//! Definition of equation system solving algorithms.
enum algo_type {
  ALGO_INVERSE                    = 0x0001,
  ALGO_GAUSS                      = 0x0002,
  ALGO_GAUSS_JORDAN               = 0x0004,
  ALGO_LU_FACTORIZATION_CROUT     = 0x0008,
  ALGO_LU_FACTORIZATION_DOOLITTLE = 0x0010,
  ALGO_LU_SUBSTITUTION_CROUT      = 0x0020,
  ALGO_LU_SUBSTITUTION_DOOLITTLE  = 0x0040,
  ALGO_LU_DECOMPOSITION           = 0x0028,
  ALGO_LU_DECOMPOSITION_CROUT     = 0x0028,
  ALGO_LU_DECOMPOSITION_DOOLITTLE = 0x0050,
  ALGO_JACOBI                     = 0x0080,
  ALGO_GAUSS_SEIDEL               = 0x0100,
  ALGO_SOR                        = 0x0200,
  ALGO_QR_DECOMPOSITION           = 0x0400,
  ALGO_QR_DECOMPOSITION_LS        = 0x0800,
  ALGO_SV_DECOMPOSITION           = 0x1000,
  // testing
  ALGO_QR_DECOMPOSITION_2         = 0x2000,
};

//! Definition of pivoting strategies.
enum pivot_type {
  PIVOT_NONE    = 0x01,
  PIVOT_PARTIAL = 0x02,
  PIVOT_FULL    = 0x04
};

#include "tvector.h"
#include "tmatrix.h"

namespace qucs {

template <class nr_type_t>
class eqnsys
{
 public:
  eqnsys ();
  eqnsys (eqnsys &);
  ~eqnsys ();
  void setAlgo (int a) { algo = a; }
  int  getAlgo (void) { return algo; }
  void passEquationSys (tmatrix<nr_type_t> *, tvector<nr_type_t> *,
			tvector<nr_type_t> *);
  void solve (void);

 private:
  int update;
  int algo;
  int pivoting;
  int * rMap;
  int * cMap;
  int N;
  nr_double_t * nPvt;

  tmatrix<nr_type_t> * A;
  tmatrix<nr_type_t> * V;
  tvector<nr_type_t> * B;
  tvector<nr_type_t> * X;
  tvector<nr_type_t> * R;
  tvector<nr_type_t> * T;
  tvector<nr_double_t> * S;
  tvector<nr_double_t> * E;

  void solve_inverse (void);
  void solve_gauss (void);
  void solve_gauss_jordan (void);
  void solve_lu_crout (void);
  void solve_lu_doolittle (void);
  void factorize_lu_crout (void);
  void factorize_lu_doolittle (void);
  void substitute_lu_crout (void);
  void substitute_lu_doolittle (void);
  void solve_qr (void);
  void solve_qr_ls (void);
  void solve_qrh (void);
  void factorize_qrh (void);
  void substitute_qrh (void);
  void factorize_qr_householder (void);
  void substitute_qr_householder (void);
  void substitute_qr_householder_ls (void);
  nr_type_t householder_create_left (int);
  void householder_apply_left (int, nr_type_t);
  nr_type_t householder_left (int);
  nr_type_t householder_create_right (int);
  void householder_apply_right (int, nr_type_t);
  void householder_apply_right_extern (int, nr_type_t);
  nr_type_t householder_right (int);
  nr_double_t euclidian_c (int, int r = 1);
  nr_double_t euclidian_r (int, int c = 1);
  void givens_apply_u (int, int, nr_double_t, nr_double_t);
  void givens_apply_v (int, int, nr_double_t, nr_double_t);
  void solve_svd (void);
  void chop_svd (void);
  void factorize_svd (void);
  void substitute_svd (void);
  void diagonalize_svd (void);
  void solve_iterative (void);
  void solve_sor (void);
  nr_double_t convergence_criteria (void);
  void ensure_diagonal (void);
  void ensure_diagonal_MNA (void);
  int countPairs (int, int&, int&);
  void preconditioner (void);
};

} // namespace qucs

#include "eqnsys.cpp"

#endif /* __EQNSYS_H__ */
