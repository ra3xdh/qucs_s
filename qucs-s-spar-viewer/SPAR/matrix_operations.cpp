/// @file matrix_operations.cpp
/// @brief Implementation of functions for matrix operations
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "SParameterCalculator.h"

vector<vector<Complex>>
SParameterCalculator::invertMatrix(const vector<vector<Complex>> &matrix) {
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
