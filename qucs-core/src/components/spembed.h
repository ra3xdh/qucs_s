/*
 * spembed.h - S-parameters embedding component class definitions
 *
 * Copyright (C) 2004, 2005, 2006, 2008, 2009 Stefan Jahn <stefan@lkcc.org>
 * Copyright (C) 2017 Qucs Team
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
 */

#ifndef SPEMBED_H
#define SPEMBED_H

#include "spfile.h"

/* S-parameters embedding component takes data from an SnP file */
class spembed : public spfile, public qucs::circuit
{
 public:
  CREATOR(spembed);

  void initSP (void);
  void calcSP (nr_double_t);
  void initDC (void);
  void initTR (void);
  void initAC (void);
  void calcAC (nr_double_t);

  void calcNoiseSP (nr_double_t);
  void calcNoiseAC (nr_double_t);

  qucs::matrix correlationMatrix (nr_double_t, nr_complex_t, nr_double_t, qucs::matrix);
  nr_double_t noiseFigure (qucs::matrix, qucs::matrix, nr_double_t&, nr_complex_t&, nr_double_t&);
  qucs::matrix expandNoiseMatrix (qucs::matrix, qucs::matrix);
  qucs::matrix shrinkNoiseMatrix (qucs::matrix, qucs::matrix);
  qucs::matrix calcMatrixCs (nr_double_t);
};

#endif /* SPEMBED_H */
