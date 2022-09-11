/*
 * fourier.h - fourier transformation class definitions
 *
 * Copyright (C) 2005, 2006, 2009 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __FOURIER_H__
#define __FOURIER_H__

namespace qucs {

class vector;

namespace fourier {

  // public functions
  qucs::vector  fft_1d (qucs::vector, int isign = 1);
  qucs::vector ifft_1d (qucs::vector);
  qucs::vector  dft_1d (qucs::vector, int isign = 1);
  qucs::vector idft_1d (qucs::vector);

  // additional public function
  qucs::vector fftshift (qucs::vector);

  // internal functions
  void  _fft_1d (nr_double_t *, int, int isign = 1);
  void _ifft_1d (nr_double_t *, int);
  void  _dft_1d (nr_double_t *, int, int isign = 1);
  void _idft_1d (nr_double_t *, int);

  void  _fft_1d_2r (nr_double_t *, nr_double_t *, int);
  void _ifft_1d_2r (nr_double_t *, nr_double_t *, int);

  void  _fft_nd (nr_double_t *, int[], int, int isign = 1);
  void _ifft_nd (nr_double_t *, int[], int);

} // namespace

} // namespace qucs

#endif /* __FOURIER_H__ */
