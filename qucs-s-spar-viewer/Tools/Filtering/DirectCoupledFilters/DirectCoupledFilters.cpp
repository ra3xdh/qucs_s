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

DirectCoupledFilters::~DirectCoupledFilters() {}

void DirectCoupledFilters::synthesize() {
  LowpassPrototypeCoeffs LP_coeffs(Specification);
  gi = LP_coeffs.getCoefficients();
  if (Specification.DC_Coupling == CapacitativeCoupledShuntResonators) {
    Synthesize_Capacitative_Coupled_Shunt_Resonators();
  }
  if (Specification.DC_Coupling == InductiveCoupledSeriesResonators) {
    Synthesize_Inductive_Coupled_Series_Resonators();
  }
}
