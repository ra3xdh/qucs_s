/***************************************************************************
                               CascadedLCSections.h
                               --------------------
    copyright            :  QUCS team
    author               :  2025: Andrés Martínez Mera
    email                :  andresmmera@protonmail.com
 ***************************************************************************/

/***************************************************************************
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 ***************************************************************************/
#ifndef CASCADEDLCSECTIONS_H
#define CASCADEDLCSECTIONS_H
#include "../../Schematic/Network.h"
#include "../../Schematic/SchematicContent.h"
#include "../../Schematic/component.h"
#include "../../Misc/general.h"

class CascadedLCSections : public Network {
public:
  CascadedLCSections();
  virtual ~CascadedLCSections();
  CascadedLCSections(MatchingNetworkDesignParameters, double);
  void synthesize();

private:
  struct MatchingNetworkDesignParameters Specs;
  int NumberOfSections;
  void CreateLowpassSolution();
  void CreateHighpassSolution();
  double f_match;
};
#endif // CASCADEDLCSECTIONS_H
