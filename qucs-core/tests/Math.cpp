/*
 * Math.cpp - Unit test for math operations
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
#include "real.h"
#include "complex.h"

#include "testDefine.h"   // constants used on tests
#include "gtest/gtest.h"  // Google Test

TEST(Real, sqrt) {
  ASSERT_EQ (18.0, qucs::sqrt (324.0));
  ASSERT_EQ (0.0, qucs::sqrt (0.0));
  //ASSERT_EQ (-1, qucs::sqrt(-22.0)); //error NaN (depending on implementation)
}

TEST(Real, round) {
  ASSERT_EQ (1.0, qucs::round (1.4));
}
TEST(real, trunc) {
  ASSERT_EQ (2.0, qucs::round (2.3));
}

TEST(Real, jn_BesselFistKind) {
  // FIXME not in qucs::
  EXPECT_NEAR ( 0.44, jn (1, 1), 0.0001);
}

TEST(Real, Factorial) {
  ASSERT_EQ ( 479001600, qucs::factorial (12) );
}

TEST (Complex, cos) {
  nr_complex_t z = nr_complex_t (1.0, 1.0);
  EXPECT_NEAR ( 0.83373002, qucs::cos (z).real(), tol);
  EXPECT_NEAR (-0.98889770, qucs::cos (z).imag(), tol);
}

TEST (Complex, jn_BesselFistKind) {
  nr_complex_t z = nr_complex_t (1.0, 1.0);
  EXPECT_NEAR ( 0.61416033492290361, qucs::jn (1, z).real(), tol);
  EXPECT_NEAR ( 0.36502802882708778, qucs::jn (1, z).imag(), tol);
}
