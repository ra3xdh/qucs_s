/*
 * jfet.h - jfet class definitions
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

#ifndef __JFET_H__
#define __JFET_H__

class jfet : public qucs::circuit
{
 public:
  CREATOR (jfet);
  void calcSP (nr_double_t);
  void calcNoiseSP (nr_double_t);
  void calcDC (void);
  void initDC (void);
  void restartDC (void);
  void saveOperatingPoints (void);
  void calcOperatingPoints (void);
  void loadOperatingPoints (void);
  void initAC (void);
  void calcAC (nr_double_t);
  void calcNoiseAC (nr_double_t);
  void initTR (void);
  void calcTR (nr_double_t);

 private:
  qucs::matrix calcMatrixY (nr_double_t);
  qucs::matrix calcMatrixCy (nr_double_t);
  void initModel (void);

 private:
  nr_double_t Ugs, Ugd, Uds, UgsPrev, UgdPrev;
  nr_double_t ggs, ggd, gm, gds, Ids, Qgs, Qgd;
  qucs::circuit * rs;
  qucs::circuit * rd;
};

#endif /* __JFET_H__ */
