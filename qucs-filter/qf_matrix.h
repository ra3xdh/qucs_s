/***************************************************************************
                               qf_matrix.h
                             ----------------
    begin                : Mon Jan 02 2006
    copyright            : (C) 2006 by Stefan Jahn
    email                : stefan@lkcc.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _QF_MATRIX_H
#define _QF_MATRIX_H

#include "qf_math.h"
#include <stdio.h>
#include <stdlib.h>

namespace qf {

class matrix {
public:
  // constructor
  matrix(unsigned int d) {
    data = (qf_float*)calloc(d * d, sizeof(qf_float));
    n    = d;
  }

  // destructor
  ~matrix() { free(data); }

  // accessor operators
  qf_float operator()(int r, int c) const { return data[r * n + c]; }
  qf_float& operator()(int r, int c) { return data[r * n + c]; }

  // size of matrix
  unsigned int n;

private:
  qf_float* data;
};
} // namespace qf

#endif // _QF_MATRIX_H
