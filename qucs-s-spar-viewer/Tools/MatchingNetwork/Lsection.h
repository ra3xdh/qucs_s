/***************************************************************************
                                Lsection.h
                                ----------
    copyright            :  QUCS team
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
#ifndef LSECTION_H
#define LSECTION_H
#include "../../Schematic/Network.h"
#include "../../Schematic/SchematicContent.h"
#include "../../Schematic/component.h"
#include "../../Misc/general.h"

class Lsection : public Network {
public:
  Lsection();
  virtual ~Lsection();
  Lsection(MatchingNetworkDesignParameters);
  void synthesize();

private:
  struct MatchingNetworkDesignParameters Specs;
};
#endif // LSECTION_H
