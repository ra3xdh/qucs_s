/*
 * matvec.h - matrix vector class definitions
 *
 * Copyright (C) 2004, 2005, 2006, 2007, 2009 Stefan Jahn <stefan@lkcc.org>
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

/*!
 * \file matvec.h
 * \brief vector of matrices class header file
 */

#ifndef __MATVEC_H__
#define __MATVEC_H__

namespace qucs {

class matrix;
class matvec;

// forward declarations
matvec transpose (matvec);
matvec conj      (matvec);
vector det       (matvec);
matvec inverse   (matvec);
matvec sqr       (matvec);
matvec pow       (matvec, int);
matvec pow       (matvec, vector);
matvec twoport   (matvec, char, char);
matvec real      (matvec);
matvec imag      (matvec);
matvec abs       (matvec);
matvec dB        (matvec);
matvec arg       (matvec);
matvec adjoint   (matvec);
vector rollet    (matvec);
vector b1        (matvec);
matvec rad2deg       (matvec);
matvec deg2rad       (matvec);
matvec stos (matvec, nr_complex_t, nr_complex_t z0 = 50.0);
matvec stos (matvec, nr_double_t, nr_double_t z0 = 50.0);
matvec stos (matvec, vector, nr_complex_t z0 = 50.0);
matvec stos (matvec, nr_complex_t, vector);
matvec stos (matvec, vector, vector);
matvec stoz (matvec, nr_complex_t z0 = 50.0);
matvec stoz (matvec, vector);
matvec ztos (matvec, nr_complex_t z0 = 50.0);
matvec ztos (matvec, vector);
matvec ztoy (matvec);
matvec stoy (matvec, nr_complex_t z0 = 50.0);
matvec stoy (matvec, vector);
matvec ytos (matvec, nr_complex_t z0 = 50.0);
matvec ytos (matvec, vector);
matvec ytoz (matvec);

class matvec
{
 public:
  matvec ();
  matvec (int, int, int);
  matvec (const matvec &);
  ~matvec ();
  int getSize (void) { return size; }
  int getCols (void) { return cols; }
  int getRows (void) { return rows; }
  void setName (const char *);
  char * getName (void);
  void set (qucs::vector, int, int);
  void set (matrix, int);
  qucs::vector get (int, int);
  matrix get (int);
  static char * createMatrixString (const char *, int, int);
  static char * createMatrixString (char, int, int);
  static char * isMatrixVector (const char *, int&, int&);
  static matvec * getMatrixVector (qucs::vector *, char *);
  static void getMatrixVectorSize (qucs::vector *, char *, int&, int&, int&);

  // operator functions
  friend matvec operator + (matvec, matvec);
  friend matvec operator + (matvec, matrix);
  friend matvec operator + (matrix, matvec);
  friend matvec operator + (matvec, nr_complex_t);
  friend matvec operator + (nr_complex_t, matvec);
  friend matvec operator + (matvec, nr_double_t);
  friend matvec operator + (nr_double_t, matvec);
  friend matvec operator + (matvec, qucs::vector);
  friend matvec operator + (qucs::vector, matvec);
  friend matvec operator - (matvec, matvec);
  friend matvec operator - (matvec, matrix);
  friend matvec operator - (matrix, matvec);
  friend matvec operator - (matvec, nr_complex_t);
  friend matvec operator - (nr_complex_t, matvec);
  friend matvec operator - (matvec, nr_double_t);
  friend matvec operator - (nr_double_t, matvec);
  friend matvec operator - (matvec, qucs::vector);
  friend matvec operator - (qucs::vector, matvec);
  friend matvec operator / (matvec, nr_complex_t);
  friend matvec operator / (matvec, nr_double_t);
  friend matvec operator / (matvec, qucs::vector);
  friend matvec operator * (matvec, qucs::vector);
  friend matvec operator * (qucs::vector, matvec);
  friend matvec operator * (matvec, nr_complex_t);
  friend matvec operator * (nr_complex_t, matvec);
  friend matvec operator * (matvec, nr_double_t);
  friend matvec operator * (nr_double_t, matvec);
  friend matvec operator * (matvec, matvec);
  friend matvec operator * (matvec, matrix);
  friend matvec operator * (matrix, matvec);

  // intrinsic operator functions
  matvec operator  - ();
  matvec operator += (matvec);
  matvec operator -= (matvec);

  // other operations
  friend matvec transpose (matvec);
  friend matvec conj      (matvec);
  friend qucs::vector det       (matvec);
  friend matvec inverse   (matvec);
  friend matvec sqr       (matvec);
  friend matvec pow       (matvec, int);
  friend matvec pow       (matvec, qucs::vector);
  friend matvec twoport   (matvec, char, char);
  friend matvec real      (matvec);
  friend matvec imag      (matvec);
  friend matvec abs       (matvec);
  friend matvec dB        (matvec);
  friend matvec arg       (matvec);
  friend matvec adjoint   (matvec);
  friend qucs::vector rollet    (matvec);
  friend qucs::vector b1        (matvec);
  friend matvec rad2deg       (matvec);
  friend matvec deg2rad       (matvec);

  friend matvec stos (matvec, nr_complex_t, nr_complex_t);
  friend matvec stos (matvec, nr_double_t, nr_double_t);
  friend matvec stos (matvec, qucs::vector, nr_complex_t);
  friend matvec stos (matvec, nr_complex_t, qucs::vector);
  friend matvec stos (matvec, qucs::vector, qucs::vector);
  friend matvec stoz (matvec, nr_complex_t);
  friend matvec stoz (matvec, qucs::vector);
  friend matvec ztos (matvec, nr_complex_t);
  friend matvec ztos (matvec, qucs::vector);
  friend matvec ztoy (matvec);
  friend matvec stoy (matvec, nr_complex_t);
  friend matvec stoy (matvec, qucs::vector);
  friend matvec ytos (matvec, nr_complex_t);
  friend matvec ytos (matvec, qucs::vector);
  friend matvec ytoz (matvec);

 private:
  int size; /*! number of matrices  */
  int rows; /*! Number of rows of each matrix */
  int cols; /*! Number of columns of each matrix */
  char * name; /*! Name of the matrix array (optional) */
  matrix * data; /*! matrix (used as array) of data matrices */
};

} // namespace qucs

#endif /* __MATVEC_H__ */
