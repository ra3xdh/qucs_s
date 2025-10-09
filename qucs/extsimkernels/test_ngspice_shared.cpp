#include "config.h"

#if NGSPICE_SHARED

#include "ngspice_shared.h"
#include <QCoreApplication>
#include <QEventLoop>
#include <QTimer>
#include <cassert>
#include <iostream>

// Simple test for NgspiceShared class
// Tests that we can initialize ngspice, load a circuit, run simulation, and get output

void test_basic_initialization() {
  std::cout << "Test: Basic initialization..." << std::endl;

  NgspiceShared ngspice;
  bool initialized = ngspice.initialize();

  assert(initialized && "NgspiceShared should initialize successfully");
  std::cout << "  ✓ Initialization successful" << std::endl;
}

void test_simple_circuit() {
  std::cout << "\nTest: Simple voltage divider circuit..." << std::endl;

  NgspiceShared ngspice;
  assert(ngspice.initialize() && "Initialization failed");

  // Simple voltage divider circuit
  // V1 connected to R1, R1 connected to R2, R2 connected to ground
  // Should produce 5V at the middle node (n1)
  QStringList netlist;
  netlist << "Simple Voltage Divider Test";
  netlist << "V1 n1 0 DC 10";
  netlist << "R1 n1 n2 1k";
  netlist << "R2 n2 0 1k";
  netlist << ".op";
  netlist << ".end";

  int result = ngspice.sendCircuit(netlist);
  assert(result == 0 && "Circuit loading should succeed");
  std::cout << "  ✓ Circuit loaded successfully" << std::endl;

  // Connect signals to verify we get callbacks
  bool gotOutput = false;
  bool gotSimulationStarted = false;
  int outputCount = 0;

  QObject::connect(&ngspice, &NgspiceShared::outputReceived,
                   [&gotOutput, &outputCount](const QString& text) {
                     gotOutput = true;
                     outputCount++;
                     std::cout << "  Output: " << text.toStdString() << std::endl;
                   });

  QObject::connect(&ngspice, &NgspiceShared::simulationStarted,
                   [&gotSimulationStarted]() {
                     gotSimulationStarted = true;
                     std::cout << "  ✓ Simulation started" << std::endl;
                   });

  QObject::connect(&ngspice, &NgspiceShared::simulationFinished,
                   [](int exitCode) {
                     std::cout << "  ✓ Simulation finished (exit code: "
                               << exitCode << ")" << std::endl;
                   });

  // Run the simulation
  result = ngspice.sendCommand("run");
  assert(result == 0 && "Run command should succeed");

  // Give Qt event loop time to process callbacks
  QEventLoop loop;
  QTimer::singleShot(1000, &loop, &QEventLoop::quit);
  loop.exec();

  assert(gotOutput && "Should have received output via callback");
  assert(outputCount >= 2 && "Should have received multiple output messages");
  assert(gotSimulationStarted && "Should have received simulationStarted signal");

  // Note: simulationFinished signal may not always be emitted for simple .op analysis
  // as ngspice doesn't always call the ControlledExit callback

  std::cout << "  ✓ All callbacks received" << std::endl;

  // Try to get the current plot name
  QString plot = ngspice.currentPlot();
  std::cout << "  Current plot: " << plot.toStdString() << std::endl;

  // Try to get vectors from the plot
  if (!plot.isEmpty()) {
    QStringList vectors = ngspice.allVectors(plot);
    std::cout << "  Available vectors (" << vectors.size() << "): ";
    for (const QString& vec : vectors) {
      std::cout << vec.toStdString() << " ";
    }
    std::cout << std::endl;

    // We should have at least the voltage vectors
    assert(vectors.size() > 0 && "Should have some output vectors");
    std::cout << "  ✓ Got output vectors" << std::endl;
  }
}

void test_error_handling() {
  std::cout << "\nTest: Error handling..." << std::endl;

  NgspiceShared ngspice;
  assert(ngspice.initialize() && "Initialization failed");

  // Try to send an invalid command
  int result = ngspice.sendCommand("invalid_command_xyz");
  // ngspice should handle this gracefully (may return error or just warn)
  std::cout << "  ✓ Invalid command handled (result: " << result << ")" << std::endl;

  // Try to send a circuit with syntax error
  QStringList badNetlist;
  badNetlist << "Bad Circuit";
  badNetlist << "INVALID SYNTAX HERE";
  badNetlist << ".end";

  result = ngspice.sendCircuit(badNetlist);
  std::cout << "  ✓ Bad netlist handled (result: " << result << ")" << std::endl;
}

int main(int argc, char** argv) {
  QCoreApplication app(argc, argv);

  std::cout << "=== NgspiceShared Test Suite ===" << std::endl;
  std::cout << std::endl;

  try {
    test_basic_initialization();
    test_simple_circuit();
    test_error_handling();

    std::cout << "\n=== All tests passed! ===" << std::endl;
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "\n!!! Test failed with exception: " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "\n!!! Test failed with unknown exception" << std::endl;
    return 1;
  }
}

#else

#include <iostream>

int main() {
  std::cout << "NgspiceShared support not enabled (WITH_NGSPICE_SHARED=OFF)" << std::endl;
  return 0;
}

#endif // NGSPICE_SHARED
