/***************************************************************************
                                EndCoupled.h
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
#ifndef ENDCOUPLED_H
#define ENDCOUPLED_H

#include "LowpassPrototypeCoeffs.h"
#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../../Misc/general.h"
#include <QPen>

class EndCoupled : public Network {
public:
  EndCoupled();
  virtual ~EndCoupled();
  EndCoupled(FilterSpecifications);
  void synthesize();

private:
  struct FilterSpecifications Specification;
};

#endif // ENDCOUPLED_H
