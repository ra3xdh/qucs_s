/*
 * tridiag.cpp - tridiagonal matrix template class implementation
 *
 * Copyright (C) 2005, 2008 Stefan Jahn <stefan@lkcc.org>
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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <vector>

#include "compat.h"
#include "complex.h"
//#include "tvector.h"

namespace qucs {

// Constructor creates an instance of the tridiag class.
template <class nr_type_t>
tridiag<nr_type_t>::tridiag () {
  abov = belo = diag = offdiag = rhs = NULL;
  type = TRIDIAG_UNKNOWN;
}

/* The copy constructor creates a new instance based on the given
   tridiag object. */
template <class nr_type_t>
tridiag<nr_type_t>::tridiag (const tridiag & t) {
  abov = t.abov;
  belo = t.belo;
  diag = t.diag;
  offdiag = t.offdiag;
  rhs = t.rhs;
  type = t.type;
}

/* The assignment copy constructor creates a new instance based on the
   given tridiag object. */
template <class nr_type_t>
const tridiag<nr_type_t>&
tridiag<nr_type_t>::operator=(const tridiag<nr_type_t> & t) {
  if (&t != this) {
    abov = t.abov;
    belo = t.belo;
    diag = t.diag;
    offdiag = t.offdiag;
    rhs = t.rhs;
    type = t.type;
  }
  return *this;
}

// Destructor deletes a tridiag object.
template <class nr_type_t>
tridiag<nr_type_t>::~tridiag () {
}

// Set the diagonal vector of the tridiagonal matrix.
template <class nr_type_t>
void tridiag<nr_type_t>::setDiagonal (::std::vector<nr_type_t> * v) {
  diag = v;
}

// Set the off-diagonal vector of the tridiagonal matrix.
template <class nr_type_t>
void tridiag<nr_type_t>::setOffDiagonal (::std::vector<nr_type_t> * v) {
  abov = belo = offdiag = v;
}

// Set the above off-diagonal vector of the tridiagonal matrix.
template <class nr_type_t>
void tridiag<nr_type_t>::setA (::std::vector<nr_type_t> * v) {
  abov = v;
}

// Set the below off-diagonal vector of the tridiagonal matrix.
template <class nr_type_t>
void tridiag<nr_type_t>::setB (::std::vector<nr_type_t> * v) {
  belo = v;
}

// Set the right hand side vector of the equation system.
template <class nr_type_t>
void tridiag<nr_type_t>::setRHS (::std::vector<nr_type_t> * v) {
  rhs = v;
}

/* Depending on the type of tridiagonal matrix the function runs one
   of the solvers specialized on this type.  The solvers are in-place
   algorithms meaning the right hand side is replaced by the solution
   and the original matrix entries are destroyed during solving the
   system. */
template <class nr_type_t>
void tridiag<nr_type_t>::solve (void) {
  switch (type) {
  case TRIDIAG_NONSYM:
    solve_ns (); break;
  case TRIDIAG_SYM:
    solve_s (); break;
  case TRIDIAG_NONSYM_CYCLIC:
    solve_ns_cyc (); break;
  case TRIDIAG_SYM_CYCLIC:
    solve_s_cyc (); break;
  }
}

/* This function solves a tridiagonal equation system given
   by
       diag[0]  abov[0]        0   .....            0
       belo[1]  diag[1]  abov[1]   .....            0
             0  belo[2]  diag[2]
             0        0  belo[3]   .....    abov[n-2]
           ...         ...
             0         ...       belo[n-1]  diag[n-1]
*/
template <class nr_type_t>
void tridiag<nr_type_t>::solve_ns (void) {
  d = al = &diag->front(); //diag->getData ();
  f = ga = &abov->front (); //abov->getData ();
  e = &belo->front(); // belo->getData ();
  b = c = x = &rhs->front(); //rhs->getData ();
  int i, n = (int)diag->size(); //diag->getSize ();

  // factorize A = LU
  al[0] = d[0];
  ga[0] = f[0] / al[0];
  for (i = 1; i < n - 1; i++) {
    al[i] = d[i] - e[i] * ga[i-1];
    ga[i] = f[i] / al[i];
  }
  al[n-1] = d[n-1] - e[n-1] * ga[n-2];

  // update b = Lc
  c[0] = b[0] / d[0];
  for (i = 1; i < n; i++) {
    c[i] = (b[i] - e[i] * c[i-1]) / al[i];
  }

  // back substitution Rx = c
  x[n-1] = c[n-1];
  for (i = n - 2; i >= 0; i--) {
    x[i] = c[i] - ga[i] * x[i+1];
  }
}

/* This function solves a cyclically tridiagonal equation system given
   by
       diag[0]  abov[0]        0   .....      belo[0]
       belo[1]  diag[1]  abov[1]   .....           0
             0  belo[2]  diag[2]
             0        0  belo[3]   .....    abov[n-2]
           ...         ...
      abov[n-1]        ...       belo[n-1]  diag[n-1]
*/
template <class nr_type_t>
void tridiag<nr_type_t>::solve_ns_cyc (void) {
  d = al = &diag->front(); //diag->getData ();
  f = ga = &abov->front (); //abov->getData ();
  e = be = &belo->front(); // belo->getData ();
  b = x = c = &rhs->front(); //rhs->getData ();
  int i, n = (int)diag->size(); //diag->getSize ();
  de = new nr_type_t[n];
  ep = new nr_type_t[n];

  // factorize A = LU
  al[0] = d[0];
  ga[0] = f[0] / al[0];
  de[0] = e[0] / al[0];
  for (i = 1; i < n - 2; i++) {
    al[i] = d[i] - e[i] * ga[i-1];
    ga[i] = f[i] / al[i];
    be[i] = e[i];
    de[i] = -be[i] * de[i-1] / al[i];
  }
  al[n-2] = d[n-2] - e[n-2] * ga[n-3];
  be[n-2] = e[n-2];
  ep[2] = f[n-1];
  for (i = 3; i < n; i++) {
    ep[i] = -ep[i-1] * ga[i-3];
  }
  ga[n-2] = (f[n-2] - be[n-2] * de[n-3]) / al[n-2];
  be[n-1] = e[n-1] - ep[n-1] * ga[n-3];
  al[n-1] = d[n-1] - be[n-1] * ga[n-2];
  for (i = 2; i < n; i++) {
    al[n-1] -= ep[i] * de[i-2];
  }

  // update Lc = b
  c[0] = b[0] / al[0];
  for (i = 1; i < n - 1; i++) {
    c[i] = (b[i] - c[i-1] * be[i]) / al[i];
  }
  c[n-1] = b[n-1] - be[n-1] * c[n-2];
  for (i = 2; i < n; i++) {
    c[n-1] -= ep[i] * c[i-2];
  }
  c[n-1] /= al[n-1];

  // back substitution Rx = c
  x[n-1] = c[n-1];
  x[n-2] = c[n-2] - ga[n-2] * x[n-1];
  for (i = n - 3; i >= 0; i--) {
    x[i] = c[i] - ga[i] * x[i+1] - de[i] * x[n-1];
  }

  delete[] de;
  delete[] ep;
}

/* This function solves a symmetric tridiagonal strongly nonsingular
   equation system given by
       diag[0]  offdiag[0]             0   .....                   0
    offdiag[0]     diag[1]    offdiag[1]   .....                   0
             0  offdiag[1]       diag[2]
             0           0    offdiag[2]   .....        offdiag[n-2]
           ...         ...
             0         ...                offdiag[n-2]     diag[n-1]
*/
template <class nr_type_t>
void tridiag<nr_type_t>::solve_s (void) {
  d = al = &diag->front(); //diag->getData ();
  f = ga = &offdiag->front (); //offdiag->getData ();
  b = z = x = c = &rhs->front(); //rhs->getData ();
  nr_type_t t;
  int i, n = (int)diag->size(); //diag->getSize ();
  de = new nr_type_t[n];

  // factorize A = LDL'
  al[0] = d[0];
  t = f[0];
  ga[0] = t / al[0];
  for (i = 1; i < n - 1; i++) {
    al[i] = d[i] - t * ga[i-1];
    t = f[i];
    ga[i] = t / al[i];
  }
  al[n-1] = d[n-1] - t * ga[n-2];

  // update L'z = b and Dc = z
  z[0] = b[0];
  for (i = 1; i < n; i++) {
    z[i] = b[i] - ga[i-1] * z[i-1];
  }
  for (i = 0; i < n; i++) {
    c[i] = z[i] / al[i];
  }

  // back substitution L'x = c
  x[n-1] = c[n-1];
  for (i = n-2; i >= 0; i--) {
    x[i] = c[i] - ga[i] * x[i+1];
  }

  delete[] de;
}

/* This function solves a symmetric cyclically tridiagonal strongly
   nonsingular equation system given by
       diag[0]  offdiag[0]             0   .....        offdiag[n-1]
    offdiag[0]     diag[1]    offdiag[1]   .....                   0
             0  offdiag[1]       diag[2]
             0           0    offdiag[2]   .....        offdiag[n-2]
           ...         ...
  offdiag[n-1]         ...                offdiag[n-2]     diag[n-1]
*/
template <class nr_type_t>
void tridiag<nr_type_t>::solve_s_cyc (void) {
  d = al = &diag->front(); //diag->getData ();
  f = ga = &offdiag->front (); //offdiag->getData ();
  b = c = z  = x = &rhs->front(); //rhs->getData ();
  nr_type_t t;
  int i, n = (int)diag->size(); //diag->getSize ();
  de = new nr_type_t[n];

  // factorize A = LDL'
  al[0] = d[0];
  t = f[0];
  ga[0] = t / al[0];
  de[0] = f[n-1] / al[0];
  for (i = 1; i < n - 2; i++) {
    al[i] = d[i] - t * ga[i-1];
    de[i] = -de[i-1] * t / al[i];
    t = f[i];
    ga[i] = t / al[i];
  }
  al[n-2] = d[n-2] - t * ga[n-3];
  ga[n-2] = (f[n-2] - t * de[n-3]) / al[n-2];
  al[n-1] = d[n-1] - al[n-2] * ga[n-2] * ga[n-2];
  for (i = 0; i < n - 2; i++) {
    al[n-1] -= al[i] * de[i] * de[i];
  }

  // update Lz = b and Dc = z
  z[0] = b[0];
  for (i = 1; i < n - 1; i++) {
    z[i] = b[i] - ga[i-1] * z[i-1];
  }
  z[n-1] = b[n-1] - ga[n-2] * z[n-2];
  for (i = 0; i < n - 2; i++) {
    z[n-1] -= de[i] * z[i];
  }
  for (i = 0; i < n; i++) {
    c[i] = z[i] / al[i];
  }

  // back substitution L'x = c
  x[n-1] = c[n-1];
  x[n-2] = c[n-2] - ga[n-2] * x[n-1];
  for (i = n - 3; i >= 0; i--) {
    x[i] = c[i] - ga[i] * x[i+1] - de[i] * x[n-1];
  }

  delete[] de;
}

} // namespace qucs
