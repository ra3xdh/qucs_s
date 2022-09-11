/*
 * history.cpp - history class implementation
 *
 * Copyright (C) 2006, 2007 Stefan Jahn <stefan@lkcc.org>
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
#include <cmath>

#include "precision.h"
#include "tvector.h"
#include "poly.h"
#include "spline.h"
#include "history.h"

namespace qucs {


/* This function drops those values in the history which are newer
   than the specified time. */
void history::truncate (const nr_double_t tcut)
{
    std::size_t i;
    std::size_t ts = this->t->size ();

    for (i = this->leftidx (); i < ts; i++)
    {
      if ( (*this->t)[i] > tcut)
      {
	break;
      }
    }
    this->resize (ts - i);
}


/* This function drops those values in the history which are older
   than the specified age of the history instance. */
void history::drop (void) {
  if(this->values->empty())
    return;
  nr_double_t f = this->first ();
  nr_double_t l = this->last ();
  if (age > 0.0 && l - f > age) {
    std::size_t r;
    std::size_t i = this->leftidx ();
    for (r = 0; i < this->t->size (); r++, i++)
      if (l - (*this->t)[i] < age)
	break;
    // keep 2 values being older than specified age
    r += this->unused ();
    if (r >= 2)
      r -= 2;
    r = std::min(r,this->values->size()-1);
    if (r > 127)
      /* erase the first r value */
      this->values->erase(this->values->begin(),this->values->begin()+r);
  }
}

/* Interpolates a value using 2 left side and 2 right side values if
   possible. */
nr_double_t history::interpol (nr_double_t tval, int idx, bool left) {
  static spline spl (SPLINE_BC_NATURAL);
  static tvector<nr_double_t> x (4);
  static tvector<nr_double_t> y (4);

  unsigned int n = left ? idx + 1: idx;
  if (n > 1 && n + 2 < this->values->size ()) {
    int i, k, l = this->leftidx ();
    for (k = 0, i = n - 2; k < 4; i++, k++) {
      x (k) = (*this->t)[i + l];
      y (k) = (*this->values)[i];
    }
    spl.vectors (y, x);
    spl.construct ();
    return spl.evaluate (tval).f0;
  }
  return (*this->values)[idx];
}

/* The function returns the value nearest to the given time value.  If
   the optional parameter is true then additionally cubic spline
   interpolation is used. */
nr_double_t history::nearest (nr_double_t tval, bool interpolate) {
  if (t->empty())
    return 0.0;

  int l = this->leftidx ();
  int r = t->size () - 1;
  int i = -1;
  nr_double_t diff = std::numeric_limits<nr_double_t>::max();
  sign = true;
  i = seek (tval, l, r, diff, i);
  i = i - l;
  if (interpolate)
    return interpol (tval, i, sign);
  return (*this->values)[i];
}

/* The function is utilized in order to find the nearest value to a
   given time value.  Since the time vector is ordered a recursive
   lookup algorithm can be used.  The function returns the current
   index into the time vector as well as the error in time. */
int history::seek (nr_double_t tval, int l, int r, nr_double_t& diff,
		   int idx) {
  int i = (l + r) / 2;
  if (l == r)
    return i;
  nr_double_t v = (*this->t)[i];
  nr_double_t d = v - tval;
  if (fabs (d) < diff) {
    // better approximation
    diff = fabs (d);
    sign = d < 0.0 ? true : false;
    idx = i;
  }
  else if (i == l) {
    // no better
    return idx;
  }
  if (d < 0.0) {
    // look later
    return seek (tval, i, r, diff, idx);
  }
  else if (d > 0.0) {
    // look earlier
    return seek (tval, l, i, diff, idx);
  }
  return idx;
}

} // namespace qucs
