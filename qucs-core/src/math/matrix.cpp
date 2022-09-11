/*
 * matrix.cpp - matrix class implementation
 *
 * Copyright (C) 2003-2009 Stefan Jahn <stefan@lkcc.org>
 * Copyright (C) 2012-2017 Qucs Team
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
/*!\file matrix.cpp
   \brief Dense matrix class implementation

  References:

  [1] Power Waves and the Scattering Matrix
      Kurokawa, K.
      Microwave Theory and Techniques, IEEE Transactions on,
      Vol.13, Iss.2, Mar 1965
      Pages: 194- 202

  [2] A Rigorous Technique for Measuring the Scattering Matrix of
      a Multiport Device with a 2-Port Network Analyzer
      John C. TIPPET, Ross A. SPECIALE
      Microwave Theory and Techniques, IEEE Transactions on,
      Vol.82, Iss.5, May 1982
      Pages: 661- 666

  [3] Comments on "A Rigorous Technique for Measuring the Scattering
      Matrix of a Multiport Device with a Two-Port Network Analyzer"
      Dropkin, H.
      Microwave Theory and Techniques, IEEE Transactions on,
      Vol. 83, Iss.1, Jan 1983
      Pages: 79 - 81

  [4] Arbitrary Impedance
      "Accurate Measurements In Almost Any
      Impedance Environment"
      in Scropion Application note
      Anritsu
      online (2016-02-21)
      https://dl.cdn-anritsu.com/en-us/test-measurement/files/Application-Notes/Application-Note/11410-00284B.pdf

  [5] Conversions between S, Z, Y, H, ABCD, and T parameters
      which are valid for complex source and load impedances
      Frickey, D.A.
      Microwave Theory and Techniques, IEEE Transactions on
      Vol. 42, Iss. 2, Feb 1994
      pages: 205 - 211
      doi: 10.1109/22.275248

  [6] Comments on "Conversions between S, Z, Y, h, ABCD,
      and T parameters which are valid for complex source and load impedances" [and reply]
      Marks, R.B.; Williams, D.F.; Frickey, D.A.
      Microwave Theory and Techniques, IEEE Transactions on,
      Vol.43, Iss.4, Apr 1995
      Pages: 914- 915
      doi: 10.1109/22.375247

  [7] Wave Techniques for Noise Modeling and Measurement
      S. W. Wedge and D. B. Rutledge,
      IEEE Transactions on Microwave Theory and Techniques,
      vol. 40, no. 11, Nov. 1992.
      pages 2004-2012,
      doi: 10.1109/22.168757
      Author copy online (2007/07/31)
      http://authors.library.caltech.edu/6226/01/WEDieeetmtt92.pdf

*/
#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <assert.h>
#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <cmath>

#include "logging.h"
#include "object.h"
#include "complex.h"
#include "vector.h"
#include "matrix.h"

namespace qucs {

/*!\brief Create an empty matrix

   Constructor creates an unnamed instance of the matrix class.
*/
matrix::matrix () {
  rows = 0;
  cols = 0;
  data = NULL;
}

/*!\brief Creates a square matrix

    Constructor creates an unnamed instance of the matrix class with a
    certain number of rows and columns.  Particularly creates a square matrix.
    \param[in] s number of rows or columns of square matrix
    \todo Why not s const?
*/
matrix::matrix (int s)  {
  rows = cols = s;
  data = (s > 0) ? new nr_complex_t[s * s] : NULL;
}

/*!\brief Creates a matrix

   Constructor creates an unnamed instance of the matrix class with a
   certain number of rows and columns.
   \param[in] r number of rows
   \param[in] c number of column
   \todo Why not r and c constant
   \todo Assert r >= 0 and c >= 0
*/
matrix::matrix (int r, int c)  {
  rows = r;
  cols = c;
  data = (r > 0 && c > 0) ? new nr_complex_t[r * c] : NULL;
}

/*!\brief copy constructor

   The copy constructor creates a new instance based on the given
   matrix object.
   \todo Add assert tests
*/
matrix::matrix (const matrix & m) {
  rows = m.rows;
  cols = m.cols;
  data = NULL;

  // copy matrix elements
  if (rows > 0 && cols > 0) {
    data = new nr_complex_t[rows * cols];
    memcpy (data, m.data, sizeof (nr_complex_t) * rows * cols);
  }
}

/*!\brief Assignment operator

  The assignment copy constructor creates a new instance based on the
  given matrix object.

  \param[in] m object to copy
  \return assigned object
  \note m = m is safe
*/
const matrix& matrix::operator=(const matrix & m) {
  if (&m != this) {
    rows = m.rows;
    cols = m.cols;
    if (data) {
      delete[] data;
      data = NULL;
    }
    if (rows > 0 && cols > 0) {
      data = new nr_complex_t[rows * cols];
      memcpy (data, m.data, sizeof (nr_complex_t) * rows * cols);
    }
  }
  return *this;
}

/*!\brief Destructor

   Destructor deletes a matrix object.
*/
matrix::~matrix () {
  delete[] data;
}

/*!\brief  Returns the matrix element at the given row and column.
   \param[in] r row number
   \param[in] c column number
   \todo Why not inline and synonymous of ()
   \todo c and r const
*/
nr_complex_t matrix::get (int r, int c) {
  return data[r * cols + c];
}

/*!\brief Sets the matrix element at the given row and column.
   \param[in] r row number
   \param[in] c column number
   \param[in] z complex number to assign
   \todo Why not inline and synonymous of ()
   \todo r c and z const
*/
void matrix::set (int r, int c, nr_complex_t z) {
  data[r * cols + c] = z;
}

#ifdef DEBUG
/*!\brief Debug function: Prints the matrix object */
void matrix::print (void) {
  for (int r = 0; r < rows; r++) {
    for (int c = 0; c < cols; c++) {
      fprintf (stderr, "%+.2e,%+.2e ", (double) real (get (r, c)),
      	       (double) imag (get (r, c)));
    }
    fprintf (stderr, "\n");
  }
}
#endif /* DEBUG */

/*!\brief Matrix addition.
   \param[a] first matrix
   \param[b] second matrix
   \note assert same size
   \todo a and b are const
*/
matrix operator + (matrix a, matrix b) {
  assert (a.getRows () == b.getRows () && a.getCols () == b.getCols ());

  matrix res (a.getRows (), a.getCols ());
  for (int r = 0; r < a.getRows (); r++)
    for (int c = 0; c < a.getCols (); c++)
      res.set (r, c, a.get (r, c) + b.get (r, c));
  return res;
}

/*!\brief Intrinsic matrix addition.
   \param[in] a matrix to add
   \note assert same size
   \todo a is const
*/
matrix matrix::operator += (matrix a) {
  assert (a.getRows () == rows && a.getCols () == cols);

  int r, c, i;
  for (i = 0, r = 0; r < a.getRows (); r++)
    for (c = 0; c < a.getCols (); c++, i++)
      data[i] += a.get (r, c);
  return *this;
}

/*!\brief Matrix subtraction.
   \param[a] first matrix
   \param[b] second matrix
   \note assert same size
   \todo a and b are const
*/
matrix operator - (matrix a, matrix b) {
  assert (a.getRows () == b.getRows () && a.getCols () == b.getCols ());

  matrix res (a.getRows (), a.getCols ());
  for (int r = 0; r < a.getRows (); r++)
    for (int c = 0; c < a.getCols (); c++)
      res.set (r, c, a.get (r, c) - b.get (r, c));
  return res;
}

/*!\brief Unary minus. */
matrix matrix::operator - () {
  matrix res (getRows (), getCols ());
  int r, c, i;
  for (i = 0, r = 0; r < getRows (); r++)
    for (c = 0; c < getCols (); c++, i++)
      res.set (r, c, -data[i]);
  return res;
}

/*!\brief Intrinsic matrix subtraction.
   \param[in] a matrix to substract
   \note assert same size
*/
matrix matrix::operator -= (matrix a) {
  assert (a.getRows () == rows && a.getCols () == cols);
  int r, c, i;
  for (i = 0, r = 0; r < a.getRows (); r++)
    for (c = 0; c < a.getCols (); c++, i++)
      data[i] -= a.get (r, c);
  return *this;
}

/*!\brief Matrix scaling complex version
   \param[in] a matrix to scale
   \param[in] z scaling complex
   \return Scaled matrix
   \todo Why not a and z const
*/
matrix operator * (matrix a, nr_complex_t z) {
  matrix res (a.getRows (), a.getCols ());
  for (int r = 0; r < a.getRows (); r++)
    for (int c = 0; c < a.getCols (); c++)
      res.set (r, c, a.get (r, c) * z);
  return res;
}

/*!\brief Matrix scaling complex version (different order)
   \param[in] a matrix to scale
   \param[in] z scaling complex
   \return Scaled matrix
   \todo Why not a and z const
   \todo Why not inline
*/
matrix operator * (nr_complex_t z, matrix a) {
  return a * z;
}

/*!\brief Matrix scaling complex version
   \param[in] a matrix to scale
   \param[in] d scaling real
   \return Scaled matrix
   \todo Why not d and a const
*/
matrix operator * (matrix a, nr_double_t d) {
  matrix res (a.getRows (), a.getCols ());
  for (int r = 0; r < a.getRows (); r++)
    for (int c = 0; c < a.getCols (); c++)
      res.set (r, c, a.get (r, c) * d);
  return res;
}

/*!\brief Matrix scaling real version (different order)
   \param[in] a matrix to scale
   \param[in] d scaling real
   \return Scaled matrix
   \todo Why not inline?
   \todo Why not d and a const
*/
matrix operator * (nr_double_t d, matrix a) {
  return a * d;
}

/*!\brief Matrix scaling division by complex version
   \param[in] a matrix to scale
   \param[in] z scaling complex
   \return Scaled matrix
   \todo Why not a and z const
*/
matrix operator / (matrix a, nr_complex_t z) {
  matrix res (a.getRows (), a.getCols ());
  for (int r = 0; r < a.getRows (); r++)
    for (int c = 0; c < a.getCols (); c++)
      res.set (r, c, a.get (r, c) / z);
  return res;
}

/*!\brief Matrix scaling division by real version
   \param[in] a matrix to scale
   \param[in] d scaling real
   \return Scaled matrix
   \todo Why not a and d const
*/
matrix operator / (matrix a, nr_double_t d) {
  matrix res (a.getRows (), a.getCols ());
  for (int r = 0; r < a.getRows (); r++)
    for (int c = 0; c < a.getCols (); c++)
      res.set (r, c, a.get (r, c) / d);
  return res;
}

/*! Matrix multiplication.

    Dumb and not optimized matrix multiplication
    \param[a] first matrix
    \param[b] second matrix
    \note assert compatibility
    \todo a and b are const
*/
matrix operator * (matrix a, matrix b) {
  assert (a.getCols () == b.getRows ());

  int r, c, i, n = a.getCols ();
  nr_complex_t z;
  matrix res (a.getRows (), b.getCols ());
  for (r = 0; r < a.getRows (); r++) {
    for (c = 0; c < b.getCols (); c++) {
      for (i = 0, z = 0; i < n; i++) z += a.get (r, i) * b.get (i, c);
      res.set (r, c, z);
    }
  }
  return res;
}

/*!\brief Complex scalar addition.
   \param[in] a matrix
   \param[in] z complex to add
   \todo Move near other +
   \todo a and z are const
*/
matrix operator + (matrix a, nr_complex_t z) {
  matrix res (a.getRows (), a.getCols ());
  for (int r = 0; r < a.getRows (); r++)
    for (int c = 0; c < a.getCols (); c++)
      res.set (r, c, a.get (r, c) + z);
  return res;
}

/*!\brief Complex scalar addition different order.
   \param[in] a matrix
   \param[in] z complex to add
   \todo Move near other +
   \todo a and z are const
   \todo Why not inline
*/
matrix operator + (nr_complex_t z, matrix a) {
  return a + z;
}

/*!\brief Real scalar addition.
   \param[in] a matrix
   \param[in] d real to add
   \todo Move near other +
   \todo a and d are const
*/
matrix operator + (matrix a, nr_double_t d) {
  matrix res (a.getRows (), a.getCols ());
  for (int r = 0; r < a.getRows (); r++)
    for (int c = 0; c < a.getCols (); c++)
      res.set (r, c, a.get (r, c) + d);
  return res;
}

/*!\brief Real scalar addition different order.
   \param[in] a matrix
   \param[in] d real to add
   \todo Move near other +
   \todo a and d are const
   \todo Why not inline
*/
matrix operator + (nr_double_t d, matrix a) {
  return a + d;
}

/*!\brief Complex scalar substraction
   \param[in] a matrix
   \param[in] z complex to add
   \todo Move near other +
   \todo a and z are const
   \todo Why not inline
*/
matrix operator - (matrix a, nr_complex_t z) {
  return -z + a;
}

/*!\brief Complex scalar substraction different order
   \param[in] a matrix
   \param[in] z complex to add
   \todo Move near other +
   \todo a and z are const
   \todo Why not inline
*/
matrix operator - (nr_complex_t z, matrix a) {
  return -a + z;
}

/*!\brief Real scalar substraction
   \param[in] a matrix
   \param[in] z real to add
   \todo Move near other +
   \todo a and z are const
   \todo Why not inline
*/
matrix operator - (matrix a, nr_double_t d) {
  return -d + a;
}

/*!\brief Real scalar substraction different order
   \param[in] a matrix
   \param[in] z real to add
   \todo Move near other +
   \todo a and z are const
   \todo Why not inline
*/
matrix operator - (nr_double_t d, matrix a) {
  return -a + d;
}

/*!\brief Matrix transposition
   \param[in] a Matrix to transpose
   \todo add transpose in place
   \todo a is const
*/
matrix transpose (matrix a) {
  matrix res (a.getCols (), a.getRows ());
  for (int r = 0; r < a.getRows (); r++)
    for (int c = 0; c < a.getCols (); c++)
      res.set (c, r, a.get (r, c));
  return res;
}

/*!\brief Conjugate complex matrix.
  \param[in] a Matrix to conjugate
  \todo add conj in place
  \todo a is const
*/
matrix conj (matrix a) {
  matrix res (a.getRows (), a.getCols ());
  for (int r = 0; r < a.getRows (); r++)
    for (int c = 0; c < a.getCols (); c++)
      res.set (r, c, conj (a.get (r, c)));
  return res;
}

/*!\brief adjoint matrix

   The function returns the adjoint complex matrix.  This is also
   called the adjugate or transpose conjugate.
   \param[in] a Matrix to transpose
   \todo add adjoint in place
   \todo Do not lazy and avoid conj and transpose copy
   \todo a is const
*/
matrix adjoint (matrix a) {
  return transpose (conj (a));
}

/*!\brief Computes magnitude of each matrix element.
   \param[in] a matrix
   \todo add abs in place
   \todo a is const
*/
matrix abs (matrix a) {
  matrix res (a.getRows (), a.getCols ());
  for (int r = 0; r < a.getRows (); r++)
    for (int c = 0; c < a.getCols (); c++)
      res.set (r, c, abs (a.get (r, c)));
  return res;
}

/*!\brief Computes magnitude in dB of each matrix element.
   \param[in] a matrix
*/
matrix dB (matrix a) {
  matrix res (a.getRows (), a.getCols ());
  for (int r = 0; r < a.getRows (); r++)
    for (int c = 0; c < a.getCols (); c++)
      res.set (r, c, dB (a.get (r, c)));
  return res;
}

/*!\brief Computes the argument of each matrix element.
   \param[in] a matrix
   \todo add arg in place
   \todo a is const
*/
matrix arg (matrix a) {
  matrix res (a.getRows (), a.getCols ());
  for (int r = 0; r < a.getRows (); r++)
    for (int c = 0; c < a.getCols (); c++)
      res.set (r, c, arg (a.get (r, c)));
  return res;
}

/*!\brief Real part matrix.
   \param[in] a matrix
   \todo add real in place
   \todo a is const
*/
matrix real (matrix a) {
  matrix res (a.getRows (), a.getCols ());
  for (int r = 0; r < a.getRows (); r++)
    for (int c = 0; c < a.getCols (); c++)
      res.set (r, c, real (a.get (r, c)));
  return res;
}

/*!\brief Imaginary part matrix.
   \param[in] a matrix
   \todo add imag in place
   \todo a is const
*/
matrix imag (matrix a) {
  matrix res (a.getRows (), a.getCols ());
  for (int r = 0; r < a.getRows (); r++)
    for (int c = 0; c < a.getCols (); c++)
      res.set (r, c, imag (a.get (r, c)));
  return res;
}

/*!\brief Multiply a matrix by itself
   \param[in] a matrix
*/
matrix sqr (matrix a) {
  return a * a;
}

/*!\brief Create identity matrix with specified number of rows and columns.
   \param[in] rs row number
   \param[in] cs column number
   \todo Avoid res.get*
   \todo Use memset
   \todo rs, cs are const
*/
matrix eye (int rs, int cs) {
  matrix res (rs, cs);
  for (int r = 0; r < res.getRows (); r++)
    for (int c = 0; c < res.getCols (); c++)
      if (r == c) res.set (r, c, 1);
  return res;
}

/*!\brief Create a square identity matrix
   \param[in] s row or column number of square matrix
   \todo Do not by lazy and implement it
   \todo s is const
*/
matrix eye (int s) {
  return eye (s, s);
}

/*!\brief Create a diagonal matrix from a vector
   \param[in] diag vector to write on the diagonal
   \todo diag is const
*/
matrix diagonal (qucs::vector diag) {
  int size = diag.getSize ();
  matrix res (size);
  for (int i = 0; i < size; i++) res (i, i) = diag (i);
  return res;
}

// Compute n-th power of the given matrix.
matrix pow (matrix a, int n) {
  matrix res;
  if (n == 0) {
    res = eye (a.getRows (), a.getCols ());
  }
  else {
    res = a = n < 0 ? inverse (a) : a;
    for (int i = 1; i < std::abs (n); i++)
      res = res * a;
  }
  return res;
}

/*!\brief Computes the complex cofactor of the given determinant

   The cofactor is the determinant obtained by deleting the row and column
   of a given element of a matrix or determinant.
   The cofactor is preceded by a + or - sign depending of the sign
   of \f$(-1)^(u+v)\f$

   \bug This algorithm is recursive! Stack overfull!
   \todo ((u + v) & 1) is cryptic use (u + v)% 2
   \todo #ifdef 0
   \todo static?
*/
nr_complex_t cofactor (matrix a, int u, int v) {
  matrix res (a.getRows () - 1, a.getCols () - 1);
  int r, c, ra, ca;
  for (ra = r = 0; r < res.getRows (); r++, ra++) {
    if (ra == u) ra++;
    for (ca = c = 0; c < res.getCols (); c++, ca++) {
      if (ca == v) ca++;
      res.set (r, c, a.get (ra, ca));
    }
  }
  nr_complex_t z = detLaplace (res);
  return ((u + v) & 1) ? -z : z;
}

/*!\brief Compute determinant of the given matrix using Laplace expansion.

   The Laplace expansion  of the determinant of
   an n by n square matrix a expresses
   the determinant of a as a sum of n determinants of (n-1) by  (n-1)
   sub-matrices of a.  There are 2n such expressions, one for each row
   and column of a.

   See Wikipedia http://en.wikipedia.org/wiki/Laplace_expansion
   \param[in] a matrix
   \bug This algorithm is recursive! Stack overfull!
   \note assert square matrix
   \todo #ifdef 0
   \todo static ?
*/
nr_complex_t detLaplace (matrix a) {
  assert (a.getRows () == a.getCols ());
  int s = a.getRows ();
  nr_complex_t res = 0;
  if (s > 1) {
    /* always use the first row for sub-determinant, but you should
       use the row or column with most zeros in it */
    int r = 0;
    for (int i = 0; i < s; i++) {
      res += a.get (r, i) * cofactor (a, r, i);
    }
    return res;
  }
  /* 1 by 1 matrix */
  else if (s == 1) {
    return a (0, 0);
  }
  /* 0 by 0 matrix */
  return 1;
}

/*!\brief Compute determinant Gaussian algorithm

   Compute determinant of the given matrix using the Gaussian
   algorithm.  This means to triangulate the matrix and multiply all
   the diagonal elements.
   \param[in] a matrix
   \note assert square matrix
   \todo static ?
   \todo a const?
   */
nr_complex_t detGauss (matrix a) {
  assert (a.getRows () == a.getCols ());
  nr_double_t MaxPivot;
  nr_complex_t f, res;
  matrix b;
  int i, c, r, pivot, n = a.getCols ();

  // return special matrix cases
  if (n == 0) return 1;
  if (n == 1) return a (0, 0);

  // make copy of original matrix
  b = matrix (a);

  // triangulate the matrix
  for (res = 1, i = 0; i < n; i++) {
    // find maximum column value for pivoting
    for (MaxPivot = 0, pivot = r = i; r < n; r++) {
      if (abs (b.get (r, i)) > MaxPivot) {
	MaxPivot = abs (b.get (r, i));
	pivot = r;
      }
    }
    // exchange rows if necessary
    assert (MaxPivot != 0);
    if (i != pivot) { b.exchangeRows (i, pivot); res = -res; }
    // compute new rows and columns
    for (r = i + 1; r < n; r++) {
      f = b.get (r, i) / b.get (i, i);
      for (c = i + 1; c < n; c++) {
	b.set (r, c, b.get (r, c) - f * b.get (i, c));
      }
    }
  }

  // now compute determinant by multiplying diagonal
  for (i = 0; i < n; i++) res *= b.get (i, i);
  return res;
}

/*!\brief Compute determinant of the given matrix.
   \param[in] a matrix
   \return Complex determinant
   \todo a const?
*/
nr_complex_t det (matrix a) {
#if 0
  return detLaplace (a);
#else
  return detGauss (a);
#endif
}

/*!\brief Compute inverse matrix using Laplace expansion

  Compute inverse matrix of the given matrix using Laplace expansion.
  \param[in] a matrix to invert
  \todo Static?
  \bug recursive! Stack overflow
  \todo a const?
  \todo #ifdef 0
*/
matrix inverseLaplace (matrix a) {
  matrix res (a.getRows (), a.getCols ());
  nr_complex_t d = detLaplace (a);
  assert (abs (d) != 0); // singular matrix
  for (int r = 0; r < a.getRows (); r++)
    for (int c = 0; c < a.getCols (); c++)
      res.set (r, c, cofactor (a, c, r) / d);
  return res;
}

/*!\brief Compute inverse matrix using Gauss-Jordan elimination

   Compute inverse matrix of the given matrix by Gauss-Jordan
   elimination.
   \todo a const?
   \todo static?
   \note assert non singulat matrix
   \param[in] a matrix to invert
*/
matrix inverseGaussJordan (matrix a) {
  nr_double_t MaxPivot;
  nr_complex_t f;
  matrix b, e;
  int i, c, r, pivot, n = a.getCols ();

  // create temporary matrix and the result matrix
  b = matrix (a);
  e = eye (n);

  // create the eye matrix in 'b' and the result in 'e'
  for (i = 0; i < n; i++) {
    // find maximum column value for pivoting
    for (MaxPivot = 0, pivot = r = i; r < n; r++) {
      if (abs (b (r, i)) > MaxPivot) {
	MaxPivot = abs (b (r, i));
	pivot = r;
      }
    }
    // exchange rows if necessary
    assert (MaxPivot != 0); // singular matrix
    if (i != pivot) {
      b.exchangeRows (i, pivot);
      e.exchangeRows (i, pivot);
    }

    // compute current row
    for (f = b (i, i), c = 0; c < n; c++) {
      b (i, c) /= f;
      e (i, c) /= f;
    }

    // compute new rows and columns
    for (r = 0; r < n; r++) {
      if (r != i) {
	for (f = b (r, i), c = 0; c < n; c++) {
	  b (r, c) -= f * b (i, c);
	  e (r, c) -= f * e (i, c);
	}
      }
    }
  }
  return e;
}

/*!\brief Compute inverse matrix
   \param[in] a matrix to invert
   \todo a is const
*/
matrix inverse (matrix a) {
#if 0
  return inverseLaplace (a);
#else
  return inverseGaussJordan (a);
#endif
}

/*!\brief S params to S params

  Convert scattering parameters with the reference impedance 'zref'
  to scattering parameters with the reference impedance 'z0'.

  Details are given in [1], under equation (32). Brief summary:

  The new scattering matrix \f$S'\f$ is:
  \f[
  S'=A^{-1}(S-\Gamma^+)(I-\Gamma S)^{-1}A^+
  \f]
  where \f$I\f$ is the identity matrix, \f$\Gamma\f$ a diagonal matrix with
  diagonal components \f$\Gamma_i=r_i\f$ and \f$A\f$ a diagonal matrix with
  diagonal components
  \f[
  A_i =  \frac{(1-r_i^*)\sqrt{|1-r_ir_i^*|}}{|1-r_i|}
  \f]
  and \f$r_i\f$ is the power wave reflexion coefficient of \f$Z_i'\f$ with
  respect to \f$Z_i^*\f$ (where \f$Z_i'\f$ is the new impedance and
  \f$Z_i\f$ is the old impedance), i.e.:
  \f[
  r_i = \frac{Z_i'-Z_i}{Z_i'+Z_i^*}
  \f]

  (\f$x^*\f$ denotes the complex conjugate of \f$x\f$ and
   \f$A^+\f$ the complex conjugate transpose (Hermitian adjoint) of \f$A\f$)

  \param[in] s original S matrix
  \param[in] zref original reference impedance
  \param[in] z0 new reference impedance
  \bug The function implements only the real zref and z0 case!
  \todo Correct documentation about standing waves [1-4]
  \todo Implement Speciale implementation [2-3] if applicable
  \return Renormalized scattering matrix
  \todo s, zref and z0 const
*/
matrix stos (matrix s, qucs::vector zref, qucs::vector z0) {
  int d = s.getRows ();
  matrix e, r;
  qucs::vector a;

  assert (d == s.getCols () && d == z0.getSize () && d == zref.getSize ());

  e = eye (d);
  r = diagonal ((z0 - zref) / (z0 + zref));
  a = sqrt (z0 / zref) * 2 * zref / (z0 + zref);
  return diagonal (1 / a) * (s - r) * inverse (e - r * s) * diagonal (a);
}

/*!\brief S renormalization with all part identic
   \param[in] s original S matrix
   \param[in] zref original reference impedance
   \param[in] z0 new reference impedance
   \todo Why not inline
   \return Renormalized scattering matrix
   \todo s, zref and z0 const
*/
matrix stos (matrix s, nr_complex_t zref, nr_complex_t z0) {
  int d = s.getRows ();
  return stos (s, qucs::vector (d, zref), qucs::vector (d, z0));
}

/*!\brief S renormalization with all part identic and real
  \param[in] s original S matrix
  \param[in] zref original reference impedance
  \param[in] z0 new reference impedance
  \todo Why not inline
  \return Renormalized scattering matrix
  \todo s, zref and z0 const
*/
matrix stos (matrix s, nr_double_t zref, nr_double_t z0) {
  return stos (s, nr_complex_t (zref, 0), nr_complex_t (z0, 0));
}

/*!\brief S renormalization (variation)
   \param[in] s original S matrix
   \param[in] zref original reference impedance
   \param[in] z0 new reference impedance
   \todo Why not inline
   \return Renormalized scattering matrix
   \todo s, zref and z0 const
*/
matrix stos (matrix s, qucs::vector zref, nr_complex_t z0) {
  return stos (s, zref, qucs::vector (zref.getSize (), z0));
}

/*!\brief S renormalization (variation)
  \param[in] s original S matrix
  \param[in] zref original reference impedance
  \param[in] z0 new reference impedance
  \todo Why not inline
  \todo s, zref and z0 const
  \return Renormalized scattering matrix
*/
matrix stos (matrix s, nr_complex_t zref, qucs::vector z0) {
  return stos (s, qucs::vector (z0.getSize (), zref), z0);
}

/*!\brief Scattering parameters to impedance matrix

  Convert scattering parameters to impedance matrix.
  According to [1] eq (19):
  \f[
  Z=F^{-1} (I- S)^{-1} (SG + G^+) F
  \f]
  Where \f$S\f$ is the scattering matrix, \f$x^+\f$
  is the adjoint of x, I the identity matrix. The matrix
  F and G are diagonal matrix defined by:
  \f{align*}
  F_i&=\frac{1}{2\sqrt{\Re\text{e}\; Z_i}} \\
  G_i&=Z_i
  \f}
  \param[in] s Scattering matrix
  \param[in] z0 Normalisation impedance
  \note We could safely drop the \f$1/2\f$ in \f$F\f$ because we compute
        \f$FXF^{-1}\f$ and therefore \f$1/2\f$ will simplify.
  \bug not correct if zref is complex
  \todo s, z0 const
  \return Impedance matrix
*/
matrix stoz (matrix s, qucs::vector z0) {
  int d = s.getRows ();
  matrix e, zref, gref;

  assert (d == s.getCols () && d == z0.getSize ());

  e = eye (d);
  zref = diagonal (z0);
  gref = diagonal (sqrt (real (1 / z0)));
  return inverse (gref) * inverse (e - s) * (s * zref + zref) * gref;
}

/*!\brief Scattering parameters to impedance matrix identic case
   \param[in] s Scattering matrix
   \param[in] z0 Normalisation impedance
   \return Impedance matrix
   \todo Why not inline?
   \todo s and z0 const?
*/
matrix stoz (matrix s, nr_complex_t z0) {
  return stoz (s, qucs::vector (s.getRows (), z0));
}

/*!\brief Convert impedance matrix scattering parameters.

   Convert scattering parameters to impedance matrix.
   According to [1] eq (18):
  \f[
  S=F(Z-G^+)(Z+G)^{-1} F^{-1}
  \f]
  Where \f$Z\f$ is the scattering matrix, \f$x^+\f$
  is the adjoint of x, I the identity matrix. The matrix
  F and G are diagonal matrix defined by:
  \f{align*}
  F_i&=\frac{1}{2\sqrt{\Re\text{e}\; Z_i}} \\
  G_i&=Z_i
  \f}
  \param[in] Z Impedance matrix
  \param[in] z0 Normalisation impedance
  \return Scattering matrix
  \note We could safely drop the \f$1/2\f$ in \f$F\f$ because we compute
        \f$FXF^{-1}\f$ and therefore \f$1/2\f$ will simplify.
  \bug not correct if zref is complex
  \todo z and z0 const?
*/
matrix ztos (matrix z, qucs::vector z0) {
  int d = z.getRows ();
  matrix e, zref, gref;

  assert (d == z.getCols () && d == z0.getSize ());

  e = eye (d);
  zref = diagonal (z0);
  gref = diagonal (sqrt (real (1 / z0)));
  return gref * (z - zref) * inverse (z + zref) * inverse (gref);
}

/*!\brief Convert impedance matrix to scattering parameters identic case
   \param[in] Z Impedance matrix
   \param[in] z0 Normalisation impedance
   \return Scattering matrix
   \todo Why not inline
   \todo z and z0 const
 */
matrix ztos (matrix z, nr_complex_t z0) {
  return ztos (z, qucs::vector (z.getRows (), z0));
}

/*!\brief impedance matrix to admittance matrix.

   Convert impedance matrix to admittance matrix. By definition
   \f$Y=Z^{-1}\f$
   \param[in] z impedance matrix
   \return Admittance matrix
   \todo Why not inline
   \todo z const
*/
matrix ztoy (matrix z) {
  assert (z.getRows () == z.getCols ());
  return inverse (z);
}

/*!\brief Scattering parameters to admittance matrix.

  Convert scattering parameters to admittance matrix.
  According to [1] eq (19):
  \f[
  Z=F^{-1} (I- S)^{-1} (SG + G^+) F
  \f]
  Where \f$S\f$ is the scattering matrix, \f$x^+\f$
  is the adjoint of x, I the identity matrix. The matrix
  F and G are diagonal matrix defined by:
  \f{align*}
  F_i&=\frac{1}{2\sqrt{\Re\text{e}\; Z_i}} \\
  G_i&=Z_i
  \f}
  Using the well know formula \f$(AB)^{-1}=B^{1}A^{1}\f$,
  we derivate:
  \f[
  Y=F^{-1} (SG+G^+)^{-1} (I-S) F
  \f]
  \param[in] s Scattering matrix
  \param[in] z0 Normalisation impedance
  \note We could safely drop the \f$1/2\f$ in \f$F\f$ because we compute
        \f$FXF^{-1}\f$ and therefore \f$1/2\f$ will simplify.
  \bug not correct if zref is complex
  \todo s and z0 const
  \return Admittance matrix
*/
matrix stoy (matrix s, qucs::vector z0) {
  int d = s.getRows ();
  matrix e, zref, gref;

  assert (d == s.getCols () && d == z0.getSize ());

  e = eye (d);
  zref = diagonal (z0);
  gref = diagonal (sqrt (real (1 / z0)));
  return inverse (gref) * inverse (s * zref + zref) * (e - s) * gref;
}

/*!\brief Convert scattering pto adminttance parameters identic case
   \param[in] S Scattering matrix
   \param[in] z0 Normalisation impedance
   \return Admittance matrix
   \todo Why not inline
   \todo s and z0 const
 */
matrix stoy (matrix s, nr_complex_t z0) {
  return stoy (s, qucs::vector (s.getRows (), z0));
}

/*!\brief Admittance matrix to scattering parameters

   Convert admittance matrix to scattering parameters.
   Using the same methodology as [1] eq (16-19), but writing
   (16) as \f$i=Yv\f$, ie
   \f[
   S=F(I-G^+Y)(I-GY)^{-1}F^{-1}
   \f]
   Where \f$S\f$ is the scattering matrix, \f$x^+\f$
   is the adjoint of x, I the identity matrix. The matrix
   F and G are diagonal matrix defined by:
   \f{align*}
   F_i&=\frac{1}{2\sqrt{\Re\text{e}\; Z_i}} \\
   G_i&=Z_i
   \f}
   Using the well know formula \f$(AB)^{-1}=B^{1}A^{1}\f$,
   we derivate:
   \f[
   Y=F^{-1} (SG+G^+)^{-1} (I-S) F
   \f]
   \param[in] y admittance matrix
   \param[in] z0 Normalisation impedance
   \note We could safely drop the \f$1/2\f$ in \f$F\f$ because we compute
         \f$FXF^{-1}\f$ and therefore \f$1/2\f$ will simplify.
   \bug not correct if zref is complex
   \todo why not y and z0 const
   \return Scattering matrix
*/
matrix ytos (matrix y, qucs::vector z0) {
  int d = y.getRows ();
  matrix e, zref, gref;

  assert (d == y.getCols () && d == z0.getSize ());

  e = eye (d);
  zref = diagonal (z0);
  gref = diagonal (sqrt (real (1 / z0)));
  return gref * (e - zref * y) * inverse (e + zref * y) * inverse (gref);
}
/*!\brief Convert Admittance matrix to scattering parameters identic case
   \param[in] y Admittance matrix
   \param[in] z0 Normalisation impedance
   \return Scattering matrix
   \todo Why not inline
   \todo y and z0 const
 */
matrix ytos (matrix y, nr_complex_t z0) {
  return ytos (y, qucs::vector (y.getRows (), z0));
}
/*!\brief Converts chain matrix to scattering parameters.

    Converts scattering parameters to chain matrix.
    Formulae are given by [5] tab 1. and are remembered here:

    \f{align*}
    A&=\frac{(Z_{01}^*+S_{11}Z_{01})(1-S_{22})
                 +S_{12}S_{21}Z_{01}}{\Delta} \\
    B&=\frac{(Z_{01}^*+S_{11}Z_{01})(Z_{02}^*+S_{22}Z_{02})
                 -S_{12}S_{21}Z_{01}Z_{02}}{\Delta} \\
    C&=\frac{(1-S_{11})(1-S_{22})
                 -S_{12}S_{21}}{\Delta} \\
    D&=\frac{(1-S_{11})(Z_{02}^*+S_{22}Z_{02})
                 +S_{12}S_{21}Z_{02}}{\Delta}
    \f}
    Where:
    \f[
    \Delta = 2 S_{21}\sqrt{\Re\text{e}\;Z_{01}\Re\text{e}\;Z_{02}}
    \f]
    \bug Do not need fabs
    \param[in] s Scattering matrix
    \param[in] z1 impedance at input 1
    \param[in] z2 impedance at input 2
    \return Chain matrix
    \note Assert 2 by 2 matrix
    \todo Why not s,z1,z2 const
*/
matrix stoa (matrix s, nr_complex_t z1, nr_complex_t z2) {
  nr_complex_t d = s (0, 0) * s (1, 1) - s (0, 1) * s (1, 0);
  nr_complex_t n = 2.0 * s (1, 0) * sqrt (fabs (real (z1) * real (z2)));
  matrix a (2);

  assert (s.getRows () >= 2 && s.getCols () >= 2);

  a.set (0, 0, (conj (z1) + z1 * s (0, 0) -
		conj (z1) * s (1, 1) - z1 * d) / n);
  a.set (0, 1, (conj (z1) * conj (z2) + z1 * conj (z2) * s (0, 0) +
		conj (z1) * z2 * s (1, 1) + z1 * z2 * d) / n);
  a.set (1, 0, (1.0 - s (0, 0) - s (1, 1) + d) / n);
  a.set (1, 1, (conj (z2) - conj (z2) * s (0, 0) +
		z2 * s (1, 1) - z2 * d) / n);
  return a;
}


/*!\brief Converts chain matrix to scattering parameters.

    Converts chain matrix to scattering parameters
    Formulae are given by [5] and are remembered here:
    \f{align*}
    S_{11}&=\frac{AZ_{02}+B-CZ_{01}^*Z_{02}-DZ_{01}^*}{\Delta} \\
    S_{12}&=\frac{2(AD-BC)
                  (\Re\text{e}\;Z_{01}\Re\text{e}\;Z_{02})^{1/2}}
                {\Delta}\\
    S_{21}&=\frac{2(\Re\text{e}\;Z_{01}\Re\text{e}\;Z_{02})^{1/2}}{\Delta}\\
    S_{22}&=\frac{-AZ_{02}^*+B-CZ_{01}^*Z_{02}+DZ_{01}}{\Delta}
    \f}
    Where:
    \f[
    \Delta =AZ_{02}+B+CZ_{01}Z_{02}-DZ_{01}
    \f]
    \param[in] a Chain matrix
    \param[in] z1 impedance at input 1
    \param[in] z2 impedance at input 2
    \return Scattering matrix
    \bug Do not use fabs
    \todo a, z1, z2 const
*/
matrix atos (matrix a, nr_complex_t z1, nr_complex_t z2) {
  nr_complex_t d = 2.0 * sqrt (fabs (real (z1) * real (z2)));
  nr_complex_t n = a (0, 0) * z2 + a (0, 1) +
    a (1, 0) * z1 * z2 + a (1, 1) * z1;
  matrix s (2);

  assert (a.getRows () >= 2 && a.getCols () >= 2);

  s.set (0, 0, (a (0, 0) * z2 + a (0, 1)
                - a (1, 0) * conj (z1) * z2 - a (1, 1) * conj (z1)) / n);
  s.set (0, 1, (a (0, 0) * a (1, 1) -
		a (0, 1) * a (1, 0)) * d / n);
  s.set (1, 0, d / n);
  s.set (1, 1, (a (1, 1) * z1 - a (0, 0) * conj (z2) +
		a (0, 1) - a (1, 0) * z1 * conj (z2)) / n);
  return s;
}

/*!\brief Converts scattering parameters to hybrid matrix.

    Converts chain matrix to scattering parameters
    Formulae are given by [5] and are remembered here:
    \f{align*}
    h_{11}&=\frac{(Z_{01}^*+S_{11}Z_{01})
                  (Z_{02}^*+S_{22}Z_{02})
		  -S_{12}S_{21}Z_{01}Z_{02}}{\Delta}\\
    h_{12}&=\frac{2S_{12}
                  (\Re\text{e}\;Z_{01} \Re\text{e}\;Z_{02})^\frac{1}{2}}
                 {\Delta} \\
    h_{21}&=\frac{-2S_{21}
                  (\Re\text{e}\;Z_{01} \Re\text{e}\;Z_{02})^\frac{1}{2}}
                 {\Delta} \\

    h_{22}&=\frac{(1-S_{11})(1-S_{22})-S_{12}S_{21}}{\Delta}
    \f}
    Where \f$\Delta\f$ is:
    \f[
    \Delta=(1-S_{11})(Z_{02}^*+S_{22}Z_{02})+S_{12}S_{21}Z_{02}
    \f]
    \bug{Programmed formulae are valid only for Z real}
    \param[in] s Scattering matrix
    \param[in] z1 impedance at input 1
    \param[in] z2 impedance at input 2
    \return hybrid matrix
    \note Assert 2 by 2 matrix
    \todo Why not s,z1,z2 const
 */
matrix stoh (matrix s, nr_complex_t z1, nr_complex_t z2) {
  nr_complex_t n = s (0, 1) * s (1, 0);
  nr_complex_t d = (1.0 - s (0, 0)) * (1.0 + s (1, 1)) + n;
  matrix h (2);

  assert (s.getRows () >= 2 && s.getCols () >= 2);

  h.set (0, 0, ((1.0 + s (0, 0)) * (1.0 + s (1, 1)) - n) * z1 / d);
  h.set (0, 1, +2.0 * s (0, 1) / d);
  h.set (1, 0, -2.0 * s (1, 0) / d);
  h.set (1, 1, ((1.0 - s (0, 0)) * (1.0 - s (1, 1)) - n) / z2 / d);
  return h;
}

/*!\brief Converts hybrid matrix to scattering parameters.

    Formulae are given by [5] and are remembered here:
    \f{align*}
    S_{11}&=\frac{(h_{11}-Z_{01}^*)(1+h_{22}Z_{02})-h_{12}h_{21}Z_{02}}
                 {\Delta}\\
    S_{12}&=\frac{-2h_{12}(\Re\text{e}\;Z_{01}\Re\text{e}\;Z_{02})^\frac{1}{2}}
                 {\Delta}\\
    S_{21}&=\frac{-2h_{21}(\Re\text{e}\;Z_{01}\Re\text{e}\;Z_{02})^\frac{1}{2}}
                 {\Delta}\\
    S_{22}&=\frac{(h_{11}+Z_{01})(1-h_{22}Z_{02}^*)-h_{12}h_{21}Z_{02}^*}
                 {\Delta}
   \f}
   Where \f$\Delta\f$ is:
   \f[
   \Delta=(Z_{01}+h_{11})(1+h_{22}Z_{02})-h_{12}h_{21}Z_{02}
   \f]
   \param[in] h hybrid matrix
   \param[in] z1 impedance at input 1
   \param[in] z2 impedance at input 2
   \return scattering matrix
   \note Assert 2 by 2 matrix
   \todo Why not h,z1,z2 const
*/
matrix htos (matrix h, nr_complex_t z1, nr_complex_t z2) {
  nr_complex_t n = h (0, 1) * h (1, 0);
  nr_complex_t d = (1.0 + h (0, 0) / z1) * (1.0 + z2 * h (1, 1)) - n;
  matrix s (2);

  assert (h.getRows () >= 2 && h.getCols () >= 2);

  s.set (0, 0, ((h (0, 0) / z1 - 1.0) * (1.0 + z2 * h (1, 1)) - n) / d);
  s.set (0, 1, +2.0 * h (0, 1) / d);
  s.set (1, 0, -2.0 * h (1, 0) / d);
  s.set (1, 1, ((1.0 + h (0, 0) / z1) * (1.0 - z2 * h (1, 1)) + n) / d);
  return s;
}

/*\brief Converts scattering parameters to second hybrid matrix.
  \bug{Programmed formulae are valid only for Z real}
  \bug{Not documented and references}
  \param[in] s Scattering matrix
  \param[in] z1 impedance at input 1
  \param[in] z2 impedance at input 2
  \return second hybrid matrix
  \note Assert 2 by 2 matrix
  \todo Why not s,z1,z2 const
*/
matrix stog (matrix s, nr_complex_t z1, nr_complex_t z2) {
  nr_complex_t n = s (0, 1) * s (1, 0);
  nr_complex_t d = (1.0 + s (0, 0)) * (1.0 - s (1, 1)) + n;
  matrix g (2);

  assert (s.getRows () >= 2 && s.getCols () >= 2);

  g.set (0, 0, ((1.0 - s (0, 0)) * (1.0 - s (1, 1)) - n) / z1 / d);
  g.set (0, 1, -2.0 * s (0, 1) / d);
  g.set (1, 0, +2.0 * s (1, 0) / d);
  g.set (1, 1, ((1.0 + s (0, 0)) * (1.0 + s (1, 1)) - n) * z2 / d);
  return g;
}

/*\brief Converts second hybrid matrix to scattering parameters.
  \bug{Programmed formulae are valid only for Z real}
  \bug{Not documented and references}
  \param[in] g second hybrid matrix
  \param[in] z1 impedance at input 1
  \param[in] z2 impedance at input 2
  \return scattering matrix
  \note Assert 2 by 2 matrix
  \todo Why not g,z1,z2 const
*/
matrix gtos (matrix g, nr_complex_t z1, nr_complex_t z2) {
  nr_complex_t n = g (0, 1) * g (1, 0);
  nr_complex_t d = (1.0 + g (0, 0) * z1) * (1.0 + g (1, 1) / z2) - n;
  matrix s (2);

  assert (g.getRows () >= 2 && g.getCols () >= 2);

  s.set (0, 0, ((1.0 - g (0, 0) * z1) * (1.0 + g (1, 1) / z2) + n) / d);
  s.set (0, 1, -2.0 * g (0, 1) / d);
  s.set (1, 0, +2.0 * g (1, 0) / d);
  s.set (1, 1, ((g (0, 0) * z1 + 1.0) * (g (1, 1) / z2 - 1.0) - n) / d);
  return s;
}

/*!\brief Convert admittance matrix to impedance matrix.

  Convert \f$Y\f$ matrix to \f$Z\f$ matrix using well known relation
  \f$Z=Y^{-1}\f$
  \param[in] y admittance matrix
  \return Impedance matrix
  \note Check if y matrix is a square matrix
  \todo Why not inline
  \todo y const
  \todo move near ztoy()
*/
matrix ytoz (matrix y) {
  assert (y.getRows () == y.getCols ());
  return inverse (y);
}

/*!\brief Admittance noise correlation matrix to S-parameter noise
   correlation matrix

   Converts admittance noise correlation matrix to S-parameter noise
   correlation matrix. According to [7] fig 2:
   \f[
   C_s=\frac{1}{4}(I+S)C_y(I+S)^+
   \f]
   Where \f$C_s\f$ is the scattering noise correlation matrix,
   \f$C_y\f$ the admittance noise correlation matrix, \f$I\f$
    the identity matrix and \f$S\f$ the scattering matrix
    of device. \f$x^+\f$ is the adjoint of \f$x\f$
   \warning cy matrix and s matrix are assumed to be normalized
   \param[in] cy Admittance noise correlation
   \param[in] s S parameter matrix of device
   \return S-parameter noise correlation matrix
   \note Assert compatibility of matrix
   \todo cy s const
*/
matrix cytocs (matrix cy, matrix s) {
  matrix e = eye (s.getRows ());

  assert (cy.getRows () == cy.getCols () && s.getRows () == s.getCols () &&
	  cy.getRows () == s.getRows ());

  return (e + s) * cy * adjoint (e + s) / 4;
}

/*!\brief Converts S-parameter noise correlation matrix to admittance noise
    correlation matrix.

    According to [7] fig 2:
    \f[
    C_y=(I+Y)C_s(I+Y)^+
    \f]
    Where \f$C_s\f$ is the scattering noise correlation matrix,
    \f$C_y\f$ the admittance noise correlation matrix, \f$I\f$
    the identity matrix and \f$S\f$ the scattering matrix
    of device. \f$x^+\f$ is the adjoint of \f$x\f$
    \warning cs matrix and y matrix are assumed to be normalized
    \param[in]  cs S parameter noise correlation
    \param[in] y Admittance matrix of device
    \return admittance noise correlation matrix
    \todo cs, y const
*/
matrix cstocy (matrix cs, matrix y) {
  matrix e = eye (y.getRows ());

  assert (cs.getRows () == cs.getCols () && y.getRows () == y.getCols () &&
	  cs.getRows () == y.getRows ());

  return (e + y) * cs * adjoint (e + y);
}

/*!\brief Converts impedance noise correlation matrix to S-parameter noise
   correlation matrix.

   According to [7] fig 2:
   \f[
   C_s=\frac{1}{4}(I-S)C_z(I-S)
   \f]
   Where \f$C_s\f$ is the scattering noise correlation matrix,
   \f$C_z\f$ the impedance noise correlation matrix, \f$I\f$
    the identity matrix and \f$S\f$ the scattering matrix
    of device. \f$x^+\f$ is the adjoint of \f$x\f$
   \warning Cz matrix and s matrix are assumed to be normalized
   \param[in] cz Impedance noise correlation
   \param[in] s S parameter matrix of device
   \return S-parameter noise correlation matrix
   \note Assert compatibility of matrix
   \todo cz, s const
*/
matrix cztocs (matrix cz, matrix s) {
  matrix e = eye (s.getRows ());

  assert (cz.getRows () == cz.getCols () && s.getRows () == s.getCols () &&
	  cz.getRows () == s.getRows ());

  return (e - s) * cz * adjoint (e - s) / 4;
}

/*!\brief Converts S-parameter noise correlation matrix to impedance noise
    correlation matrix.

    According to [7] fig 2:
    \f[
    C_z=(I+Z)C_s(I+Z)^+
    \f]
    Where \f$C_s\f$ is the scattering noise correlation matrix,
    \f$C_z\f$ the impedance noise correlation matrix, \f$I\f$
    the identity matrix and \f$S\f$ the scattering matrix
    of device. \f$x^+\f$ is the adjoint of \f$x\f$
    \warning cs matrix and y matrix are assumed to be normalized
    \param[in]  cs S parameter noise correlation
    \param[in] z Impedance matrix of device
    \return Impedance noise correlation matrix
    \todo cs, z const
*/
matrix cstocz (matrix cs, matrix z) {
  assert (cs.getRows () == cs.getCols () && z.getRows () == z.getCols () &&
	  cs.getRows () == z.getRows ());
  matrix e = eye (z.getRows ());
  return (e + z) * cs * adjoint (e + z);
}

/*!\brief Converts impedance noise correlation matrix to admittance noise
    correlation matrix.

    According to [7] fig 2:
    \f[
    C_y=YC_zY^+
    \f]
    Where \f$C_z\f$ is the impedance correlation matrix,
    \f$I\f$ the identity matrix and \f$C_y\f$ the admittance noise
    correlation matrix.
    \f$x^+\f$ is the adjoint of \f$x\f$
    \warning cz matrix and y matrix are assumed to be normalized
    \param[in]  cz impedance noise correlation
    \param[in]  y Admittance matrix of device
    \return admittance noise correlation matrix
    \todo cs, y const
*/
matrix cztocy (matrix cz, matrix y) {
  assert (cz.getRows () == cz.getCols () && y.getRows () == y.getCols () &&
	  cz.getRows () == y.getRows ());

  return y * cz * adjoint (y);
}

/*!\brief Converts admittance noise correlation matrix to impedance noise
    correlation matrix.

    According to [7] fig 2:
    \f[
    C_z=ZC_yZ^+
    \f]
    Where \f$C_z\f$ is the impedance correlation matrix,
    \f$I\f$ the identity matrix and \f$C_y\f$ the admittance noise
    correlation matrix.
    \f$x^+\f$ is the adjoint of \f$x\f$
    \warning cy matrix and z matrix are assumed to be normalized
    \param[in]  cy Admittance noise correlation
    \param[in]  z Impedance matrix of device
    \return Impedance noise correlation matrix
    \todo cs, z const
*/
matrix cytocz (matrix cy, matrix z) {
  assert (cy.getRows () == cy.getCols () && z.getRows () == z.getCols () &&
	  cy.getRows () == z.getRows ());
  return z * cy * adjoint (z);
}

/*!\brief The function swaps the given rows with each other.
  \param[in] r1 source row
  \param[in] r2 destination row
  \note assert not out of bound r1 and r2
  \todo r1 and r2 const
*/
void matrix::exchangeRows (int r1, int r2) {
  nr_complex_t * s = new nr_complex_t[cols];
  int len = sizeof (nr_complex_t) * cols;

  assert (r1 >= 0 && r2 >= 0 && r1 < rows && r2 < rows);

  memcpy (s, &data[r1 * cols], len);
  memcpy (&data[r1 * cols], &data[r2 * cols], len);
  memcpy (&data[r2 * cols], s, len);
  delete[] s;
}

/*!\brief The function swaps the given column with each other.
  \param[in] c1 source column
  \param[in] c2 destination column
  \note assert not out of bound r1 and r2
  \todo c1 and c2 const
*/
void matrix::exchangeCols (int c1, int c2) {
  nr_complex_t s;

  assert (c1 >= 0 && c2 >= 0 && c1 < cols && c2 < cols);

  for (int r = 0; r < rows * cols; r += cols) {
    s = data[r + c1];
    data[r + c1] = data[r + c2];
    data[r + c2] = s;
  }
}

/*!\brief Return a generic rectangular submatrix

  Extract a block of size (p,q), starting at (i,j)
  \param[in] i start row
  \param[in] j start column
  \param[in] p number of rows for the submatrix
  \param[in] q number of columns for the submatrix
  \todo check that (p+i)<rows and (q+j)<cols?
*/
matrix matrix::getBlock(int i, int j, int p, int q) {
  matrix bm = matrix(p, q);

  // copy matrix elements
  for (int r = 0; r < p; r++) {
    for (int c = 0; c < q; c++) {
      bm.set(r, c, data[(r + i) * cols + c + j]);
    }
  }
  return bm;
}

/*!\brief Return a rectangular submatrix flush against the top-left corner

  Extract a block of size p by q flush against the top-left corner
  \param[in] p number of rows for the submatrix
  \param[in] q number of columns for the submatrix
*/
matrix matrix::getTopLeftCorner(int p, int q) {
  return getBlock(0, 0, p, q);
}

/*!\brief Return a rectangular submatrix flush against the bottom-left corner

  Extract a block of size p by q flush against the bottom-left corner
  \param[in] p number of rows for the submatrix
  \param[in] q number of columns for the submatrix
*/
matrix matrix::getBottomLeftCorner(int p, int q) {
  // bottom-left p by q block
  return getBlock(getRows()-p, 0, p, q);
}

/*!\brief Return a rectangular submatrix flush against the top-right corner

  Extract a block of size p by q flush against the top-right corner
  \param[in] p number of rows for the submatrix
  \param[in] q number of columns for the submatrix
*/
matrix matrix::getTopRightCorner(int p, int q) {
  // top-right p by q block
  return getBlock(0, getCols()-q, p, q);
}

/*!\brief Return a rectangular submatrix flush against the bottom-right corner

  Extract a block of size p by q flush against the bottom-right corner
  \param[in] p number of rows for the submatrix
  \param[in] q number of columns for the submatrix
*/
matrix matrix::getBottomRightCorner(int p, int q) {
  // bottom-right p by q block
  return getBlock(getRows()-p, getCols()-q, p, q);
}

/*!\brief Set a generic rectangular submatrix portion

  Assign a block of size (p,q), starting at (i,j)
  \param[in] m matrix block to assign
  \param[in] i start row
  \param[in] j start column
  \param[in] p number of rows to assign
  \param[in] q number of columns to assign
  \return matrix with assigned submatrix portion
  \todo check that (p+i)<rows and (q+j)<cols?
*/
void matrix::setBlock(matrix m, int i, int j, int p, int q) {
  // copy matrix elements
  for (int r = 0; r < p; r++) {
    for (int c = 0; c < q; c++) {
      data[(r + i) * cols + c + j] = m.get(r, c);
    }
  }
}

/*!\brief Set a rectangular submatrix portion adjacent to the top-left corner

  Assign a block of size p by q, starting at the top-left corner
  \param[in] m matrix block to assign
  \param[in] p number of rows to assign
  \param[in] q number of columns to assign
  \return matrix with assigned submatrix portion
*/
void matrix::setTopLeftCorner(matrix m, int p, int q) {
  setBlock(m, 0, 0, p, q);
}

/*!\brief Set a rectangular submatrix portion adjacent to the bottom-left corner

  Assign a block of size p by q, starting at the bottom-left corner
  \param[in] m matrix block to assign
  \param[in] p number of rows to assign
  \param[in] q number of columns to assign
  \return matrix with assigned submatrix portion
*/
void matrix::setBottomLeftCorner(matrix m, int p, int q) {
  setBlock(m, getRows()-p, 0, p, q);
}

/*!\brief Set a rectangular submatrix portion adjacent to the top-right corner

  Assign a block of size p by q, starting at the top-right corner
  \param[in] m matrix block to assign
  \param[in] p number of rows to assign
  \param[in] q number of columns to assign
  \return matrix with assigned submatrix portion
*/
void matrix::setTopRightCorner(matrix m, int p, int q) {
  setBlock(m, 0, getCols()-q, p, q);
}

/*!\brief Set a rectangular submatrix portion adjacent to the bottom-right corner

  Assign a block of size p by q, starting at the bottom-right corner
  \param[in] m matrix block to assign
  \param[in] p number of rows to assign
  \param[in] q number of columns to assign
  \return matrix with assigned submatrix portion
*/
void matrix::setBottomRightCorner(matrix m, int p, int q) {
  setBlock(m, getRows()-p, getCols()-q, p, q);
}

/*!\brief Generic conversion matrix

  This function converts 2x2 matrices from any of the matrix forms Y,
  Z, H, G and A to any other.  Also converts S<->(A, T, H, Y and Z)
  matrices.
  Conversion assumed:

  Y->Y, Y->Z, Y->H, Y->G, Y->A, Y->S,
  Z->Y, Z->Z, Z->H, Z->G, Z->A, Z->S,
  H->Y, H->Z, H->H, H->G, H->A, H->S,
  G->Y, G->Z, G->H, G->G, G->A, G->S,
  A->Y, A->Z, A->H, A->G, A->A, A->S,
  S->Y, S->Z, S->H, S->G, S->A, S->S,
  S->T,T->T,T->S
  \note assert 2x2 matrix
  \param[in] m base matrix
  \param[in] in matrix
  \param[in] out matrix
  \return matrix given by format out
  \todo m, in, out const
*/
matrix twoport (matrix m, char in, char out) {
  assert (m.getRows () >= 2 && m.getCols () >= 2);
  nr_complex_t d;
  matrix res (2);

  switch (in) {
  case 'Y':
    switch (out) {
    case 'Y': // Y to Y
      res = m;
      break;
    case 'Z': // Y to Z
      d = m (0, 0) * m (1, 1) - m (0, 1) * m (1, 0);
      res.set (0, 0, m (1, 1) / d);
      res.set (0, 1, -m (0, 1) / d);
      res.set (1, 0, -m (1, 0) / d);
      res.set (1, 1, m (0, 0) / d);
      break;
    case 'H': // Y to H
      d = m (0, 0);
      res.set (0, 0, 1.0 / d);
      res.set (0, 1, -m (0, 1) / d);
      res.set (1, 0, m (1, 0) / d);
      res.set (1, 1, m (1, 1) - m (0, 1) * m (1, 0) / d);
      break;
    case 'G': // Y to G
      d = m (1, 1);
      res.set (0, 0, m (0, 0) - m (0, 1) * m (1, 0) / d);
      res.set (0, 1, m (0, 1) / d);
      res.set (1, 0, -m (1, 0) / d);
      res.set (1, 1, 1.0 / d);
      break;
    case 'A': // Y to A
      d = m (1, 0);
      res.set (0, 0, -m (1, 1) / d);
      res.set (0, 1, -1.0 / d);
      res.set (1, 0, m (0, 1) - m (1, 1) * m (0, 0) / d);
      res.set (1, 1, -m (0, 0) / d);
      break;
    case 'S': // Y to S
      res = ytos (m);
      break;
    }
    break;
  case 'Z':
    switch (out) {
    case 'Y': // Z to Y
      d = m (0, 0) * m (1, 1) - m (0, 1) * m (1, 0);
      res.set (0, 0, m (1, 1) / d);
      res.set (0, 1, -m (0, 1) / d);
      res.set (1, 0, -m (1, 0) / d);
      res.set (1, 1, m (0, 0) / d);
      break;
    case 'Z': // Z to Z
      res = m;
      break;
    case 'H': // Z to H
      d = m (1, 1);
      res.set (0, 0, m (0, 0) - m (0, 1) * m (1, 0) / d);
      res.set (0, 1, m (0, 1) / d);
      res.set (1, 0, -m (1, 0) / d);
      res.set (1, 1, 1.0 / d);
      break;
    case 'G': // Z to G
      d = m (0, 0);
      res.set (0, 0, 1.0 / d);
      res.set (0, 1, -m (0, 1) / d);
      res.set (1, 0, m (1, 0) / d);
      res.set (1, 1, m (1, 1) - m (0, 1) * m (1, 0) / d);
      break;
    case 'A': // Z to A
      d = m (1, 0);
      res.set (0, 0, m (0, 0) / d);
      res.set (0, 1, m (0, 0) * m (1, 1) / d - m (0, 1));
      res.set (1, 0, 1.0 / d);
      res.set (1, 1, m (1, 1) / d);
      break;
    case 'S': // Z to S
      res = ztos (m);
      break;
    }
    break;
  case 'H':
    switch (out) {
    case 'Y': // H to Y
      d = m (0, 0);
      res.set (0, 0, 1.0 / d);
      res.set (0, 1, -m (0, 1) / d);
      res.set (1, 0, m (1, 0) / d);
      res.set (1, 1, m (1, 1) - m (0, 1) * m.get(2, 1) / d);
      break;
    case 'Z': // H to Z
      d = m (1, 1);
      res.set (0, 0, m (0, 0) - m (0, 1) * m (1, 0) / d);
      res.set (0, 1, m (0, 1) / d);
      res.set (1, 0, -m (1, 0) / d);
      res.set (1, 1, 1.0 / d);
      break;
    case 'H': // H to H
      res = m;
      break;
    case 'G': // H to G
      d = m (0, 0) * m (1, 1) - m (0, 1) * m (1, 0);
      res.set (0, 0, m (1, 1) / d);
      res.set (0, 1, -m (0, 1) / d);
      res.set (1, 0, -m (1, 0) / d);
      res.set (1, 1, m (0, 0) / d);
      break;
    case 'A': // H to A
      d = m (1, 0);
      res.set (0, 0, m (0, 1) - m (0, 0) * m (1, 1) / d);
      res.set (0, 1, -m (0, 0) / d);
      res.set (1, 0, -m (1, 1) / d);
      res.set (1, 1, -1.0 / d);
      break;
    case 'S': // H to S
      res = htos (m);
      break;
    }
    break;
  case 'G':
    switch (out) {
    case 'Y': // G to Y
      d = m (1, 1);
      res.set (0, 0, m (0, 0) - m (0, 1) * m (1, 0) / d);
      res.set (0, 1, m (0, 1) / d);
      res.set (1, 0, -m (1, 0) / d);
      res.set (1, 1, 1.0 / d);
      break;
    case 'Z': // G to Z
      d = m (0, 0);
      res.set (0, 0, 1.0 / d);
      res.set (0, 1, -m (0, 1) / d);
      res.set (1, 0, m (1, 0) / d);
      res.set (1, 1, m (1, 1) - m (0, 1) * m (1, 0) / d);
      break;
    case 'H': // G to H
      d = m (0, 0) * m (1, 1) - m (0, 1) * m (1, 0);
      res.set (0, 0, m (1, 1) / d);
      res.set (0, 1, -m (0, 1) / d);
      res.set (1, 0, -m (1, 0) / d);
      res.set (1, 1, m (0, 0) / d);
      break;
    case 'G': // G to G
      res = m;
      break;
    case 'A': // G to A
      d = m (1, 0);
      res.set (0, 0, 1.0 / d);
      res.set (0, 1, m (1, 1) / d);
      res.set (1, 0, m (0, 0) / d);
      res.set (1, 1, m (0, 0) * m (1, 1) / d - m (0, 1));
      break;
    case 'S': // G to S
      res = gtos (m);
      break;
    }
    break;
  case 'A':
    switch (out) {
    case 'Y': // A to Y
      d = m (0, 1);
      res.set (0, 0, m (1, 1) / d);
      res.set (0, 1, m (1, 0) - m (0, 0) * m (1, 1) / d);
      res.set (1, 0, -1.0 / d);
      res.set (1, 1, m (0, 0) / d);
      break;
    case 'Z': // A to Z
      d = m (1, 0);
      res.set (0, 0, m (0, 0) / d);
      res.set (0, 1, m (0, 0) * m (1, 1) / d - m (0, 1));
      res.set (1, 0, 1.0 / d);
      res.set (1, 1, m (1, 1) / d);
      break;
    case 'H': // A to H
      d = m (1, 1);
      res.set (0, 0, m (0, 1) / d);
      res.set (0, 1, m (0, 0) - m (0, 1) * m (1, 0) / d);
      res.set (1, 0, -1.0 / d);
      res.set (1, 1, m (1, 0) / d);
      break;
    case 'G': // A to G
      d = m (0, 0);
      res.set (0, 0, m (1, 0) / d);
      res.set (0, 1, m (1, 0) * m (0, 1) / d - m (1, 1));
      res.set (1, 0, 1.0 / d);
      res.set (1, 1, m (0, 1) / d);
      break;
    case 'A': // A to A
      res = m;
      break;
    case 'S': // A to S
      res = atos (m);
      break;
    }
    break;
  case 'S':
    switch (out) {
    case 'S': // S to S
      res = m;
      break;
    case 'T': // S to T
      d = m (1, 0);
      res.set (0, 0, m (0, 1) - m (0, 0) * m (1, 1) / d);
      res.set (0, 1, m (0, 0) / d);
      res.set (1, 0, -m (1, 1) / d);
      res.set (0, 1, 1.0 / d);
      break;
    case 'A': // S to A
      res = stoa (m);
      break;
    case 'H': // S to H
      res = stoh (m);
      break;
    case 'G': // S to G
      res = stog (m);
      break;
    case 'Y': // S to Y
      res = stoy (m);
      break;
    case 'Z': // S to Z
      res = stoz (m);
      break;
    }
    break;
  case 'T':
    switch (out) {
    case 'S': // T to S
      d = m (1, 1);
      res.set (0, 0, m (0, 1) / d);
      res.set (0, 1, m (0, 0) - m (0, 1) * m (1, 0) / d);
      res.set (1, 0, 1.0 / d);
      res.set (0, 1, -m (1, 0) / d);
      break;
    case 'T': // T to T
      res = m;
      break;
    }
    break;
  }
  return res;
}

/*!\brief Compute the Rollet stability factor

   The function returns the Rollet stability factor (\f$K\f) of the given
   S-parameter matrix:
   \[
   K=\frac{1-|S_{11}|^2-|S_{22}|^2+|\delta|^2}{2|S_{12}S_{21}|}
   \]
   Where:
   \[
   \Delta=S_{11}S_{22}-S_{12}S_{21}
   \]
   \param[in] m S parameter matrix
   \return Rollet factor
   \note Assert 2x2 matrix
   \todo m const?
   \todo Rewrite with abs and expand det. It is cleaner.
*/
nr_double_t rollet (matrix m) {
  assert (m.getRows () >= 2 && m.getCols () >= 2);
  nr_double_t res;
  res = (1 - norm (m (0, 0)) - norm (m (1, 1)) + norm (det (m))) /
    2 / abs (m (0, 1) * m (1, 0));
  return res;
}

/* Computes stability measure B1 of the given S-parameter matrix. */
nr_double_t b1 (matrix m) {
  assert (m.getRows () >= 2 && m.getCols () >= 2);
  nr_double_t res;
  res = 1 + norm (m (0, 0)) - norm (m (1, 1)) - norm (det (m));
  return res;
}


matrix rad2deg (matrix a) {
  matrix res (a.getRows (), a.getCols ());
  for (int r = 0; r < a.getRows (); r++)
    for (int c = 0; c < a.getCols (); c++)
      res.set (r, c, rad2deg (a.get (r, c)));
  return res;
}

matrix deg2rad (matrix a) {
  matrix res (a.getRows (), a.getCols ());
  for (int r = 0; r < a.getRows (); r++)
    for (int c = 0; c < a.getCols (); c++)
      res.set (r, c, deg2rad (a.get (r, c)));
  return res;
}

} // namespace qucs
