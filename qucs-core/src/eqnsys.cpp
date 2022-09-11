/*
 * eqnsys.cpp - equation system solver class implementation
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

#include <assert.h>
#include <time.h>
#include <cmath>
#include <float.h>

#include <limits>

#include "compat.h"
#include "logging.h"
#include "precision.h"
#include "complex.h"
#include "tmatrix.h"
#include "eqnsys.h"
#include "exception.h"
#include "exceptionstack.h"

//! Little helper macro.
#define Swap(type,a,b) { type t; t = a; a = b; b = t; }

namespace qucs {

//! Constructor creates an unnamed instance of the eqnsys class.
template <class nr_type_t>
eqnsys<nr_type_t>::eqnsys () {
  A = V = NULL;
  B = X = NULL;
  S = E = NULL;
  T = R = NULL;
  nPvt = NULL;
  cMap = rMap = NULL;
  update = 1;
  pivoting = PIVOT_PARTIAL;
  N = 0;
}

//! Destructor deletes the eqnsys class object.
template <class nr_type_t>
eqnsys<nr_type_t>::~eqnsys () {
  delete R;
  delete T;
  delete B;
  delete S;
  delete E;
  delete V;
  delete[] rMap;
  delete[] cMap;
  delete[] nPvt;
}

/*! The copy constructor creates a new instance of the eqnsys class
   based on the given eqnsys object. */
template <class nr_type_t>
eqnsys<nr_type_t>::eqnsys (eqnsys & e) {
  A = e.A;
  V = NULL;
  S = E = NULL;
  T = R = NULL;
  B = e.B ? new tvector<nr_type_t> (*(e.B)) : NULL;
  cMap = rMap = NULL;
  nPvt = NULL;
  update = 1;
  X = e.X;
  N = 0;
}

/*! With this function the describing matrices for the equation system
   is passed to the equation system solver class.  Matrix A is the
   left hand side of the equation system and B the right hand side
   vector.  The reference pointer to the X vector is going to be the
   solution vector.  The B vector will be locally copied. */
template <class nr_type_t>
void eqnsys<nr_type_t>::passEquationSys (tmatrix<nr_type_t> * nA,
					 tvector<nr_type_t> * refX,
					 tvector<nr_type_t> * nB) {
  if (nA != NULL) {
    A = nA;
    update = 1;
    if (N != A->getCols ()) {
      N = A->getCols ();
      delete[] cMap; cMap = new int[N];
      delete[] rMap; rMap = new int[N];
      delete[] nPvt; nPvt = new nr_double_t[N];
    }
  }
  else {
    update = 0;
  }
  delete B;
  B = new tvector<nr_type_t> (*nB);
  X = refX;
}

/*! Depending on the algorithm applied to the equation system solver
   the function stores the solution of the system into the matrix
   pointed to by the X matrix reference. */
template <class nr_type_t>
void eqnsys<nr_type_t>::solve (void) {
#if DEBUG && 0
  time_t t = time (NULL);
#endif
  switch (algo) {
  case ALGO_INVERSE:
    solve_inverse ();
    break;
  case ALGO_GAUSS:
    solve_gauss ();
    break;
  case ALGO_GAUSS_JORDAN:
    solve_gauss_jordan ();
    break;
  case ALGO_LU_DECOMPOSITION_CROUT:
    solve_lu_crout ();
    break;
  case ALGO_LU_DECOMPOSITION_DOOLITTLE:
    solve_lu_doolittle ();
    break;
  case ALGO_LU_FACTORIZATION_CROUT:
    factorize_lu_crout ();
    break;
  case ALGO_LU_FACTORIZATION_DOOLITTLE:
    factorize_lu_doolittle ();
    break;
  case ALGO_LU_SUBSTITUTION_CROUT:
    substitute_lu_crout ();
    break;
  case ALGO_LU_SUBSTITUTION_DOOLITTLE:
    substitute_lu_doolittle ();
    break;
  case ALGO_JACOBI: case ALGO_GAUSS_SEIDEL:
    solve_iterative ();
    break;
  case ALGO_SOR:
    solve_sor ();
    break;
  case ALGO_QR_DECOMPOSITION:
    solve_qr ();
    break;
  case ALGO_QR_DECOMPOSITION_LS:
    solve_qr_ls ();
    break;
  case ALGO_SV_DECOMPOSITION:
    solve_svd ();
    break;
  case ALGO_QR_DECOMPOSITION_2:
    solve_qrh ();
    break;
  }
#if DEBUG && 0
  logprint (LOG_STATUS, "NOTIFY: %dx%d eqnsys solved in %ld seconds\n",
	    A->getRows (), A->getCols (), time (NULL) - t);
#endif
}

/*! Simple matrix inversion is used to solve the equation system. */
template <class nr_type_t>
void eqnsys<nr_type_t>::solve_inverse (void) {
  *X = inverse (*A) * *B;
}

#define A_ (*A)
#define X_ (*X)
#define B_ (*B)

/*! The function solves the equation system applying Gaussian
   elimination with full column pivoting only (no row pivoting). */
template <class nr_type_t>
void eqnsys<nr_type_t>::solve_gauss (void) {
  nr_double_t MaxPivot;
  nr_type_t f;
  int i, c, r, pivot;

  // triangulate the matrix
  for (i = 0; i < N; i++) {
    // find maximum column value for pivoting
    for (MaxPivot = 0, pivot = r = i; r < N; r++) {
      if (abs (A_(r, i)) > MaxPivot) {
	MaxPivot = abs (A_(r, i));
	pivot = r;
      }
    }
    // exchange rows if necessary
    assert (MaxPivot != 0);
    if (i != pivot) {
      A->exchangeRows (i, pivot);
      B->exchangeRows (i, pivot);
    }
    // compute new rows and columns
    for (r = i + 1; r < N; r++) {
      f = A_(r, i) / A_(i, i);
      for (c = i + 1; c < N; c++) A_(r, c) -= f * A_(i, c);
      B_(r) -= f * B_(i);
    }
  }

  // backward substitution
  for (i = N - 1; i >= 0; i--) {
    f = B_(i);
    for (c = i + 1; c < N; c++) f -= A_(i, c) * X_(c);
    X_(i) = f / A_(i, i);
  }
}

/*! The function solves the equation system applying a modified
   Gaussian elimination with full column pivoting only (no row
   pivoting). */
template <class nr_type_t>
void eqnsys<nr_type_t>::solve_gauss_jordan (void) {
  nr_double_t MaxPivot;
  nr_type_t f;
  int i, c, r, pivot;

  // create the eye matrix
  for (i = 0; i < N; i++) {
    // find maximum column value for pivoting
    for (MaxPivot = 0, pivot = r = i; r < N; r++) {
      if (abs (A_(r, i)) > MaxPivot) {
	MaxPivot = abs (A_(r, i));
	pivot = r;
      }
    }
    // exchange rows if necessary
    assert (MaxPivot != 0);
    if (i != pivot) {
      A->exchangeRows (i, pivot);
      B->exchangeRows (i, pivot);
    }

    // compute current row
    f = A_(i, i);
    for (c = i + 1; c < N; c++) A_(i, c) /= f;
    B_(i) /= f;

    // compute new rows and columns
    for (r = 0; r < N; r++) {
      if (r != i) {
	f = A_(r, i);
        for (c = i + 1; c < N; c++) A_(r, c) -= f * A_(i, c);
        B_(r) -= f * B_(i);
      }
    }
  }

  // right hand side is now the solution
  *X = *B;
}

#define LU_FAILURE 0
#define VIRTUAL_RES(txt,i) {					  \
  qucs::exception * e = new qucs::exception (EXCEPTION_SINGULAR); \
  e->setText (txt);						  \
  e->setData (rMap[i]);						  \
  A_(i, i) = NR_TINY; /* virtual resistance to ground */	  \
  throw_exception (e); }

/*! The function uses LU decomposition and the appropriate forward and
   backward substitutions in order to solve the linear equation
   system.  It is very useful when dealing with equation systems where
   the left hand side (the A matrix) does not change but the right
   hand side (the B vector) only. */
template <class nr_type_t>
void eqnsys<nr_type_t>::solve_lu_crout (void) {

  // skip decomposition if requested
  if (update) {
    // perform LU composition
    factorize_lu_crout ();
  }

  // finally solve the equation system
  substitute_lu_crout ();
}

/*! The other LU decomposition. */
template <class nr_type_t>
void eqnsys<nr_type_t>::solve_lu_doolittle (void) {

  // skip decomposition if requested
  if (update) {
    // perform LU composition
    factorize_lu_doolittle ();
  }

  // finally solve the equation system
  substitute_lu_doolittle ();
}

/*! This function decomposes the left hand matrix into an upper U and
   lower L matrix.  The algorithm is called LU decomposition (Crout's
   definition).  The function performs the actual LU decomposition of
   the matrix A using (implicit) partial row pivoting. */
template <class nr_type_t>
void eqnsys<nr_type_t>::factorize_lu_crout (void) {
  nr_double_t d, MaxPivot;
  nr_type_t f;
  int k, c, r, pivot;

  // initialize pivot exchange table
  for (r = 0; r < N; r++) {
    for (MaxPivot = 0, c = 0; c < N; c++)
      if ((d = abs (A_(r, c))) > MaxPivot)
	MaxPivot = d;
    if (MaxPivot <= 0) MaxPivot = NR_TINY;
    nPvt[r] = 1 / MaxPivot;
    rMap[r] = r;
  }

  // decompose the matrix into L (lower) and U (upper) matrix
  for (c = 0; c < N; c++) {
    // upper matrix entries
    for (r = 0; r < c; r++) {
      f = A_(r, c);
      for (k = 0; k < r; k++) f -= A_(r, k) * A_(k, c);
      A_(r, c) = f / A_(r, r);
    }
    // lower matrix entries
    for (MaxPivot = 0, pivot = r; r < N; r++) {
      f = A_(r, c);
      for (k = 0; k < c; k++) f -= A_(r, k) * A_(k, c);
      A_(r, c) = f;
      // larger pivot ?
      if ((d = nPvt[r] * abs (f)) > MaxPivot) {
	MaxPivot = d;
	pivot = r;
      }
    }

    // check pivot element and throw appropriate exception
    if (MaxPivot <= 0) {
#if LU_FAILURE
      qucs::exception * e = new qucs::exception (EXCEPTION_PIVOT);
      e->setText ("no pivot != 0 found during Crout LU decomposition");
      e->setData (c);
      throw_exception (e);
      goto fail;
#else /* insert virtual resistance */
      VIRTUAL_RES ("no pivot != 0 found during Crout LU decomposition", c);
#endif
    }

    // swap matrix rows if necessary and remember that step in the
    // exchange table
    if (c != pivot) {
      A->exchangeRows (c, pivot);
      Swap (int, rMap[c], rMap[pivot]);
      Swap (nr_double_t, nPvt[c], nPvt[pivot]);
    }
  }
#if LU_FAILURE
 fail:
#endif
}

/*! This function decomposes the left hand matrix into an upper U and
   lower L matrix.  The algorithm is called LU decomposition
   (Doolittle's definition).  The function performs the actual LU
   decomposition of the matrix A using (implicit) partial row pivoting. */
template <class nr_type_t>
void eqnsys<nr_type_t>::factorize_lu_doolittle (void) {
  nr_double_t d, MaxPivot;
  nr_type_t f;
  int k, c, r, pivot;

  // initialize pivot exchange table
  for (r = 0; r < N; r++) {
    for (MaxPivot = 0, c = 0; c < N; c++)
      if ((d = abs (A_(r, c))) > MaxPivot)
	MaxPivot = d;
    if (MaxPivot <= 0) MaxPivot = NR_TINY;
    nPvt[r] = 1 / MaxPivot;
    rMap[r] = r;
  }

  // decompose the matrix into L (lower) and U (upper) matrix
  for (c = 0; c < N; c++) {
    // upper matrix entries
    for (r = 0; r < c; r++) {
      f = A_(r, c);
      for (k = 0; k < r; k++) f -= A_(r, k) * A_(k, c);
      A_(r, c) = f;
    }
    // lower matrix entries
    for (MaxPivot = 0, pivot = r; r < N; r++) {
      f = A_(r, c);
      for (k = 0; k < c; k++) f -= A_(r, k) * A_(k, c);
      A_(r, c) = f;
      // larger pivot ?
      if ((d = nPvt[r] * abs (f)) > MaxPivot) {
	MaxPivot = d;
	pivot = r;
      }
    }

    // check pivot element and throw appropriate exception
    if (MaxPivot <= 0) {
#if LU_FAILURE
      qucs::exception * e = new qucs::exception (EXCEPTION_PIVOT);
      e->setText ("no pivot != 0 found during Doolittle LU decomposition");
      e->setData (c);
      throw_exception (e);
      goto fail;
#else /* insert virtual resistance */
      VIRTUAL_RES ("no pivot != 0 found during Doolittle LU decomposition", c);
#endif
    }

    // swap matrix rows if necessary and remember that step in the
    // exchange table
    if (c != pivot) {
      A->exchangeRows (c, pivot);
      Swap (int, rMap[c], rMap[pivot]);
      Swap (nr_double_t, nPvt[c], nPvt[pivot]);
    }

    // finally divide by the pivot element
    if (c < N - 1) {
      f = 1.0 / A_(c, c);
      for (r = c + 1; r < N; r++) A_(r, c) *= f;
    }
  }
#if LU_FAILURE
 fail:
#endif
}

/*! The function is used in order to run the forward and backward
   substitutions using the LU decomposed matrix (Crout's definition -
   Uii are ones).  */
template <class nr_type_t>
void eqnsys<nr_type_t>::substitute_lu_crout (void) {
  nr_type_t f;
  int i, c;

  // forward substitution in order to solve LY = B
  for (i = 0; i < N; i++) {
    f = B_(rMap[i]);
    for (c = 0; c < i; c++) f -= A_(i, c) * X_(c);
    X_(i) = f / A_(i, i);
  }

  // backward substitution in order to solve UX = Y
  for (i = N - 1; i >= 0; i--) {
    f = X_(i);
    for (c = i + 1; c < N; c++) f -= A_(i, c) * X_(c);
    // remember that the Uii diagonal are ones only in Crout's definition
    X_(i) = f;
  }
}

/*! The function is used in order to run the forward and backward
   substitutions using the LU decomposed matrix (Doolittle's
   definition - Lii are ones).  This function is here because of
   transposed LU matrices as used in the AC noise analysis. */
template <class nr_type_t>
void eqnsys<nr_type_t>::substitute_lu_doolittle (void) {
  nr_type_t f;
  int i, c;

  // forward substitution in order to solve LY = B
  for (i = 0; i < N; i++) {
    f = B_(rMap[i]);
    for (c = 0; c < i; c++) f -= A_(i, c) * X_(c);
    // remember that the Lii diagonal are ones only in Doolittle's definition
    X_(i) = f;
  }

  // backward substitution in order to solve UX = Y
  for (i = N - 1; i >= 0; i--) {
    f = X_(i);
    for (c = i + 1; c < N; c++) f -= A_(i, c) * X_(c);
    X_(i) = f / A_(i, i);
  }
}

/*! The function solves the equation system using a full-step iterative
   method (called Jacobi's method) or a single-step method (called
   Gauss-Seidel) depending on the given algorithm.  If the current X
   vector (the solution vector) is the solution within a
   Newton-Raphson iteration it is a good initial guess and the method
   is very likely to converge.  On divergence the method falls back to
   LU decomposition. */
template <class nr_type_t>
void eqnsys<nr_type_t>::solve_iterative (void) {
  nr_type_t f;
  int error, conv, i, c, r;
  int MaxIter = N; // -> less than N^3 operations
  nr_double_t reltol = 1e-4;
  nr_double_t abstol = NR_TINY;
  nr_double_t diff, crit;

  // ensure that all diagonal values are non-zero
  ensure_diagonal ();

  // try to raise diagonal dominance
  preconditioner ();

  // decide here about possible convergence
  if ((crit = convergence_criteria ()) >= 1) {
#if DEBUG && 0
    logprint (LOG_STATUS, "NOTIFY: convergence criteria: %g >= 1 (%dx%d)\n",
	      crit, N, N);
#endif
    //solve_lu ();
    //return;
  }

  // normalize the equation system to have ones on its diagonal
  for (r = 0; r < N; r++) {
    f = A_(r, r);
    assert (f != 0); // singular matrix
    for (c = 0; c < N; c++) A_(r, c) /= f;
    B_(r) /= f;
  }

  // the current X vector is a good initial guess for the iteration
  tvector<nr_type_t> * Xprev = new tvector<nr_type_t> (*X);

  // start iterating here
  i = 0; error = 0;
  do {
    // compute new solution vector
    for (r = 0; r < N; r++) {
      for (f = 0, c = 0; c < N; c++) {
	if (algo == ALGO_GAUSS_SEIDEL) {
	  // Gauss-Seidel
	  if (c < r)      f += A_(r, c) * X_(c);
	  else if (c > r) f += A_(r, c) * Xprev->get (c);
	}
	else {
	  // Jacobi
	  if (c != r) f += A_(r, c) * Xprev->get (c);
	}
      }
      X_(r) = B_(r) - f;
    }
    // check for convergence
    for (conv = 1, r = 0; r < N; r++) {
      diff = abs (X_(r) - Xprev->get (r));
      if (diff >= abstol + reltol * abs (X_(r))) {
	conv = 0;
	break;
      }
      if (!std::isfinite (diff)) { error++; break; }
    }
    // save last values
    *Xprev = *X;
  }
  while (++i < MaxIter && !conv);

  delete Xprev;

  if (!conv || error) {
    logprint (LOG_ERROR,
	      "WARNING: no convergence after %d %s iterations\n",
	      i, algo == ALGO_JACOBI ? "jacobi" : "gauss-seidel");
    solve_lu_crout ();
  }
#if DEBUG && 0
  else {
    logprint (LOG_STATUS,
	      "NOTIFY: %s convergence after %d iterations\n",
	      algo == ALGO_JACOBI ? "jacobi" : "gauss-seidel", i);
  }
#endif
}

/*! The function solves the linear equation system using a single-step
   iterative algorithm.  It is a modification of the Gauss-Seidel
   method and is called successive over relaxation.  The function uses
   an adaptive scheme to adjust the relaxation parameter. */
template <class nr_type_t>
void eqnsys<nr_type_t>::solve_sor (void) {
  nr_type_t f;
  int error, conv, i, c, r;
  int MaxIter = N; // -> less than N^3 operations
  nr_double_t reltol = 1e-4;
  nr_double_t abstol = NR_TINY;
  nr_double_t diff, crit, l = 1, d, s;

  // ensure that all diagonal values are non-zero
  ensure_diagonal ();

  // try to raise diagonal dominance
  preconditioner ();

  // decide here about possible convergence
  if ((crit = convergence_criteria ()) >= 1) {
#if DEBUG && 0
    logprint (LOG_STATUS, "NOTIFY: convergence criteria: %g >= 1 (%dx%d)\n",
	      crit, N, N);
#endif
    //solve_lu ();
    //return;
  }

  // normalize the equation system to have ones on its diagonal
  for (r = 0; r < N; r++) {
    f = A_(r, r);
    assert (f != 0); // singular matrix
    for (c = 0; c < N; c++) A_(r, c) /= f;
    B_(r) /= f;
  }

  // the current X vector is a good initial guess for the iteration
  tvector<nr_type_t> * Xprev = new tvector<nr_type_t> (*X);

  // start iterating here
  i = 0; error = 0;
  do {
    // compute new solution vector
    for (r = 0; r < N; r++) {
      for (f = 0, c = 0; c < N; c++) {
	if (c < r)      f += A_(r, c) * X_(c);
	else if (c > r) f += A_(r, c) * Xprev->get (c);
      }
      X_(r) = (1 - l) * Xprev->get (r) + l * (B_(r) - f);
    }
    // check for convergence
    for (s = 0, d = 0, conv = 1, r = 0; r < N; r++) {
      diff = abs (X_(r) - Xprev->get (r));
      if (diff >= abstol + reltol * abs (X_(r))) {
	conv = 0;
	break;
      }
      d += diff; s += abs (X_(r));
      if (!std::isfinite (diff)) { error++; break; }
    }
    if (!error) {
      // adjust relaxation based on average errors
      if ((s == 0 && d == 0) || d >= abstol * N + reltol * s) {
	// values <= 1 -> non-convergence to convergence
	if (l >= 0.6) l -= 0.1;
	if (l >= 1.0) l = 1.0;
      }
      else {
	// values >= 1 -> faster convergence
	if (l < 1.5) l += 0.01;
	if (l < 1.0) l = 1.0;
      }
    }
    // save last values
    *Xprev = *X;
  }
  while (++i < MaxIter && !conv);

  delete Xprev;

  if (!conv || error) {
    logprint (LOG_ERROR,
	      "WARNING: no convergence after %d sor iterations (l = %g)\n",
	      i, l);
    solve_lu_crout ();
  }
#if DEBUG && 0
  else {
    logprint (LOG_STATUS,
	      "NOTIFY: sor convergence after %d iterations\n", i);
  }
#endif
}

/*! The function computes the convergence criteria for iterative
   methods like Jacobi or Gauss-Seidel as defined by Schmidt and
   v.Mises. */
template <class nr_type_t>
nr_double_t eqnsys<nr_type_t>::convergence_criteria (void) {
  nr_double_t f = 0;
  for (int r = 0; r < A->getCols (); r++) {
    for (int c = 0; c < A->getCols (); c++) {
      if (r != c) f += norm (A_(r, c) / A_(r, r));
    }
  }
  return sqrt (f);
}

/*! The function tries to ensure that there are non-zero diagonal
   elements in the equation system matrix A.  This is required for
   iterative solution methods. */
template <class nr_type_t>
void eqnsys<nr_type_t>::ensure_diagonal (void) {
  ensure_diagonal_MNA ();
}

/*! The function ensures that there are non-zero diagonal elements in
   the equation system matrix A.  It achieves this condition for
   non-singular matrices which have been produced by the modified
   nodal analysis.  It takes advantage of the fact that the zero
   diagonal elements have pairs of 1 and -1 on the same column and
   row. */
template <class nr_type_t>
void eqnsys<nr_type_t>::ensure_diagonal_MNA (void) {
  int restart, exchanged, begin = 0, pairs;
  int pivot1, pivot2, i;
  do {
    restart = exchanged = 0;
    /* search for zero diagonals with lone pairs */
    for (i = begin; i < N; i++) {
      if (A_(i, i) == 0) {
	pairs = countPairs (i, pivot1, pivot2);
	if (pairs == 1) { /* lone pair found, substitute rows */
	  A->exchangeRows (i, pivot1);
	  B->exchangeRows (i, pivot1);
	  exchanged = 1;
	}
	else if ((pairs > 1) && !restart) {
	  restart = 1;
	  begin = i;
	}
      }
    }

    /* all lone pairs are gone, look for zero diagonals with multiple pairs */
    if (restart) {
      for (i = begin; !exchanged && i < N; i++) {
	if (A_(i, i) == 0) {
	  pairs = countPairs (i, pivot1, pivot2);
	  A->exchangeRows (i, pivot1);
	  B->exchangeRows (i, pivot1);
	  exchanged = 1;
	}
      }
    }
  }
  while (restart);
}

/*! Helper function for the above ensure_diagonal_MNA() function.  It
   looks for the pairs of 1 and -1 on the given row and column index. */
template <class nr_type_t>
int eqnsys<nr_type_t>::countPairs (int i, int& r1, int& r2) {
  int pairs = 0;
  for (int r = 0; r < N; r++) {
    if (fabs (real (A_(r, i))) == 1.0) {
      r1 = r;
      pairs++;
      for (r++; r < N; r++) {
	if (fabs (real (A_(r, i))) == 1.0) {
	  r2 = r;
	  if (++pairs >= 2) return pairs;
	}
      }
    }
  }
  return pairs;
}

/*! The function tries to raise the absolute value of diagonal elements
   by swapping rows and thereby make the A matrix diagonally
   dominant. */
template <class nr_type_t>
void eqnsys<nr_type_t>::preconditioner (void) {
  int pivot, r;
  nr_double_t MaxPivot;
  for (int i = 0; i < N; i++) {
    // find maximum column value for pivoting
    for (MaxPivot = 0, pivot = i, r = 0; r < N; r++) {
      if (abs (A_(r, i)) > MaxPivot &&
	  abs (A_(i, r)) >= abs (A_(r, r))) {
        MaxPivot = abs (A_(r, i));
        pivot = r;
      }
    }
    // swap matrix rows if possible
    if (i != pivot) {
      A->exchangeRows (i, pivot);
      B->exchangeRows (i, pivot);
    }
  }
}

#define R_ (*R)
#define T_ (*T)

/*! This function uses the QR decomposition using householder
   transformations in order to solve the given equation system. */
template <class nr_type_t>
void eqnsys<nr_type_t>::solve_qrh (void) {
  factorize_qrh ();
  substitute_qrh ();
}

/*! This function uses the QR decomposition using householder
   transformations in order to solve the given equation system. */
template <class nr_type_t>
void eqnsys<nr_type_t>::solve_qr (void) {
  factorize_qr_householder ();
  substitute_qr_householder ();
}

/*! The function uses the QR decomposition using householder
   transformations in order to solve the given under-determined
   equation system in its minimum norm (least square) sense. */
template <class nr_type_t>
void eqnsys<nr_type_t>::solve_qr_ls (void) {
  A->transpose ();
  factorize_qr_householder ();
  substitute_qr_householder_ls ();
}

/*! Helper function for the euclidian norm calculators. */
static inline void
euclidian_update (nr_double_t a, nr_double_t& n, nr_double_t& scale) {
  nr_double_t x, ax;
  if ((x = a) != 0) {
    ax = fabs (x);
    if (scale < ax) {
      x = scale / ax;
      n = 1 + n * x * x;
      scale = ax;
    }
    else {
      x = ax / scale;
      n += x * x;
    }
  }
}

/*! The following function computes the euclidian norm of the given
   column vector of the matrix A starting from the given row. */
template <class nr_type_t>
nr_double_t eqnsys<nr_type_t>::euclidian_c (int c, int r) {
  nr_double_t scale = 0, n = 1;
  for (int i = r; i < N; i++) {
    euclidian_update (real (A_(i, c)), n, scale);
    euclidian_update (imag (A_(i, c)), n, scale);
  }
  return scale * sqrt (n);
}

/*! The following function computes the euclidian norm of the given
   row vector of the matrix A starting from the given column. */
template <class nr_type_t>
nr_double_t eqnsys<nr_type_t>::euclidian_r (int r, int c) {
  nr_double_t scale = 0, n = 1;
  for (int i = c; i < N; i++) {
    euclidian_update (real (A_(r, i)), n, scale);
    euclidian_update (imag (A_(r, i)), n, scale);
  }
  return scale * sqrt (n);
}

template <typename nr_type_t>
inline nr_type_t cond_conj (nr_type_t t) {
  return std::conj(t);
}

template <>
inline double cond_conj (double t) {
  return t;
}

/*! The function decomposes the matrix A into two matrices, the
   orthonormal matrix Q and the upper triangular matrix R.  The
   original matrix is replaced by the householder vectors in the lower
   triangular (including the diagonal) and the upper triangular R
   matrix with its diagonal elements stored in the R vector. */
template <class nr_type_t>
void eqnsys<nr_type_t>::factorize_qrh (void) {
  int c, r, k, pivot;
  nr_type_t f, t;
  nr_double_t s, MaxPivot;

  delete R; R = new tvector<nr_type_t> (N);

  for (c = 0; c < N; c++) {
    // compute column norms and save in work array
    nPvt[c] = euclidian_c (c);
    cMap[c] = c; // initialize permutation vector
  }

  for (c = 0; c < N; c++) {

    // put column with largest norm into pivot position
    MaxPivot = nPvt[c]; pivot = c;
    for (r = c + 1; r < N; r++) {
      if ((s = nPvt[r]) > MaxPivot) {
	pivot = r;
	MaxPivot = s;
      }
    }
    if (pivot != c) {
      A->exchangeCols (pivot, c);
      Swap (int, cMap[pivot], cMap[c]);
      Swap (nr_double_t, nPvt[pivot], nPvt[c]);
    }

    // compute householder vector
    if (c < N) {
      nr_type_t a, b;
      s = euclidian_c (c, c + 1);
      a = A_(c, c);
      b = -sign (a) * xhypot (a, s); // Wj
      t = xhypot (s, a - b);         // || Vi - Wi ||
      R_(c) = b;
      // householder vector entries Ui
      A_(c, c) = (a - b) / t;
      for (r = c + 1; r < N; r++) A_(r, c) /= t;
    }
    else {
      R_(c) = A_(c, c);
    }

    // apply householder transformation to remaining columns
    for (r = c + 1; r < N; r++) {
      for (f = 0, k = c; k < N; k++) f += cond_conj (A_(k, c)) * A_(k, r);
      for (k = c; k < N; k++) A_(k, r) -= 2.0 * f * A_(k, c);
    }

    // update norms of remaining columns too
    for (r = c + 1; r < N; r++) {
      nPvt[r] = euclidian_c (r, c + 1);
    }
  }
}

/*! The function decomposes the matrix A into two matrices, the
   orthonormal matrix Q and the upper triangular matrix R.  The
   original matrix is replaced by the householder vectors in the lower
   triangular and the upper triangular R matrix (including the
   diagonal).  The householder vectors are normalized to have one in
   their first position. */
template <class nr_type_t>
void eqnsys<nr_type_t>::factorize_qr_householder (void) {
  int c, r, pivot;
  nr_double_t s, MaxPivot;

  delete T; T = new tvector<nr_type_t> (N);

  for (c = 0; c < N; c++) {
    // compute column norms and save in work array
    nPvt[c] = euclidian_c (c);
    cMap[c] = c; // initialize permutation vector
  }

  for (c = 0; c < N; c++) {

    // put column with largest norm into pivot position
    MaxPivot = nPvt[c]; pivot = c;
    for (r = c + 1; r < N; r++)
      if ((s = nPvt[r]) > MaxPivot) {
	pivot = r;
	MaxPivot = s;
      }
    if (pivot != c) {
      A->exchangeCols (pivot, c);
      Swap (int, cMap[pivot], cMap[c]);
      Swap (nr_double_t, nPvt[pivot], nPvt[c]);
    }

    // compute and apply householder vector
    T_(c) = householder_left (c);

    // update norms of remaining columns too
    for (r = c + 1; r < N; r++) {
      if ((s = nPvt[r]) > 0) {
	nr_double_t y = 0;
	nr_double_t t = norm (A_(c, r) / s);
	if (t < 1)
	  y = s * sqrt (1 - t);
	if (fabs (y / s) < NR_TINY)
	  nPvt[r] = euclidian_c (r, c + 1);
	else
	  nPvt[r] = y;
      }
    }
  }
}

/*! The function uses the householder vectors in order to compute Q'B,
   then the equation system RX = B is solved by backward substitution. */
template <class nr_type_t>
void eqnsys<nr_type_t>::substitute_qrh (void) {
  int c, r;
  nr_type_t f;

  // form the new right hand side Q'B
  for (c = 0; c < N - 1; c++) {
    // scalar product u_k^T * B
    for (f = 0, r = c; r < N; r++) f += cond_conj (A_(r, c)) * B_(r);
    // z - 2 * f * u_k
    for (r = c; r < N; r++) B_(r) -= 2.0 * f * A_(r, c);
  }

  // backward substitution in order to solve RX = Q'B
  for (r = N - 1; r >= 0; r--) {
    f = B_(r);
    for (c = r + 1; c < N; c++) f -= A_(r, c) * X_(cMap[c]);
    if (abs (R_(r)) > std::numeric_limits<nr_double_t>::epsilon())
      X_(cMap[r]) = f / R_(r);
    else
      X_(cMap[r]) = 0;
  }
}

/*! The function uses the householder vectors in order to compute Q'B,
   then the equation system RX = B is solved by backward substitution. */
template <class nr_type_t>
void eqnsys<nr_type_t>::substitute_qr_householder (void) {
  int c, r;
  nr_type_t f;

  // form the new right hand side Q'B
  for (c = 0; c < N; c++) {
    if (T_(c) != 0) {
      // scalar product u' * B
      for (f = B_(c), r = c + 1; r < N; r++) f += cond_conj (A_(r, c)) * B_(r);
      // z - T * f * u
      f *= cond_conj (T_(c)); B_(c) -= f;
      for (r = c + 1; r < N; r++) B_(r) -= f * A_(r, c);
    }
  }

  // backward substitution in order to solve RX = Q'B
  for (r = N - 1; r >= 0; r--) {
    for (f = B_(r), c = r + 1; c < N; c++) f -= A_(r, c) * X_(cMap[c]);
    if (abs (A_(r, r)) > std::numeric_limits<nr_double_t>::epsilon())
      X_(cMap[r]) = f / A_(r, r);
    else
      X_(cMap[r]) = 0;
  }
}

/*! The function uses the householder vectors in order to the solve the
   equation system R'X = B by forward substitution, then QX is computed
   to obtain the least square solution of the under-determined equation
   system AX = B. */
template <class nr_type_t>
void eqnsys<nr_type_t>::substitute_qr_householder_ls (void) {
  int c, r;
  nr_type_t f;

  // forward substitution in order to solve R'X = B
  for (r = 0; r < N; r++) {
    for (f = B_(r), c = 0; c < r; c++) f -= A_(c, r) * B_(c);
    if (abs (A_(r, r)) > std::numeric_limits<nr_double_t>::epsilon())
      B_(r) = f / A_(r, r);
    else
      B_(r) = 0;
  }

  // compute the least square solution QX
  for (c = N - 1; c >= 0; c--) {
    if (T_(c) != 0) {
      // scalar product u' * B
      for (f = B_(c), r = c + 1; r < N; r++) f += cond_conj (A_(r, c)) * B_(r);
      // z - T * f * u_k
      f *= T_(c); B_(c) -= f;
      for (r = c + 1; r < N; r++) B_(r) -= f * A_(r, c);
    }
  }

  // permute solution vector
  for (r = 0; r < N; r++) X_(cMap[r]) = B_(r);
}

#define sign_(a) (real (a) < 0 ? -1 : 1)

/*! The function creates the left-hand householder vector for a given
   column which eliminates the column except the first element.  The
   householder vector is normalized to have one in the first position.
   The diagonal element is replaced by the applied householder vector
   and the vector itself is located in the free matrix positions.  The
   function returns the normalization factor. */
template <class nr_type_t>
nr_type_t eqnsys<nr_type_t>::householder_create_left (int c) {
  nr_type_t a, b, t;
  nr_double_t s, g;
  s = euclidian_c (c, c + 1);
  if (s == 0 && imag (A_(c, c)) == 0) {
    // no reflection necessary
    t = 0;
  }
  else {
    // calculate householder reflection
    a = A_(c, c);
    g = sign_(a) * xhypot (a, s);
    b = a + g;
    t = b / g;
    // store householder vector
    for (int r = c + 1; r < N; r++) A_(r, c) /= b;
    A_(c, c) = -g;
  }
  return t;
}

/*! The function computes a Householder vector to zero-out the matrix
   entries in the given column, stores it in the annihilated vector
   space (in a packed form) and applies the transformation to the
   remaining right-hand columns. */
template <class nr_type_t>
nr_type_t eqnsys<nr_type_t>::householder_left (int c) {
  // compute householder vector
  nr_type_t t = householder_create_left (c);
  // apply householder transformation to remaining columns if necessary
  if (t != 0) {
    householder_apply_left (c, t);
  }
  return t;
}

/*! The function computes a Householder vector to zero-out the matrix
   entries in the given row, stores it in the annihilated vector space
   (in a packed form) and applies the transformation to the remaining
   downward rows. */
template <class nr_type_t>
nr_type_t eqnsys<nr_type_t>::householder_right (int r) {
  // compute householder vector
  nr_type_t t = householder_create_right (r);
  // apply householder transformation to remaining rows if necessary
  if (t != 0) {
    householder_apply_right (r, t);
  }
  return t;
}

/*! The function creates the right-hand householder vector for a given
   row which eliminates the row except the first element.  The
   householder vector is normalized to have one in the first position.
   The super-diagonal element is replaced by the applied householder
   vector and the vector itself is located in the free matrix
   positions.  The function returns the normalization factor. */
template <class nr_type_t>
nr_type_t eqnsys<nr_type_t>::householder_create_right (int r) {
  nr_type_t a, b, t;
  nr_double_t s, g;
  s = euclidian_r (r, r + 2);
  if (s == 0 && imag (A_(r, r + 1)) == 0) {
    // no reflection necessary
    t = 0;
  }
  else {
    // calculate householder reflection
    a = A_(r, r + 1);
    g = sign_(a) * xhypot (a, s);
    b = a + g;
    t = b / g;
    // store householder vector
    for (int c = r + 2; c < N; c++) A_(r, c) /= b;
    A_(r, r + 1) = -g;
  }
  return t;
}

/*! Applies the householder vector stored in the given column to the
   right-hand columns using the given normalization factor. */
template <class nr_type_t>
void eqnsys<nr_type_t>::householder_apply_left (int c, nr_type_t t) {
  nr_type_t f;
  int k, r;
  // apply the householder vector to each right-hand column
  for (r = c + 1; r < N; r++) {
    // calculate f = u' * A (a scalar product)
    f = A_(c, r);
    for (k = c + 1; k < N; k++) f += cond_conj (A_(k, c)) * A_(k, r);
    // calculate A -= T * f * u
    f *= cond_conj (t); A_(c, r) -= f;
    for (k = c + 1; k < N; k++) A_(k, r) -= f * A_(k, c);
  }
}

/*! Applies the householder vector stored in the given row to the
   downward rows using the given normalization factor. */
template <class nr_type_t>
void eqnsys<nr_type_t>::householder_apply_right (int r, nr_type_t t) {
  nr_type_t f;
  int k, c;
  // apply the householder vector to each downward row
  for (c = r + 1; c < N; c++) {
    // calculate f = u' * A (a scalar product)
    f = A_(c, r + 1);
    for (k = r + 2; k < N; k++) f += cond_conj (A_(r, k)) * A_(c, k);
    // calculate A -= T * f * u
    f *= cond_conj (t); A_(c, r + 1) -= f;
    for (k = r + 2; k < N; k++) A_(c, k) -= f * A_(r, k);
  }
}

// Some helper defines for SVD.
#define V_ (*V)
#define S_ (*S)
#define E_ (*E)
#define U_ (*A)

/*! The function does exactly the same as householder_apply_right()
   except that it applies the householder transformations to another
   matrix. */
template <class nr_type_t>
void eqnsys<nr_type_t>::householder_apply_right_extern (int r, nr_type_t t) {
  nr_type_t f;
  int k, c;
  // apply the householder vector to each downward row
  for (c = r + 1; c < N; c++) {
    // calculate f = u' * A (a scalar product)
    f = V_(c, r + 1);
    for (k = r + 2; k < N; k++) f += cond_conj (A_(r, k)) * V_(c, k);
    // calculate A -= T * f * u
    f *= cond_conj (t); V_(c, r + 1) -= f;
    for (k = r + 2; k < N; k++) V_(c, k) -= f * A_(r, k);
  }
}

/*! This function solves the equation system AX = B using the singular
   value decomposition (Golub-Reinsch-SVD). */
template <class nr_type_t>
void eqnsys<nr_type_t>::solve_svd (void) {
  factorize_svd ();
  chop_svd ();
  substitute_svd ();
}

//! Annihilates near-zero singular values.
template <class nr_type_t>
void eqnsys<nr_type_t>::chop_svd (void) {
  int c;
  nr_double_t Max, Min;
  Max = 0.0;
  for (c = 0; c < N; c++) if (fabs (S_(c)) > Max) Max = fabs (S_(c));
  Min = Max * std::numeric_limits<nr_double_t>::epsilon();
  for (c = 0; c < N; c++) if (fabs (S_(c)) < Min) S_(c) = 0.0;
}

/*! The function uses the singular value decomposition A = USV' to
   solve the equation system AX = B by simple matrix multiplications.
   Remember that the factorization actually computed U, S and V'. */
template <class nr_type_t>
void eqnsys<nr_type_t>::substitute_svd (void) {
  int c, r;
  nr_type_t f;
  // calculate U'B
  for (c = 0; c < N; c++) {
    f = 0.0;
    // non-zero result only if S is non-zero
    if (S_(c) != 0.0) {
      for (r = 0; r < N; r++) f += cond_conj (U_(r, c)) * B_(r);
      // this is the divide by S
      f /= S_(c);
    }
    R_(c) = f;
  }
  // matrix multiply by V to get the final solution
  for (r = 0; r < N; r++) {
    for (f = 0.0, c = 0; c < N; c++) f += cond_conj (V_(c, r)) * R_(c);
    X_(r) = f;
  }
}

/*! The function decomposes the matrix A into three other matrices U, S
   and V'.  The matrix A is overwritten by the U matrix, S is stored
   in a separate vector and V in a separate matrix. */

template <class nr_type_t>
void eqnsys<nr_type_t>::factorize_svd (void) {
  int i, j, l;
  nr_type_t t;

  // allocate space for vectors and matrices
  delete R; R = new tvector<nr_type_t> (N);
  delete T; T = new tvector<nr_type_t> (N);
  delete V; V = new tmatrix<nr_type_t> (N);
  delete S; S = new tvector<nr_double_t> (N);
  delete E; E = new tvector<nr_double_t> (N);

  // bidiagonalization through householder transformations
  for (i = 0; i < N; i++) {
    T_(i) = householder_left (i);
    if (i < N - 1) R_(i) = householder_right (i);
  }

  // copy over the real valued bidiagonal values
  for (i = 0; i < N; i++) S_(i) = real (A_(i, i));
  for (E_(0) = 0, i = 1; i < N; i++) E_(i) = real (A_(i - 1, i));

  // backward accumulation of right-hand householder transformations
  // yields the V' matrix
  for (l = N, i = N - 1; i >= 0; l = i--) {
    if (i < N - 1) {
      if ((t = R_(i)) != 0.0) {
	householder_apply_right_extern (i, cond_conj (t));
      }
      else for (j = l; j < N; j++) // cleanup this row
	V_(i, j) = V_(j, i) = 0.0;
    }
    V_(i, i) = 1.0;
  }

  // backward accumulation of left-hand householder transformations
  // yields the U matrix in place of the A matrix
  for (l = N, i = N - 1; i >= 0; l = i--) {
    for (j = l; j < N; j++) // cleanup upper row
      A_(i, j) = 0.0;
    if ((t = T_(i)) != 0.0) {
      householder_apply_left (i, cond_conj (t));
      for (j = l; j < N; j++) A_(j, i) *= -t;
    }
    else for (j = l; j < N; j++) // cleanup this column
      A_(j, i) = 0.0;
    A_(i, i) = 1.0 - t;
  }

  // S and E contain diagonal and super-diagonal, A contains U, V'
  // calculated; now diagonalization can begin
  diagonalize_svd ();
}

#ifndef MAX
# define MAX(x,y) (((x) > (y)) ? (x) : (y))
#endif

//! Helper function computes Givens rotation.
static inline nr_double_t
givens (nr_double_t a, nr_double_t b, nr_double_t& c, nr_double_t& s) {
  nr_double_t z = xhypot (a, b);
  c = a / z;
  s = b / z;
  return z;
}

template <class nr_type_t>
void eqnsys<nr_type_t>::givens_apply_u (int c1, int c2,
					nr_double_t c, nr_double_t s) {
  for (int i = 0; i < N; i++) {
    nr_type_t y = U_(i, c1);
    nr_type_t z = U_(i, c2);
    U_(i, c1) = y * c + z * s;
    U_(i, c2) = z * c - y * s;
  }
}

template <class nr_type_t>
void eqnsys<nr_type_t>::givens_apply_v (int r1, int r2,
					nr_double_t c, nr_double_t s) {
  for (int i = 0; i < N; i++) {
    nr_type_t y = V_(r1, i);
    nr_type_t z = V_(r2, i);
    V_(r1, i) = y * c + z * s;
    V_(r2, i) = z * c - y * s;
  }
}

/*! This function diagonalizes the upper bidiagonal matrix formed by
   the diagonal S and the super-diagonal vector E.  Both vectors are
   real valued.  Thus real valued calculations even when solving a
   complex valued equation systems is possible except for the matrix
   updates of U and V'. */
template <class nr_type_t>
void eqnsys<nr_type_t>::diagonalize_svd (void) {
  bool split;
  int i, l, j, its, k, n, MaxIters = 30;
  nr_double_t an, f, g, h, d, c, s, b, a;

  // find largest bidiagonal value
  for (an = 0, i = 0; i < N; i++)
    an = MAX (an, fabs (S_(i)) + fabs (E_(i)));

  // diagonalize the bidiagonal matrix (stored as super-diagonal
  // vector E and diagonal vector S)
  for (k = N - 1; k >= 0; k--) {
    // loop over singular values
    for (its = 0; its <= MaxIters; its++) {
      split = true;
      // check for a zero entry along the super-diagonal E, if there
      // is one, it is possible to QR iterate on two separate matrices
      // above and below it
      for (n = 0, l = k; l >= 1; l--) {
	// note that E_(0) is always zero
	n = l - 1;
	if (fabs (E_(l)) + an == an) { split = false; break; }
	if (fabs (S_(n)) + an == an) break;
      }
      // if there is a zero on the diagonal S, it is possible to zero
      // out the corresponding super-diagonal E entry to its right
      if (split) {
	// cancellation of E_(l), if l > 0
	c = 0.0;
	s = 1.0;
	for (i = l; i <= k; i++) {
	  f = -s * E_(i);
	  E_(i) *= c;
	  if (fabs (f) + an == an) break;
	  g = S_(i);
	  S_(i) = givens (f, g, c, s);
	  // apply inverse rotation to U
	  givens_apply_u (n, i, c, s);
	}
      }

      d = S_(k);
      // convergence
      if (l == k) {
	// singular value is made non-negative
	if (d < 0.0) {
	  S_(k) = -d;
	  for (j = 0; j < N; j++) V_(k, j) = -V_(k, j);
	}
	break;
      }
      if (its == MaxIters) {
	logprint (LOG_ERROR, "WARNING: no convergence in %d SVD iterations\n",
		  MaxIters);
      }

      // shift from bottom 2-by-2 minor
      a = S_(l);
      n = k - 1;
      b = S_(n);
      g = E_(n);
      h = E_(k);

      // compute QR shift value (as close as possible to the largest
      // eigenvalue of the 2-by-2 minor matrix)
      f  = (b - d) * (b + d) + (g - h) * (g + h);
      f /= 2.0 * h * b;
      f += sign_(f) * xhypot (f, 1.0);
      f  = ((a - d) * (a + d) + h * (b / f - h)) / a;
      // f => (B_{ll}^2 - u) / B_{ll}
      // u => eigenvalue of T = B' * B nearer T_{22} (Wilkinson shift)

      // next QR transformation
      c = s = 1.0;
      for (j = l; j <= n; j++) {
	i = j + 1;
	g = E_(i);
	b = S_(i);
	h = s * g; // h => right-hand non-zero to annihilate
	g *= c;
	E_(j) = givens (f, h, c, s);
	// perform the rotation
	f = a * c + g * s;
	g = g * c - a * s;
	h = b * s;
	b *= c;
	// here: +-   -+
	//       | f g | = B * V'_j (also first V'_1)
	//       | h b |
	//       +-   -+

	// accumulate the rotation in V'
	givens_apply_v (j, i, c, s);
	d = S_(j) = xhypot (f, h);
	// rotation can be arbitrary if d = 0
	if (d != 0.0) {
	  // d => non-zero result on diagonal
	  d = 1.0 / d;
	  // rotation coefficients to annihilate the lower non-zero
	  c = f * d;
	  s = h * d;
	}
	f = c * g + s * b;
	a = c * b - s * g;
	// here: +-   -+
	//       | d f | => U_j * B
	//       | 0 a |
	//       +-   -+

	// accumulate rotation into U
	givens_apply_u (j, i, c, s);
      }
      E_(l) = 0;
      E_(k) = f;
      S_(k) = a;
    }
  }
}

} // namespace qucs

#undef V_
