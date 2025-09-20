#ifndef SPARAMETERCALCULATOR_H
#define SPARAMETERCALCULATOR_H

#include <iostream>
#include <vector>
#include <complex>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QList>
#include <stdexcept>
#include <iomanip>
#include <QTextStream>
#include <QFile>
#include <QRegularExpression>


#include "../Misc/general.h"

using namespace std;
using Complex = complex<double>;

// Component types
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
  FREQUENCY_DEPENDENT_SPAR_BLOCK
};

// Circuit component structure
struct Component_SPAR {
  ComponentType_SPAR type;
  string name;
  vector<int> nodes;
  QMap<QString, double> value;
  QMap<QString, Complex> Zvalue;
  vector<vector<Complex>> Smatrix; // For the S-parameter device
  double frequency; // For frequency-dependent components


  QMap<QString, QList<double>> freqDepData;

  // S-parameter
  int numRFPorts;
  double referenceImpedance;

  Component_SPAR(ComponentType_SPAR t, const string& n, const vector<int>& nds, const vector<vector<Complex>>& S, int rfPorts, double Z0 = 50.0)
      : type(t), name(n), nodes(nds), Smatrix(S), frequency(0.0),  numRFPorts(rfPorts), referenceImpedance(Z0) {}

  Component_SPAR(ComponentType_SPAR t, const string& n, const vector<int>& nds, QMap<QString, QList<double>> freqData, int rfPorts, double Z0 = 50.0)
      : type(t), name(n), nodes(nds), frequency(0.0), freqDepData(freqData), numRFPorts(rfPorts), referenceImpedance(Z0) {}

  Component_SPAR(ComponentType_SPAR t, const string& n, const vector<int>& nds, QMap<QString, double> val);
  Component_SPAR(ComponentType_SPAR t, const string& n, const vector<int>& nds, QMap<QString, Complex> zval); // Constructor for complex impedances
  Component_SPAR(ComponentType_SPAR t, const string& n, const vector<int>& nds, const vector<vector<Complex>>& S)
      : type(t), name(n), nodes(nds), Smatrix(S), frequency(0.0) {} // S-parameter device

  // FREQUENCY_DEPENDENT_IMPEDANCE
  Component_SPAR(ComponentType_SPAR t, const string& n, const vector<int>& nds,
                 QMap<QString, QList<double>> freqData)
      : type(t), name(n), nodes(nds), frequency(0.0), freqDepData(freqData) {}
};

// Network port definition
struct Port {
  int node;
  double impedance; // Characteristic impedance (usually 50 ohms)

  Port(int n, double z = 50.0);
};

class SParameterCalculator {
private:
  vector<Component_SPAR> components;
  vector<Port> ports;
  int numNodes;
  double frequency;
  QString currentNetlist;

         // Matrix operations
  vector<vector<Complex>> createMatrix(int rows, int cols);
  vector<vector<Complex>> invertMatrix(const vector<vector<Complex>>& matrix);
  Complex getImpedance(const Component_SPAR& comp, double freq);
  vector<vector<Complex>> buildAdmittanceMatrix();

         // Coupled line specific methods
  void addCoupledLineToAdmittance(vector<vector<Complex>>& Y, const Component_SPAR& comp);
  vector<vector<Complex>> calculateCoupledLineYMatrix(double Z0e, double Z0o, double length, double freq);

         // Ideal coupler specific methods
  void addIdealCouplerToAdmittance(vector<vector<Complex>>& Y, const Component_SPAR& comp);
  vector<vector<Complex>> calculateIdealCouplerYMatrix(double k, double phase_deg, double Z0);

  // Frequency dependent S-parameter file
  vector<vector<Complex>> interpolateFrequencyDependentSMatrix(const Component_SPAR& comp, double freq);
  vector<vector<Complex>> extractSMatrixAtIndex(const Component_SPAR& comp, int freqIndex);
  void addFrequencyDependentSParamBlockToAdmittance(vector<vector<Complex>>& Y, const Component_SPAR& comp);
  vector<vector<Complex>> parseInlineSMatrix(const QString& matrixStr, int numPorts);
  void addOnePortSParamToAdmittance(vector<vector<Complex>>& Y, const Component_SPAR& comp);
  void addTwoPortSParamToAdmittance(vector<vector<Complex>>& Y, const Component_SPAR& comp);
  void addSParameterDevice(const string& name, const vector<int>& nodes, const vector<vector<Complex>>& Smatrix, int numRFPorts, double Z0);

         // Frequency sweep parameters
  double f_start = 1e6;
  double f_stop = 1e9;
  int n_points = 20;

         // Simulation data
  std::vector<std::vector<std::vector<Complex>>> sweepResults;
  QMap<QString, QList<double>> data;

  double parseScaledValue(const QString& input, QString unit_type = QString(""));

public:
  // Constructor
  SParameterCalculator();

         // Set and parse netlist from QString
  bool setNetlist(const QString& netlist);

         // Get current netlist
  const QString& getNetlist() const { return currentNetlist; }

         // Add component to the circuit (for programmatic building)
  void addComponent(ComponentType_SPAR type, const string& name, const vector<int>& nodes, QMap<QString, double> value);
  void addComponent(ComponentType_SPAR type, const string& name, const vector<int>& nodes, QMap<QString, Complex> Zvalue);
  // FREQUENCY_DEPENDENT_IMPEDANCE
  void addComponent(ComponentType_SPAR type, const string& name, const vector<int>& nodes, QMap<QString, QList<double>> freqDepData);

         // Add port to the circuit (for programmatic building)
  void addPort(int node, double impedance = 50.0);

         // Calculate S-parameters at current frequency
  vector<vector<Complex>> calculateSParameters();

  // SPAR Block
  vector<vector<Complex>> convertS2Y(const vector<vector<Complex>>& S, double Z0);
  void addSParamBlockToAdmittance(vector<vector<Complex>>& Y, const Component_SPAR& comp);
  void addSParameterBlock(const string& name, const vector<int>& nodes, const vector<vector<Complex>>& Smatrix);

         // Print S-parameters in a readable format
  void printSParameters(const vector<vector<Complex>>& S);

         // Export S-parameters to Touchstone format
  void exportTouchstone(const QString& filename, const vector<vector<Complex>>& S);

         // Clear all components and ports
  void clear();

         // Getter methods
  int getNumNodes() const { return numNodes; }
  size_t getNumComponents() const { return components.size(); }
  size_t getNumPorts() const { return ports.size(); }
  double getFrequency() const { return frequency; }
  void setFrequency(double freq) { frequency = freq; }

         // Set frequency sweep parameters
  void setFrequencySweep(double start, double stop, int points);

         // Calculate sweep and store internally
  void calculateSParameterSweep();

         // Print all S-parameters for the stored sweep
  void printSParameterSweep() const;

         // Export frequency sweep to Touchstone format
  void exportSweepTouchstone(const QString& filename) const;
  QMap<QString, QList<double>> getData();

private:
  // Parse netlist content
  bool parseNetlist();
};

#endif // SPARAMETERCALCULATOR_H
