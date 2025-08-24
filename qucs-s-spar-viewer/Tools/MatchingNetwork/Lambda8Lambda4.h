/***************************************************************************
Lambda8Lambda4.h
----------------
author : 2025: Andrés Martínez Mera
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
