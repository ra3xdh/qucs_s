/***************************************************************************
                                CanonicalFilter.h
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
#ifndef CANONICALFILTER_H
#define CANONICALFILTER_H

#include "LowpassPrototypeCoeffs.h"
#include "../../Schematic/Network.h"
#include "../../Schematic/SchematicContent.h"
#include "../../Schematic/component.h"
#include "../../Misc/general.h"
#include <QPen>
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
