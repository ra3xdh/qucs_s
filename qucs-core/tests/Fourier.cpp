/*
 * Fourier.cpp - Unit test for operations in Fourier namespace
 *
 * Copyright (C) 2015 Guilherme Brondani Torri <guitorri@gmail.com>
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

#include "qucs_typedefs.h"
#include "object.h"
#include "vector.h"
#include "fourier.h"

#include "gtest/gtest.h"  // Google Test

TEST (fourier, fft) {
  // fft of a DC vector
  // in   [1, 1, 1, 1, 1, 1, 1, 1]
  // out  [8, 0, 0, 0, 0, 0, 0, 0]
  qucs::vector vec = qucs::vector(8);

  for (int k = 0; k < vec.getSize(); k++)
    vec.set(1, k);

  qucs::vector vdif  = qucs::fourier::fft_1d ( vec ) ;

  for (int k = 0; k < vec.getSize(); k++)
    if (k==0)
      EXPECT_EQ ( 8 , vdif.get(k).real() );
    else
      EXPECT_EQ ( 0 , vdif.get(k).real() );
}
