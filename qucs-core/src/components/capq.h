/*
 * capq.h - Lossy inductor class definition
 *
 * Copyright (C) 2015 Andres Martinez-Mera <andresmartinezmera@gmail.com>
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

#ifndef CAPQ_H
#define CAPQ_H
#include "matrix.h"


class capq : public qucs::circuit
{
 public:
  CREATOR (capq);
  void calcSP (nr_double_t);
  void calcNoiseSP (nr_double_t);
  void initDC (void);
  void initAC (void);
  void initSP (void);
  void calcAC (nr_double_t);
  void calcNoiseAC (nr_double_t);

 private:
  void calcYp (nr_double_t);
  nr_complex_t Yp;
};

#endif /* __capq_H__ */

