/*
 * nasolution.h - nodal analysis solution template class definitions
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

#ifndef __NASOLUTION_H__
#define __NASOLUTION_H__

#include <string>
#include <unordered_map>

namespace qucs {

/* The solution entry class. */
template <class nr_type_t>
class naentry
{
 public:
  naentry () = default;
  naentry (const naentry &) = default;
  naentry (nr_type_t &v, int c):
    current(c), value(v) {};
  ~naentry () = default;

 public:
  int current;
  nr_type_t value;
};

/* The solution class. */
template <typename T>
    using  nasolution = std::unordered_map<std::string,naentry<T> >;


} // namespace qucs


#endif /* __NASOLUTION_H__ */
