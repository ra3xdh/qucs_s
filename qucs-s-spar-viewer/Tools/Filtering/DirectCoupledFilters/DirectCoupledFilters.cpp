/*
 *  Copyright (C) 2019-2025 Andrés Martínez Mera - andresmmera@protonmail.com
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

#include "DirectCoupledFilters.h"

DirectCoupledFilters::DirectCoupledFilters() {}

DirectCoupledFilters::DirectCoupledFilters(FilterSpecifications FS) {
  Specification = FS;
}

///
/// \brief Class destructor
///
DirectCoupledFilters::~DirectCoupledFilters() {}

///
/// \brief Handles the direct-coupled filter implementation
///
/// \internal 1) First of all, the lowpass prototype coefficients are obtained,
/// depending on the desired response. 2) Then, depending on the topology
/// specified, this function calls the appropiate synthesis function
///
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
