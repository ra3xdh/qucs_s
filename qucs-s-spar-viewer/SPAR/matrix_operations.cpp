/*
 *  Copyright (C) 2025 Andrés Martínez Mera - andresmmera@protonmail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "SParameterCalculator.h"

vector<vector<Complex>> SParameterCalculator::createMatrix(int rows, int cols) {
  return vector<vector<Complex>>(rows, vector<Complex>(cols, Complex(0, 0)));
}

vector<vector<Complex>>
SParameterCalculator::invertMatrix(const vector<vector<Complex>>& matrix) {
  int n = matrix.size();
  vector<vector<Complex>> augmented(n, vector<Complex>(2 * n, Complex(0, 0)));

  // Create augmented matrix [A|I]
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      augmented[i][j] = matrix[i][j];
    }
    augmented[i][i + n] = Complex(1, 0);
  }

  // Forward elimination
  for (int i = 0; i < n; i++) {
    // Find pivot
    int pivot = i;
    for (int j = i + 1; j < n; j++) {
      if (abs(augmented[j][i]) > abs(augmented[pivot][i])) {
        pivot = j;
      }
    }

    // Swap rows
    if (pivot != i) {
      swap(augmented[i], augmented[pivot]);
    }

    // Make diagonal element 1
    Complex diag = augmented[i][i];
    if (abs(diag) < 1e-12) {
      throw runtime_error("Matrix is singular and cannot be inverted");
    }

    for (int j = 0; j < 2 * n; j++) {
      augmented[i][j] /= diag;
    }

    // Eliminate column
    for (int j = 0; j < n; j++) {
      if (i != j) {
        Complex factor = augmented[j][i];
        for (int k = 0; k < 2 * n; k++) {
          augmented[j][k] -= factor * augmented[i][k];
        }
      }
    }
  }

  // Extract inverse matrix
  vector<vector<Complex>> inverse(n, vector<Complex>(n));
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      inverse[i][j] = augmented[i][j + n];
    }
  }

  return inverse;
}
