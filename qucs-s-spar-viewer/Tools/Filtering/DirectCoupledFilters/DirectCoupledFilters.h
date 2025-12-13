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

/**
 * @file DirectCoupledFilters.h
 * @brief Synthesis of different types of Direct Coupled filters
 * @author Andrés Martínez Mera
 * @date 2025
 */

#ifndef DIRECTCOUPLEDFILTERS_H
#define DIRECTCOUPLEDFILTERS_H


#include "./../../../Schematic/Network.h"
#include "./../../../Schematic/component.h"
#include "./../LowpassPrototypeCoeffs.h"

/**
 * @class DirectCoupledFilters
 * @brief Synthesis of different topologies of Direct-Coupled filters
 *
 * This class implements the design equations for the design of the following Direct-Coupled circuit topologies:
 * - Capacitatively-coupled shunt resonators
 * - Inductively-coupled shunt resonators
 *
 * The resonators' values may be calculated by fixing the inductance or the capacitance
 */
class DirectCoupledFilters : public Network {
public:
  DirectCoupledFilters();
  virtual ~DirectCoupledFilters();
  DirectCoupledFilters(FilterSpecifications);
  void synthesize();

private:
  struct FilterSpecifications Specification; /// Filter specifications (band, response type, ripple, etc.)
  std::deque<double> gi; /// Lowpass prototype. Initialized when the synthesize function is called.

  ///
  /// \brief Synthesis of capacitive-coupled shunt resonators filters
  ///
  void Synthesize_Capacitive_Coupled_Shunt_Resonators();

  ///
  /// \brief Synthesis of inductive-coupled shunt resonators filters
  ///
  void Synthesize_Inductive_Coupled_Shunt_Resonators();

  ///
  /// \brief Synthesis of capacitive-coupled series resonators filters
  ///
  void Synthesize_Capacitive_Coupled_Series_Resonators();

  ///
  /// \brief Synthesis of inductive-coupled series resonators filters
  ///
  void Synthesize_Inductive_Coupled_Series_Resonators();

  ///
  /// \brief Synthesis of (quarter-wavelength transmission line)-coupled shunt resonators filters
  ///
  void Synthesize_QuarterWave_Coupled_Shunt_Resonators();


};

#endif // DIRECTCOUPLED_H
