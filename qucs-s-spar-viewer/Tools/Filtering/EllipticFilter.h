/// @file EllipticFilter.h
/// @brief Elliptic filter synthesis with equiripple passband and stopband (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 4, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef ELLIPTICFILTER_H
#define ELLIPTICFILTER_H

#include "../../Schematic/Network.h"
#include "../../Schematic/component.h"
#include "../TransmissionLineSynthesis/Microstrip.h"

class ComponentInfo;
class WireInfo;

/// @class EllipticFilter
/// @brief Synthesizes elliptic filters
/// @ref Amstutz - "Elliptic Approximation and Elliptic Filter Design" (1978)
class EllipticFilter : public Network {
public:
  /// @brief Default constructor
  EllipticFilter() { virtual_nodes = 0; }

  /// @brief Constructor with filter specifications
  /// @param specs Filter parameters (order, ripple, stopband attenuation, type)
  EllipticFilter(FilterSpecifications FS) {
    Specification = FS;
    Cshunt_LP = new std::vector<double>(FS.order + 1);
    Lseries_LP = new std::vector<double>(FS.order + 1);
    Cseries_LP = new std::vector<double>(FS.order);
    virtual_nodes = 0;
  }

  /// @brief Destructor - cleans up dynamically allocated arrays
  virtual ~EllipticFilter() {
    delete Cshunt_LP;
    delete Lseries_LP;
    delete Cseries_LP;
  }

  /// @brief Performs complete elliptic filter synthesis
  void synthesize();

  /// @brief Enables semi-lumped implementation mode
  /// @param mode true to use transmission line stubs (Type S only)
  void setSemilumpedMode(bool mode) { this->semilumped = mode; }

private:
  FilterSpecifications Specification;   ///< Design parameters

  std::vector<double> *Cseries_LP;     ///< Series capacitor values (resonator tuning)
  std::vector<double> *Lseries_LP;     ///< Series inductor values
  std::vector<double> *Cshunt_LP;      ///< Shunt capacitor values
  double RL;                            ///< Load resistance (Type A/B/C differ from source)
  bool semilumped = false;              ///< Enable transmission line stub replacement
  unsigned int virtual_nodes;           ///< Hidden nodes for schematic clarity

  /// @brief Calculates Type S elliptic filter prototype (equal terminations)
  /// @details Type S has equal source and load impedances
  void EllipticTypeS();

  /// @brief Calculates Type A/B/C elliptic filter prototypes (unequal terminations)
  /// @details Types A/B/C have different source and load impedances
  void EllipticTypesABC();

  /// @brief Computes Jacobi elliptic sine function Sn(u,k)
  /// @param u Argument
  /// @param k Modulus (real)
  /// @return Sn(u,k) value
  double Sn(double, double);

  /// @brief Computes Jacobi elliptic sine function with complex argument
  /// @param u Real parameter
  /// @param z Complex argument
  /// @return Complex Sn value
  std::complex<double> Sn(double, std::complex<double>);

  //***********  Schematic synthesis ********************
  /// @brief Generates complete elliptic filter schematic
  void SynthesizeEllipticFilter();

  /// @brief Inserts one elliptic section (resonator + series elements)
  /// @param posx Current x-position for schematic placement
  /// @param UnconnectedComponents Map of components awaiting connection
  /// @param j Section index
  /// @param flip Reverse component orientation
  /// @param CentralSection true for middle shunt element
  void InsertEllipticSection(int&, QMap<QString, unsigned int>&, int, bool,
                             bool);

  /// @brief Inserts lowpass min-L elliptic section
  void Insert_LowpassMinL_Section(int&, QMap<QString, unsigned int>&,
                                  unsigned int, bool, bool);

  /// @brief Inserts highpass min-C elliptic section
  void Insert_HighpassMinC_Section(int&, QMap<QString, unsigned int>&,
                                   unsigned int, bool, bool);

  /// @brief Inserts lowpass min-C elliptic sectio
  void Insert_LowpassMinC_Section(int&, QMap<QString, unsigned int>&,
                                  unsigned int, bool, bool);

  /// @brief Inserts highpass min-L elliptic section
  void Insert_HighpassMinL_Section(int&, QMap<QString, unsigned int>&,
                                   unsigned int, bool, bool);

  /// @brief Inserts semi-lumped lowpass min-C section
  void Insert_LowpassSemilumpedMinC_Section(int&, QMap<QString, unsigned int>&,
                                            unsigned int, bool, bool);

  /// @brief Inserts semi-lumped highpass min-L section
  void Insert_HighpassSemilumpedMinL_Section(int&, QMap<QString, unsigned int>&,
                                             unsigned int, bool, bool);

  /// @brief Inserts bandpass elliptic section
  void Insert_Bandpass_1_Section(int&, QMap<QString, unsigned int>&,
                                 unsigned int, bool, bool);

  /// @brief Inserts bandpass elliptic section
  void Insert_Bandpass_2_Section(int&, QMap<QString, unsigned int>&,
                                 unsigned int, bool, bool);

  /// @brief Inserts bandstop elliptic section
  void Insert_Bandstop_1_Section(int&, QMap<QString, unsigned int>&,
                                 unsigned int, bool, bool);

  /// @brief Inserts bandstop elliptic section
  void Insert_Bandstop_2_Section(int&, QMap<QString, unsigned int>&,
                                 unsigned int, bool, bool);
};
#endif
