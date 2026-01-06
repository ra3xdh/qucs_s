/// @file SingleStub.h
/// @brief Single stub matching network synthesis (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 6, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef SINGLESTUB_H
#define SINGLESTUB_H

#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"

/// @class DoubleStub
/// @brief Double stub matching network synthesis
/// Reference: 'Microwave Engineering', David Pozar. John Wiley and Sons. 4th
/// Edition. Pg 234-241
class SingleStub : public Network {
  public:
    /// @brief Class constructor
    SingleStub() {}

    /// @brief Class constructor with parameters
    /// @param AS Design specifications
    /// @param freq Matching frequency
    SingleStub(MatchingNetworkDesignParameters AS, double freq) {
      Specs = AS;
      f_match = freq;
    }

    /// @brief Class destructor
    virtual ~SingleStub() {}

    /// @brief Calculate component values and build schematic
    void synthesize();

  private:
    /// @brief Matching network specifications
    struct MatchingNetworkDesignParameters Specs;

    double f_match; ///< Matching frequency

    /// @brief Build schematic with ideal transmission lines
    void buildMatchingNetwork_IdealTL(double d, double lstub);

    /// @brief Build schematic with microstrip transmission lines
    void buildMatchingNetwork_Microstrip(double d, double lstub);

    /// @name Design equations
    /// @{
    /// @brief Normalize phase if it is lower than 0
    /// @param phase Phase [rad]
    /// @return Normalized phase [rad]
    double normalizePhase(double phase);

    /// @brief Calculate normalized susceptance at the stub position
    /// @param t Electrical length of the line section (rad)
    /// @param RL Load resistance (Ohm)
    /// @param XL Load reactance (Ohm)
    /// @param Z0 Characteristic impedance (Ohm)
    /// @return Susceptance
    double calculateSusceptance(double t, double RL, double XL, double Z0);

    /// @brief Calculate electrical length to the stub position
    /// @param RL Load resistance [Ohm]
    /// @param XL Load reactance [Ohm]
    /// @param Z0 Characteristic impedance [Ohm]
    /// @return Electrical length (rad)
    double calculateT(double RL, double XL, double Z0);

    /// @brief Convert electrical length to physical line distance
    /// @param t Electrical length [rad]
    /// @param lambda Wavelength [m]
    /// @return Physical distance [m]
    double calculateLineDistance(double t, double lambda);

    /// @brief Calculate stub length from susceptance
    /// @param B Susceptance
    /// @param Z0 Characteristic impedance [Ohm]
    /// @param lambda Wavelength [m]
    /// @return Stub length [m]
    double calculateStubLength(double B, double Z0, double lambda);
    /// @}
};
#endif // SINGLESTUB_H
