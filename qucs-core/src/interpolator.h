/*
 * interpolator.h - interpolator class definitions
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

#ifndef __INTERPOLATOR_H__
#define __INTERPOLATOR_H__

// Type of data and interpolators.
#define INTERPOL_LINEAR  1
#define INTERPOL_CUBIC   2
#define INTERPOL_HOLD    4

#define REPEAT_NO 	 1
#define REPEAT_YES       2

#define DATA_RECTANGULAR 0x0100
#define DATA_POLAR       0x0200
#define DATA_MASK_DOMAIN 0xFF00
#define DATA_COMPLEX     0x0001
#define DATA_REAL        0x0002
#define DATA_MASK_TYPE   0x00FF

namespace qucs {

class interpolator
{
 public:
  interpolator ();
  ~interpolator ();

  void vectors (nr_double_t *, nr_double_t *, int);
  void vectors (nr_complex_t *, nr_double_t *, int);
  void rvectors (qucs::vector *, qucs::vector *);
  void cvectors (qucs::vector *, qucs::vector *);
  void prepare (int, int, int domain = DATA_RECTANGULAR);
  nr_double_t rinterpolate (nr_double_t);
  nr_complex_t cinterpolate (nr_double_t);

private:
  int findIndex (nr_double_t);
  int findIndex_old (nr_double_t);
  nr_double_t linear (nr_double_t,
		      nr_double_t, nr_double_t, nr_double_t, nr_double_t);
  nr_double_t rlinear (nr_double_t, int);
  nr_complex_t clinear (nr_double_t, int);
  void cleanup (void);

private:
  int dataType;
  int interpolType;
  int repeat;
  int length;
  nr_double_t * rx;
  nr_double_t * ry;
  nr_double_t duration;
  spline * rsp, * isp;
  nr_complex_t * cy;
};

} // namespace qucs

#endif /* __INTERPOLATOR_H__ */
