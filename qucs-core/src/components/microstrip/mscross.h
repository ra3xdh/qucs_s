/*
 * mscross.h - microstrip cross-junction class definitions
 *
 * Copyright (C) 2004, 2007, 2008 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __MSCROSS_H__
#define __MSCROSS_H__

class mscross : public qucs::circuit
{
 public:
  CREATOR (mscross);
  void initDC (void);
  void initAC (void);
  void calcAC (nr_double_t);
  void initSP (void);
  void calcSP (nr_double_t);
  void initTR (void);

 private:
  void initModel (void);
  qucs::matrix calcMatrixY (nr_double_t);
  nr_double_t capCorrection (nr_double_t, nr_double_t);
  nr_double_t calcCap (nr_double_t, nr_double_t, nr_double_t);
  nr_double_t calcInd (nr_double_t, nr_double_t, nr_double_t);
};

#endif /* __MSCROSS_H__ */
