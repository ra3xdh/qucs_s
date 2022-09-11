/*
 * msline.h - microstrip transmission line class definitions
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

#ifndef __MSLINE_H__
#define __MSLINE_H__

class msline : public qucs::circuit
{
 public:
  CREATOR (msline);
  void initDC (void);
  void calcNoiseSP (nr_double_t);
  void calcSP (nr_double_t);
  void calcPropagation (nr_double_t);
  void initAC (void);
  void calcAC (nr_double_t);
  void calcNoiseAC (nr_double_t);
  void saveCharacteristics (nr_double_t);

  static void analyseQuasiStatic (nr_double_t, nr_double_t, nr_double_t,
				  nr_double_t, const char * const,
				  nr_double_t&, nr_double_t&, nr_double_t&);
  static void analyseDispersion (nr_double_t, nr_double_t, nr_double_t,
				 nr_double_t, nr_double_t, nr_double_t, const char * const,
				 nr_double_t&, nr_double_t&);
  static void Hammerstad_ab (nr_double_t, nr_double_t,
			     nr_double_t&, nr_double_t&);
  static void Hammerstad_er (nr_double_t, nr_double_t, nr_double_t,
			     nr_double_t, nr_double_t&);
  static void Hammerstad_zl (nr_double_t, nr_double_t&);
  static void Getsinger_disp (nr_double_t, nr_double_t, nr_double_t,
			      nr_double_t, nr_double_t,
			      nr_double_t&, nr_double_t&);
  static void Kirschning_er (nr_double_t, nr_double_t, nr_double_t,
			     nr_double_t, nr_double_t&);
  static void Kirschning_zl (nr_double_t, nr_double_t, nr_double_t,
			     nr_double_t, nr_double_t, nr_double_t,
			     nr_double_t&, nr_double_t&);
  static void analyseLoss (nr_double_t, nr_double_t, nr_double_t, nr_double_t,
			   nr_double_t, nr_double_t, nr_double_t, nr_double_t,
			   nr_double_t, nr_double_t, const char *,
			   nr_double_t&, nr_double_t&);

 private:
  nr_double_t alpha, beta, zl, ereff;
};

#endif /* __MSLINE_H__ */
