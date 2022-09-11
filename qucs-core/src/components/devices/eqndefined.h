/*
 * eqndefined.h - equation defined device class definitions
 *
 * Copyright (C) 2007, 2008 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __EQNDEFINED_H__
#define __EQNDEFINED_H__

class eqndefined : public qucs::circuit
{
 public:
  CREATOR (eqndefined);
  ~eqndefined ();
  void initDC (void);
  void calcDC (void);
  void initAC (void);
  void calcAC (nr_double_t);
  void saveOperatingPoints (void);
  void initSP (void);
  void calcSP (nr_double_t);
  void initTR (void);
  void calcTR (nr_double_t);
  void initHB (int);
  void calcHB (int);

 private:
  void initModel (void);
  char * createVariable (const char *, int, int, bool prefix = true);
  char * createVariable (const char *, int, bool prefix = true);
  void setResult (void *, nr_double_t);
  nr_double_t getResult (void *);
  qucs::matrix calcMatrixY (nr_double_t);
  void evalOperatingPoints (void);
  void updateLocals (void);

 private:
  void ** veqn;
  void ** ieqn;
  void ** geqn;
  void ** qeqn;
  void ** ceqn;
  nr_double_t * _jstat;
  nr_double_t * _jdyna;
  nr_double_t * _charges;
  bool doHB;
};

#endif /* __EQNDEFINED_H__ */
