/*
 * matrix.h - matrix class definitions
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

/*!\file matrix.h
   \brief Dense matrix class header file
*/

#ifndef __MATRIX_H__
#define __MATRIX_H__

namespace qucs {

class vector;
class matrix;

matrix eye (int);
matrix transpose (matrix);
matrix conj (matrix);
matrix abs (matrix);
matrix dB (matrix);
matrix arg (matrix);
matrix adjoint (matrix);
matrix real (matrix);
matrix imag (matrix);
matrix sqr (matrix);
matrix eye (int, int);
matrix diagonal (vector);
matrix pow (matrix, int);
nr_complex_t cofactor (matrix, int, int);
nr_complex_t detLaplace (matrix);
nr_complex_t detGauss (matrix);
nr_complex_t det (matrix);
matrix inverseLaplace (matrix);
matrix inverseGaussJordan (matrix);
matrix inverse (matrix);
matrix stos (matrix, nr_complex_t, nr_complex_t z0 = 50.0);
matrix stos (matrix, nr_double_t, nr_double_t z0 = 50.0);
matrix stos (matrix, vector, nr_complex_t z0 = 50.0);
matrix stos (matrix, nr_complex_t, vector);
matrix stos (matrix, vector, vector);
matrix stoz (matrix, nr_complex_t z0 = 50.0);
matrix stoz (matrix, vector);
matrix ztos (matrix, nr_complex_t z0 = 50.0);
matrix ztos (matrix, vector);
matrix ztoy (matrix);
matrix stoy (matrix, nr_complex_t z0 = 50.0);
matrix stoy (matrix, vector);
matrix ytos (matrix, nr_complex_t z0 = 50.0);
matrix ytos (matrix, vector);
matrix ytoz (matrix);
matrix stoa (matrix, nr_complex_t z1 = 50.0, nr_complex_t z2 = 50.0);
matrix atos (matrix, nr_complex_t z1 = 50.0, nr_complex_t z2 = 50.0);
matrix stoh (matrix, nr_complex_t z1 = 50.0, nr_complex_t z2 = 50.0);
matrix htos (matrix, nr_complex_t z1 = 50.0, nr_complex_t z2 = 50.0);
matrix stog (matrix, nr_complex_t z1 = 50.0, nr_complex_t z2 = 50.0);
matrix gtos (matrix, nr_complex_t z1 = 50.0, nr_complex_t z2 = 50.0);
matrix cytocs (matrix, matrix);
matrix cztocs (matrix, matrix);
matrix cztocy (matrix, matrix);
matrix cstocy (matrix, matrix);
matrix cytocz (matrix, matrix);
matrix cstocz (matrix, matrix);
matrix twoport (matrix, char, char);
nr_double_t rollet (matrix);
nr_double_t b1 (matrix);
matrix rad2deg     (matrix);
matrix deg2rad     (matrix);


/*!\class matrix
 * \brief Dense complex matrix class
 * This class defines a matrix object with its methods, operators and operations.
*/
class matrix
{
 public:
  matrix ();
  matrix (int);
  matrix (int, int);
  matrix (const matrix &);
  const matrix& operator = (const matrix &);
  ~matrix ();
  nr_complex_t get (int, int);
  void set (int, int, nr_complex_t);
  int getCols (void) { return cols; }
  int getRows (void) { return rows; }
  nr_complex_t * getData (void) { return data; }
  void print (void);
  void exchangeRows (int, int);
  void exchangeCols (int, int);

  // operator functions
  friend matrix operator + (matrix, matrix);
  friend matrix operator + (nr_complex_t, matrix);
  friend matrix operator + (matrix, nr_complex_t);
  friend matrix operator + (nr_double_t, matrix);
  friend matrix operator + (matrix, nr_double_t);
  friend matrix operator - (matrix, matrix);
  friend matrix operator - (nr_complex_t, matrix);
  friend matrix operator - (matrix, nr_complex_t);
  friend matrix operator - (nr_double_t, matrix);
  friend matrix operator - (matrix, nr_double_t);
  friend matrix operator / (matrix, nr_complex_t);
  friend matrix operator / (matrix, nr_double_t);
  friend matrix operator * (nr_complex_t, matrix);
  friend matrix operator * (matrix, nr_complex_t);
  friend matrix operator * (nr_double_t, matrix);
  friend matrix operator * (matrix, nr_double_t);
  friend matrix operator * (matrix, matrix);

  // intrinsic operator functions
  matrix operator  - ();
  matrix operator += (matrix);
  matrix operator -= (matrix);

  // block operations
  matrix getBlock(int, int, int, int);
  matrix getTopLeftCorner(int, int);
  matrix getBottomLeftCorner(int, int);
  matrix getTopRightCorner(int, int);
  matrix getBottomRightCorner(int, int);
  void setBlock(matrix, int, int, int, int);
  void setTopLeftCorner(matrix, int, int);
  void setBottomLeftCorner(matrix, int, int);
  void setTopRightCorner(matrix, int, int);
  void setBottomRightCorner(matrix, int, int);

  // other operations
  friend matrix transpose (matrix);
  friend matrix conj (matrix);
  friend matrix abs (matrix);
  friend matrix dB (matrix);
  friend matrix arg (matrix);
  friend matrix adjoint (matrix);
  friend matrix real (matrix);
  friend matrix imag (matrix);
  friend matrix sqr (matrix);
  friend matrix eye (int, int);
  friend matrix diagonal (qucs::vector);
  friend matrix pow (matrix, int);
  friend nr_complex_t cofactor (matrix, int, int);
  friend nr_complex_t detLaplace (matrix);
  friend nr_complex_t detGauss (matrix);
  friend nr_complex_t det (matrix);
  friend matrix inverseLaplace (matrix);
  friend matrix inverseGaussJordan (matrix);
  friend matrix inverse (matrix);
  friend matrix stos (matrix, nr_complex_t, nr_complex_t);
  friend matrix stos (matrix, nr_double_t, nr_double_t);
  friend matrix stos (matrix, qucs::vector, nr_complex_t);
  friend matrix stos (matrix, nr_complex_t, qucs::vector);
  friend matrix stos (matrix, qucs::vector, qucs::vector);
  friend matrix stoz (matrix, nr_complex_t);
  friend matrix stoz (matrix, qucs::vector);
  friend matrix ztos (matrix, nr_complex_t);
  friend matrix ztos (matrix, qucs::vector);
  friend matrix ztoy (matrix);
  friend matrix stoy (matrix, nr_complex_t);
  friend matrix stoy (matrix, qucs::vector);
  friend matrix ytos (matrix, nr_complex_t);
  friend matrix ytos (matrix, qucs::vector);
  friend matrix ytoz (matrix);
  friend matrix stoa (matrix, nr_complex_t, nr_complex_t);
  friend matrix atos (matrix, nr_complex_t, nr_complex_t);
  friend matrix stoh (matrix, nr_complex_t, nr_complex_t);
  friend matrix htos (matrix, nr_complex_t, nr_complex_t);
  friend matrix stog (matrix, nr_complex_t, nr_complex_t);
  friend matrix gtos (matrix, nr_complex_t, nr_complex_t);
  friend matrix cytocs (matrix, matrix);
  friend matrix cztocs (matrix, matrix);
  friend matrix cztocy (matrix, matrix);
  friend matrix cstocy (matrix, matrix);
  friend matrix cytocz (matrix, matrix);
  friend matrix cstocz (matrix, matrix);

  friend matrix twoport (matrix, char, char);
  friend nr_double_t rollet (matrix);
  friend nr_double_t b1 (matrix);

  friend matrix rad2deg    (matrix);
  friend matrix deg2rad    (matrix);

  /*! \brief Read access operator
      \param[in] r: row number (from 0 like usually in C)
      \param[in] c: column number (from 0 like usually in C)
      \return Cell in the row r and column c
      \todo: Why not inline
      \todo: Why not r and c not const
      \todo: Create a debug version checking out of bound (using directly assert)
  */
  nr_complex_t  operator () (int r, int c) const { return data[r * cols + c]; }
  /*! \brief Write access operator
      \param[in] r: row number (from 0 like usually in C)
      \param[in] c: column number (from 0 like usually in C)
      \return Reference to cell in the row r and column c
      \todo: Why not inline
      \todo: Why r and c not const
      \todo: Create a debug version checking out of bound (using directly assert)
  */
  nr_complex_t& operator () (int r, int c) { return data[r * cols + c]; }

 private:
  /*! Number of colunms */
  int cols;
  /*! Number of rows */
  int rows;
  /*! Matrix data */
  nr_complex_t * data;
};

} // namespace qucs

#endif /* __MATRIX_H__ */
