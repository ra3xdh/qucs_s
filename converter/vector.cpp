/*
 * vector.cpp - vector class implementation
 *
 * Copyright (C) 2003, 2004, 2005, 2006, 2007 Stefan Jahn <stefan@lkcc.org>
 * Copyright (C) 2006, 2007 Gunther Kraut <gn.kraut@t-online.de>
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

#include <complex>
#include "qucs_typedefs.h"

#include <limits>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <float.h>
#include <assert.h>

#include "real.h"
#include "complex.h"
#include "object.h"
#include "logging.h"
#include "strlist.h"
#include "vector.h"
#include "consts.h"

namespace qucs {

// Constructor creates an unnamed instance of the vector class.
vector::vector () : object () {
  capacity = size = 0;
  data = NULL;
  dependencies = NULL;
  origin = NULL;
  requested = 0;
  next = prev = nullptr;
}

/* Constructor creates an unnamed instance of the vector class with a
   given initial size. */
vector::vector (int s) : object () {
  assert (s >= 0);
  capacity = size = s;
  data = s > 0 ? (nr_complex_t *)
    calloc (capacity, sizeof (nr_complex_t)) : NULL;
  dependencies = NULL;
  origin = NULL;
  requested = 0;
  next = prev = nullptr;
}

/* Constructor creates an unnamed instance of the vector class with a
   given initial size and content. */
vector::vector (int s, nr_complex_t val) : object () {
  assert (s >= 0);
  capacity = size = s;
  data = s > 0 ? (nr_complex_t *)
    calloc (capacity, sizeof (nr_complex_t)) : NULL;
  for (int i = 0; i < s; i++) data[i] = val;
  dependencies = NULL;
  origin = NULL;
  requested = 0;
  next = prev = nullptr;
}

// Constructor creates an named instance of the vector class.
vector::vector (const std::string &n) : object (n) {
  capacity = size = 0;
  data = NULL;
  dependencies = NULL;
  origin = NULL;
  requested = 0;
  next = prev = nullptr;
}

/* This constructor creates a named instance of the vector class with
   a given initial size. */
  vector::vector (const std::string &n, int s) : object (n) {
  assert (s >= 0);
  capacity = size = s;
  data = s > 0 ? (nr_complex_t *)
    calloc (capacity, sizeof (nr_complex_t)) : NULL;
  dependencies = NULL;
  origin = NULL;
  requested = 0;
  next = prev = nullptr;
}

/* The copy constructor creates a new instance based on the given
   vector object. */
vector::vector (const vector & v) : object (v) {
  size = v.size;
  capacity = v.capacity;
  data = (nr_complex_t *) malloc (sizeof (nr_complex_t) * capacity);
  memcpy (data, v.data, sizeof (nr_complex_t) * size);
  dependencies = v.dependencies ? new strlist (*v.dependencies) : NULL;
  origin = v.origin ? strdup (v.origin) : NULL;
  requested = v.requested;
  next = v.next;
  prev = v.prev;
}

/* The assignment copy constructor creates a new instance based on the
   given vector object.  It copies the data only and leaves any other
   properties untouched. */
const vector& vector::operator=(const vector & v) {
  if (&v != this) {
    size = v.size;
    capacity = v.capacity;
    if (data) { free (data); data = NULL; }
    if (capacity > 0) {
      data = (nr_complex_t *) malloc (sizeof (nr_complex_t) * capacity);
      if (size > 0) memcpy (data, v.data, sizeof (nr_complex_t) * size);
    }
  }
  return *this;
}

// Destructor deletes a vector object.
vector::~vector () {
  free (data);
  delete dependencies;
  free (origin);
}

// Returns data dependencies.
strlist * vector::getDependencies (void) {
  return dependencies;
}

// Sets the data dependencies.
void vector::setDependencies (strlist * s) {
  delete dependencies;
  dependencies = s;
}

/* The function appends a new complex data item to the end of the
   vector and ensures that the vector can hold the increasing number
   of data items. */
void vector::add (nr_complex_t c) {
  if (data == NULL) {
    size = 0; capacity = 64;
    data = (nr_complex_t *) malloc (sizeof (nr_complex_t) * capacity);
  }
  else if (size >= capacity) {
    capacity *= 2;
    data = (nr_complex_t *) realloc (data, sizeof (nr_complex_t) * capacity);
  }
  data[size++] = c;
}

/* This function appends the given vector to the vector. */
void vector::add (vector * v) {
  if (v != NULL) {
    if (data == NULL) {
      size = 0; capacity = v->getSize ();
      data = (nr_complex_t *) malloc (sizeof (nr_complex_t) * capacity);
    }
    else if (size + v->getSize () > capacity) {
      capacity += v->getSize ();
      data = (nr_complex_t *) realloc (data, sizeof (nr_complex_t) * capacity);
    }
    for (int i = 0; i < v->getSize (); i++) data[size++] = v->get (i);
  }
}

// Returns the complex data item at the given position.
nr_complex_t vector::get (int i) {
  return data[i];
}

void vector::set (nr_double_t d, int i) {
  data[i] = nr_complex_t (d);
}

void vector::set (const nr_complex_t z, int i) {
  data[i] = nr_complex_t (z);
}

// The function returns the current size of the vector.
int vector::getSize (void) const {
  return size;
}

int vector::checkSizes (vector v1, vector v2) {
  if (v1.getSize () != v2.getSize ()) {
    logprint (LOG_ERROR, "vector '%s' and '%s' have different sizes\n",
	      v1.getName (), v2.getName ());
    return 0;
  }
  return 1;
}

// searches the maximum value of the vector elements.
// complex numbers in the 1. and 4. quadrant are counted as "abs(c)".
// complex numbers in the 2. and 3. quadrant are counted as "-abs(c)".
nr_double_t vector::maximum (void) {
  nr_complex_t c;
  nr_double_t d, max_D = -std::numeric_limits<nr_double_t>::max();
  for (int i = 0; i < getSize (); i++) {
    c = data[i];
    d = fabs (arg (c)) < pi_over_2 ? abs (c) : -abs (c);
    if (d > max_D) max_D = d;
  }
  return max_D;
}

// searches the minimum value of the vector elements.
// complex numbers in the 1. and 4. quadrant are counted as "abs(c)".
// complex numbers in the 2. and 3. quadrant are counted as "-abs(c)".
nr_double_t vector::minimum (void) {
  nr_complex_t c;
  nr_double_t d, min_D = +std::numeric_limits<nr_double_t>::max();
  for (int i = 0; i < getSize (); i++) {
    c = data[i];
    d = fabs (arg (c)) < pi_over_2 ? abs (c) : -abs (c);
    if (d < min_D) min_D = d;
  }
  return min_D;
}

/* Unwraps a phase vector in radians.  Adds +/- 2*Pi if consecutive
   values jump about |Pi|. */
vector unwrap (vector v, nr_double_t tol, nr_double_t step) {
  vector result (v.getSize ());
  nr_double_t add = 0;
  result (0) = v (0);
  for (int i = 1; i < v.getSize (); i++) {
    nr_double_t diff = real (v (i) - v (i-1));
    if (diff > +tol) {
      add -= step;
    } else if (diff < -tol) {
      add += step;
    }
    result (i) = v (i) + add;
  }
  return result;
}

nr_complex_t sum (vector v) {
  nr_complex_t result (0.0);
  for (int i = 0; i < v.getSize (); i++) result += v.get (i);
  return result;
}

nr_complex_t prod (vector v) {
  nr_complex_t result (1.0);
  for (int i = 0; i < v.getSize (); i++) result *= v.get (i);
  return result;
}

nr_complex_t avg (vector v) {
  nr_complex_t result (0.0);
  for (int i = 0; i < v.getSize (); i++) result += v.get (i);
  return result / (nr_double_t) v.getSize ();
}

vector signum (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (signum (v.get (i)), i);
  return result;
}

vector sign (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (sign (v.get (i)), i);
  return result;
}

vector xhypot (vector v, const nr_complex_t z) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (xhypot (v.get(i), z), i);
  return result;
}

vector xhypot (vector v, const nr_double_t d) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (xhypot (v.get(i), d), i);
  return result;
}

vector xhypot (const nr_complex_t z, vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (xhypot (z, v.get (i)), i);
  return result;
}

vector xhypot (const nr_double_t d, vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (xhypot (d, v.get (i)), i);
  return result;
}

vector xhypot (vector v1, vector v2) {
  int j, i, n, len, len1 = v1.getSize (), len2 = v2.getSize ();
  if (len1 >= len2) {
    assert (len1 % len2 == 0);
    len = len1;
  } else {
    assert (len2 % len1 == 0);
    len = len2;
  }
  vector res (len);
  for (j = i = n = 0; n < len; n++) {
    res (n) = xhypot (v1 (i), v2 (j));
    if (++i >= len1) i = 0; if (++j >= len2) j = 0;
  }
  return res;
}

vector sinc (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (sinc (v.get (i)), i);
  return result;
}

vector abs (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (abs (v.get (i)), i);
  return result;
}

vector norm (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (norm (v.get (i)), i);
  return result;
}

vector arg (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (arg (v.get (i)), i);
  return result;
}

vector real (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (real (v.get (i)), i);
  return result;
}

vector imag (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (imag (v.get (i)), i);
  return result;
}

vector conj (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (conj (v.get (i)), i);
  return result;
}

vector dB (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++)
    result.set (10.0 * std::log10 (norm (v.get (i))), i);
  return result;
}

vector sqrt (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (sqrt (v.get (i)), i);
  return result;
}

vector exp (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (exp (v.get (i)), i);
  return result;
}

vector limexp (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (limexp (v.get (i)), i);
  return result;
}

vector log (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (log (v.get (i)), i);
  return result;
}

vector log10 (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (log10 (v.get (i)), i);
  return result;
}

vector log2 (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (log2 (v.get (i)), i);
  return result;
}

vector pow (vector v, const nr_complex_t z) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (pow (v.get(i), z), i);
  return result;
}

vector pow (vector v, const nr_double_t d) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (pow (v.get(i), d), i);
  return result;
}

vector pow (const nr_complex_t z, vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (pow (z, v.get (i)), i);
  return result;
}

vector pow (const nr_double_t d, vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (pow (d, v.get (i)), i);
  return result;
}

vector pow (vector v1, vector v2) {
  int j, i, n, len, len1 = v1.getSize (), len2 = v2.getSize ();
  if (len1 >= len2) {
    assert (len1 % len2 == 0);
    len = len1;
  } else {
    assert (len2 % len1 == 0);
    len = len2;
  }
  vector res (len);
  for (j = i = n = 0; n < len; n++) {
    res (n) = pow (v1 (i), v2 (j));
    if (++i >= len1) i = 0; if (++j >= len2) j = 0;
  }
  return res;
}

vector sin (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (sin (v.get (i)), i);
  return result;
}

vector asin (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (asin (v.get (i)), i);
  return result;
}

vector acos (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (acos (v.get (i)), i);
  return result;
}

vector cos (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (cos (v.get (i)), i);
  return result;
}

vector tan (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (tan (v.get (i)), i);
  return result;
}

vector atan (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (atan (v.get (i)), i);
  return result;
}

vector cot (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (cot (v.get (i)), i);
  return result;
}

vector acot (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (acot (v.get (i)), i);
  return result;
}

vector sinh (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (sinh (v.get (i)), i);
  return result;
}

vector asinh (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (asinh (v.get (i)), i);
  return result;
}

vector cosh (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (cosh (v.get (i)), i);
  return result;
}

vector sech (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (sech (v.get (i)), i);
  return result;
}

vector cosech (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (cosech (v.get (i)), i);
  return result;
}

vector acosh (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (acosh (v.get (i)), i);
  return result;
}

vector asech (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (asech (v.get (i)), i);
  return result;
}

vector tanh (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (tanh (v.get (i)), i);
  return result;
}

vector atanh (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (atanh (v.get (i)), i);
  return result;
}

vector coth (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (coth (v.get (i)), i);
  return result;
}

vector acoth (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (acoth (v.get (i)), i);
  return result;
}

// converts impedance to reflexion coefficient
vector ztor (vector v, nr_complex_t zref) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result (i) = ztor (v (i), zref);
  return result;
}

// converts admittance to reflexion coefficient
vector ytor (vector v, nr_complex_t zref) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result (i) = ytor (v (i), zref);
  return result;
}

// converts reflexion coefficient to impedance
vector rtoz (vector v, nr_complex_t zref) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result (i) = rtoz (v (i), zref);
  return result;
}

// converts reflexion coefficient to admittance
vector rtoy (vector v, nr_complex_t zref) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result (i) = rtoy (v (i), zref);
  return result;
}

// differentiates 'var' with respect to 'dep' exactly 'n' times
vector diff (vector var, vector dep, int n) {
  int k, xi, yi, exchange = 0;
  vector x, y;
  // exchange dependent and independent variable if necessary
  if (var.getSize () < dep.getSize ()) {
    x = vector (var);
    y = vector (dep);
    exchange++;
  }
  else {
    x = vector (dep);
    y = vector (var);
  }
  assert (y.getSize () % x.getSize () == 0 && x.getSize () >= 2);
  vector result (y);
  nr_complex_t c;

  for (k = 0; k < n; k++) {  // differentiate n times
    for (yi = xi = 0; yi < y.getSize (); yi++, xi++) {
      if (xi == x.getSize ()) xi = 0; // roll through independent vector
      if (xi == 0) {
	c = (y.get (yi + 1) - y.get (yi)) / (x.get (xi + 1) - x.get (xi));
      } else if (xi == x.getSize () - 1) {
	c = (y.get (yi) - y.get (yi - 1)) / (x.get (xi) - x.get (xi - 1));
      }
      else {
	c =
	  ((y.get (yi) - y.get (yi - 1)) / (x.get (xi) - x.get (xi - 1)) +
	   (y.get (yi + 1) - y.get (yi)) / (x.get (xi + 1) - x.get (xi))) /
	  2.0;
      }
      result.set (exchange ? 1.0 / c : c, yi);
    }
    y = result;
  }
  return result;
}

vector vector::operator=(const nr_complex_t c) {
  for (int i = 0; i < size; i++) data[i] = c;
  return *this;
}

vector vector::operator=(const nr_double_t d) {
  for (int i = 0; i < size; i++) data[i] = d;
  return *this;
}

vector vector::operator+=(vector v) {
  int i, n, len = v.getSize ();
  assert (size % len == 0);
  for (i = n = 0; i < size; i++) { data[i] += v (n); if (++n >= len) n = 0; }
  return *this;
}

vector vector::operator+=(const nr_complex_t c) {
  for (int i = 0; i < size; i++) data[i] += c;
  return *this;
}

vector vector::operator+=(const nr_double_t d) {
  for (int i = 0; i < size; i++) data[i] += d;
  return *this;
}

vector operator+(vector v1, vector v2) {
  int len1 = v1.getSize (), len2 = v2.getSize ();
  vector result;
  if (len1 >= len2) {
    result  = v1;
    result += v2;
  } else {
    result  = v2;
    result += v1;
  }
  return result;
}

vector operator+(vector v, const nr_complex_t c) {
  vector result (v);
  result += c;
  return result;
}

vector operator+(const nr_complex_t c, vector v) {
  return v + c;
}

vector operator+(vector v, const nr_double_t d) {
  vector result (v);
  result += d;
  return result;
}

vector operator+(const nr_double_t d, vector v) {
  return v + d;
}

vector vector::operator-() {
  vector result (size);
  for (int i = 0; i < size; i++) result (i) = -data[i];
  return result;
}

vector vector::operator-=(vector v) {
  int i, n, len = v.getSize ();
  assert (size % len == 0);
  for (i = n = 0; i < size; i++) { data[i] -= v (n); if (++n >= len) n = 0; }
  return *this;
}

vector vector::operator-=(const nr_complex_t c) {
  for (int i = 0; i < size; i++) data[i] -= c;
  return *this;
}

vector vector::operator-=(const nr_double_t d) {
  for (int i = 0; i < size; i++) data[i] -= d;
  return *this;
}

vector operator-(vector v1, vector v2) {
  int len1 = v1.getSize (), len2 = v2.getSize ();
  vector result;
  if (len1 >= len2) {
    result  = v1;
    result -= v2;
  } else {
    result  = -v2;
    result += v1;
  }
  return result;
}

vector operator-(vector v, const nr_complex_t c) {
  vector result (v);
  result -= c;
  return result;
}

vector operator-(vector v, const nr_double_t d) {
  vector result (v);
  result -= d;
  return result;
}

vector operator-(const nr_complex_t c, vector v) {
  vector result (-v);
  result += c;
  return result;
}

vector operator-(const nr_double_t d, vector v) {
  vector result (-v);
  result += d;
  return result;
}

vector vector::operator*=(vector v) {
  int i, n, len = v.getSize ();
  assert (size % len == 0);
  for (i = n = 0; i < size; i++) { data[i] *= v (n); if (++n >= len) n = 0; }
  return *this;
}

vector vector::operator*=(const nr_complex_t c) {
  for (int i = 0; i < size; i++) data[i] *= c;
  return *this;
}

vector vector::operator*=(const nr_double_t d) {
  for (int i = 0; i < size; i++) data[i] *= d;
  return *this;
}

vector operator*(vector v1, vector v2) {
  int len1 = v1.getSize (), len2 = v2.getSize ();
  vector result;
  if (len1 >= len2) {
    result  = v1;
    result *= v2;
  } else {
    result  = v2;
    result *= v1;
  }
  return result;
}

vector operator*(vector v, const nr_complex_t c) {
  vector result (v);
  result *= c;
  return result;
}

vector operator*(vector v, const nr_double_t d) {
  vector result (v);
  result *= d;
  return result;
}

vector operator*(const nr_complex_t c, vector v) {
  return v * c;
}

vector operator*(const nr_double_t d, vector v) {
  return v * d;
}

vector vector::operator/=(vector v) {
  int i, n, len = v.getSize ();
  assert (size % len == 0);
  for (i = n = 0; i < size; i++) { data[i] /= v (n); if (++n >= len) n = 0; }
  return *this;
}

vector vector::operator/=(const nr_complex_t c) {
  for (int i = 0; i < size; i++) data[i] /= c;
  return *this;
}

vector vector::operator/=(const nr_double_t d) {
  for (int i = 0; i < size; i++) data[i] /= d;
  return *this;
}

vector operator/(vector v1, vector v2) {
  int len1 = v1.getSize (), len2 = v2.getSize ();
  vector result;
  if (len1 >= len2) {
    assert (len1 % len2 == 0);
    result  = v1;
    result /= v2;
  } else {
    assert (len2 % len1 == 0);
    result  = 1 / v2;
    result *= v1;
  }
  return result;
}

vector operator/(vector v, const nr_complex_t c) {
  vector result (v);
  result /= c;
  return result;
}

vector operator/(vector v, const nr_double_t d) {
  vector result (v);
  result /= d;
  return result;
}

vector operator/(const nr_complex_t c, vector v) {
  vector result (v);
  result  = c;
  result /= v;
  return result;
}

vector operator/(const nr_double_t d, vector v) {
  vector result (v);
  result  = d;
  result /= v;
  return result;
}

vector operator%(vector v, const nr_complex_t z) {
  int len = v.getSize ();
  vector result (len);
  for (int i = 0; i < len; i++) result (i) = v (i) % z;
  return result;
}

vector operator%(vector v, const nr_double_t d) {
  int len = v.getSize ();
  vector result (len);
  for (int i = 0; i < len; i++) result (i) = v (i) % d;
  return result;
}

vector operator%(const nr_complex_t z, vector v) {
  int len = v.getSize ();
  vector result (len);
  for (int i = 0; i < len; i++) result (i) = z % v (i);
  return result;
}

vector operator%(const nr_double_t d, vector v) {
  int len = v.getSize ();
  vector result (len);
  for (int i = 0; i < len; i++) result (i) = d % v (i);
  return result;
}

vector operator%(vector v1, vector v2) {
  int j, i, n, len, len1 = v1.getSize (), len2 = v2.getSize ();
  if (len1 >= len2) {
    assert (len1 % len2 == 0);
    len = len1;
  } else {
    assert (len2 % len1 == 0);
    len = len2;
  }
  vector res (len);
  for (j = i = n = 0; n < len; n++) {
    res (n) = v1 (i) % v2 (j);
    if (++i >= len1) i = 0;  if (++j >= len2) j = 0;
  }
  return res;
}

/* This function reverses the order of the data list. */
void vector::reverse (void) {
  nr_complex_t * buffer = (nr_complex_t *)
    malloc (sizeof (nr_complex_t) * size);
  for (int i = 0; i < size; i++) buffer[i] = data[size - 1 - i];
  free (data);
  data = buffer;
  capacity = size;
}

// Sets the origin (the analysis) of the vector.
void vector::setOrigin (const char * n) {
  free (origin);
  origin = n ? strdup (n) : NULL;
}

// Returns the origin (the analysis) of the vector.
char * vector::getOrigin (void) {
  return origin;
}

/* The function returns the number of entries with the given value
   deviating no more than the given epsilon. */
int vector::contains (nr_complex_t val, nr_double_t eps) {
  int count = 0;
  for (int i = 0; i < size; i++) {
    if (abs (data[i] - val) <= eps) count++;
  }
  return count;
}

// Sorts the vector either in ascending or descending order.
void vector::sort (bool ascending) {
  nr_complex_t t;
  for (int i = 0; i < size; i++) {
    for (int n = 0; n < size - 1; n++) {
      if (ascending ? data[n] > data[n+1] : data[n] < data[n+1]) {
	t = data[n];
	data[n] = data[n+1];
	data[n+1] = t;
      }
    }
  }
}

/* The function creates a linear stepped vector of values starting at
   the given start value, ending with the given stop value and
   containing points elements. */
vector linspace (nr_double_t start, nr_double_t stop, int points) {
  vector result (points);
  nr_double_t val, step = (stop - start) / (points - 1);
  for (int i = 0; i < points; i++) {
    val = start + (i * step);
    if (i != 0 && fabs (val) < fabs (step) / 4 && fabs (val) < std::numeric_limits<nr_double_t>::epsilon())
      val = 0.0;
    result.set (val, i);
  }
  return result;
}

/* The function creates a logarithmic stepped vector of values
   starting at the given start value, ending with the given stop value
   and containing points elements. */
vector logspace (nr_double_t start, nr_double_t stop, int points) {
  assert (start * stop > 0);
  vector result (points);
  nr_double_t step, first, last, d;

  // ensure the last value being larger than the first
  if (fabs (start) > fabs (stop)) {
    first = fabs (stop);
    last = fabs (start);
  }
  else {
    first = fabs (start);
    last = fabs (stop);
  }
  // check direction and sign of values
  d = fabs (start) > fabs (stop) ? -1 : 1;
  // compute logarithmic step size
  step = (::log (last) - ::log (first)) / (points - 1);
  for (int i = 0, j = points - 1; i < points; i++, j--) {
    if (d > 0)
      result.set (start * ::exp (step * i), i);
    else
      result.set (stop * ::exp (step * i), j);
  }
  return result;
}

vector cumsum (vector v) {
  vector result (v);
  nr_complex_t val (0.0);
  for (int i = 0; i < v.getSize (); i++) {
    val += v.get (i);
    result.set (val, i);
  }
  return result;
}

vector cumavg (vector v) {
  vector result (v);
  nr_complex_t val (0.0);
  for (int i = 0; i < v.getSize (); i++) {
    val = (val * (nr_double_t) i + v.get (i)) / (i + 1.0);
    result.set (val, i);
  }
  return result;
}

vector cumprod (vector v) {
  vector result (v);
  nr_complex_t val (1.0);
  for (int i = 0; i < v.getSize (); i++) {
    val *= v.get (i);
    result.set (val, i);
  }
  return result;
}

vector ceil (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (ceil (v.get (i)), i);
  return result;
}

vector fix (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (fix (v.get (i)), i);
  return result;
}

vector floor (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (floor (v.get (i)), i);
  return result;
}

vector round (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (round (v.get (i)), i);
  return result;
}

vector sqr (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (sqr (v.get (i)), i);
  return result;
}

vector step (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (step (v.get (i)), i);
  return result;
}

static nr_double_t integrate_n (vector v) { /* using trapezoidal rule */
  nr_double_t result = 0.0;
  for (int i = 1; i < v.getSize () - 1; i++) result += norm (v.get (i));
  result += 0.5 * norm (v.get (0));
  result += 0.5 * norm (v.get (v.getSize () - 1));
  return result;
}

nr_double_t vector::rms (void) {
  nr_double_t result = std::sqrt (integrate_n (*this) / getSize ());
  return result;
}

nr_double_t vector::variance (void) {
  nr_double_t result = 0.0;
  nr_complex_t average = avg (*this);
  for (int i = 0; i < getSize (); i++) result += norm (get (i) - average);
  if (getSize () > 1)
    return result / (getSize () - 1);
  return 0.0;
}

nr_double_t vector::stddev (void) {
  return std::sqrt (variance ());
}

/*
vector erf (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (erf (v.get (i)), i);
  return result;
}

vector erfc (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (erfc (v.get (i)), i);
  return result;
}

vector erfinv (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (erfinv (v.get (i)), i);
  return result;
}

vector erfcinv (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (erfcinv (v.get (i)), i);
  return result;
}*/

vector rad2deg (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (rad2deg (v.get (i)), i);
  return result;
}

vector deg2rad (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (deg2rad (v.get (i)), i);
  return result;
}

/*
vector i0 (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (i0 (v.get (i)), i);
  return result;
}

vector jn (const int n, vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (jn (n, v.get (i)), i);
  return result;
}*/

vector yn (const int n, vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (yn (n, v.get (i)), i);
  return result;
}

vector polar (const nr_complex_t a, vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (qucs::polar (a, v.get (i)), i);
  return result;
}

vector polar (vector v, const nr_complex_t p) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (qucs::polar (v.get (i), p), i);
  return result;
}

vector polar (vector a, vector p) {
  int j, i, n, len, len1 = a.getSize (), len2 = p.getSize ();
  if (len1 >= len2) {
    assert (len1 % len2 == 0);
    len = len1;
  } else {
    assert (len2 % len1 == 0);
    len = len2;
  }
  vector res (len);
  for (j = i = n = 0; n < len; n++) {
    res (n) = qucs::polar (a (i), p (j));
    if (++i >= len1) i = 0;  if (++j >= len2) j = 0;
  }
  return res;
}

vector atan2 (const nr_double_t y, vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++)
    result.set (atan2 (y, v.get (i)), i);
  return result;
}

vector atan2 (vector v, const nr_double_t x) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++)
    result.set (atan2 (v.get (i), x) , i);
  return result;
}

vector atan2 (vector y, vector x) {
  int j, i, n, len, len1 = y.getSize (), len2 = x.getSize ();
  if (len1 >= len2) {
    assert (len1 % len2 == 0);
    len = len1;
  } else {
    assert (len2 % len1 == 0);
    len = len2;
  }
  vector res (len);
  for (j = i = n = 0; n < len; n++) {
    res (n) = atan2 (y (i), x (j));
    if (++i >= len1) i = 0; if (++j >= len2) j = 0;
  }
  return res;
}

vector w2dbm (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++)
    result.set (10.0 * log10 (v.get (i) / 0.001), i);
  return result;
}

vector dbm2w (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++)
    result.set (0.001 * pow (10.0 , v.get (i) / 10.0), i);
  return result;
}

nr_double_t integrate (vector v, const nr_double_t h) {
  nr_double_t s = real (v.get (0) ) / 2;
  for (int i = 1; i < v.getSize () - 1; i++)
    s += real (v.get (i));
  return (s + real (v.get (v.getSize () - 1) ) / 2) * h;
}

nr_complex_t integrate (vector v, const nr_complex_t h) {
  nr_complex_t s;
  s = v.get (0) / 2.0;
  for (int i = 1; i < v.getSize () - 1; i++)
    s += v.get (i);
  return (s + v.get (v.getSize () - 1) / 2.0) * h;
}

vector dbm (vector v, const nr_complex_t z) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++)
    result.set (10.0 * log10 (norm (v.get (i)) / conj (z) / 0.001), i);
  return result;
}

vector runavg (const nr_complex_t x, const int n) {
  vector result (n);
  for (int i = 0; i < n; i++) result.set (x, i);
  return result;
}

vector runavg (vector v, const int n) {
  nr_complex_t s (0.0), y;
  int len = v.getSize () - n + 1, i;
  vector result (len);
  for (i = 0; i < n; i++) s += v.get (i);
  y = s / (nr_double_t) n; // first running average value
  result.set (y, 0);
  for (i = 0; i < len - 1; i++) {
    y += (v.get (i + n) - v.get (i)) / (nr_double_t) n;
    result.set (y, i + 1);
  }
  return result;
}

// smooth a vector over an aperture a
// done extending the vector endpoints and using a two-sided moving average
// moving average length is always odd
vector smooth(vector v, nr_double_t a) {
  int len = v.getSize (), i;
  int t2 = floor(len/2 * a / 100); // moving average is over 2*t2+1 elements
  // auxiliary vector, extend original vector at beginning and end
  int extvlen = len + 2 * t2;
  vector extv(extvlen), result (len);
  // fill auxiliary vector
  for (i = 0; i < extvlen; i++) {
    if (i < t2) {
      extv.set (v(0), i);
    } else if (i >= (len + t2)) {
      extv.set (v(len-1), i);
    } else {
      extv.set (v(i - t2), i);
    }
  }
  return runavg(extv, 2*t2+1);
}

//Ths function calculates the group delay from the svec=S[i,j] and frequency vectors
vector groupdelay(vector svec, vector freq)
{
 return -diff(unwrap(arg(svec)), (2*pi)*(freq));
}

#ifdef DEBUG
// Debug function: Prints the vector object.
void vector::print (void) {
  for (int r = 0; r < size; r++) {
    fprintf (stderr, "%+.2e%+.2ei\n", (double) real (get (r)),
	     (double) imag (get (r)));
  }
}
#endif /* DEBUG */

} // namespace qucs
