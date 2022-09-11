/*
 * spline.h - spline class definitions
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

#ifndef __SPLINE_H__
#define __SPLINE_H__

#include <vector>
#include "tvector.h"

namespace qucs {

// Types of boundary conditions.
enum spline_boundary_type {
  SPLINE_BC_UNKNOWN = -1,
  SPLINE_BC_NATURAL,       // natural splines -- zero derivatives
  SPLINE_BC_CLAMPED,       // endpoint derivatives given
  SPLINE_BC_PERIODIC       // periodic splines
};

class vector;
class poly;

class spline
{
 public:
  spline ();
  spline (int);
  spline (tvector<nr_double_t>, tvector<nr_double_t>);
  spline (qucs::vector, qucs::vector);
  spline (std::vector<nr_double_t>, std::vector<nr_double_t>);
  ~spline ();

  void vectors (qucs::vector, qucs::vector);
  void vectors (tvector<nr_double_t>, tvector<nr_double_t>);
  void vectors (std::vector<nr_double_t>, std::vector<nr_double_t>);
  void vectors (nr_double_t *, nr_double_t *, int);
  void construct (void);
  poly evaluate (nr_double_t);
  void setBoundary (int b) { boundary = b; }
  void setDerivatives (nr_double_t l, nr_double_t r) { d0 = l; dn = r; }

 private:
  nr_double_t * upper_bound (nr_double_t *, nr_double_t *, nr_double_t);
  void realloc (int);

 private:
  nr_double_t * x;
  nr_double_t * f0;
  nr_double_t * f1;
  nr_double_t * f2;
  nr_double_t * f3;
  nr_double_t d0, dn;
  int n;
  int boundary;
};

} // namespace qucs

#endif /* __SPLINE_H__ */
