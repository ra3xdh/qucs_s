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

using namespace std;
using Complex = complex<double>;

// Component types
enum class ComponentType_SPAR {
  RESISTOR,
  CAPACITOR,
  INDUCTOR,
  VOLTAGE_SOURCE,
  CURRENT_SOURCE,
  TRANSMISSION_LINE
};

// Circuit component structure
struct Component_SPAR {
  ComponentType_SPAR type;
  string name;
  vector<int> nodes;
  QMap<QString, double> value;
  double frequency; // For frequency-dependent components

  Component_SPAR(ComponentType_SPAR t, const string& n, const vector<int>& nds, QMap<QString, double> val);
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

         // Frequency sweep parameters
  double f_start = 1e6;
  double f_stop = 1e9;
  int n_points = 20;

  // Simulation data
  std::vector<std::vector<std::vector<Complex>>> sweepResults;
  QMap<QString, QList<double>> data;

  double parseScaledValue(const QString& input);

public:
  // Constructor
  SParameterCalculator();

         // Set and parse netlist from QString
  bool setNetlist(const QString& netlist);

  // Get current netlist
  const QString& getNetlist() const { return currentNetlist; }

         // Add component to the circuit (for programmatic building)
  void addComponent(ComponentType_SPAR type, const string& name,
                    const vector<int>& nodes, QMap<QString, double> value);

         // Add port to the circuit (for programmatic building)
  void addPort(int node, double impedance = 50.0);

         // Calculate S-parameters at current frequency
  vector<vector<Complex>> calculateSParameters();

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
