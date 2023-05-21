/*
 * vector.h - vector class definitions
 *
 * Copyright (C) 2003, 2004, 2005, 2006, 2007 Stefan Jahn <stefan@lkcc.org>
 * Copyright (C) 2006 Gunther Kraut <gn.kraut@t-online.de>
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

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <limits>

#include "consts.h"
#include "precision.h"
#include "complex.h"

#ifdef log2
#undef log2
#endif

namespace qucs {

class strlist;
class vector;

qucs::vector linspace (nr_double_t, nr_double_t, int);
qucs::vector logspace (nr_double_t, nr_double_t, int);
qucs::vector runavg (qucs::vector, const int);
qucs::vector runavg (const nr_complex_t, const int);

class vector : public object
{
 public:
  vector * getNext (void) const { return this->next; }
  void setNext (vector * const o) { this->next = o; }
  vector * getPrev (void) const { return prev; }
  void setPrev (vector * const o) { this->prev = o; }
 private:
  vector * next;
  vector * prev;

 public:
  vector ();
  vector (const std::string &);
  vector (int);
  vector (int, nr_complex_t);
  vector (const std::string &, int);
  vector (const vector &);
  const vector& operator = (const vector &);
  ~vector ();
  void add (nr_complex_t);
  void add (vector *);
  nr_complex_t get (int);
  void set (nr_double_t, int);
  void set (const nr_complex_t, int);
  int getSize (void) const;
  int checkSizes (vector, vector);
  int getRequested (void) { return requested; }
  void setRequested (int n) { requested = n; }
  void reverse (void);
  strlist * getDependencies (void);
  void setDependencies (strlist *);
  void setOrigin (const char *);
  char * getOrigin (void);
  int contains (nr_complex_t, nr_double_t eps = std::numeric_limits<nr_double_t>::epsilon());
  void sort (bool ascending = true);
  void print (void);

  nr_double_t maximum  (void);
  nr_double_t minimum  (void);
  nr_double_t rms      (void);
  nr_double_t variance (void);
  nr_double_t stddev   (void);

  friend nr_complex_t sum     (vector);
  friend nr_complex_t prod    (vector);
  friend nr_complex_t avg     (vector);
  friend vector  cumsum  (vector);
  friend vector  cumprod (vector);
  friend vector  cumavg  (vector);
  friend vector  smooth  (vector, const nr_double_t);
  friend vector  groupdelay  (vector, vector);
  friend vector  dbm     (vector, const nr_complex_t);
  friend nr_complex_t integrate (vector v, const nr_complex_t);
  friend nr_double_t integrate (vector v, const nr_double_t);

  // vector manipulations
  friend vector real   (vector);  // the real part
  friend vector imag   (vector);  // the imaginary part
  friend vector conj   (vector);  // the complex conjugate
  friend vector norm   (vector);  // the square of the magnitude
  friend vector arg    (vector);  // the angle in the plane
  friend vector dB     (vector);
  friend vector log    (vector);
  friend vector log2   (vector);
  friend vector pow    (vector, const nr_complex_t);
  friend vector pow    (vector, const nr_double_t);
  friend vector pow    (const nr_complex_t, vector);
  friend vector pow    (const nr_double_t, vector);
  friend vector pow    (vector, vector);
  friend vector ztor   (vector, nr_complex_t);
  friend vector rtoz   (vector, nr_complex_t);
  friend vector ytor   (vector, nr_complex_t);
  friend vector rtoy   (vector, nr_complex_t);
  friend vector diff   (vector, vector, int);
  friend vector unwrap (vector, nr_double_t, nr_double_t);

  friend vector polar   (vector, const nr_complex_t);
  friend vector polar   (const nr_complex_t, vector);
  friend vector polar   (vector, vector);
  friend vector atan2   (vector, const nr_double_t);
  friend vector atan2   (const nr_double_t, vector);
  friend vector atan2   (vector, vector);
  friend vector dbm2w   (vector);
  friend vector w2dbm   (vector);
  friend vector xhypot  (vector, vector);
  friend vector xhypot  (vector, const nr_complex_t);
  friend vector xhypot  (vector, const nr_double_t);
  friend vector xhypot  (const nr_complex_t, vector);
  friend vector xhypot  (const nr_double_t, vector);

  // overloaded math functions
  friend vector abs     (vector);
  friend vector log10   (vector);
  friend vector exp     (vector);
  friend vector limexp  (vector);
  friend vector sqrt    (vector);
  friend vector sin     (vector);
  friend vector asin    (vector);
  friend vector cos     (vector);
  friend vector acos    (vector);
  friend vector tan     (vector);
  friend vector atan    (vector);
  friend vector cot     (vector);
  friend vector acot    (vector);
  friend vector sinh    (vector);
  friend vector asinh   (vector);
  friend vector cosh    (vector);
  friend vector sech    (vector);
  friend vector cosech  (vector);
  friend vector acosh   (vector);
  friend vector asech   (vector);
  friend vector tanh    (vector);
  friend vector atanh   (vector);
  friend vector coth    (vector);
  friend vector acoth   (vector);
  friend vector signum  (vector);
  friend vector sign    (vector);
  friend vector sinc    (vector);
  friend vector ceil    (vector);
  friend vector floor   (vector);
  friend vector fix     (vector);
  friend vector round   (vector);
  friend vector sqr     (vector);
  friend vector step    (vector);
  friend vector jn      (const int, vector);
  friend vector yn      (const int, vector);
  friend vector i0      (vector);
  friend vector erf     (vector);
  friend vector erfc    (vector);
  friend vector erfinv  (vector);
  friend vector erfcinv (vector);
  friend vector rad2deg     (vector);
  friend vector deg2rad     (vector);

  // operator functions
  friend vector operator + (vector, vector);
  friend vector operator + (vector, const nr_complex_t);
  friend vector operator + (vector, const nr_double_t);
  friend vector operator + (const nr_complex_t, vector);
  friend vector operator + (const nr_double_t, vector);
  friend vector operator - (vector, vector);
  friend vector operator - (vector, const nr_complex_t);
  friend vector operator - (vector, const nr_double_t);
  friend vector operator - (const nr_complex_t, vector);
  friend vector operator - (const nr_double_t, vector);
  friend vector operator * (vector, vector);
  friend vector operator * (vector, const nr_complex_t);
  friend vector operator * (vector, const nr_double_t);
  friend vector operator * (const nr_complex_t, vector);
  friend vector operator * (const nr_double_t, vector);
  friend vector operator / (vector, vector);
  friend vector operator / (vector, const nr_complex_t);
  friend vector operator / (vector, const nr_double_t);
  friend vector operator / (const nr_complex_t, vector);
  friend vector operator / (const nr_double_t, vector);
  friend vector operator % (vector, vector);
  friend vector operator % (vector, const nr_complex_t);
  friend vector operator % (vector, const nr_double_t);
  friend vector operator % (const nr_complex_t, vector);
  friend vector operator % (const nr_double_t, vector);

  // comparisons
  //  friend int      operator == (const vector *, const vector *);
  //  friend int      operator != (const vector *, const vector *);

  // assignment operations
  vector operator  - ();
  vector operator  = (const nr_complex_t);
  vector operator  = (const nr_double_t);
  vector operator += (vector);
  vector operator += (const nr_complex_t);
  vector operator += (const nr_double_t);
  vector operator -= (vector);
  vector operator -= (const nr_complex_t);
  vector operator -= (const nr_double_t);
  vector operator *= (vector);
  vector operator *= (const nr_complex_t);
  vector operator *= (const nr_double_t);
  vector operator /= (vector);
  vector operator /= (const nr_complex_t);
  vector operator /= (const nr_double_t);

  // easy accessor operators
  nr_complex_t  operator () (int i) const { return data[i]; }
  nr_complex_t& operator () (int i) { return data[i]; }

 private:
  int requested;
  int size;
  int capacity;
  strlist * dependencies;
  nr_complex_t * data;
  char * origin;
};

/* declarations of friend functions to make them available in the
   qucs namespace without argument-dependent lookup, see

   http://stackoverflow.com/questions/7785886/access-friend-function-defined-in-class

   for more info
*/
nr_complex_t sum     (vector);
nr_complex_t prod    (vector);
nr_complex_t avg     (vector);
vector  cumsum  (vector);
vector  cumprod (vector);
vector  cumavg  (vector);
vector  smooth  (vector, const nr_double_t);
vector  dbm     (vector, const nr_complex_t z = 50.0);
nr_complex_t integrate (vector v, const nr_complex_t);
nr_double_t integrate (vector v, const nr_double_t);
vector real   (vector);  // the real part
vector imag   (vector);  // the imaginary part
vector conj   (vector);  // the complex conjugate
vector norm   (vector);  // the square of the magnitude
vector arg    (vector);  // the angle in the plane
vector dB     (vector);
vector log    (vector);
vector log2   (vector);
vector pow    (vector, const nr_complex_t);
vector pow    (vector, const nr_double_t);
vector pow    (const nr_complex_t, vector);
vector pow    (const nr_double_t, vector);
vector pow    (vector, vector);
vector ztor   (vector, nr_complex_t zref = 50.0);
vector rtoz   (vector, nr_complex_t zref = 50.0);
vector ytor   (vector, nr_complex_t zref = 50.0);
vector rtoy   (vector, nr_complex_t zref = 50.0);
vector diff   (vector, vector, int n = 1);
vector unwrap (vector, nr_double_t tol = pi, nr_double_t step = 2 * pi);
vector polar   (vector, const nr_complex_t);
vector polar   (const nr_complex_t, vector);
vector polar   (vector, vector);
vector atan2   (vector, const nr_double_t);
vector atan2   (const nr_double_t, vector);
vector atan2   (vector, vector);
vector dbm2w   (vector);
vector w2dbm   (vector);
vector xhypot  (vector, vector);
vector xhypot  (vector, const nr_complex_t);
vector xhypot  (vector, const nr_double_t);
vector xhypot  (const nr_complex_t, vector);
vector xhypot  (const nr_double_t, vector);
vector abs     (vector);
vector log10   (vector);
vector exp     (vector);
vector limexp  (vector);
vector sqrt    (vector);
vector sin     (vector);
vector asin    (vector);
vector cos     (vector);
vector acos    (vector);
vector tan     (vector);
vector atan    (vector);
vector cot     (vector);
vector acot    (vector);
vector sinh    (vector);
vector asinh   (vector);
vector cosh    (vector);
vector sech    (vector);
vector cosech  (vector);
vector acosh   (vector);
vector asech   (vector);
vector tanh    (vector);
vector atanh   (vector);
vector coth    (vector);
vector acoth   (vector);
vector signum  (vector);
vector sign    (vector);
vector sinc    (vector);
vector ceil    (vector);
vector floor   (vector);
vector fix     (vector);
vector round   (vector);
vector sqr     (vector);
vector step    (vector);
vector jn      (const int, vector);
vector yn      (const int, vector);
vector i0      (vector);
vector erf     (vector);
vector erfc    (vector);
vector erfinv  (vector);
vector erfcinv (vector);
vector rad2deg     (vector);
vector deg2rad     (vector);

} // namespace qucs

#endif /* __VECTOR_H__ */
