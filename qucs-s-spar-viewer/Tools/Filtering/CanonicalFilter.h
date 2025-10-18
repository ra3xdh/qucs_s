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

#ifndef CANONICALFILTER_H
#define CANONICALFILTER_H

#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"
#include "LowpassPrototypeCoeffs.h"

class CanonicalFilter : public Network {
public:
  CanonicalFilter();
  virtual ~CanonicalFilter();
  CanonicalFilter(FilterSpecifications);
  void synthesize();
  void setSemilumpedMode(bool);

private:
  struct FilterSpecifications Specification;
  bool semilumped = false; // Activate semilumped implementation mode

  std::deque<double> gi;

  //***********  Schematic synthesis ********************
  void SynthesizeLPF();           // Lowpass
  void SynthesizeSemilumpedLPF(); // Semilumped Lowpass implementation
  void SynthesizeHPF();           // Highpass
  void SynthesizeBPF();           // Bandpass
  void SynthesizeBSF();           // Bandstop
};

#endif // CANONICALFILTER_H
