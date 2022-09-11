/*
 * precision.h - global precision header file
 *
 * Copyright (C) 2005, 2006 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __PRECISION_H__
#define __PRECISION_H__

#include <limits>


#if NR_DOUBLE_SIZE == 4  /* S 23bit MAN | S 7bit EXP */
  /* single precision */
#define NR_TINY 1e-6

#elif NR_DOUBLE_SIZE == 8  /* S 52bit MAN | S 10bit EXP */
  /* double precision */
#define NR_TINY 1e-12

#elif NR_DOUBLE_SIZE == 12 /* S 63bit MAN | S 14bit EXP */
  /* IEEE 80-bit floating point */
#define NR_TINY 1e-15

#elif NR_DOUBLE_SIZE == 16 /* S 112bit MAN | S 14bit EXP */
  /* IEEE 128-bit floating point */
#define NR_TINY 1e-15
#endif



#endif /* __PRECISION_H__ */
