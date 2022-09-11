/*
 * spfile.h - S-parameter file class definitions
 *
 * Copyright (C) 2004, 2005, 2006, 2008, 2009 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __SPFILE_H__
#define __SPFILE_H__

namespace qucs {
  class vector;
  class matvec;
  class dataset;
  class interpolator;
}

class spfile_vector
{
 public:
  spfile_vector ();
  ~spfile_vector ();

 public:
  void prepare (qucs::vector *, qucs::vector *, bool, int, int);
  nr_complex_t interpolate (nr_double_t);

 public:
  qucs::vector * v;
  qucs::vector * f;
  int isreal;
  qucs::interpolator * inter;
  int r;
  int c;
};

class spfile
{
 public:
  spfile ();
  ~spfile ();
  void createIndex (void);
  void prepare (void);
  void createVector (int, int);
  qucs::matrix expandSParaMatrix (qucs::matrix);
  qucs::matrix shrinkSParaMatrix (qucs::matrix);
  qucs::matrix getInterpolMatrixS (nr_double_t);

  int nPorts;
  qucs::dataset * data;
  qucs::vector * sfreq;
  qucs::vector * nfreq;
  spfile_vector * spara;
  spfile_vector * RN;
  spfile_vector * FMIN;
  spfile_vector * SOPT;
  char paraType;
  int  dataType;
  int  interpolType;
};

#endif /* __SPFILE_H__ */
