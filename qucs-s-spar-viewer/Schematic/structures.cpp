#include "structures.h"

// Function for parsing the component type as text
QString ComponentTypeToString(ComponentType type) {
  static const QMap<ComponentType, QString> names = {
      {Capacitor, "Capacitor"},
      {Inductor, "Inductor"},
      {Term, "Term"},
      {GND, "GND"},
      {ConnectionNodes, "ConnectionNodes"},
      {Resistor, "Resistor"},
      {TransmissionLine, "TransmissionLine"},
      {MicrostripLine, "MicrostripLine"},
      {MicrostripStep, "MicrostripStep"},
      {MicrostripOpen, "MicrostripOpen"},
      {MicrostripVia, "MicrostripVia"},
      {MicrostripCoupledLines, "MicrostripCoupledLines"},
      {OpenStub, "OpenStub"},
      {ShortStub, "ShortStub"},
      {CoupledLines, "CoupledLines"},
      {Coupler, "Coupler"},
      {ComplexImpedance, "ComplexImpedance"},
      {SPAR_Block, "SPAR_Block"}};

  return names.value(type, "Unknown");
}
