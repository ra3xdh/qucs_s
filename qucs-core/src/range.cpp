/*
 * range.cpp - range class implementation
 *
 * Copyright (C) 2006 Stefan Jahn <stefan@lkcc.org>
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

#include "range.h"

namespace qucs {

// Constructor creates an instance of the range class.
range::range () {
  il = ih = '.';
  l = h = 0.0;
  txt = NULL;
}

// Constructor creates an fully qualified instance of the range class.
range::range (char ilo, nr_double_t lo, nr_double_t hi, char ihi) {
  il = ilo;
  ih = ihi;
  if (lo > hi) {
    h = lo;
    l = hi;
  } else {
    l = lo;
    h = hi;
  }
  txt = NULL;
}

/* This copy constructor creates a instance of the range class based
   on the given range. */
range::range (const range & r) {
  txt = r.txt ? strdup (r.txt) : NULL;
  il = r.il;
  ih = r.ih;
  l = r.l;
  h = r.h;
}

/* Checks whether the given value is outside the range. */
bool range::outside (nr_double_t value) {
  return !inside (value);
}

/* Checks whether the given value is inside the range. */
bool range::inside (nr_double_t value) {
  int err = 0;
  if (il == '[' &&  (value < l))
    err++;
  if (il == ']' && !(value > l))
    err++;
  if (ih == '[' && !(value < h))
    err++;
  if (ih == ']' &&  (value > h))
    err++;
  return err == 0;
}

// Destructor deletes an instance of the range class.
range::~range () {
  free (txt);
}

/* Returns a text representation of the range object. */
char * range::toString (void) {
  char str[64];
  sprintf (str, "%c%g,%g%c", il, l, h, ih);
  free (txt);
  txt = strdup (str);
  return txt;
}

} // namespace qucs
