/*
 * tvector.h - simple vector template class definitions
 *
 * Copyright (C) 2004, 2005, 2006 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __TVECTOR_H__
#define __TVECTOR_H__

#include <vector>
#include <assert.h>

#include <limits>

#include "precision.h"

namespace qucs {

template <class nr_type_t>
class tvector;

// Forward declarations of friend functions.
template <class nr_type_t>
nr_type_t   scalar (tvector<nr_type_t>, tvector<nr_type_t>);
template <class nr_type_t>
nr_double_t maxnorm (tvector<nr_type_t>);
template <class nr_type_t>
nr_double_t norm (tvector<nr_type_t>);
template <class nr_type_t>
nr_type_t   sum (tvector<nr_type_t>);
template <class nr_type_t>
tvector<nr_type_t> conj (tvector<nr_type_t>);
template <class nr_type_t>
tvector<nr_type_t> operator + (tvector<nr_type_t>, tvector<nr_type_t>);
template <class nr_type_t>
tvector<nr_type_t> operator + (tvector<nr_type_t>, nr_type_t);
template <class nr_type_t>
tvector<nr_type_t> operator + (nr_type_t, tvector<nr_type_t>);
template <class nr_type_t>
tvector<nr_type_t> operator - (tvector<nr_type_t>, tvector<nr_type_t>);
template <class nr_type_t>
tvector<nr_type_t> operator * (tvector<nr_type_t>, nr_double_t);
template <class nr_type_t>
tvector<nr_type_t> operator * (nr_double_t, tvector<nr_type_t>);
template <class nr_type_t>
tvector<nr_type_t> operator * (tvector<nr_type_t>, tvector<nr_type_t>);
template <class nr_type_t>
tvector<nr_type_t> operator - (tvector<nr_type_t>);
template <class nr_type_t>
bool operator < (tvector<nr_type_t>, tvector<nr_type_t>);
template <class nr_type_t>
bool operator > (tvector<nr_type_t>, tvector<nr_type_t>);

template <class nr_type_t>
class tvector
{
 public:
  tvector () = default;
  tvector (const std::size_t i) : data(i) {};
  tvector (const tvector &) = default;
  ~tvector () = default;
  nr_type_t get (int);
  void set (int, nr_type_t);
  void set (nr_type_t);
  void set (nr_type_t, int, int);
  void set (tvector, int, int);
  std::size_t  size (void) const { return data.size (); }
  nr_type_t * getData (void) { return data.data(); }
  void clear (void);
  void exchangeRows (int, int);
  int  isFinite (void);
  void print (void);
  void reorder (int *);
  int  contains (nr_type_t, nr_double_t eps = std::numeric_limits<nr_double_t>::epsilon());

  // some basic vector operations
#ifndef _MSC_VER
  friend tvector operator +<> (tvector, tvector);
  friend tvector operator -<> (tvector, tvector);
  friend tvector operator *<> (tvector, nr_double_t);
  friend tvector operator *<> (nr_double_t, tvector);
  friend tvector operator *<> (tvector, tvector);
  friend tvector operator -<> (tvector);
  friend tvector operator +<> (tvector, nr_type_t);
  friend tvector operator +<> (nr_type_t, tvector);
#endif

  // other operations
#ifndef _MSC_VER
  friend nr_double_t norm<> (tvector);
  friend nr_double_t maxnorm<> (tvector);
  friend nr_type_t   sum<> (tvector);
  friend nr_type_t   scalar<> (tvector, tvector);
  friend tvector     conj<> (tvector);
#endif

  // comparisons
#ifndef _MSC_VER
  friend bool operator < <> (tvector, tvector);
  friend bool operator > <> (tvector, tvector);
#endif

  // intrinsic operators
  tvector operator += (tvector);
  tvector operator -= (tvector);
  tvector operator *= (nr_double_t);
  tvector operator /= (nr_double_t);

  // assignment operators
  tvector operator = (const nr_type_t);

  // easy accessor operators
  nr_type_t  operator () (int i) const {
    return data.at(i);
  }
  nr_type_t& operator () (int i) {
    return data.at(i); }
   nr_type_t  operator [] (int i) const {
    return data[i];
  }
  nr_type_t& operator [] (int i) {
    return data[i];
  }

 protected:
  std::vector<nr_type_t> data;

};

  /*
  int  contains (nr_type_t val, nr_double_t eps = std::numeric_limits<nr_double_t>::epsilon()) {
    int count = 0;
    for (int i = 0; i < (int)data.size (); i++) if (abs ((data)[i] - val) <= eps) count++;
    return count;
    }*/


  
} // namespace qucs
  
#include "tvector.cpp"

#endif /* __TVECTOR_H__ */
