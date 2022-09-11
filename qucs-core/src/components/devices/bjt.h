/*
 * bjt.h - bipolar junction transistor class definitions
 *
 * Copyright (C) 2004, 2005, 2006, 2008 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __BJT_H__
#define __BJT_H__

class bjt : public qucs::circuit
{
 public:
  CREATOR (bjt);
  void calcSP (nr_double_t);
  void calcNoiseSP (nr_double_t);
  void initSP (void);
  void calcDC (void);
  void initDC (void);
  void restartDC (void);
  void saveOperatingPoints (void);
  void loadOperatingPoints (void);
  void calcOperatingPoints (void);
  void initAC (void);
  void calcAC (nr_double_t);
  void calcNoiseAC (nr_double_t);
  void initTR (void);
  void calcTR (nr_double_t);

 private:
  void initModel (void);
  void processCbcx (void);
  qucs::matrix calcMatrixY (nr_double_t);
  qucs::matrix calcMatrixCy (nr_double_t);
  void excessPhase (int, nr_double_t&, nr_double_t&);

 private:
  nr_double_t Ucs, Ubx, Ube, Ubc, Uce, UbePrev, UbcPrev;
  qucs::circuit * re;
  qucs::circuit * rc;
  qucs::circuit * rb;
  qucs::circuit * cbcx;
  nr_double_t dQbedUbc, dQbdUbe, dQbdUbc, If, Qb, Ir, It;
  nr_double_t gbei, gben, gbci, gbcn, gitf, gitr, gif, gir, Rbb, Ibe;
  nr_double_t Qbe, Qbci, Qbcx, Qcs;
  bool doTR;
};

#endif /* __BJT_H__ */
