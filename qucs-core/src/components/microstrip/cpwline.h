/*
 * cpwline.h - coplanar waveguide line class definitions
 *
 * Copyright (C) 2004, 2005 Vincent Habchi, F5RCS <10.50@free.fr>
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

#ifndef __CPWLINE_H__
#define __CPWLINE_H__

class cpwline : public qucs::circuit
{
 public:
  CREATOR (cpwline);
  void initSP (void);
  void calcSP (nr_double_t);
  void calcNoiseSP (nr_double_t);
  void initDC (void);
  void initTR (void);
  void initAC (void);
  void calcAC (nr_double_t);
  void calcNoiseAC (nr_double_t);
  void saveCharacteristics (nr_double_t);

  static void ellipke (nr_double_t, nr_double_t &, nr_double_t &);
  static nr_double_t ellipk (nr_double_t);
  static nr_double_t ellipa (nr_double_t);
  static nr_double_t KoverKp(nr_double_t);

  static void analyseQuasiStatic (nr_double_t, nr_double_t, nr_double_t,
				  nr_double_t, nr_double_t, int,
				  nr_double_t&, nr_double_t&);
  static void analyseDispersion (nr_double_t, nr_double_t, nr_double_t,
				 nr_double_t, nr_double_t,
				 nr_double_t, nr_double_t,
				 nr_double_t&, nr_double_t&);

 private:
  void calcAB (nr_double_t, nr_double_t&, nr_double_t&, nr_double_t&);
  void initPropagation (void);

  nr_double_t sr_er;
  nr_double_t sr_er0;
  nr_double_t zl_factor;
  nr_double_t ac_factor;
  nr_double_t ad_factor;
  nr_double_t bt_factor;
  nr_double_t fte, G;
  nr_double_t len, tand, rho;
  nr_double_t Zl, Er;
};

#endif /* __CPWLINE_H__ */
