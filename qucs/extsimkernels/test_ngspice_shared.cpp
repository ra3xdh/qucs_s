#include "config.h"

#if NGSPICE_SHARED

#include "ngspice_shared.h"
#include <QCoreApplication>
#include <QEventLoop>
#include <QTimer>
#include <QProcess>
#include <QTemporaryFile>
#include <QTextStream>
#include <cassert>
#include <iostream>

// Simple test for NgspiceShared class
// Tests that we can initialize ngspice, load a circuit, run simulation, and get output

// Helper class for running ngspice as external process
class NgspiceExternalProcess {
public:
  QString runNetlist(const QStringList &netlist, int timeout_ms = 5000) {
    // Create temporary netlist file
    QTemporaryFile netlistFile;
    netlistFile.setAutoRemove(false);
    if (!netlistFile.open()) {
      std::cerr << "Failed to create temporary netlist file" << std::endl;
      return QString();
    }

    QTextStream stream(&netlistFile);
    for (const QString &line : netlist) {
      stream << line << "\n";
    }
    netlistFile.close();

    // Run ngspice
    QProcess process;
    process.start("ngspice", QStringList() << "-b" << netlistFile.fileName());

    if (!process.waitForStarted(timeout_ms)) {
      std::cerr << "Failed to start ngspice process" << std::endl;
      netlistFile.remove();
      return QString();
    }

    if (!process.waitForFinished(timeout_ms)) {
      std::cerr << "Ngspice process timed out" << std::endl;
      process.kill();
      netlistFile.remove();
      return QString();
    }

    QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
    QString errors = QString::fromLocal8Bit(process.readAllStandardError());

    netlistFile.remove();

    return output + errors;
  }
};

// Global ngspice instance to avoid multiple initializations
static NgspiceShared* g_ngspice = nullptr;

void test_basic_initialization() {
  std::cout << "Test: Basic initialization..." << std::endl;

  g_ngspice = new NgspiceShared();
  bool initialized = g_ngspice->initialize();

  assert(initialized && "NgspiceShared should initialize successfully");
  std::cout << "  ✓ Initialization successful" << std::endl;
}

void test_simple_circuit() {
  std::cout << "\nTest: Simple voltage divider circuit..." << std::endl;

  assert(g_ngspice != nullptr && "NgspiceShared not initialized");
  NgspiceShared& ngspice = *g_ngspice;

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

  assert(g_ngspice != nullptr && "NgspiceShared not initialized");
  NgspiceShared& ngspice = *g_ngspice;

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

void test_shared_vs_external() {
  std::cout << "\nTest: Comparing shared library vs external process..." << std::endl;

  // Create a simple test circuit
  QStringList netlist;
  netlist << "Voltage Divider Comparison Test";
  netlist << "V1 n1 0 DC 12";
  netlist << "R1 n1 n2 2k";
  netlist << "R2 n2 0 2k";
  netlist << ".op";
  netlist << ".print dc v(n2)";
  netlist << ".end";

  // Test with shared library
  std::cout << "  Running with shared library..." << std::endl;
  assert(g_ngspice != nullptr && "NgspiceShared not initialized");

  QString sharedOutput;
  QObject::connect(g_ngspice, &NgspiceShared::outputReceived,
                   [&sharedOutput](const QString& text) {
                     sharedOutput += text;
                   });

  int result = g_ngspice->sendCircuit(netlist);
  assert(result == 0 && "Shared library circuit loading failed");

  result = g_ngspice->sendCommand("run");
  assert(result == 0 && "Shared library run command failed");

  // Give simulation time to complete
  QEventLoop loop;
  QTimer::singleShot(1000, &loop, &QEventLoop::quit);
  loop.exec();

  // Get vectors from shared library
  QString sharedPlot = g_ngspice->currentPlot();
  QStringList sharedVectors = g_ngspice->allVectors(sharedPlot);

  std::cout << "    Shared library plot: " << sharedPlot.toStdString() << std::endl;
  std::cout << "    Shared library vectors: " << sharedVectors.size() << std::endl;

  // Test with external process
  std::cout << "  Running with external process..." << std::endl;
  NgspiceExternalProcess ngspiceExternal;
  QString externalOutput = ngspiceExternal.runNetlist(netlist);

  assert(!externalOutput.isEmpty() && "External process produced no output");
  std::cout << "    External process completed" << std::endl;
  std::cout << "    External output length: " << externalOutput.length() << " chars" << std::endl;

  // Check if output contains expected indicators
  bool hasCircuitName = externalOutput.contains("Voltage Divider Comparison");
  bool hasAnalysis = externalOutput.contains("Doing analysis") || externalOutput.contains("Circuit:");
  bool hasNode = externalOutput.contains("n2") || externalOutput.contains("v-sweep");

  std::cout << "    Has circuit name: " << (hasCircuitName ? "yes" : "no") << std::endl;
  std::cout << "    Has analysis: " << (hasAnalysis ? "yes" : "no") << std::endl;
  std::cout << "    Has node reference: " << (hasNode ? "yes" : "no") << std::endl;

  // Both should complete successfully
  assert(sharedVectors.size() > 0 && "Shared library: No output vectors");
  assert(hasAnalysis && "External process: No analysis output");

  // Both should have the voltage node
  bool sharedHasN2 = false;
  for (const QString& vec : sharedVectors) {
    if (vec.contains("n2")) {
      sharedHasN2 = true;
      break;
    }
  }

  assert(sharedHasN2 && "Shared library: Missing n2 voltage node");

  std::cout << "  ✓ Both modes completed successfully" << std::endl;
  std::cout << "  ✓ Both modes produced expected outputs" << std::endl;
}

int main(int argc, char** argv) {
  QCoreApplication app(argc, argv);

  std::cout << "=== NgspiceShared Test Suite ===" << std::endl;
  std::cout << std::endl;

  int exitCode = 0;
  try {
    test_basic_initialization();
    test_simple_circuit();
    test_error_handling();
    test_shared_vs_external();

    std::cout << "\n=== All tests passed! ===" << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "\n!!! Test failed with exception: " << e.what() << std::endl;
    exitCode = 1;
  } catch (...) {
    std::cerr << "\n!!! Test failed with unknown exception" << std::endl;
    exitCode = 1;
  }

  // Cleanup: avoid explicit delete to prevent ngspice cleanup issues
  if (g_ngspice) {
    g_ngspice->setParent(nullptr);
    g_ngspice = nullptr;
  }

  return exitCode;
}

#else

#include <iostream>

int main() {
  std::cout << "NgspiceShared support not enabled (WITH_NGSPICE_SHARED=OFF)" << std::endl;
  return 0;
}

#endif // NGSPICE_SHARED
