/// @file DirectCoupledFilters.cpp
/// @brief Synthesis of different types of direct-coupled resonator filters
/// (implementation)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 4, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "DirectCoupledFilters.h"

void DirectCoupledFilters::synthesize() {
  LowpassPrototypeCoeffs LP_coeffs(Specification);
  gi = LP_coeffs.getCoefficients();

  switch (Specification.DC_Coupling) {
  case CapacitiveCoupledShuntResonators:
    Synthesize_Capacitive_Coupled_Shunt_Resonators();
    break;

  case InductiveCoupledSeriesResonators:
    Synthesize_Inductive_Coupled_Series_Resonators();
    break;

  case CapacitiveCoupledSeriesResonators:
    Synthesize_Capacitive_Coupled_Series_Resonators();
    break;

  case InductiveCoupledShuntResonators:
    Synthesize_Inductive_Coupled_Shunt_Resonators();
    break;

  case QWCoupledShuntResonators:
    Synthesize_QuarterWave_Coupled_Shunt_Resonators();
    break;
  }
}
