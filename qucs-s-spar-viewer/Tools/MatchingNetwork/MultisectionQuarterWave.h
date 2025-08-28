/***************************************************************************
MultisectionQuarterWave.h
-------------------------
author : 2025: Andrés Martínez Mera
***************************************************************************/

#ifndef MULTISECTIONQUARTERWAVE_H
#define MULTISECTIONQUARTERWAVE_H

#include "../../Schematic/Network.h"
#include "../../Schematic/SchematicContent.h"
#include "../../Schematic/component.h"
#include "../../Misc/general.h"
#include <vector>
#include <cmath>

class MultisectionQuarterWave : public Network {

public:
    MultisectionQuarterWave();
    virtual ~MultisectionQuarterWave();

    MultisectionQuarterWave(MatchingNetworkDesignParameters, double);
    void synthesize();

private:
    struct MatchingNetworkDesignParameters Specs;

    // Helper functions
    int BinomialCoeff(int n, int k);
    void designBinomial(std::vector<double> &Zs);
    void designChebyshev(std::vector<double> &Zs);

    double f_match;
};

#endif // MULTISECTIONQUARTERWAVE_H
