/***************************************************************************
                               qf_poly.cpp
                             ----------------
    begin                : Mon Jan 02 2006
    copyright            : (C) 2006 by Vincent Habchi, F5RCS
    email                : 10.50@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// Class for polynoms with real coefficients (R[X])
// Basic operations are covered
// It includes also an algorithm to find all the roots of a real
// polynom.

#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <iostream>

#undef _QF_POLY_DEBUG

#include "qf_poly.h"

// A polynom is essentially a structure with an order (max. index)
// and a table storing coefficients
qf_poly::qf_poly () :
  rep (NONE), d (0), krts (0), p (NULL), rts (NULL) {
}

// Creates default polynoms
qf_poly::qf_poly (unsigned o) :
  rep (NONE), d (o), krts (0), p (NULL), rts (NULL) {
}

// This function creates irreductible real polynoms
// That is either constants, monoms, or binoms
qf_poly::qf_poly (qf_double_t a, qf_double_t b, qf_double_t c, unsigned deg) {

#ifdef _QF_POLY_DEBUG
  std::cout << "qf_poly (ax^2+bx+c), a = " << a << ", b = " << b
    << ", c = " << c << ", d° = " << deg << "\n";
#endif

  // Pathological cases 
  if ((deg == 2) && (a == 0)) {
    deg = 1;
    a = b;
    b = c;
  }
  if ((deg == 1) && (a == 0)) {
    deg = 0;
    a = b;
  }

  // Proceed with normal cases
  switch (deg) {
  case 0:
    // Constant
    d = 0;
    p = new qf_double_t[1];
    p[0] = a;
    rts = NULL;			// no root (or an infinite # of them)
    krts = a;
    rep = BOTH;
    break;
  case 1:
    // (aX + b)
    d = 1;
    p = new qf_double_t[2];
    p[0] = b;
    p[1] = a;
    rts = new qf_double_t[2];
    rts[0] = ROUND_ROOT (-b / a);
    rts[1] = 0;
    krts = a;
    rep = BOTH;
    break;
  default:
    // Polynom of d° 2 (aX^2 + bX + c)
    if (deg > 2)
      std::cout << "Warning: qf_poly called with deg > 2.\n";
    d = 2;
    p = new qf_double_t[3];
    p[0] = c;
    p[1] = b;
    p[2] = a;
    rts = new qf_double_t[4];
    krts = a;
    qf_double_t dlt = (b * b - 4 * a * c);
    if (dlt == 0) {
      // Double root (should not occur)
      rts[0] = rts[2] = ROUND_ROOT (-b / (2 * a));
      rts[1] = rts[3] = 0;
    } else if (dlt > 0) {
      // Two real roots (should not occur)
      rts[1] = rts[3] = 0;
      rts[0] = ROUND_ROOT ((-b + sqrt (dlt)) / (2 * a));
      rts[2] = ROUND_ROOT (-(b + sqrt (dlt)) / (2 * a));
    } else {
      // Two conjugate complex root (normal case)
      rts[0] = rts[2] = ROUND_ROOT (-b / (2 * a));
      rts[1] = ROUND_ROOT (sqrt (-dlt) / (2 * a));
      rts[3] = -rts[1];
    }
    rep = BOTH;
    break;
  }

#ifdef	_QF_POLY_DEBUG
  std::cout << "qf_poly ax^2+bx+c: ";
  this->disp ("prod");
#endif
}

// Creates a polynom and instantiates it out of a constant table
qf_poly::qf_poly (int o, const qf_double_t coef[]) :
  rep (COEFF), d (o), krts (0), rts (NULL) {

  p = new qf_double_t[o + 1];
  for (int i = o; i >= 0; i--) p[i] = coef[o - i];

#ifdef	_QF_POLY_DEBUG
  std::cout << "qf_poly coeffs: ";
  this->disp ("P");
#endif
  return;
}

// Creates a polynom out of its roots and a constant factor
// The roots are complex numbers
// If a root is complex, then its conjugate is also a root
// since the coefficients are real.
qf_poly::qf_poly (int o, qf_double_t k, const qf_double_t r[]) :
  rep (ROOTS), d (o), p (NULL) {

  rts = new qf_double_t[2 * o];
  for (int i = 0; i < 2 * o; i++)
    rts[i] = ROUND_ROOT (r[i]);
  krts = k;

#ifdef	_QF_POLY_DEBUG
  std::cout << "qf_poly (roots): ";
  this->disp ("P");
#endif
  return;
}

// Copy constructor
qf_poly::qf_poly (const qf_poly & P) :
  rep (P.rep), d (P.d), krts (0), p (NULL), rts (NULL) {

  if (rep & COEFF) {
    p = new qf_double_t[d + 1];
    memcpy (p, P.p, sizeof (qf_double_t) * (d + 1));
  }
  if (rep & ROOTS) {
    rts = new qf_double_t[2 * d];
    memcpy (rts, P.rts, sizeof (qf_double_t) * 2 * d);
    krts = P.krts;
  }
}

// Assignment operator
// Identical to previous
qf_poly & qf_poly::operator = (const qf_poly & P) {
  if (&P == this)		// Self copy, nothing to do
    return (*this);

  d = P.d;
  rep = P.rep;

  if (p != NULL) delete[] p;
  if (rts != NULL) delete[] rts;
  p = rts = NULL;
  krts = 0;

  if (rep & COEFF) {
    p = new qf_double_t[d + 1];
    memcpy (p, P.p, sizeof (qf_double_t) * (d + 1));
  }
  if (rep & ROOTS) {
    rts = new qf_double_t[2 * d];
    memcpy (rts, P.rts, sizeof (qf_double_t) * (2 * d));
    krts = P.krts;
  }
  return (*this);
}

// Garbage bin
qf_poly::~qf_poly () {
  if (p != NULL) delete[] p;
  if (rts != NULL) delete[] rts;
}

// Basic functions.

// Access to the element of nth order
// [] overload
qf_double_t & qf_poly::operator [] (int i) {
  if (rep == NONE) {
    std::cout << "qf_poly::[] used on a NONE polynom.\n";
    exit (-1);
  }
  if (rep & COEFF)
    return p[i];
  return rts[i];
}

// Returns d° (order) of polynom
unsigned qf_poly::deg () {
  return d;
}

qf_double_t qf_poly::k () {
  if (rep == NONE) {
    std::cout << "qf_poly::k () used on a NONE polynom.\n";
    exit (-1);
  }
  if (rep & ROOTS)
    return krts;
  return p[d];
}

// Simplifies a polynom
// This function looks for the highest non-zero term and sets
// d accordingly, so that we do not perform useless operations on 0s
// Note that the unused 0s are not freed. We cannot do that at that
// time without copying, which is a ** overhead
// Useful after additions
void qf_poly::spl () {
  int i = d;

  if (rep == NONE) {
    std::cout << "qf_poly::spl () used on a NONE polynom.\n";
    exit (-1);
  }

  if (d == 0)
    return;

  if (rep == ROOTS)
    return;

  // We scan from highest to lowest order
  while (i > 0) {
    if (p[i] == 0)
      i--;
    else
      break;
  }
  d = i;

  return;
}

// Arithmetical operations

// Negates (Unary minus : P -> -P)
qf_poly qf_poly::operator - (void) {
  if (rep == NONE) {
    std::cout << "qf_poly::unary - used on a NONE polynom.\n";
    exit (-1);
  }

  qf_poly R (d);

  if (rep & COEFF) {
    R.p = new qf_double_t[d + 1];
    for (unsigned i = 0; i <= d; i++)
      R.p[i] = -p[i];
  }
  if (rep & ROOTS) {
    R.rts = new qf_double_t[2 * d];
    memcpy (R.rts, rts, sizeof (qf_double_t) * 2 * d);
    R.krts = -krts;
  }

  R.rep = rep;
  return R;
}

// Addition
qf_poly operator + (qf_poly P, qf_poly Q) {
  if ((Q.rep == NONE) || (P.rep == NONE)) {
    std::cout << "qf_poly::+ used on a NONE polynom.\n";
    exit (-1);
  }

  if (Q.d >= P.d) {
    qf_poly R (Q);
    return R += P;
  }
  else {
    qf_poly R (P);
    return R += Q;
  }
}

// Self-Addition
qf_poly qf_poly::operator += (qf_poly P) {
  if ((rep == NONE) || (P.rep == NONE)) {
    std::cout << "qf_poly::+= used on a NONE polynom.\n";
    exit (-1);
  }

  // We cannot add two polynoms if one of them is under the ROOTS form
  if (rep == ROOTS)
    to_coeff ();

  // We add coefficients, not roots!
  if (P.rep == ROOTS)
    P.to_coeff ();

  if (d >= P.d) {
    for (unsigned i = 0; i <= P.d; i++)
      p[i] += P.p[i];
  }
  else {
    qf_double_t * pp = new qf_double_t[P.d];
    memcpy (pp, P.p, sizeof (qf_double_t) * P.d);
    for (unsigned i = 0; i <= d; i++)
      pp[i] += p[i];
    delete[] p;
    p = pp;
  }

  if (rep & ROOTS) {
    rep = COEFF;	 	// We must recompute roots if needed
    delete[] rts;
    rts = NULL;
    krts = 0;
  }
  spl ();			// Simplifies
  return (*this);
}

// Substraction
qf_poly operator - (qf_poly P, qf_poly Q) {
  if ((P.rep == NONE) || (Q.rep == NONE)) {
    std::cout << "qf_poly::- used on a NONE polynom.\n";
    exit (-1);
  }

  if (P.d >= Q.d) {
    qf_poly R (P);
    return R -= Q;
  }
  else {
    qf_poly R (Q);
    return R -= P;
  }
}

// Self-Substraction
qf_poly qf_poly::operator -= (qf_poly P) {
  if ((rep == NONE) || (P.rep == NONE)) {
    std::cout << "qf_poly::-= used on a NONE polynom.\n";
    exit (-1);
  }

  if (rep == ROOTS)
    to_coeff ();

  if (P.rep == ROOTS)
    P.to_coeff ();

  if (d >= P.d) {
    for (unsigned i = 0; i <= P.d; i++)
      p[i] -= P.p[i];
  }
  else {
    qf_double_t * pp = new qf_double_t[P.d + 1];
    memcpy (pp, P.p, sizeof (qf_double_t) * (P.d + 1));
    for (unsigned i = 0; i <= P.d; i++)
      if (i <= d)
	pp[i] = p[i] - pp[i];
      else
	pp[i] = -pp[i];
    delete[] p;
    p = pp;
  }

  if (rep & ROOTS) {
    rep = COEFF;	 	// We must recompute roots if needed
    delete[] rts;
    rts = NULL;
    krts = 0;
  }
  spl ();			// Simplifies
  return (*this);
}

// Multiplication of two polynoms
qf_poly operator * (qf_poly P, qf_poly Q) {
  if ((P.rep == NONE) || (Q.rep == NONE)) {
    std::cout << "qf_poly::* used on a NONE polynom.\n";
    exit (-1);
  }

  qf_poly R (P);
  R *= Q;
  return R;
}

// Multiplication with a scalar
qf_poly operator * (qf_poly P, const qf_double_t m) {
  if (P.rep == NONE) {
    std::cout << "qf_poly::* (scalar) used on a NONE polynom.\n";
    exit (-1);
  }

  qf_poly R (P);
  R *= m;
  return R;
}

// Self-Multiply 
qf_poly qf_poly::operator *= (qf_poly P) {
  if ((rep == NONE) || (P.rep == NONE)) {
    std::cout << "qf_poly::*= () used on a NONE polynom.\n";
    exit (-1);
  }

  // Just a constant to multiply
  if (P.d < 1) {
    if (P.rep & COEFF)
      return ((*this) *= P.p[0]);
    else
      return ((*this) *= P.krts);
  }

  // Resizes the coefficient list
  if (rep & COEFF) {
    if (!(P.rep & COEFF)) P.to_coeff ();
    qf_double_t * q = new qf_double_t[d + P.d + 1];
    memset (q, 0, sizeof (qf_double_t) * (d + P.d + 1));
    for (unsigned i = 0; i <= d; i++)
      for (unsigned j = 0; j <= P.d; j++)
	q[i + j] += p[i] * P.p[j];
    delete[] p;
    p = q;
  }

  // The roots are the concatenation of the roots of both polynoms
  if (rep & ROOTS) {
    if (!(P.rep & ROOTS)) P.to_roots ();
    qf_double_t * rtsp = new qf_double_t[2 * (d + P.d)];
    memcpy (rtsp, rts, sizeof (qf_double_t) * 2 * d);
    memcpy (&rtsp[2 * d], P.rts, sizeof (qf_double_t) * 2 * P.d);
    delete[] rts;
    rts = rtsp;
    krts *= P.krts;
  }

  d += P.d;
  return (*this);
}

// Self-Scalar-Multiply 
qf_poly qf_poly::operator *= (const qf_double_t m) {
  if (rep == NONE) {
    std::cout << "qf_poly::*= (scalar) used on a NONE polynom.\n";
    exit (-1);
  }

  if (m == 0) {
    krts = d = 0;
    delete[] rts;
    delete[] p;
    rts = p = NULL;
    rep = COEFF;
    return (*this);
  }

  if (m == 1)
    return (*this);

  if (rep & COEFF)
    for (unsigned i = 0; i <= d; i++)
      p[i] *= m;

  if (rep & ROOTS)
    krts *= m;

  return (*this);
}

// Test
bool qf_poly::operator == (qf_poly P) {
  if (rep == NONE)
    return false;

  // Two polynoms can be equal only if their degree is the same
  if (d != P.d)
    return false;

  // We can't compare two polynoms using the roots, because they can
  // be stored in different order, and therefore the comparison would
  // be cumbersome. It is shorter to translate the polynoms in COEFF
  // form, then make a comparison of each coefficient

  if (rep == ROOTS)
    to_coeff ();

  if (P.rep == ROOTS)
    P.to_coeff ();

  for (unsigned i = 0; i <= d; i++)
    if (p[i] != P.p[i])
      return false;

  return true;
}

bool qf_poly::operator != (qf_poly P) {
  return !((*this) == P);
}

bool qf_poly::is_null (void) {
  if (rep == NONE) {
    std::cout << "Warning qf_poly::is_null() on a NONE polynom.\n";
    return true;
  }

  if (d == 0)
    return true;

  if (d > 1)
    return false;

  if (rep & ROOTS)
    return (krts == 0);
  else
    return (p[0] == 0);
}

// Basic division by x^n == left shift n places
qf_poly qf_poly::operator << (unsigned n) {
  if (rep == NONE) {
    std::cout << "qf_poly::<< used on a NONE polynom.\n";
    exit (-1);
  }

  if (n == 0)
    return (*this);

  if (d < n)
    return qf_poly (0, 0, 0, 0);	// 0

  else if (d == n)
    return qf_poly (p[d], 0, 0, 0);	// Q(x) = P(n)

  qf_poly R;

  if (rep & COEFF) {
    for (unsigned i = 0; i < n; i++)
      if (p[i] != 0) {
	std::cout << "Warning: << by " << n << " asked for but only "
	  << i << " possible.\n";
	n = i;
      }

    // Okay, proceed
    R.p = new qf_double_t[d - n + 1];
    memcpy (R.p, &(p[n]), sizeof (qf_double_t) * (d - n + 1));
    R.d = d - n;
  }

  if (rep & ROOTS) {
    int nbz = n;
    R.rts = new qf_double_t[2 * d];
    R.krts = krts;

    // Eliminates n zero roots
    for (unsigned i = 0, j = 0; i < 2 * d; i += 2) {
      if ((rts[i] == 0) && (rts[i + 1] == 0) && (nbz != 0))
	nbz--;

      else {
	R.rts[j] = rts[i];
	R.rts[j + 1] = rts[i + 1];
	j += 2;
      }
    }

    R.d = d - n;

    // We did not found a sufficient number of zeros
    if (nbz != 0) {
      std::cout << "Warning: << by " << n << " asked for but only "
		<< n - nbz << " possible.\n";
      R.d += nbz;
    }
  }

  R.rep = rep;
  return R;
}

// Multiplies by x^n
qf_poly qf_poly::operator >> (unsigned n) {
  if (rep == NONE) {
    std::cout << "qf_poly::>> used on a NONE polynom.\n";
    exit (-1);
  }

  if (n == 0)
    return (*this);

  qf_poly R (d + n);

  if (rep & COEFF) {
    R.p = new qf_double_t[d + n + 1];
    memset (R.p, 0, sizeof (qf_double_t) * n);
    memcpy (&(R.p[n]), p, sizeof (qf_double_t) * (d + 1));
  }

  if (rep & ROOTS) {
    R.rts = new qf_double_t[2 * (d + n)];
    memset (R.rts, 0, sizeof (qf_double_t) * 2 * n);	// n times root = 0
    memcpy (&(R.rts[2 * n]), rts, sizeof (qf_double_t) * 2 * d);
    R.krts = krts;
  }

  R.rep = rep;
  return R;
}

// Creates the odd part of a polynom
qf_poly qf_poly::odd () {
  qpr orep = rep;

  if (rep == NONE) {
    std::cout << "qf_poly::odd () used on a NONE polynom.\n";
    exit (-1);
  }

  if (rep == ROOTS)
    to_coeff ();

  qf_poly P (*this);

  int i = d;

  if ((i % 2) == 1)
    i--;

  for (; i >= 0; i -= 2)
    P.p[i] = 0;

  P.spl ();

  if ((orep == ROOTS) || (orep == BOTH))
    P.to_roots ();

  return P;
}

// Creates the even part of a polynom
qf_poly qf_poly::even () {
  qpr orep = rep;

  if (rep == NONE) {
    std::cout << "qf_poly::even () used on a NONE polynom.\n";
    exit (-1);
  }


  if (rep == ROOTS)
    to_coeff ();

  qf_poly P (*this);

  int i = d;

  if (i == 0)
    return P;

  if ((i % 2) == 0)
    i--;

  for (; i >= 1; i -= 2)
    P.p[i] = 0;

  P.spl ();

  if ((orep == ROOTS) || (orep == BOTH))
    P.to_roots ();

  return P;
}

// computes P(-X)
qf_poly qf_poly::mnx () {
  if (rep == NONE) {
    std::cout << "qf_poly::mnx () used on a NONE polynom.\n";
    exit (-1);
  }

  qf_poly P (d);

  if ((rep == COEFF) || (rep == BOTH)) {
    P.p = new qf_double_t[d + 1];
    for (unsigned i = 0; i <= d; i++)
      P.p[i] = ((i % 2) == 0 ? p[i] : -p[i]);
  }

  if ((rep == ROOTS) || (rep == BOTH)) {
    P.rts = new qf_double_t[2 * d];

    for (unsigned i = 0; i < 2 * d; i++)
      P.rts[i] = -rts[i];

    P.krts = ((d % 2) == 0 ? krts : -krts);
  }

  P.rep = rep;
  return P;
}

// "Half square" : P(X) * P(-X)
qf_poly qf_poly::hsq () {
  if (rep == NONE) {
    std::cout << "qf_poly::hsq () used on a NONE polynom.\n";
    exit (-1);
  }

  qf_poly P (*this);

  P *= mnx ();

  return P;
}

// Q(X) <- P(X^2)
qf_poly qf_poly::sqr () {
  if (rep == NONE) {
    std::cout << "qf_poly::sqr () used on a NONE polynom.\n";
    exit (-1);
  }

  if (rep == ROOTS)
    to_coeff ();

  qf_poly P (even ());

  // Contains odd order terms
  if ((*this) != P) {
    std::cout << "Error! qf_poly::sqr () used on a non-square polynom.\n";
    exit (-1);
  }

  qf_poly Q (d / 2);

  Q.p = new qf_double_t[d / 2 + 1];

  for (unsigned i = 0; i <= d / 2; i++)
    Q.p[i] = p[2 * i];

  Q.rep = COEFF;

  if ((rep == BOTH) || (rep == ROOTS))
    Q.to_roots ();

  return Q;			// Q(X) = P(X^2)
}

// Eliminates a prime factor
void qf_poly::div (qf_double_t r, qf_double_t i) {
  if (rep == NONE) {
    std::cout << "qf_poly::div () used on a NONE polynom.\n";
    exit (-1);
  }

  i = fabs (i);			// Imaginary part must be > 0 

  // First examins pathological cases

  if (d == 0) {
    std::cout << "Warning: Div () called on a constant polynom.\n";
    return;
  }

  if ((d == 1) && (i != 0)) {
    std::cout << "Div () real/complex error.\n";
    return;
  }

  if (d == 1) {
    if (((rep == ROOTS) || (rep == BOTH))
	&& (fabs (rts[0] - r) < ROOT_TOL)
	&& (fabs (rts[1]) < ROOT_TOL)) {
      d = 0;
      delete[] rts;
      rts = NULL;
      delete[] p;
      p = new qf_double_t[1];
      p[0] = krts;
      rep = BOTH;
      return;
    }

    if ((rep == COEFF) && (fabs (p[0] / p[1] + r) < ROOT_TOL)) {
      qf_double_t temp = p[1];
      d = 0;
      delete[] p;
      p = new qf_double_t[1];
      p[0] = temp;
      delete[] rts;
      krts = temp;
      rep = BOTH;
      return;
    }

    std::cout << "Warning: Div () error. Specified factor not found.\n";
    return;
  }

  // Proceed to general case.
  // If i = 0, we divide by (X - r)
  // If i != 0, we divide by (X^2 - 2rX + r^2+i^2), that is to say
  // by (X - (r + iI))(X - (r - iI)) where I^2 = -1
  if (rep == COEFF)
    to_roots ();

  qf_double_t * rtsp = new qf_double_t[2 * d];

  bool found = false;

  for (unsigned k = 0, j = 0; k < 2 * d; k += 2) {
#ifdef _QF_POLY_DEBUG
    std::cout << "Div: " << fabs (rts[k] - r) << " "
	      << fabs (rts[k + 1] - i) << "\n";
#endif

    if ((fabs (rts[k] - r) > ROOT_TOL) || (fabs (rts[k + 1] - i) > ROOT_TOL)) {
      rtsp[j] = rts[k];
      rtsp[j + 1] = rts[k + 1];
      j += 2;
    }

    else {
      if (i != 0)
	// If complex root, eliminates also next root (conjugate)
	k += 2;

      found = true;
    }
  }

  if (!found) {
    delete[] rtsp;
    std::cout << "Div () : factor not found! \n";
    return;
  }

  delete[] rts;
  rts = rtsp;

  if (i == 0)
    d--;
  else
    d -= 2;

  rep = ROOTS;
}

// Simplifies polynoms : eliminates common roots
void smpf (qf_poly & N, qf_poly & D) {
  unsigned dN = N.d;
  unsigned dD = D.d;
  unsigned i, j;

  std::cout << "dN: " << dN << " dD : " << dD << '\n';

  bool * Ln = new bool[dN];
  bool * Ld = new bool[dD];

  // Init
  for (i = 0; i < dN; i++)
    Ln[i] = true;

  for (i = 0; i < dD; i++)
    Ld[i] = true;

  if (N.rep == COEFF)
    N.to_roots ();

  if (D.rep == COEFF)
    D.to_roots ();

  // Seek for common roots

  unsigned ndN = dN;
  unsigned ndD = dD;

  for (i = 0; i < 2 * dN; i += 2) {
    for (j = 0; j < 2 * dD; j += 2) {
      std::cout << "N.rts[" << i << "] = " << N.rts[i] << ", ";
      std::cout << "D.rts[" << j << "] = " << D.rts[j] << "\n";
      std::cout << "N.rts[" << i + 1 << "] = " << N.rts[i + 1] << ", ";
      std::cout << "D.rts[" << j + 1 << "] = " << D.rts[j + 1] << "\n";
      if ((Ld[j / 2]) &&
	  (fabs (N.rts[i] - D.rts[j]) < ROOT_TOL) &&
	  (fabs (N.rts[i + 1] - D.rts[j + 1]) < ROOT_TOL)) {
	Ln[i / 2] = false;	// Eliminates both roots
	Ld[j / 2] = false;
	ndN--;
	ndD--;
	std::cout << "Common root: (" << D.rts[j]
		  << ", " << D.rts[j + 1] << "i)\n";
	break;			// Direct to next root
      }
    }
  }

  if (ndN != dN) {		// We have simplified sth
    qf_double_t * nrN = new qf_double_t[2 * ndN];
    qf_double_t * nrD = new qf_double_t[2 * ndD];

    for (i = 0, j = 0; i < 2 * dN; i += 2) {
      if (Ln[i / 2]) {		// Non common root
	nrN[j] = N.rts[i];
	nrN[j + 1] = N.rts[i + 1];
	j += 2;
      }
    }

    delete[] N.rts;
    N.rts = nrN;
    N.d = ndN;
    N.rep = ROOTS;

    for (i = 0, j = 0; i < 2 * D.d; i += 2) {
      if (Ld[i / 2]) {		// Non common root
	nrD[j] = D.rts[i];
	nrD[j + 1] = D.rts[i + 1];
	j += 2;
      }
    }

    delete[] D.rts;
    D.rts = nrD;
    D.d = ndD;
    D.rep = ROOTS;

    N.to_coeff ();
    D.to_coeff ();
    std::cout << "ndN: " << ndN << " ndD : " << ndD << '\n';
  }
  delete[] Ln;
  delete[] Ld;
}

// Hurwitzes a polynom. That is to say, eliminate its roots whose real part
// is positive
void qf_poly::hurw () {
  if (rep == NONE) {
    std::cout << "qf_poly::hurw () used on a NONE polynom.\n";
    return;
  }

  if (rep == COEFF)
    to_roots ();

  qf_double_t * rtsp = new qf_double_t[2 * d];
  unsigned j = 0;

  for (unsigned i = 0; i < 2 * d; i += 2) {
    if (rts[i] > 0)
      if (rts[i + 1] == 0)	// Real positive root
	continue;

      else {
	i += 2;			// Skips conjugate
	continue;
      }

    else {
      rtsp[j] = rts[i];
      rtsp[j + 1] = rts[i + 1];
      j += 2;
    }
  }

  delete[] rts;
  rts = rtsp;
  d = j / 2;

  if (krts < 0)
    krts = -krts;

  rep = ROOTS;
}

// Evaluates a polynom. Computes P(a) for real a
qf_double_t qf_poly::eval (qf_double_t a) {
  if (rep == NONE) {
    std::cout << "qf_poly::eval () used on a NONE polynom.\n";
    return 0;
  }

  if ((rep == COEFF) || (rep == BOTH)) {

    if (d == 0)
      return p[0];		// Constant

    qf_double_t r = p[d];

    for (int i = d - 1; i >= 0; i--)
      r = r * a + p[i];

    return r;
  }
  // ROOTS form : P (a) = k prod (a - r[i])
  if (d == 0)
    return krts;

  qf_double_t r = krts;

  for (unsigned i = 0; i < 2 * d; i += 2) {
    if (rts[i + 1] == 0)	// Real root
      r *= (a - rts[i]);

    else {
      qf_double_t m = rts[i] * rts[i] +	rts[i + 1] * rts[i + 1];
      qf_double_t n = -2 * rts[i];

      r *= (a * a + n * a + m);
      i += 2;			// Skips conjugate (following root)
    }
  }

  return r;
}

// Evaluates a polynom P(X^2) for X^2 = c (c can be negative)
qf_double_t qf_poly::evalX2 (qf_double_t c) {
  return (sqr ()).eval (c);
}

#ifdef _QF_POLY_DEBUG
// Pretty prints a polynom
void qf_poly::disp (const char *name) {

  if (rep == NONE) {
    std::cout << name << "(x) is not initalized.\n";
    return;
  }

  if ((rep == COEFF) || (rep == BOTH)) {
    std::cout << name << "(x) = ";
    disp_c ();
  }

  if ((rep == ROOTS) || (rep == BOTH)) {
    std::cout << name << "(x) = ";
    disp_r ();
  }
}
#else
void qf_poly::disp (const char *) { }
#endif	// _QF_POLY_DEBUG

void qf_poly::disp_c (void) {
  if (d == 0) {
    std::cout << p[0] << '\n';
    return;
  }

  if (p[d] < 0)
    std::cout << "-";

  if (fabs (p[d]) != 1)
    std::cout << fabs (p[d]);

  if (d == 1) {
    std::cout << " x ";
  }

  else {
    std::cout << " x^" << d << ' ';

    for (unsigned i = d - 1; i > 1; i--) {
      if (p[i] == 0)		// Null monome
	continue;

      if (p[i] > 0)
	std::cout << "+ ";
      else
	std::cout << "- ";

      if (fabs (p[i]) != 1)
	std::cout << fabs (p[i]);

      std::cout << " x^" << i << ' ';
    }

    if (p[1] != 0) {
      if (p[1] > 0)
	std::cout << "+ ";
      else
	std::cout << "- ";

      if (fabs (p[1]) != 1)
	std::cout << fabs (p[1]);

      std::cout << " x ";
    }
  }

  if (p[0] != 0) {
    if (p[0] > 0)
      std::cout << "+ ";
    else
      std::cout << "- ";

    std::cout << fabs (p[0]);

  }
  std::cout << '\n';
}

void qf_poly::disp_r (void) {
  if (krts == -1)
    std::cout << "- ";

  else if (krts != 1)
    std::cout << krts << ' ';

  for (unsigned i = 0; i < 2 * d; i += 2) {
    if (rts[i + 1] == 0) { // Real root
      std::cout << "(X";

      if (rts[i] == 0)
	std::cout << ") ";

      else if (rts[i] < 0)
	std::cout << " + " << fabs (rts[i]) << ") ";

      else
	std::cout << " - " << rts[i] << ") ";
    }
    else { // Complex conjugate root
      std::cout << "(X^2 ";

      qf_double_t m = rts[i] * rts[i] + rts[i + 1] * rts[i + 1];
      qf_double_t n = 2 * rts[i];

      if (n > 0)
	std::cout << "- " << n << "X ";

      if (n < 0)
	std::cout << "+ " << fabs (n) << "X ";

      std::cout << "+ " << m << ") ";

      i += 2;			// Skips next root (conjugate of present one)
    }
  }
  std::cout << '\n';
}

/* This function calculates P(X) = sum (a[i] * X^i) (sum form) out of
   the roots (product form) P(X) = k * prod (X - r[i]) */
void qf_poly::to_coeff (void) {
  if (rep == NONE) {
    std::cout << "qf_poly::to_coeff () used on a NONE polynom.\n";
    exit (-1);
  }

  if ((rep == COEFF) || (rep == BOTH))
    return;

  if (p != NULL)
    delete[] p;

  rep = BOTH;

  p = new qf_double_t[1];
  p[0] = krts;

  if ((rts == NULL) || (d == 0))
    return;

  if (d == 1) {
    p = new qf_double_t[2];
    p[0] = -krts * rts[0];
    p[1] = krts;
    return;
  }

  unsigned r = 0;

  do {
    if (rts[2 * r + 1] == 0) {	// Real root
      qf_double_t *q = new qf_double_t[r + 2];

      q[0] = 0;
      memcpy (&(q[1]), p, sizeof (qf_double_t) * (r + 1));	// Q(X) = XP(X)

      for (unsigned j = 0; j < r + 1; j++)
	q[j] -= rts[2 * r] * p[j];	// Q(X) -= r[i]P(X)

      delete[] p;
      p = q;
      r++;
    }

    else {			// Complex conjugate root
      qf_double_t m, n;
      qf_double_t *q = new qf_double_t[r + 3];
      qf_double_t *s = new qf_double_t[r + 2];

      m = rts[2 * r] * rts[2 * r] + rts[2 * r + 1] * rts[2 * r + 1];
      n = -2 * rts[2 * r];

      q[0] = q[1] = s[0] = 0;

      memcpy (&(q[2]), p, sizeof (qf_double_t) * (r + 1)); // Q(X) = X^2P(X)
      memcpy (&(s[1]), p, sizeof (qf_double_t) * (r + 1)); // S(X) = XP(X) 

      for (unsigned j = 0; j < r + 1; j++)	// Q(X) = (X^2 + nX + m) P(X)
	q[j] += n * s[j] + m * p[j];

      q[r + 1] += n * s[r + 1];

      delete[] p;
      delete[] s;
      p = q;
      r += 2;
    }
  }
  while (r < d);

  (*this).disp ("qf_poly::to_coeff: ");
}

/* The function finds the complex roots of the polynom given by:
   p(x) = a_{n-1} * x^{n-1} + ... a_{2} * x^{2} + a_{1} * x + a_{0}
   The results are stored in the vector rst, real part followed by
   imaginary part for each complex root.  It return zero on success
   and non-zero otherwise. */
void qf_poly::to_roots (void) {
  if (rep == NONE) {
    std::cout << "qf_poly::to_roots () used on a NONE polynom.\n";
    exit (-1);
  }

  int status;

  if ((rep == ROOTS) || (rep == BOTH))
    return;			// Nothing to do

  if (d == 0)
    // cannot solve for only one term
    return;

  qf_matrix m (d);

  if (rts != NULL)
    delete[] rts;

  rts = new qf_double_t[2 * d];

  krts = p[d];

  qf_scm (m);
  qf_bcm (m);
  status = qf_qrc (m, rts);

  // root solving qr method failed to converge
  for (unsigned i = 0; i < 2 * d; i++) {
    if (fabs (rts[i]) <= ROOT_PREC)
      rts[i] = 0;
    else
      rts[i] = ROUND_ROOT (rts[i]);
//    std::cout << "root(" << i/2 << ") = " << rts [i] << 
//               " + " << rts [i+1] << " i\n" ;
  }

  rep = BOTH;
}

// Private functions used by qf_poly::solve

// Set companion matrix.
void qf_poly::qf_scm (qf_matrix & m) {
  unsigned int i, j;
  for (i = 0; i < m.n; i++)
    for (j = 0; j < m.n; j++)
      m (i, j) = 0;

  for (i = 1; i < m.n; i++)
    m (i, i - 1) = 1;

  for (i = 0; i < m.n; i++)
    m (i, m.n - 1) = -p[i] / p[m.n];
}

// Balance companion matrix
void qf_poly::qf_bcm (qf_matrix & m) {
  int not_converged = 1;
  qf_double_t row_norm = 0;
  qf_double_t col_norm = 0;

  while (not_converged) {
    unsigned int i, j;
    qf_double_t g, f, s;

    not_converged = 0;

    for (i = 0; i < m.n; i++) {
      /* column norm, excluding the diagonal */
      if (i != m.n - 1) {
	col_norm = fabs (m (i + 1, i));
      }

      else {
	col_norm = 0;

	for (j = 0; j < m.n - 1; j++) {
	  col_norm += fabs (m (j, m.n - 1));
	}
      }

      /* row norm, excluding the diagonal */
      if (i == 0) {
	row_norm = fabs (m (0, m.n - 1));
      }

      else if (i == m.n - 1) {
	row_norm = fabs (m (i, i - 1));
      }

      else {
	row_norm = fabs (m (i, i - 1)) + fabs (m (i, m.n - 1));
      }

      if ((col_norm == 0) || (row_norm == 0)) {
	continue;
      }

      g = row_norm / RADIX;
      f = 1;
      s = col_norm + row_norm;

      while (col_norm < g) {
	f *= RADIX;
	col_norm *= RADIX2;
      }

      g = row_norm * RADIX;

      while (col_norm > g) {
	f /= RADIX;
	col_norm /= RADIX2;
      }

      if ((row_norm + col_norm) < 0.95 * s * f) {
	not_converged = 1;
	g = 1 / f;
	if (i == 0) {
	  m (0, m.n - 1) *= g;
	}

	else {
	  m (i, i - 1) *= g;
	  m (i, m.n - 1) *= g;
	}

	if (i == m.n - 1) {
	  for (j = 0; j < m.n; j++) {
	    m (j, i) *= f;
	  }
	}
	else {
	  m (i + 1, i) *= f;
	}
      }
    }
  }
}

// Root solver using QR method.
int qf_poly::qf_qrc (qf_matrix & h, qf_double_t * zroot) {
  qf_double_t t = 0;
  unsigned int iterations, e, i, j, k, m;
  qf_double_t w, x, y, s, z;
  qf_double_t p = 0, q = 0, r = 0;
  int notlast;
  unsigned int n = d;

next_root:
  if (n == 0)
    return 0;
  iterations = 0;

next_iteration:
  for (e = n; e >= 2; e--) {
    qf_double_t a1 = fabs (h (e - 1, e - 2));
    qf_double_t a2 = fabs (h (e - 2, e - 2));
    qf_double_t a3 = fabs (h (e - 1, e - 1));

    if (a1 <= EPSILON * (a2 + a3))
      break;
  }

  x = h (n - 1, n - 1);

  if (e == n) {
    SET_COMPLEX_PACKED (zroot, n - 1, x + t, 0);	/* one real root */
    n--;
    goto next_root;
  }

  y = h (n - 2, n - 2);
  w = h (n - 2, n - 1) * h (n - 1, n - 2);

  if (e == n - 1) {
    p = (y - x) / 2;
    q = p * p + w;
    y = sqrt (fabs (q));
    x += t;

    if (q > 0) {		/* two real roots */
      if (p < 0)
	y = -y;

      y += p;
      SET_COMPLEX_PACKED (zroot, n - 1, x - w / y, 0);
      SET_COMPLEX_PACKED (zroot, n - 2, x + y, 0);
    }

    else {
      SET_COMPLEX_PACKED (zroot, n - 1, x + p, -y);
      SET_COMPLEX_PACKED (zroot, n - 2, x + p, y);
    }

    n -= 2;
    goto next_root;
  }

  /* No more roots found yet, do another iteration */
  if (iterations == MAX_ITERATIONS) {	/* increased from 30 to 60 */
    /* too many iterations - give up! */
    return -1;
  }

  if (iterations % 10 == 0 && iterations > 0) {
    /* use an exceptional shift */
    t += x;

    for (i = 0; i < n; i++) {
      h (i, i) -= x;
    }

    s = fabs (h (n - 1, n - 2)) + fabs (h (n - 2, n - 3));
    y = 0.75 * s;
    x = y;
    w = -0.4375 * s * s;
  }

  iterations++;

  for (m = n - 2; m >= e; m--) {
    qf_double_t a1, a2, a3;

    z = h (m - 1, m - 1);
    r = x - z;
    s = y - z;
    p = h (m - 1, m) + (r * s - w) / h (m, m - 1);
    q = h (m, m) - z - r - s;
    r = h (m + 1, m);
    s = fabs (p) + fabs (q) + fabs (r);
    p /= s;
    q /= s;
    r /= s;

    if (m == e)
      break;

    a1 = fabs (h (m - 1, m - 2));
    a2 = fabs (h (m - 2, m - 2));
    a3 = fabs (h (m, m));

    if (a1 * (fabs (q) + fabs (r)) <= EPSILON * fabs (p) * (a2 + a3))
      break;
  }

  for (i = m + 2; i <= n; i++) {
    h (i - 1, i - 3) = 0;
  }
  for (i = m + 3; i <= n; i++) {
    h (i - 1, i - 4) = 0;
  }

  /* double QR step */
  for (k = m; k <= n - 1; k++) {
    notlast = (k != n - 1);

    if (k != m) {
      p = h (k - 1, k - 2);
      q = h (k, k - 2);
      r = notlast ? h (k + 1, k - 2) : 0;
      x = fabs (p) + fabs (q) + fabs (r);

      if (x == 0)
	continue;		/* FIXME????? */

      p /= x;
      q /= x;
      r /= x;
    }

    s = sqrt (p * p + q * q + r * r);

    if (p < 0)
      s = -s;

    if (k != m) {
      h (k - 1, k - 2) = -s * x;
    } else if (e != m) {
      h (k - 1, k - 2) *= -1;
    }

    p += s;
    x = p / s;
    y = q / s;
    z = r / s;
    q /= p;
    r /= p;

    /* do row modifications */
    for (j = k; j <= n; j++) {
      p = h (k - 1, j - 1) + q * h (k, j - 1);

      if (notlast) {
	p += r * h (k + 1, j - 1);
	h (k + 1, j - 1) -= p * z;
      }

      h (k, j - 1) -= p * y;
      h (k - 1, j - 1) -= p * x;
    }
    j = (k + 3 < n) ? (k + 3) : n;

    /* do column modifications */
    for (i = e; i <= j; i++) {
      p = x * h (i - 1, k - 1) + y * h (i - 1, k);

      if (notlast) {
	p += z * h (i - 1, k + 1);
	h (i - 1, k + 1) -= p * r;
      }

      h (i - 1, k) -= p * q;
      h (i - 1, k - 1) -= p;
    }
  }

  goto next_iteration;
}
