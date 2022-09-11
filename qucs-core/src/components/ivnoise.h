/*
 * ivnoise.h - correlated noise sources class definitions
 *
 * Copyright (C) 2005, 2008 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __IVNOISE_H__
#define __IVNOISE_H__

class ivnoise : public qucs::circuit
{
 public:
  CREATOR (ivnoise);
  void initSP (void);
  void calcNoiseSP (nr_double_t);
  void calcNoiseAC (nr_double_t);
  void initDC (void);
  void initAC (void);
  void initTR (void);
};

#endif /* __IVNOISE_H__ */
