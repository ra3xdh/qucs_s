/*
 * poly.h - poly class definitions and implementation
 *
 * Copyright (C) 2005 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __POLY_H__
#define __POLY_H__

namespace qucs {

class poly
{
 public:
  poly (nr_double_t _x, nr_double_t _y)
    : x(_x), f0(_y) { f1 = f2 = 0; }
  poly (nr_double_t _x, nr_double_t _f0, nr_double_t _f1)
    : x(_x), f0(_f0), f1(_f1) { f2 = 0; }
  poly (nr_double_t _x, nr_double_t _f0, nr_double_t _f1, nr_double_t _f2)
    : x(_x), f0(_f0), f1(_f1), f2(_f2) { }
  ~poly ()
    { }
  nr_double_t eval (nr_double_t _x) {
    nr_double_t dx = _x - x; return f0 + dx * (f1 + dx * f2);
  }

  nr_double_t x;  // x     - argument
  nr_double_t f0; // f(x)  - value
  nr_double_t f1; // f'(x) - 1st derivative
  nr_double_t f2; // f"(x) - 2nd derivative
};

} // namespace qucs

#endif /* __POLY_H__ */
