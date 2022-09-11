/*
 * Spline.cpp - Unit test for spline class
 *
 * Copyright (C) 2017 Guilherme Brondani Torri <guitorri@gmail.com>
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

#include <iostream>

#include "qucs_typedefs.h"
#include "real.h"
#include "poly.h"
#include "spline.h"

#include "gtest/gtest.h"  // Google Test


// unit test for https://github.com/Qucs/qucs/issues/676
TEST(spline, issue676) {
  qucs::spline *rsp = new qucs::spline (qucs::SPLINE_BC_PERIODIC);

  // min size for spline
  int length = 3;

  // allocate arrays
  nr_double_t * rx  = new nr_double_t[length];
  nr_double_t * ry  = new nr_double_t[length];

  // x data
  rx[0] = 0.0;
  rx[1] = 1.0;
  rx[2] = 2.0;

  // y data
  ry[0] = 0.0;
  ry[1] = 10.0;
  ry[2] = 20.0;

  // copy data
  rsp->vectors (ry, rx, length);

  // build cubic spline
  rsp->construct ();

  // test interpolation
  nr_double_t x = 1.01;
  nr_double_t f0 = rsp->evaluate(x).f0; // f(x)
  EXPECT_NEAR (f0, 10.1, 0.01);

  // issue #676 should fail here
  rsp->qucs::spline::~spline();
}

