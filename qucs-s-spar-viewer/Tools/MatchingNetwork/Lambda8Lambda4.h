/***************************************************************************
                               Lambda8Lambda4.h
                               ----------------------
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

#ifndef LAMBDA8LAMBDA4_H
#define LAMBDA8LAMBDA4_H

#include "../../Schematic/Network.h"
#include "../../Schematic/SchematicContent.h"
#include "../../Schematic/component.h"
#include "../../Misc/general.h"

class Lambda8Lambda4 : public Network {

public:
    Lambda8Lambda4();
    virtual ~Lambda8Lambda4();

    Lambda8Lambda4(MatchingNetworkDesignParameters);
    void synthesize();

private:
    struct MatchingNetworkDesignParameters Specs;
};

#endif // LAMBDA8LAMBDA4_H
