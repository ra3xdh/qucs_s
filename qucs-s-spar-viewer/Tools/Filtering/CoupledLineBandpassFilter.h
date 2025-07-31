/***************************************************************************
                                CoupledLineBandpassFilter.h
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
#ifndef COUPLEDLINEBANDPASSFILTER_H
#define COUPLEDLINEBANDPASSFILTER_H

#include "LowpassPrototypeCoeffs.h"
#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../../Misc/general.h"
#include <QPen>

class CoupledLineBandpassFilter : public Network {
public:
  CoupledLineBandpassFilter();
  virtual ~CoupledLineBandpassFilter();
  CoupledLineBandpassFilter(FilterSpecifications);
  void synthesize();

private:
  struct FilterSpecifications Specification;
};

#endif // COUPLEDLINEBANDPASSFILTER_H
