/*
 * rlcg.h - lossy RLCG transmission line class definitions
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

#ifndef __RLCG_H__
#define __RLCG_H__

class rlcg : public qucs::circuit
{
 public:
  CREATOR (rlcg);
  void calcSP (nr_double_t);
  void initDC (void);
  void initAC (void);
  void calcAC (nr_double_t);
  void initTR (void);
  void calcNoiseAC (nr_double_t);
  void calcNoiseSP (nr_double_t);
  void saveCharacteristics (nr_double_t);

 private:
  void calcPropagation (nr_double_t);
  nr_complex_t g;
  nr_complex_t z;
};

#endif /* __RLCG_H__ */
