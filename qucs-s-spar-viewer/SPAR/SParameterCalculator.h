/// @file SParameterCalculator.h
/// @brief S-parameter network analysis calculator
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef SPARAMETERCALCULATOR_H
#define SPARAMETERCALCULATOR_H

#include <QFile>
#include <QList>
#include <QMap>
#include <QRegularExpression>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <algorithm>
#include <cmath>
#include <complex>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility> // std::as_const()

#include "../Misc/general.h"

using namespace std;
using Complex = complex<double>;

/// @enum ComponentType_SPAR
/// @brief Circuit component types supported in S-parameter analysis
enum class ComponentType_SPAR {
  RESISTOR,
  CAPACITOR,
  INDUCTOR,
  VOLTAGE_SOURCE,
  CURRENT_SOURCE,
  TRANSMISSION_LINE,
  OPEN_STUB,
  SHORT_STUB,
  COUPLED_LINE,
  IDEAL_COUPLER,
  COMPLEX_IMPEDANCE,
  SPAR_BLOCK,
  FREQUENCY_DEPENDENT_SPAR_BLOCK,
  MICROSTRIP_LINE,
  MICROSTRIP_STEP,
  MICROSTRIP_OPEN,
  MICROSTRIP_VIA,
  MICROSTRIP_COUPLED_LINES
};

/// @struct Component_SPAR
/// @brief Circuit component structure
/// @details It includes all the parameters and connectivity information
struct Component_SPAR {
  ComponentType_SPAR type;          ///< Component type identifier
  string name;                       ///< Component name/label
  vector<int> nodes;                 ///< Node connectivity list
  double frequency;                  ///< Operating frequency for frequency-dependent components
  QMap<QString, double> value;       ///< Real-valued parameters (R, L, C, etc.)
  QMap<QString, Complex> Zvalue;     ///< Complex impedance values
  vector<vector<Complex>> Smatrix;   ///< S-parameter matrix for network blocks
  QMap<QString, QList<double>> freqDepData; ///< Frequency-dependent data tables
  int numRFPorts;                    ///< Number of RF ports for network blocks
  double referenceImpedance;         ///< Reference impedance (typically 50Ω)

  /// @brief Constructor for S-parameter network block with matrix
  Component_SPAR(ComponentType_SPAR t, const string& n, const vector<int>& nds,
                 const vector<vector<Complex>>& S, int rfPorts,
                 double Z0 = 50.0)
      : type(t), name(n), nodes(nds), frequency(0.0),
        Smatrix(S), numRFPorts(rfPorts), referenceImpedance(Z0) {}

  /// @brief Constructor for frequency-dependent S-parameter block
  Component_SPAR(ComponentType_SPAR t, const string& n, const vector<int>& nds,
                 QMap<QString, QList<double>> freqData, int rfPorts,
                 double Z0 = 50.0)
      : type(t), name(n), nodes(nds), frequency(0.0),
        freqDepData(freqData), numRFPorts(rfPorts), referenceImpedance(Z0) {}

  /// @brief Constructor for lumped components with real parameters
  Component_SPAR(ComponentType_SPAR t, const string& n, const vector<int>& nds,
                 QMap<QString, double> val)
      : type(t), name(n), nodes(nds), frequency(0.0), value(val) {}

  /// @brief Constructor for complex impedance components
  Component_SPAR(ComponentType_SPAR t, const string& n, const vector<int>& nds,
                 QMap<QString, Complex> zval)
      : type(t), name(n), nodes(nds), frequency(0.0), Zvalue(zval) {}

  /// @brief Constructor for S-parameter device without port count
  Component_SPAR(ComponentType_SPAR t, const string& n, const vector<int>& nds,
                 const vector<vector<Complex>>& S)
      : type(t), name(n), nodes(nds), frequency(0.0), Smatrix(S) {}

  /// @brief Constructor for frequency-dependent impedance
  Component_SPAR(ComponentType_SPAR t, const string& n, const vector<int>& nds,
                 QMap<QString, QList<double>> freqData)
      : type(t), name(n), nodes(nds), frequency(0.0), freqDepData(freqData) {}
};

/// @struct Port
/// @brief Network port definition with node and impedance
struct Port {
  int node;              ///< Node number where port is connected
  double impedance;      ///< Port characteristic impedance (typically 50Ω)

  /// @brief Constructor with default 50Ω impedance
  /// @param n Node number
  /// @param z Port impedance (default 50Ω)
  Port(int n, double z = 50.0) : node(n), impedance(z) {}
};

/// @class SParameterCalculator
/// @brief Calculates S-parameters using nodal analysis
///
/// This class performs frequency-domain analysis of linear RF circuits including:
/// - Lumped elements (R, L, C)
/// - Transmission lines (ideal and microstrip)
/// - Coupled lines and directional couplers
/// - S-parameter blocks from Touchstone files
/// - Microstrip discontinuities (steps, opens, vias)
class SParameterCalculator {
private:
  vector<Component_SPAR> components;  ///< Circuit component list
  vector<Port> ports;                 ///< Port definitions
  int numNodes;                       ///< Total number of circuit nodes
  double frequency;                   ///< Current analysis frequency
  QString currentNetlist;             ///< Stored netlist string

  // Matrix operations

  /// @brief Creates a complex matrix initialized to zero
  /// @param rows Number of rows
  /// @param cols Number of columns
  /// @return Complex matrix with size (rows x cols)
  vector<vector<Complex>> createMatrix(int rows, int cols) {
    return vector<vector<Complex>>(rows, vector<Complex>(cols, Complex(0, 0)));
  }

  /// @brief Inverts a complex square matrix using Gaussian elimination
  /// @param matrix Input square matrix to be inverted
  /// @return Inverse matrix (matrix^-1)
  /// @details Uses LU decomposition with row pivoting for numerical stability.
  ///          Required for solving the augmented nodal equations in S-parameter extraction.
  vector<vector<Complex>> invertMatrix(const vector<vector<Complex>>& matrix);

  /// @brief Calculates frequency-dependent impedance for a component
  /// @param comp Component_SPAR object, which indicates the component type and contains its parameters
  /// @param freq Frequency at which the impedance must be calculated
  /// @return Complex value with the impedance
  Complex getImpedance(const Component_SPAR& comp, double freq);

  /// @brief Constructs nodal admittance matrix for the circuit
  /// @return Admittance matrix of the network
  vector<vector<Complex>> buildAdmittanceMatrix();

  /// @brief Adds coupled transmission line to admittance matrix
  /// @param Y Reference to circuit admittance matrix
  /// @param comp Component containing coupled line parameters (Z0e, Z0o, length)
  void addCoupledLineToAdmittance(vector<vector<Complex>>& Y,
                                  const Component_SPAR& comp);

  /// @brief Calculates Y-matrix for coupled transmission lines
  /// @param Z0e Even-mode characteristic impedance (Ω)
  /// @param Z0o Odd-mode characteristic impedance (Ω)
  /// @param length Physical length of coupled section (m)
  /// @param freq Operating frequency (Hz)
  /// @return 4×4 complex Y-parameter matrix for coupled line
  vector<vector<Complex>> calculateCoupledLineYMatrix(double Z0e, double Z0o,
                                                      double length,
                                                      double freq);

  /// @brief Adds ideal directional coupler to admittance matrix
  /// @param Y Reference to circuit admittance matrix
  /// @param comp Component containing coupler parameters (k, phase, Z0)
  void addIdealCouplerToAdmittance(vector<vector<Complex>>& Y,
                                   const Component_SPAR& comp);

  /// @brief Calculates Y-matrix for ideal coupler with coupling coefficient and phase
  /// @param k Linear coupling coefficient (0 to 1, where k²=coupled power fraction)
  /// @param phase_deg Phase shift between coupled and through ports (degrees)
  /// @param Z0 Reference impedance for all ports (Ω)
  /// @return 4×4 complex Y-parameter matrix for ideal coupler
  vector<vector<Complex>>
  calculateIdealCouplerYMatrix(double k, double phase_deg, double Z0);


  /// @brief Adds ideal transmission line to admittance matrix
  /// @param Y Reference to circuit admittance matrix
  /// @param comp Component containing line parameters (Z0, length)
  void addTransmissionLineToAdmittance(vector<vector<Complex>>& Y,
                                       const Component_SPAR& comp);

  /// @brief Interpolates S-matrix from frequency-dependent data
  /// @param comp Component containing S-parameter data
  /// @param freq Target frequency for interpolation (Hz)
  /// @return Interpolated S-parameter matrix at specified frequency
  /// @note Required for frequency-dependent components
  vector<vector<Complex>>
  interpolateFrequencyDependentSMatrix(const Component_SPAR& comp, double freq);

  /// @brief Extracts S-matrix at specific frequency index
  /// @param comp Component containing frequency-dependent S-parameter data
  /// @param freqIndex Index into frequency array
  /// @return S-parameter matrix at the indexed frequency point
  /// @details Direct lookup without interpolation.
  /// @note Used when analysis frequency exactly matches a tabulated point, or as part of interpolation routine.
  vector<vector<Complex>> extractSMatrixAtIndex(const Component_SPAR& comp,
                                                int freqIndex);

  /// @brief Adds frequency-dependent S-parameter block to admittance matrix
  /// @param Y Reference to circuit admittance matrix
  /// @param comp Component with S-parameter data (multiple frequency points)
  /// @details Interpolates S-parameters at current analysis frequency from
  /// S-parameter data,
  void addFrequencyDependentSParamBlockToAdmittance(vector<vector<Complex>>& Y,
                                                    const Component_SPAR& comp);

  /// @brief Parses inline S-matrix from netlist string format
  /// @param matrixStr String containing S-parameters in format: (re,im) (re,im); ...
  /// @param numPorts Number of ports
  /// @return S-parameter matrix extracted from string
  vector<vector<Complex>> parseInlineSMatrix(const QString& matrixStr,
                                             int numPorts);

  /// @brief Adds one-port S-parameter device to admittance matrix
  /// @param Y Reference to circuit admittance matrix
  /// @param comp Component containing single S11 parameter
  void addOnePortSParamToAdmittance(vector<vector<Complex>>& Y,
                                    const Component_SPAR& comp);

  /// @brief Adds two-port S-parameter device to admittance matrix
  /// @param Y Reference to circuit admittance matrix
  /// @param comp Component containing 2×2 S-parameter matrix
  void addTwoPortSParamToAdmittance(vector<vector<Complex>>& Y,
                                    const Component_SPAR& comp);

  /// @brief Adds S-parameter device component to circuit
  /// @param name Component identifier string
  /// @param nodes Vector of node numbers for connections
  /// @param Smatrix S-parameter matrix (frequency-independent)
  /// @param numRFPorts Number of RF ports (1, 2, 3, or 4)
  /// @param Z0 Reference impedance for S-parameters (typically 50Ω)
  void addSParameterDevice(const string& name, const vector<int>& nodes,
                           const vector<vector<Complex>>& Smatrix,
                           int numRFPorts, double Z0);

  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  // Microstrip line analysis methods

  /// @brief Adds microstrip transmission line to admittance matrix
  /// @param Y Reference to circuit admittance matrix (modified in place)
  /// @param comp Component with microstrip parameters (W, L, substrate properties)
  void addMicrostripLineToAdmittance(vector<vector<Complex>>& Y,
                                     const Component_SPAR& comp);

  /// @brief Calculates propagation parameters for microstrip line
  /// @param W Line width (m)
  /// @param h Substrate height (m)
  /// @param er Relative permittivity (dielectric constant)
  /// @param t Metal thickness (m)
  /// @param tand Loss tangent (dielectric dissipation factor)
  /// @param rho Metal resistivity (Ω·m)
  /// @param frequency Operating frequency (Hz)
  /// @param[out] alpha Attenuation constant (Np/m) - conductor + dielectric loss
  /// @param[out] beta Phase constant (rad/m) - determines wavelength
  /// @param[out] zl Characteristic impedance (Ω) at frequency
  /// @param[out] ereff Effective relative permittivity at frequency
  void calcMicrostripPropagation(double W, double h, double er,
                                 double t, double tand, double rho,
                                 double frequency, double& alpha, double& beta,
                                 double& zl, double& ereff);

  /// @brief Analyzes quasi-static microstrip parameters
  /// @param W Line width (m)
  /// @param h Substrate height (m)
  /// @param t Metal thickness (m)
  /// @param er Relative permittivity
  /// @param Model Analysis model selector ("Hammerstad", "Wheeler", etc.)
  /// @param[out] ZlEff Effective characteristic impedance at DC (Ω)
  /// @param[out] ErEff Effective relative permittivity at DC
  /// @param[out] WEff Effective width accounting for thickness (m)
  void analyseQuasiStatic(double W, double h, double t, double er,
                          const string& Model, double& ZlEff, double& ErEff,
                          double& WEff);

  /// @brief Analyzes frequency dispersion in microstrip
  /// @param W Line width (m)
  /// @param h Substrate height (m)
  /// @param er Substrate relative permittivity
  /// @param ZlEff DC characteristic impedance (Ω)
  /// @param ErEff DC effective permittivity
  /// @param frequency Operating frequency (Hz)
  /// @param Model Dispersion model ("Kirschning", "Getsinger", "Kobayashi")
  /// @param[out] ZlEffFreq Frequency-dependent characteristic impedance (Ω)
  /// @param[out] ErEffFreq Frequency-dependent effective permittivity
  void analyseDispersion(double W, double h, double er, double ZlEff,
                         double ErEff, double frequency, const string& Model,
                         double& ZlEffFreq, double& ErEffFreq);

  /// @brief Analyzes conductor and dielectric losses in microstrip (α = αc + αd)
  /// @param W Line width (m)
  /// @param t Metal thickness (m)
  /// @param er Relative permittivity
  /// @param rho Metal resistivity (Ω·m, e.g., 1.68e-8 for copper)
  /// @param D Surface roughness RMS height (m)
  /// @param tand Loss tangent (tan δ) of dielectric
  /// @param ZlEff1 Low-frequency characteristic impedance (Ω)
  /// @param ZlEff2 High-frequency characteristic impedance (Ω)
  /// @param ErEff Effective relative permittivity
  /// @param frequency Operating frequency (Hz)
  /// @param Model Loss model selector ("Hammerstad", "Wheeler")
  /// @param[out] ac Conductor attenuation constant (Np/m)
  /// @param[out] ad Dielectric attenuation constant (Np/m)
  void analyseLoss(double W, double t, double er, double rho, double D,
                   double tand, double ZlEff1, double ZlEff2, double ErEff,
                   double frequency, const string& Model, double& ac,
                   double& ad);

  // Helper functions for microstrip calculations
  /// @brief Hammerstad and Jensen parameters for microstrip analysis
  /// @param u Normalized width W/h
  /// @param er Substrate relative permittivity
  /// @param[out] a Fitting parameter 'a' for effective permittivity calculation
  /// @param[out] b Fitting parameter 'b' for effective permittivity calculation
  void Hammerstad_ab(double u, double er, double& a, double& b);

  /// @brief Calculates effective permittivity using Hammerstad method
  /// @param u Normalized width W/h
  /// @param er Substrate relative permittivity
  /// @param a Fitting parameter from Hammerstad_ab()
  /// @param b Fitting parameter from Hammerstad_ab()
  /// @param[out] e Effective relative permittivity εr_eff
  void Hammerstad_er(double u, double er, double a, double b, double& e);

  /// @brief Calculates characteristic impedance using Hammerstad method
  /// @param u Normalized width W/h
  /// @param[out] zl Characteristic impedance in vacuum (Ω)
  void Hammerstad_zl(double u, double& zl);

  /// @brief Kirschning-Jansen dispersion model for effective permittivity
  /// @param u Normalized width W/h
  /// @param fn Normalized frequency f·h (GHz·mm)
  /// @param er Substrate relative permittivity
  /// @param ErEff DC effective permittivity
  /// @param[out] ErEffFreq Frequency-dependent effective permittivity
  void Kirschning_er(double u, double fn, double er, double ErEff, double& ErEffFreq);

  /// @brief Kirschning-Jansen dispersion model for characteristic impedance
  /// @param ErEff DC effective permittivity
  /// @param ErEffFreq Frequency-dependent effective permittivity
  /// @param ZlEff DC characteristic impedance (Ω)
  /// @param[out] r17 Intermediate dispersion parameter
  /// @param[out] ZlEffFreq Frequency-dependent characteristic impedance (Ω)
  void Kirschning_zl(double ErEff, double ErEffFreq, double ZlEff, double& r17, double& ZlEffFreq);

  ///////////////////////////////////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  // Microstrip step
  /// @brief Adds microstrip impedance step to admittance matrix
  /// @param Y Reference to circuit admittance matrix (modified in place)
  /// @param comp Component with step parameters (W1, W2, substrate properties)
  void addMicrostripStepToAdmittance(vector<vector<Complex>>& Y,
                                     const Component_SPAR& comp);
  void calcMicrostripStepZ(double W1, double W2, double h, double er, double t,
                           double frequency, const string& SModel,
                           const string& DModel, Complex& z11, Complex& z12,
                           Complex& z21, Complex& z22);
  ///////////////////////////////////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  // Microstrip open
  /// @brief Adds microstrip open-end to admittance matrix
  /// @param Y Reference to circuit admittance matrix (modified in place)
  /// @param comp Component with open-end parameters (W, substrate properties)
  void addMicrostripOpenToAdmittance(vector<vector<Complex>>& Y,
                                     const Component_SPAR& comp);

  /// @brief Calculates admittance of microstrip open-end
  /// @param W Line width (m)
  /// @param h Substrate height (m)
  /// @param er Relative permittivity
  /// @param t Metal thickness (m)
  /// @param frequency Operating frequency (Hz)
  /// @param Model Analysis model selector
  /// @param SModel Quasi-static model
  /// @param DModel Dispersion model
  /// @return Complex admittance Y = jωC_end (S)
  Complex calcMicrostripOpenY(double W, double h, double er, double t,
                              double frequency, const string& Model,
                              const string& SModel, const string& DModel);

  /// @brief Calculates end-effect capacitance for microstrip open
  /// @param W Line width (m)
  /// @param h Substrate height (m)
  /// @param er Relative permittivity
  /// @param t Metal thickness (m)
  /// @param frequency Operating frequency (Hz)
  /// @param Model Analysis model selector
  /// @param SModel Quasi-static model
  /// @param DModel Dispersion model
  /// @return End capacitance C_end (F)
  double calcMicrostripOpenCend(double W, double h, double er, double t,
                                double frequency, const string& Model,
                                const string& SModel, const string& DModel);
  ///////////////////////////////////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Microstrip via
  /// @brief Adds microstrip via to admittance matrix
  /// @param Y Reference to circuit admittance matrix (modified in place)
  /// @param comp Component with via parameters (D, h, substrate properties)
  void addMicrostripViaToAdmittance(vector<vector<Complex>>& Y,
                                    const Component_SPAR& comp);

  /// @brief Calculates impedance of microstrip via
  /// @param D Via diameter (m)
  /// @param h Substrate height (via length, m)
  /// @param t Metal plating thickness (m)
  /// @param rho Metal resistivity (Ω·m)
  /// @param frequency Operating frequency (Hz)
  /// @return Complex via impedance Z = R + jωL (Ω)
  Complex calcMicrostripViaImpedance(double D, double h, double t, double rho,
                                     double frequency);

  /// @brief Calculates DC resistance of microstrip via
  /// @param D Via diameter (m)
  /// @param h Via length/substrate height (m)
  /// @param t Metal plating thickness (m)
  /// @param rho Metal resistivity (Ω·m, typically 1.68e-8 for copper)
  /// @return DC resistance R_dc (Ω)
  double calcMicrostripViaResistance(double D, double h, double t, double rho);
  ///////////////////////////////////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Microstrip coupled lines
  /// @brief Adds microstrip coupled lines to admittance matrix
  /// @param Y Reference to circuit admittance matrix (modified in place)
  /// @param comp Component with coupled line parameters (W, S, L, substrate)
  void addMicrostripCoupledLinesToAdmittance(vector<vector<Complex>>& Y,
                                             const Component_SPAR& comp);

  /// @brief Calculates propagation parameters for microstrip coupled lines
  /// @param W Line width (m)
  /// @param S Spacing between lines (edge-to-edge, m)
  /// @param h Substrate height (m)
  /// @param er Relative permittivity
  /// @param t Metal thickness (m)
  /// @param tand Loss tangent
  /// @param rho Metal resistivity (Ω·m)
  /// @param frequency Operating frequency (Hz)
  /// @param[out] alpha_e Even-mode attenuation (Np/m)
  /// @param[out] beta_e Even-mode phase constant (rad/m)
  /// @param[out] zl_e Even-mode characteristic impedance (Ω)
  /// @param[out] ereff_e Even-mode effective permittivity
  /// @param[out] alpha_o Odd-mode attenuation (Np/m)
  /// @param[out] beta_o Odd-mode phase constant (rad/m)
  /// @param[out] zl_o Odd-mode characteristic impedance (Ω)
  /// @param[out] ereff_o Odd-mode effective permittivity
  void calcMicrostripCoupledPropagation(double W, double S, double h,
                                        double er, double t, double tand,
                                        double rho, double frequency,
                                        double& alpha_e, double& beta_e,
                                        double& zl_e, double& ereff_e,
                                        double& alpha_o, double& beta_o,
                                        double& zl_o, double& ereff_o);

  /// @brief Analyzes quasi-static parameters for coupled microstrip lines
  /// @param W Line width (m)
  /// @param h Substrate height (m)
  /// @param s Spacing between lines (m)
  /// @param t Metal thickness (m)
  /// @param er Relative permittivity
  /// @param Model Analysis model ("Kirschning", "Hammerstad", etc.)
  /// @param[out] Zle Even-mode impedance (Ω)
  /// @param[out] Zlo Odd-mode impedance (Ω)
  /// @param[out] ErEffe Even-mode effective permittivity
  /// @param[out] ErEffo Odd-mode effective permittivity
  void analyseQuasiStaticCoupled(double W, double h, double s, double t,
                                 double er, const string& Model, double& Zle,
                                 double& Zlo, double& ErEffe, double& ErEffo);

  /// @brief Analyzes frequency dispersion for coupled microstrip lines
  /// @param W Line width (m)
  /// @param h Substrate height (m)
  /// @param s Spacing between lines (m)
  /// @param t Metal thickness (m)
  /// @param er Relative permittivity
  /// @param Zle Even-mode DC impedance (Ω)
  /// @param Zlo Odd-mode DC impedance (Ω)
  /// @param ErEffe Even-mode DC effective permittivity
  /// @param ErEffo Odd-mode DC effective permittivity
  /// @param frequency Operating frequency (Hz)
  /// @param DModel Dispersion model ("Kirschning", "Getsinger")
  /// @param[out] ZleFreq Even-mode impedance at frequency (Ω)
  /// @param[out] ZloFreq Odd-mode impedance at frequency (Ω)
  /// @param[out] ErEffeFreq Even-mode effective permittivity at frequency
  /// @param[out] ErEffoFreq Odd-mode effective permittivity at frequency
  void analyseDispersionCoupled(double W, double h, double s, double t,
                                double er, double Zle, double Zlo,
                                double ErEffe, double ErEffo, double frequency,
                                const string& DModel, double& ZleFreq,
                                double& ZloFreq, double& ErEffeFreq,
                                double& ErEffoFreq);

  /// @brief Analyzes losses for coupled microstrip lines (even or odd mode)
  /// @param W Line width (m)
  /// @param t Metal thickness (m)
  /// @param er Relative permittivity
  /// @param rho Metal resistivity (Ω·m)
  /// @param D Surface roughness RMS (m)
  /// @param tand Loss tangent
  /// @param ZlEff1 Low-frequency characteristic impedance (Ω)
  /// @param ZlEff2 High-frequency characteristic impedance (Ω)
  /// @param ErEff Effective permittivity for the mode
  /// @param frequency Operating frequency (Hz)
  /// @param Model Loss model ("Hammerstad", "Wheeler")
  /// @param evenMode true for even mode, false for odd mode
  /// @param[out] ac Conductor attenuation (Np/m)
  /// @param[out] ad Dielectric attenuation (Np/m)
  void analyseLossCoupled(double W, double t, double er, double rho,
                          double D, double tand, double ZlEff1, double ZlEff2,
                          double ErEff, double frequency, const string& Model,
                          bool evenMode, double& ac, double& ad);
  ///////////////////////////////////////////////////////////////////////////////////////////////////////

  // Frequency sweep parameters
  double f_start = 1e6;   ///< Frequency sweep start (Hz)
  double f_stop = 1e9;    ///< Frequency sweep stop (Hz)
  int n_points = 20;      ///< Number of frequency points

  // Simulation data
  std::vector<std::vector<std::vector<Complex>>> sweepResults; ///< Stored S-parameter sweep data
  QMap<QString, QList<double>> data; ///< Formatted sweep results for export

  /// @brief Parses value with SI prefixes and unit conversion
  /// @param input String containing numerical value with optional SI prefix (k, M, G, m, u, n, p)
  /// @param unit_type Optional unit type for special conversions ("Length", "Frequency", etc.)
  /// @return Numerical value in base SI units (meters, Hz, Ohms, etc.)
  double parseScaledValue(const QString& input,
                          QString unit_type = QString(""));

public:
  /// @brief Constructor
  SParameterCalculator() : numNodes(0), frequency(1e9) {}

  /// @brief Sets netlist and parses components
  /// @param netlist Circuit netlist in custom format
  /// @return true if parsing succeeded, false otherwise
  bool setNetlist(const QString &netlist) {
    currentNetlist = netlist;
    return parseNetlist();
  }

  /// @brief Returns current netlist string
  const QString& getNetlist() const { return currentNetlist; }

  /// @brief Adds lumped component with real-valued parameters
  void addComponent(ComponentType_SPAR type, const string& name,
                    const vector<int>& nodes, QMap<QString, double> value);

  /// @brief Adds component with complex impedance
  void addComponent(ComponentType_SPAR type, const string& name,
                    const vector<int>& nodes, QMap<QString, Complex> Zvalue);

  /// @brief Adds frequency-dependent component
  void addComponent(ComponentType_SPAR type, const string& name,
                    const vector<int>& nodes,
                    QMap<QString, QList<double>> freqDepData);

  /// @brief Adds port to circuit
  /// @param node Node number for port connection
  /// @param impedance Port characteristic impedance (default 50Ω)
  void addPort(int node, double impedance = 50.0);

  /// @brief Calculates S-parameters at current frequency
  /// @return S-parameter matrix
  vector<vector<Complex>> calculateSParameters();

  // SPAR Block component
  /// @brief Converts S-parameters to Y-parameters
  vector<vector<Complex>> convertS2Y(const vector<vector<Complex>>& S,
                                     double Z0);

  /// @brief Adds S-parameter block to admittance matrix
  void addSParamBlockToAdmittance(vector<vector<Complex>>& Y,
                                  const Component_SPAR& comp);

  /// @brief Adds S-parameter block component

  void addSParameterBlock(const string& name, const vector<int>& nodes,
                          const vector<vector<Complex>>& Smatrix);

  /// @brief Prints S-parameters in readable format to console
  void printSParameters(const vector<vector<Complex>>& S);

  /// @brief Exports S-parameters to Touchstone file format
  void exportTouchstone(const QString& filename,
                        const vector<vector<Complex>>& S);

  /// @brief Clears all components and ports
  void clear(){
    components.clear();
    ports.clear();
    numNodes = 0;
  }

  // Getter methods
  /// @brief Returns total number of circuit nodes
  int getNumNodes() const { return numNodes; }

  /// @brief Returns number of components in circuit
  size_t getNumComponents() const { return components.size(); }

  /// @brief Returns number of ports defined
  size_t getNumPorts() const { return ports.size(); }

  /// @brief Returns current analysis frequency
  double getFrequency() const { return frequency; }

  /// @brief Returns formatted sweep data
  QMap<QString, QList<double>> getData() { return data; }

  // Setter methods
  /// @brief Sets current analysis frequency
  void setFrequency(double freq) { frequency = freq; }

  /// @brief Configures frequency sweep parameters
  void setFrequencySweep(double start, double stop, int points);

  /// @brief Performs S-parameter calculation over frequency sweep
  void calculateSParameterSweep();

  /// @brief Prints all S-parameters from stored sweep
  void printSParameterSweep() const;

  /// @brief Exports frequency sweep to Touchstone file
  void exportSweepTouchstone(const QString& filename) const;

private:
  /// @brief Parses netlist from currentNetlist string line by line and populates
  /// the circuit
  /// @return true if parsing succeeded, false on error
  bool parseNetlist();
};

#endif // SPARAMETERCALCULATOR_H
