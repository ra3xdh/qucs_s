/*
 * ecvs.h - externally controlled voltage source class definitions
 *
 * Copyright (C) 2014 Richard Crozier <richard dot crozier at yahoo dot co dot uk>
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

/** \file ecvs.h
  * \brief The externally controlled voltage source component header file.
  *
  */

/**
  * \ingroup QucsInterface
  */

#ifndef __ECVS_H__
#define __ECVS_H__

class ecvs : public qucs::circuit
{
 public:
  CREATOR (ecvs);
  void initSP (void);
  void initDC (void);
//  void calcDC (void);
  void initAC (void);
//  void initHB (void);
//  void calcHB (nr_double_t frequency);
  void initTR (void);
  void calcTR (nr_double_t);
};

#endif /* __ECVS_H__ */
