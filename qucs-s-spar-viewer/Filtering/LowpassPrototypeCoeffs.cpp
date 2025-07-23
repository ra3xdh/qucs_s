/***************************************************************************
                                LowpassPrototypeCoeffs.cpp
                                ----------
    author                :  2019 Andres Martinez-Mera
    email                  :  andresmmera@protonmail.com
 ***************************************************************************/

/***************************************************************************
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 ***************************************************************************/
#include "LowpassPrototypeCoeffs.h"

LowpassPrototypeCoeffs::LowpassPrototypeCoeffs(FilterSpecifications SP) {
  Specification = SP;
}

std::deque<double> LowpassPrototypeCoeffs::getCoefficients() {
  std::deque<double> gi;

  switch (Specification.FilterResponse) {
  case Butterworth:
    gi = calcButterworth_gi();
    break;
  default:
    gi = calcChebyshev_gi();
    break;
  }

  return gi;
}

// Calculation of the Butterworth lowpass filter prototype
// References:
// [1] Design of Microwave Filters, Impedance-Matching Networks and coupling
// structures.
//     Volume I G.L Matthai and L. Young and E.M.T. Jones. U.S. Army electronic
//     research and development laboratory. SRI. Page 97
std::deque<double> LowpassPrototypeCoeffs::calcButterworth_gi() {
  std::deque<double> gi;
  gi.clear();
  gi.push_back(1); // Source
  int N = Specification.order;
  for (int k = 1; k <= N; k++) {
    gi.push_back(2 * sin(M_PI * (2 * k - 1) / (2 * N)));
  }
  gi.push_back(1); // Load

  // Debug
  /*qDebug() << "*************Butterworth coefficients**********";
  for (int i = 0; i <=N+1; i++) qDebug() << "g" << i << ": " << gi[i];
  qDebug() << "***********************************************";*/
  return gi;
}

// Calculation of the Chebyshev lowpass filter prototype
// References:
// [1] Design of Microwave Filters, Impedance-Matching Networks and coupling
// structures.
//     Volume I G.L Matthai and L. Young and E.M.T. Jones. U.S. Army electronic
//     research and development laboratory. SRI. Page 99
std::deque<double> LowpassPrototypeCoeffs::calcChebyshev_gi() {
  std::deque<double> gi;
  gi.clear();
  int N = Specification.order;
  double beta = std::log(1 / tanh(Specification.Ripple / 17.37));
  double gamma = sinh(beta / (2 * N));
  std::vector<double> ak, bk;
  for (int k = 1; k <= N; k++) {
    ak.push_back(sin((M_PI * (2 * k - 1)) / (2 * N)));
    bk.push_back(gamma * gamma + sin(k * M_PI / N) * sin(k * M_PI / N));
  }

  gi.push_back(1); // Source
  gi.push_back(2 * ak[0] / gamma);
  for (int k = 2; k <= N; k++) {
    gi.push_back((4 * ak[k - 2] * ak[k - 1]) / (bk[k - 2] * gi[k - 1]));
  }

  // Load
  if (N % 2 == 0) // Even
  {
    gi.push_back(1. / (tanh(beta / 4) * tanh(beta / 4)));
  } else // Odd
  {
    gi.push_back(1);
  }

  for (unsigned int i = 0; i < gi.size(); i++) {
    qDebug() << "Lowpass coeff:" << gi.at(i);
  }

  /*//Debug
  qDebug() << "*************Chebyshev coefficients**********";
  for (int i = 0; i <=N+1; i++) qDebug() << "g" << i << ": " << gi[i];
  qDebug() << "***********************************************";*/
  return gi;
}

