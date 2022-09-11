/*
 * Matrix.cpp - Unit test for matrix operations
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
#include "matrix.h"

#include "gtest/gtest.h"  // Google Test

TEST (matrix, getCols) {
    qucs::matrix data =  qucs::eye(3,3);
    EXPECT_EQ ( 3 , data.getCols() );
}

