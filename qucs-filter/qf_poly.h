/***************************************************************************
                               poly.h
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

#include "qf_math.h"
#include "qf_matrix.h"
#include <cmath>

namespace qf {
// A polynom can be described either by a product of monoms equal to
// (x - r[i]) where r[i] is the ith root and a constant factor, or by
// the classical series of coefficient a[0]...a[n], or both.
enum poly_rep {
  NONE  = 0, // Not initialized
  ROOTS = 1, // P(X) = k * prod (x - r[i])
  COEFF = 2, // P(X) = sum (a[i] * x^i)
  BOTH  = 3  // Both have been computed
};

typedef enum poly_rep qpr;

class poly {
private:
  qpr rep;       // Type of representation
  unsigned d;    // Current degree
  qf_float krts; // Constant k
  qf_float* p;   // Table of coefficients
  qf_float* rts; // Table of complex roots

  // Functions used by solve
  void bcm(matrix&);
  int qrc(matrix&, qf_float*);
  void scm(matrix&);

public:
  poly();
  poly(unsigned);                               // id with d°
  poly(qf_float, qf_float, qf_float, unsigned); // Up to d°=2
  poly(int, const qf_float[]);                  // Id, with inst.
  poly(int, qf_float, const qf_float[]);
  poly(const poly&); // Copy
  ~poly();

  // access operators
  poly& operator=(const poly&); // P = Q
  qf_float& operator[](int i);  // Access to element

  // arithmetic operators
  poly operator-(void); // Unary -

  friend poly operator+(poly, poly);
  friend poly operator-(poly, poly);
  friend poly operator*(poly, poly);
  friend poly operator*(poly, const qf_float);

  poly operator+=(poly);
  poly operator-=(poly);
  poly operator*=(poly); // P(X) = P(X)*Q(X)
  poly operator*=(const qf_float);

  poly operator<<(unsigned); // Basic div by X^n
  poly operator>>(unsigned); // Multiply by X^n

  bool operator==(poly); // Test
  bool operator!=(poly); // Test
  bool is_null(void);

  unsigned deg(void);        // Degree of poly
  void spl(void);            // Simplify
  poly odd(void);            // Odd part
  poly even(void);           // Even part
  poly mnx(void);            // P(X) -> P(-X)
  poly hsq(void);            // P(X)*P(-X)
  poly sqr(void);            // Q(X) = P(X^2)
  qf_float eval(qf_float);   // P(X = a)
  qf_float evalX2(qf_float); // P(X^2 = a)

  void to_roots(void);          // Solves
  qf_float k(void);             // Return krts factor
  void to_coeff(void);          // Calculate normal form
  void div(qf_float, qf_float); // Simple division
  void hurw(void);              // "Hurwitzes" polynom

  void disp(const char*); // Prints P(X)
  void disp_c(void);
  void disp_r(void);

  friend void smpf(poly&, poly&); // Simplify
};

// For solve, we need some gibber

// Save complex value elements
#define SET_COMPLEX_PACKED(zp, n, r, i)                                        \
  *((zp) + 2 * (n) + 0) = (r);                                                 \
  *((zp) + 2 * (n) + 1) = (i);

// Some constants

// IEEE long precision 2^{-52}
// # define EPSILON      2.2204460492503131e-16
// IEEE double long precision 2^{-63}
#define EPSILON 1.0842021724855044e-19
#define ROOT_PREC 1e-9
#define ROOT_TOL 1e-7

inline qf_float ROUND_ROOT(qf_float k) {
  if (k > 0) {
    return std::floor(k / ROOT_PREC) * ROOT_PREC;
  } else {
    return std::ceil(k / ROOT_PREC) * ROOT_PREC;
  }
}

#define RADIX 2
#define RADIX2 (RADIX * RADIX)
#define MAX_ITERATIONS 60
} // namespace qf
#endif // _QF_POLY_H
