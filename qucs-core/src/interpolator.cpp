/*
 * interpolator.cpp - interpolator class implementation
 *
 * Copyright (C) 2009 Stefan Jahn <stefan@lkcc.org>
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

#include "poly.h"
#include "spline.h"
#include "object.h"
#include "vector.h"
#include "interpolator.h"

namespace qucs {

// Constructor creates an instance of the interpolator class.
interpolator::interpolator () {
  rsp = isp = NULL;
  rx = ry = NULL;
  cy = NULL;
  repeat = dataType = interpolType = length = 0;
  duration = 0.0;
}


// Destructor deletes an instance of the interpolator class.
interpolator::~interpolator () {
  delete rsp;
  delete isp;
  free (rx);
  free (ry);
  free (cy);
}

// Cleans up vector storage.
void interpolator::cleanup (void) {
  if (rx) { free (rx); rx = NULL; }
  if (ry) { free (ry); ry = NULL; }
  if (cy) { free (cy); cy = NULL; }
}

// Pass real interpolation datapoints as pointers.
void interpolator::vectors (nr_double_t * y, nr_double_t * x, int len) {
  int len1 = len;
  int len2 = 2 + len * sizeof (nr_double_t);
  if (len > 0) {
    ry = (nr_double_t *) malloc (len2 * sizeof (nr_double_t));
    memcpy (ry, y, len1 * sizeof (nr_double_t));
  }
  if (len > 0) {
    rx = (nr_double_t *) malloc (len2 * sizeof (nr_double_t));
    memcpy (rx, x, len1 * sizeof (nr_double_t));
  }

  dataType = (DATA_REAL & DATA_MASK_TYPE);
  length = len;
}

// Pass real interpolation datapoints as vectors.
void interpolator::rvectors (vector * y, vector * x) {
  int len  = y->getSize ();
  int len1 = len;
  int len2 = 2 + len * sizeof (nr_double_t);
  cleanup ();
  if (len > 0) {
    ry = (nr_double_t *) malloc (len2 * sizeof (nr_double_t));
    for (int i = 0; i < len1; i++) ry[i] = real (y->get (i));
  }
  if (len > 0) {
    rx = (nr_double_t *) malloc (len2 * sizeof (nr_double_t));
    for (int i = 0; i < len1; i++) rx[i] = real (x->get (i));
  }

  dataType = (DATA_REAL & DATA_MASK_TYPE);
  length = len;
}

// Pass complex interpolation datapoints as pointers.
void interpolator::vectors (nr_complex_t * y, nr_double_t * x, int len) {
  int len1 = len;
  int len2 = 2 + len;
  cleanup ();
  if (len > 0) {
    cy = (nr_complex_t *) malloc (len2 * sizeof (nr_complex_t));
    memcpy (cy, y, len1 * sizeof (nr_complex_t));
  }
  if (len > 0) {
    rx = (nr_double_t *) malloc (len2 * sizeof (nr_double_t));
    memcpy (rx, x, len1 * sizeof (nr_double_t));
  }

  dataType = (DATA_COMPLEX & DATA_MASK_TYPE);
  length = len;
}

// Pass complex interpolation datapoints as vectors.
void interpolator::cvectors (vector * y, vector * x) {
  int len  = y->getSize ();
  int len1 = len;
  int len2 = 2 + len;
  cleanup ();
  if (len > 0) {
    cy = (nr_complex_t *) malloc (len2 * sizeof (nr_complex_t));
    for (int i = 0; i < len1; i++) cy[i] = y->get (i);
  }
  if (len > 0) {
    rx = (nr_double_t *) malloc (len2 * sizeof (nr_double_t));
    for (int i = 0; i < len1; i++) rx[i] = real (x->get (i));
  }

  dataType = (DATA_COMPLEX & DATA_MASK_TYPE);
  length = len;
}

// Prepares interpolator instance, e.g. setups spline object.
void interpolator::prepare (int interpol, int repitition, int domain) {
  interpolType = interpol;
  dataType |= (domain & DATA_MASK_DOMAIN);
  repeat = repitition;

  // preparations for cyclic interpolations
  if (repeat & REPEAT_YES) {
    duration = rx[length - 1] - rx[0];
    // set first value to the end of the value vector
    if (cy) cy[length - 1] = cy[0];
    if (ry) ry[length - 1] = ry[0];
  }

  // preparations for polar complex data
  if (cy != NULL && (domain & DATA_POLAR) && length > 1) {
    // unwrap phase of complex data vector
    vector ang = vector (length);
    for (int i = 0; i < length; i++) ang (i) = arg (cy[i]);
    ang = unwrap (ang);
    // store complex data
    for (int i = 0; i < length; i++) {
      cy[i] = nr_complex_t (abs (cy[i]), real (ang (i)));
    }
  }

  // preparations spline interpolations
  if (interpolType & INTERPOL_CUBIC) {

    // prepare complex vector interpolation using splines
    if (cy != NULL) {
      // create splines if necessary
      delete rsp;
      delete isp;
      rsp = new spline (SPLINE_BC_NATURAL);
      isp = new spline (SPLINE_BC_NATURAL);
      if (repeat & REPEAT_YES) {
	rsp->setBoundary (SPLINE_BC_PERIODIC);
	isp->setBoundary (SPLINE_BC_PERIODIC);
      }
      // prepare data vectors
      vector rv = vector (length);
      vector iv = vector (length);
      vector rt = vector (length);
      for (int i = 0; i < length; i++) {
	rv (i) = real (cy[i]);
	iv (i) = imag (cy[i]);
	rt (i) = rx[i];
      }
      // pass data vectors to splines and construct these
      rsp->vectors (rv, rt);
      isp->vectors (iv, rt);
      rsp->construct ();
      isp->construct ();
    }

    // prepare real vector interpolation using spline
    else {
      delete rsp;
      rsp = new spline (SPLINE_BC_NATURAL);
      if (repeat & REPEAT_YES) rsp->setBoundary (SPLINE_BC_PERIODIC);
      rsp->vectors (ry, rx, length);
      rsp->construct ();
    }
  }
}

/* The function performs a binary search on the ascending sorted
   x-vector in order to return the left-hand-side index pointer into
   the x-vector based on the given value. */
int interpolator::findIndex (nr_double_t x) {
  int lo = 0;
  int hi = length;
  int av;
  while (lo < hi) {
    av = lo + ((hi - lo) / 2);
    if (x >= rx[av])
      lo = av + 1;
    else
      // can't be hi = av-1: here rx[av] >= x,
      // so hi can't be < av if rx[av] == x
      hi = av;
  }
  // hi == lo, using hi or lo depends on taste
  if (lo <= length && lo > 0 && x >= rx[lo - 1])
    return lo - 1; // found
  else
    return 0; // not found
}

/* Return the left-hand-side index pointer into the x-vector based on
   the given value.  Returns 0 or 'length' if the value is beyond the
   x-vectors scope. */
int interpolator::findIndex_old (nr_double_t x) {
  int idx = 0;
  for (int i = 0; i < length; i++) {
    if (x >= rx[i]) idx = i;
  }
  return idx;
}

/* Computes simple linear interpolation value for the given values. */
nr_double_t interpolator::linear (nr_double_t x,
				  nr_double_t x1, nr_double_t x2,
				  nr_double_t y1, nr_double_t y2) {
  if (x1 == x2)
    return (y1 + y2) / 2;
  else
    return ((x2 - x) * y1 + (x - x1) * y2) / (x2 - x1);
}

/* Returns real valued linear interpolation. */
nr_double_t interpolator::rlinear (nr_double_t x, int idx) {
  return linear (x, rx[idx], rx[idx+1], ry[idx], ry[idx+1]);
}

/* Returns complex valued linear interpolation. */
nr_complex_t interpolator::clinear (nr_double_t x, int idx) {
  nr_double_t x1, x2, r, i;
  nr_complex_t y1, y2;
  x1 = rx[idx]; x2 = rx[idx+1];
  y1 = cy[idx]; y2 = cy[idx+1];
  r = linear (x, x1, x2, real (y1), real (y2));
  i = linear (x, x1, x2, imag (y1), imag (y2));
  return nr_complex_t (r, i);
}

/* This function interpolates for real values.  Returns the linear
   interpolation of the real y-vector for the given value in the
   x-vector. */
nr_double_t interpolator::rinterpolate (nr_double_t x) {
  int idx = -1;
  nr_double_t res = 0.0;

  // no chance to interpolate
  if (length <= 0) {
    return res;
  }
  // no interpolation necessary
  else if (length == 1) {
    res = ry[0];
    return res;
  }
  else if (repeat & REPEAT_YES)
    x = x - std::floor (x / duration) * duration;

  // linear interpolation
  if (interpolType & INTERPOL_LINEAR) {
    idx = findIndex (x);
    // dependency variable in scope or beyond
    if (x == rx[idx])
      res = ry[idx];
    // dependency variable is beyond scope; use last tangent
    else {
      if (idx == length - 1) idx--;
      res = rlinear (x, idx);
    }
  }
  // cubic spline interpolation
  else if (interpolType & INTERPOL_CUBIC) {
    // evaluate spline functions
    res = rsp->evaluate (x).f0;
  }
  else if (interpolType & INTERPOL_HOLD) {
    // find appropriate dependency index
    idx = findIndex (x);
    res = ry[idx];
  }
  return res;
}

/* This function interpolates for complex values.  Returns the complex
   interpolation of the real y-vector for the given value in the
   x-vector. */
nr_complex_t interpolator::cinterpolate (nr_double_t x) {
  int idx = -1;
  nr_complex_t res = 0.0;

  // no chance to interpolate
  if (length <= 0) {
    return res;
  }
  // no interpolation necessary
  else if (length == 1) {
    res = cy[0];
    return res;
  }
  else if (repeat & REPEAT_YES)
    x = x - std::floor (x / duration) * duration;

  // linear interpolation
  if (interpolType & INTERPOL_LINEAR) {
    idx = findIndex (x);
    // dependency variable in scope or beyond
    if (x == rx[idx])
      res = cy[idx];
    // dependency variable is beyond scope; use last tangent
    else {
      if (idx == length - 1) idx--;
      res = clinear (x, idx);
    }
  }
  // cubic spline interpolation
  else if (interpolType & INTERPOL_CUBIC) {
    // evaluate spline functions
    nr_double_t r = rsp->evaluate (x).f0;
    nr_double_t i = isp->evaluate (x).f0;
    res = nr_complex_t (r, i);
  }
  else if (interpolType & INTERPOL_HOLD) {
    // find appropriate dependency index
    idx = findIndex (x);
    res = cy[idx];
  }

  // depending on the data type return appropriate complex value
  if (dataType & DATA_POLAR)
    return std::polar (real (res), imag (res));
  else
    return res;
}

} // namespace qucs
