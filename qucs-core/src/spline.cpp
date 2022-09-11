/*
 * spline.cpp - spline class implementation
 *
 * Copyright (C) 2005, 2006, 2009 Stefan Jahn <stefan@lkcc.org>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <vector>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "vector.h"
#include "poly.h"
#include "tvector.h"
#include "tridiag.h"
#include "spline.h"

namespace qucs {

// Constructor creates an instance of the spline class.
spline::spline () {
  x = f0 = f1 = f2 = f3 = NULL;
  d0 = dn = 0;
  n = 0;
  boundary = SPLINE_BC_NATURAL;
}

// Constructor creates an instance of the spline class with given boundary.
spline::spline (int b) {
  x = f0 = f1 = f2 = f3 = NULL;
  d0 = dn = 0;
  n = 0;
  boundary = b;
}

// Constructor creates an instance of the spline class with vector data given.
spline::spline (qucs::vector y, qucs::vector t) {
  x = f0 = f1 = f2 = f3 = NULL;
  d0 = dn = 0;
  n = 0;
  boundary = SPLINE_BC_NATURAL;
  vectors (y, t);
  construct ();
}

// Constructor creates an instance of the spline class with tvector data given.
spline::spline (std::vector<nr_double_t> y, std::vector<nr_double_t> t) {
  x = f0 = f1 = f2 = f3 = NULL;
  d0 = dn = 0;
  n = 0;
  boundary = SPLINE_BC_NATURAL;
  vectors (y, t);
  construct ();
}

// Constructor creates an instance of the spline class with tvector data given.
spline::spline (tvector<nr_double_t> y, tvector<nr_double_t> t) {
  x = f0 = f1 = f2 = f3 = NULL;
  d0 = dn = 0;
  n = 0;
  boundary = SPLINE_BC_NATURAL;
  vectors (y, t);
  construct ();
}

#define t_ (t)
#define y_ (y)

// Pass interpolation datapoints as vectors.
void spline::vectors (qucs::vector y, qucs::vector t) {
  int i = t.getSize ();
  assert (y.getSize () == i && i >= 3);

  // create local copy of f(x)
  realloc (i);
  for (i = 0; i <= n; i++) {
    f0[i] = real (y_(i)); x[i] = real (t_(i));
  }
}

// Pass interpolation datapoints as tvectors.
void spline::vectors (std::vector<nr_double_t> y, std::vector<nr_double_t> t) {
  int i = (int)t.size ();
  assert ((int)y.size () == i && i >= 3);

  // create local copy of f(x)
  realloc (i);
  for (i = 0; i <= n; i++) {
    f0[i] = y[i]; x[i] = t[i];
  }
}

// Pass interpolation datapoints as tvectors.
void spline::vectors (tvector<nr_double_t> y, tvector<nr_double_t> t) {
  int i = t.size ();
  assert (y.size () == static_cast<unsigned int>(i) && i >= 3);

  // create local copy of f(x)
  realloc (i);
  for (i = 0; i <= n; i++) {
    f0[i] = y_(i); x[i] = t_(i);
  }
}

// Pass interpolation datapoints as pointers.
void spline::vectors (nr_double_t * y, nr_double_t * t, int len) {
  int i = len;
  assert (i >= 3);

  // create local copy of f(x)
  realloc (i);
  for (i = 0; i <= n; i++) {
    f0[i] = y[i]; x[i] = t[i];
  }
}

// Reallocate vector data if necessary.
void spline::realloc (int size) {
  if (n != size - 1) {
    n = size - 1;
    delete[] f0;
    f0 = new nr_double_t[n+1];
    delete[] x;
    x  = new nr_double_t[n+1];
  }
  delete[] f1;
  delete[] f2;
  delete[] f3;
}

// Construct cubic spline interpolation coefficients.
void spline::construct (void) {

  // calculate first derivative h = f'(x)
  int i;
  nr_double_t * h  = new nr_double_t[n+1];
  for (i = 0; i < n; i++) {
    h[i] = x[i+1] - x[i];
    if (h[i] == 0.0) {
      logprint (LOG_ERROR, "ERROR: Duplicate points in spline: %g, %g\n",
		x[i], x[i+1]);
    }
  }

  // first kind of cubic splines
  if (boundary == SPLINE_BC_NATURAL || boundary == SPLINE_BC_CLAMPED) {

    // setup right hand side
    nr_double_t * b = new nr_double_t[n+1]; // b as in Ax = b
    for (i = 1; i < n; i++) {
      nr_double_t _n = f0[i+1] * h[i-1] - f0[i] * (h[i] + h[i-1]) +
	f0[i-1] * h[i];
      nr_double_t _d = h[i-1] * h[i];
      b[i] = 3 * _n / _d;
    }
    if (boundary == SPLINE_BC_NATURAL) {
      // natural boundary condition
      b[0] = 0;
      b[n] = 0;
    } else if (boundary == SPLINE_BC_CLAMPED) {
      // start and end derivatives given
      b[0] = 3 * ((f0[1] - f0[0]) / h[0] - d0);
      b[n] = 3 * (dn - (f0[n] - f0[n-1]) / h[n-1]);
    }

    nr_double_t * u = new nr_double_t[n+1];
    nr_double_t * z = b; // reuse storage
    if (boundary == SPLINE_BC_NATURAL) {
      u[0] = 0;
      z[0] = 0;
    } else if (boundary == SPLINE_BC_CLAMPED) {
      u[0] = h[0] / (2 * h[0]);
      z[0] = b[0] / (2 * h[0]);
    }

    for (i = 1; i < n; i++) {
      nr_double_t p = 2 * (h[i] + h[i-1]) - h[i-1] * u[i-1]; // pivot
      u[i] = h[i] / p;
      z[i] = (b[i] - z[i-1] * h[i-1]) / p;
    }
    if (boundary == SPLINE_BC_NATURAL) {
      z[n] = 0;
    } else if (boundary == SPLINE_BC_CLAMPED) {
      nr_double_t p = h[n-1] * (2 - u[n-1]);
      z[n] = (b[n] - z[n-1] * h[n-1]) / p;
    }

    // back substitution
    f1 = u; // reuse storage
    f2 = z;
    f3 = h;
    f2[n] = z[n];
    f3[n] = 0;
    for (i = n - 1; i >= 0; i--) {
      f2[i] = z[i] - u[i] * f2[i+1];
      f1[i] = (f0[i+1] - f0[i]) / h[i] - h[i] * (f2[i+1] + 2 * f2[i]) / 3;
      f3[i] = (f2[i+1] - f2[i]) / (3 * h[i]);
    }

    // set up last slot for extrapolation above
    if (boundary == SPLINE_BC_NATURAL) {
      f1[n] = f1[n-1] + (x[n] - x[n-1]) * f2[n-1];
    } else if (boundary == SPLINE_BC_CLAMPED) {
      f1[n] = dn;
    }
    f2[n] = 0;
    f3[n] = 0;
  }

  // second kind of cubic splines
  else if (boundary == SPLINE_BC_PERIODIC) {
    // non-trigdiagonal equations - periodic boundary condition
    //std::vector<nr_double_t> z (n+1);
    nr_double_t *z = new nr_double_t[n+1];
    if (n == 2) {
      nr_double_t B = h[0] + h[1];
      nr_double_t A = 2 * B;
      nr_double_t b[2], det;
      b[0] = 3 * ((f0[2] - f0[1]) / h[1] - (f0[1] - f0[0]) / h[0]);
      b[1] = 3 * ((f0[1] - f0[2]) / h[0] - (f0[2] - f0[1]) / h[1]);
      det = 3 * B * B;
      z[1] = ( A * b[0] - B * b[1]) / det;
      z[2] = (-B * b[0] + A * b[1]) / det;
      z[0] = z[2];
    }
    else {
      tridiag<nr_double_t> sys;
      std::vector<nr_double_t> o (n);
      std::vector<nr_double_t> d (n);
      std::vector<nr_double_t> b(&z[1],&z[n]);
      //b.setData (&z[1], n);
      for (i = 0; i < n - 1; i++) {
        o[i] = h[i+1];
        d[i] = 2 * (h[i+1] + h[i]);
        b[i] = 3 * ((f0[i+2] - f0[i+1]) / h[i+1] - (f0[i+1] - f0[i]) / h[i]);
        z[i+1] = b[i];
      }
      o[i] = h[0];
      d[i] = 2 * (h[0] + h[i]);
      b[i] = 3 * ((f0[1] - f0[i+1]) / h[0] - (f0[i+1] - f0[i]) / h[i]);
      z[i+1] = b[i];
      sys.setDiagonal (&d);
      sys.setOffDiagonal (&o);
      sys.setRHS (&b);
      sys.setType (TRIDIAG_SYM_CYCLIC);
      sys.solve ();
      z[0] = z[n];
    }

    f1 = new nr_double_t[n+1];
    //f2 = &z.front (); // reuse storage
    f2 = z;
    f3 = h;
    for (i = n - 1; i >= 0; i--) {
      f1[i] = (f0[i+1] - f0[i]) / h[i] - h[i] * (z[i+1] + 2 * z[i]) / 3;
      f3[i] = (z[i+1] - z[i]) / (3 * h[i]);
    }
    f1[n] = f1[0];
    f2[n] = f2[0];
    f3[n] = f3[0];
  }
}

// Returns pointer to the first value greater than the given one.
nr_double_t * spline::upper_bound (nr_double_t * first, nr_double_t * last,
				   nr_double_t value) {
  int half, len = last - first;
  nr_double_t * centre;

  while (len > 0) {
    half = len >> 1;
    centre = first;
    centre += half;
    if (value < *centre)
      len = half;
    else {
      first = centre;
      ++first;
      len = len - half - 1;
    }
  }
  return first;
}

// Evaluates the spline at the given position.
poly spline::evaluate (nr_double_t t) {

#ifndef PERIOD_DISABLED
  if (boundary == SPLINE_BC_PERIODIC) {
    // extrapolation easy: periodically
    nr_double_t period = x[n] - x[0];
    while (t > x[n]) t -= period;
    while (t < x[0]) t += period;
  }
#endif /* PERIOD_DISABLED */

  nr_double_t * here = upper_bound (x, x+n+1, t);
  nr_double_t y0, y1, y2;
  if (here == x) {
    nr_double_t dx = t - x[0];
    y0 = f0[0] + dx * f1[0];
    y1 = f1[0];
    return poly (t, y0, y1);
  }
  else {
    int i = here - x - 1;
    nr_double_t dx = t - x[i];
    // value
    y0 = f0[i] + dx * (f1[i] + dx * (f2[i] + dx * f3[i]));
    // first derivative
    y1 = f1[i] + dx * (2 * f2[i] + 3 * dx * f3[i]);
    // second derivative
    y2 = 2 * f2[i] + 6 * dx * f3[i];
    return poly (t, y0, y1, y2);
  }
}

// Destructor deletes an instance of the spline class.
spline::~spline () {
  if (x)  delete[] x;
  delete[] f0;
  delete[] f1;
  delete[] f2;
  delete[] f3;
}

} // namespace qucs
