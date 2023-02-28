/***************************************************************************
                               qf_poly.h
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

#ifndef _QF_POLY_H
#define _QF_POLY_H

/* Headers for R[X] arithmetic */

#define qf_double_t long double

#include "qf_matrix.h"

// A polynom can be described either by a product of monoms equal to
// (x - r[i]) where r[i] is the ith root and a constant factor, or by
// the classical series of coefficient a[0]...a[n], or both.

enum qf_poly_rep
{
  NONE  = 0,			// Not initialized
  ROOTS = 1,			// P(X) = k * prod (x - r[i])
  COEFF = 2,			// P(X) = sum (a[i] * x^i)
  BOTH  = 3			// Both have been computed
};

typedef enum qf_poly_rep qpr;

class qf_poly
{
 private:
  qpr rep;		        // Type of representation
  unsigned d;			// Current degree
  qf_double_t krts;		// Constant k
  qf_double_t * p;		// Table of coefficients
  qf_double_t * rts;		// Table of complex roots

  // Functions used by solve
  void qf_bcm (qf_matrix &);
  int  qf_qrc (qf_matrix &, qf_double_t *);
  void qf_scm (qf_matrix &);

 public:
  qf_poly ();
  qf_poly (unsigned);		// id with d°
  qf_poly (qf_double_t, qf_double_t, qf_double_t, unsigned); // Up to d°=2    
  qf_poly (int, const qf_double_t[]);	// Id, with inst.
  qf_poly (int, qf_double_t, const qf_double_t[]);
  qf_poly (const qf_poly &);	// Copy
  ~qf_poly ();

  // access operators
  qf_poly & operator = (const qf_poly &); // P = Q
  qf_double_t & operator [] (int i);	  // Access to element

  // arithmetic operators
  qf_poly operator - (void);	// Unary -

  friend qf_poly operator + (qf_poly, qf_poly);
  friend qf_poly operator - (qf_poly, qf_poly);
  friend qf_poly operator * (qf_poly, qf_poly);
  friend qf_poly operator * (qf_poly, const qf_double_t);

  qf_poly operator += (qf_poly);
  qf_poly operator -= (qf_poly);
  qf_poly operator *= (qf_poly);	// P(X) = P(X)*Q(X)
  qf_poly operator *= (const qf_double_t);

  qf_poly operator << (unsigned);	// Basic div by X^n
  qf_poly operator >> (unsigned);	// Multiply by X^n

  bool operator == (qf_poly);	// Test
  bool operator != (qf_poly);	// Test
  bool is_null (void);

  unsigned deg (void);		// Degree of poly
  void spl (void);		// Simplify
  qf_poly odd (void);		// Odd part
  qf_poly even (void);		// Even part
  qf_poly mnx (void);		// P(X) -> P(-X)
  qf_poly hsq (void);		// P(X)*P(-X)
  qf_poly sqr (void);		// Q(X) = P(X^2)
  qf_double_t eval (qf_double_t);	// P(X = a) 
  qf_double_t evalX2 (qf_double_t);	// P(X^2 = a) 

  void to_roots (void);		// Solves
  qf_double_t k (void);		// Return krts factor
  void to_coeff (void);		// Calculate normal form
  void div (qf_double_t, qf_double_t);	// Simple division
  void hurw (void);		// "Hurwitzes" polynom

  void disp (const char *);	// Prints P(X)
  void disp_c (void);
  void disp_r (void);

  friend void smpf (qf_poly &, qf_poly &);	// Simplify
};

// For solve, we need some gibber

// Save complex value elements 
#define SET_COMPLEX_PACKED(zp,n,r,i) \
	*((zp)+2*(n)+0)=(r); *((zp)+2*(n)+1)=(i);

// Some constants

// IEEE long precision 2^{-52}
// # define EPSILON      2.2204460492503131e-16
// IEEE double long precision 2^{-63}
#define EPSILON	        1.0842021724855044e-19
#define ROOT_PREC	1e-9
#define ROOT_TOL	1e-7

inline qf_double_t ROUND_ROOT (qf_double_t k) {
  if (k > 0)
    return floor (k / ROOT_PREC) * ROOT_PREC;
  else
    return ceil (k / ROOT_PREC) * ROOT_PREC;
}

#define RADIX		2
#define RADIX2		(RADIX*RADIX)
#define MAX_ITERATIONS	60

#endif // _QF_POLY_H
