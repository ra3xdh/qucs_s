/// @file LowpassPrototypeCoeffs.cpp
/// @brief Calculation of the normalized lowpass filter coefficients
/// (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 4, 2026
/// @copyright Copyright (C) 2019-2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

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
  case Bessel:
    gi = calcBessel_gi();
    break;
  case Gaussian:
    gi = calcGaussian_gi();
    break;
  case Legendre:
    gi = calcLegendre_gi();
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

  /* // LP coefficients
    QStringList coeffStrings;
    for (const auto &coeff : gi) {
      coeffStrings << QString::number(coeff);
    }
    qDebug() << "Lowpass coeffs:" << coeffStrings.join(' ');
  */

  return gi;
}

// Bessel LPF prototype coefficients
// References:
// [1] Handbook of Filter Synthesis by Anatol I. Zverev, John Wiley & Sons, 1967
// (pages 323 to 325)
std::deque<double> LowpassPrototypeCoeffs::calcBessel_gi() {
  std::deque<double> gi;
  gi.clear();
  int N = Specification.order;

  // Pre-calculated normalized Bessel filter element values for orders 1-10
  // These values are normalized to provide maximally flat group delay (RL = RS)
  std::vector<std::vector<double>> besselCoeffs = {
      {},                                                 // Order 0 (unused)
      {1.0, 2.0000, 1.0},                                 // Order 1
      {1.0, 0.5775, 2.1478, 1.0},                         // Order 2
      {1.0, 0.3374, 0.9705, 2.2034, 1.0},                 // Order 3
      {1.0, 0.2334, 0.6725, 1.0815, 2.2404, 1.0},         // Order 4
      {1.0, 0.1743, 0.5072, 0.8040, 1.1110, 2.2582, 1.0}, // Order 5
      {1.0, 0.1365, 0.4002, 0.6392, 0.8538, 1.1126, 2.2645, 1.0}, // Order 6
      {1.0, 0.1106, 0.3259, 0.5249, 0.7020, 0.8690, 1.1052, 2.2659,
       1.0}, // Order 7
      {1.0, 0.0919, 0.2719, 0.4409, 0.5936, 0.7303, 0.8695, 1.0956, 2.2656,
       1.0}, // Order 8
      {1.0, 0.0780, 0.2313, 0.3770, 0.5108, 0.6306, 0.7407, 0.8639, 1.0863,
       2.2649, 1.0}, // Order 9
      {1.0, 0.0672, 0.1998, 0.3270, 0.4454, 0.5528, 0.6493, 0.7420, 0.8561,
       1.0781, 2.2641, 1.0} // Order 10
  };

  for (size_t i = 0; i < besselCoeffs[N].size(); i++) {
    gi.push_back(besselCoeffs[N][i]);
  }

  // Debug
  /*qDebug() << "*************Bessel coefficients**********";
  for (int i = 0; i <= N+1; i++) qDebug() << "g" << i << ": " << gi[i];
  qDebug() << "*******************************************";*/

  return gi;
}

// Gaussian LPF prototype coefficients
// References:
// [1] Handbook of Filter Synthesis by Anatol I. Zverev, John Wiley & Sons, 1967
// (pages 332 to 334)
std::deque<double> LowpassPrototypeCoeffs::calcGaussian_gi() {
  std::deque<double> gi;
  gi.clear();
  int N = Specification.order;

  // Pre-calculated normalized Gaussian filter element values for orders 1-10
  // These values are normalized to provide maximally flat group delay (RL = RS)
  std::vector<std::vector<double>> guassianCoeffs = {
      {},                         // Order 0 (unused)
      {1.0, 2.0000, 1.0},         // Order 1 (Not used -> This is Butterworth)
      {1.0, 0.4738, 2.1850, 1.0}, // Order 2
      {1.0, 0.2624, 0.8167, 2.2262, 1.0},                         // Order 3
      {1.0, 0.1772, 0.5302, 0.9321, 2.2450, 1.0},                 // Order 4
      {1.0, 0.1312, 0.3896, 0.6485, 0.9782, 2.2533, 1.0},         // Order 5
      {1.0, 0.1026, 0.3045, 0.5004, 0.7050, 0.9982, 2.2568, 1.0}, // Order 6
      {1.0, 0.0833, 0.2473, 0.4055, 0.5606, 0.7333, 1.0073, 2.2583,
       1.0}, // Order 7
      {1.0, 0.0695, 0.2065, 0.3388, 0.4658, 0.5942, 0.7479, 1.0116, 2.2590,
       1.0}, // Order 8
      {1.0, 0.0591, 0.1761, 0.2892, 0.3973, 0.5025, 0.6134, 0.7556, 1.0137,
       2.2593, 1.0}, // Order 9
      {1.0, 0.0512, 0.1525, 0.2509, 0.3451, 0.4353, 0.5250, 0.6244, 0.7597,
       1.0147, 2.2594, 1.0} // Order 10
  };

  for (size_t i = 0; i < guassianCoeffs[N].size(); i++) {
    gi.push_back(guassianCoeffs[N][i]);
  }

  // Debug
  /*qDebug() << "*************Gaussian coefficients**********";
  for (int i = 0; i <= N+1; i++) qDebug() << "g" << i << ": " << gi[i];
  qDebug() << "*******************************************";*/

  return gi;
}

// Legendre LPF prototype coefficients
// References:
// [1] Handbook of Filter Synthesis by Anatol I. Zverev, John Wiley & Sons, 1967
// (pages 339 to 340)
std::deque<double> LowpassPrototypeCoeffs::calcLegendre_gi() {
  std::deque<double> gi;
  gi.clear();
  int N = Specification.order;

  // Pre-calculated normalized Legendre filter element values for orders 1-10
  // These values are normalized to provide maximally flat group delay (RL = RS)
  std::vector<std::vector<double>> legendreCoeffs = {
      {},                         // Order 0 (unused)
      {1.0, 2.0000, 1.0},         // Order 1 (Not used -> This is Butterworth)
      {1.0, 0.4738, 2.1850, 1.0}, // Order 2 (Not used -> This is Butterworth)
      {1.0, 1.1737, 1.3538, 2.1801, 1.0},                         // Order 3
      {1.0, 1.0826, 1.4769, 1.9584, 1.5645, 1.0},                 // Order 4
      {1.0, 0.9512, 1.4780, 2.0673, 1.5395, 1.9990, 1.0},         // Order 5
      {1.0, 0.9160, 1.4852, 1.9857, 1.7442, 1.9040, 1.5763, 1.0}, // Order 6
      {1.0, 0.8394, 1.4770, 1.9394, 1.7270, 2.1506, 1.5895, 1.8640,
       1.0}, // Order 7
      {1.0, 0.8205, 1.4688, 1.9115, 1.7672, 2.0515, 1.8411, 1.8501, 1.5564,
       1.0}, // Order 8
      {1.0, 0.7695, 1.4555, 1.8674, 1.7755, 2.0662, 1.7816, 2.1585, 1.6134,
       1.7645, 1.0}, // Order 9
      {1.0, 0.7575, 1.4454, 1.8537, 1.7839, 2.0327, 1.8453, 2.0409, 1.8953,
       1.8122, 1.5286, 1.0} // Order 10
  };

  for (size_t i = 0; i < legendreCoeffs[N].size(); i++) {
    gi.push_back(legendreCoeffs[N][i]);
  }

  // Debug
  /*qDebug() << "*************Gaussian coefficients**********";
  for (int i = 0; i <= N+1; i++) qDebug() << "g" << i << ": " << gi[i];
  qDebug() << "*******************************************";*/

  return gi;
}
