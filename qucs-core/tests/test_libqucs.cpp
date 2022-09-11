/*
 * test_libqucs.cpp - Miscellaneous unit tests for Qucs core library
 *
 * Copyright (C) 2014, 2015 Guilherme Brondani Torri <guitorri@gmail.com>
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
#include "object.h"
#include "module.h"
#include "components.h"

#include "testDefine.h"   // constants used on tests
#include "gtest/gtest.h"  // Google Test

TEST (module, constructor) {
  //std::cout << "can we print info"  << std::endl;
  module *m = new module ();
  EXPECT_EQ(NULL, m->definition);
  EXPECT_EQ(NULL, m->circreate);
  EXPECT_EQ(NULL, m->anacreate);
}

TEST (component, resistor_getType) {
  resistor *res = new resistor();
  //res->initDC();
  std::cout << CIR_RESISTOR << " -- "<< res->getType() << std::endl;
  EXPECT_EQ( CIR_RESISTOR, res->getType());
}


// --------------------

#include "tridiag.h"
#include "tvector.h"

TEST (tridiag, solve_s_cyc) {
/* # test program in Python
  import numpy as np
  A = np.array(
        [[-2,  1,  0,  0,  2],
         [ 1, -2,  1,  0,  0],
         [ 0,  1, -2,  1,  0],
         [ 0,  0,  1, -2,  1],
         [ 2,  0,  0,  1, -2]])
  b = np.array([1, 2, 3, 4, 5])
  x = np.linalg.solve(A,b)
  print x
  [ 8.21428571  3.85714286  1.5  2.14285714  6.78571429]
*/

  int n = 5;
  std::vector<nr_double_t> x (n);
  x[0] = 8.21428571;
  x[1] = 3.85714286;
  x[2] = 1.5;
  x[3] = 2.14285714;
  x[4] = 6.78571429;

  tridiag<nr_double_t> sys;
  std::vector<nr_double_t> o (n);
  std::vector<nr_double_t> d (n);
  std::vector<nr_double_t> b (n);

  for (int i = 0; i < n; i++) {
    d[i] = -2.;
    o[i] =  1.;
    b[i] = i+1.;
  }
  o[n-1] = 2.;

  /*
  for (int i = 0; i < n; i++) {
    std::cout << d(i);
  }
  std::cout << '\n';
  for (int i = 0; i < n; i++) {
    std::cout << o(i);
  }
  std::cout << '\n';
  for (int i = 0; i < n; i++) {
    std::cout << b(i);
  }
  std::cout << '\n';
  */
  sys.setDiagonal (&d);
  sys.setOffDiagonal (&o);
  sys.setRHS (&b);
  sys.setType (TRIDIAG_SYM_CYCLIC);
  sys.solve ();
  // in-place solver, result in b

  //for (int i = 0; i < n; i++) {
  //  std::cout << b(i) << ", ";
  //}
  //
  for (int i = 0; i < n; i++) {
    EXPECT_NEAR (x[i], b[i],tol);
  }
}
