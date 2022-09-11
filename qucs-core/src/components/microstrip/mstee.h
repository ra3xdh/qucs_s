/*
 * mstee.h - microstrip t-junction class definitions
 *
 * Copyright (C) 2004, 2005, 2008 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __MSTEE_H__
#define __MSTEE_H__

namespace qucs {
  class circuit;
  class net;
}

class mstee : public qucs::circuit
{
 public:
  CREATOR (mstee);
  void initDC (void);
  void initAC (void);
  void calcAC (nr_double_t);
  void initNoiseAC (void);
  void initSP (void);
  void initNoiseSP (void);
  void calcSP (nr_double_t);
  void initTR (void);

  friend qucs::circuit * splitMicrostrip (qucs::circuit *, qucs::circuit *, qucs::net *,
                                          const char *, const char *, int);
  friend void disableMicrostrip (qucs::circuit *, qucs::circuit *, qucs::net *, int);

 private:
  void calcPropagation (nr_double_t);
  void initLines (void);

 private:
  nr_double_t Bt, La, Lb, L2, Ta2, Tb2;
  qucs::circuit * lineA;
  qucs::circuit * lineB;
  qucs::circuit * line2;
};

#endif /* __MSTEE_H__ */
