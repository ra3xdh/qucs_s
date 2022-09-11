/*
 * msopen.h - microstrip open end class definitions
 *
 * Copyright (C) 2004, 2008 Stefan Jahn <stefan@lkcc.org>
 * Copyright (C) 2004 Michael Margraf <Michael.Margraf@alumni.TU-Berlin.DE>
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

#ifndef __MSOPEN_H__
#define __MSOPEN_H__

class msopen : public qucs::circuit
{
 public:
  CREATOR (msopen);
  static nr_double_t calcCend (nr_double_t, nr_double_t, nr_double_t,
			       nr_double_t, nr_double_t, const char * const, const char * const,
			       const char * const);
  void calcSP (nr_double_t);
  void initDC (void);
  void calcAC (nr_double_t);
  nr_complex_t calcY (nr_double_t);
};

#endif /* __MSOPEN_H__ */
