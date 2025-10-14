#include "config.h"

#include <QTest>
#include <QSignalSpy>
#include <QEventLoop>
#include <QTimer>
#include <cmath>

#if NGSPICE_SHARED
#include "ngspice_shared.h"
#endif

/**
 * @brief Test suite for different SPICE simulation types
 *
 * This test suite validates various simulation types supported by Ngspice:
 * - DC Operating Point Analysis (.op)
 * - AC Frequency Analysis (.ac)
 * - Transient Time-Domain Analysis (.tran)
 *
 * Tests verify both simulation execution and data retrieval for each type.
 */
class TestSimulationTypes : public QObject
{
  Q_OBJECT

private slots:
  void initTestCase();
  void cleanupTestCase();

#if NGSPICE_SHARED
  void testDCOperatingPoint();
  void testACFrequencyAnalysis();
  void testTransientAnalysis();
  void testComplexCircuitAC();
  void testTransientRCResponse();
#endif

private:
#if NGSPICE_SHARED
  NgspiceShared* m_ngspice = nullptr;

  // Helper function to wait for simulation completion
  bool waitForSimulation(int timeoutMs = 2000);
#endif
};

void TestSimulationTypes::initTestCase()
{
  qDebug() << "Starting Simulation Types Test Suite";
#if NGSPICE_SHARED
  qDebug() << "NgspiceShared support: ENABLED";
  qDebug() << "Testing DC, AC, and Transient simulations";

  // Initialize NgspiceShared instance for all tests
  m_ngspice = new NgspiceShared(this);
  bool initialized = m_ngspice->initialize();
  QVERIFY2(initialized, "NgspiceShared initialized successfully");
#else
  qDebug() << "NgspiceShared support: DISABLED - tests will be skipped";
#endif
}

void TestSimulationTypes::cleanupTestCase()
{
#if NGSPICE_SHARED
  QTest::qWait(100);
  if (m_ngspice) {
    m_ngspice->setParent(nullptr);
    m_ngspice = nullptr;
  }
#endif
  qDebug() << "Simulation Types Test Suite completed";
}

#if NGSPICE_SHARED

bool TestSimulationTypes::waitForSimulation(int timeoutMs)
{
  QEventLoop loop;
  QTimer timer;
  timer.setSingleShot(true);

  // Wait for output to be received (indicates simulation is running/complete)
  QObject::connect(m_ngspice, &NgspiceShared::outputReceived, &loop, &QEventLoop::quit);
  QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

  timer.start(timeoutMs);
  loop.exec();

  // Give a bit more time for simulation to fully complete
  QTest::qWait(200);

  return timer.isActive(); // true if we quit before timeout
}

void TestSimulationTypes::testDCOperatingPoint()
{
  qDebug() << "\n=== Testing DC Operating Point Analysis ===";

  // Create a voltage divider circuit: V1(10V) -> R1(1k) -> n1 -> R2(1k) -> GND
  // Expected: n1 should be at 5V
  QStringList netlist;
  netlist << "DC Operating Point Test - Voltage Divider";
  netlist << "V1 vin 0 DC 10";
  netlist << "R1 vin n1 1k";
  netlist << "R2 n1 0 1k";
  netlist << ".op";
  netlist << ".end";

  int result = m_ngspice->sendCircuit(netlist);
  QCOMPARE(result, 0);

  result = m_ngspice->sendCommand("run");
  QCOMPARE(result, 0);

  QVERIFY2(waitForSimulation(), "DC simulation completed");

  // Verify plot exists
  QString plot = m_ngspice->currentPlot();
  QVERIFY2(!plot.isEmpty(), "DC analysis plot available");
  qDebug() << "DC plot:" << plot;

  // Get all vectors
  QStringList vectors = m_ngspice->allVectors(plot);
  QVERIFY2(vectors.size() > 0, "DC analysis produced vectors");
  qDebug() << "DC vectors:" << vectors;

  // Verify voltage nodes are present
  bool hasVin = false;
  bool hasN1 = false;
  for (const QString& vec : vectors) {
    if (vec.contains("vin") || vec.contains("v(vin)")) {
      hasVin = true;
    }
    if (vec.contains("n1") || vec.contains("v(n1)")) {
      hasN1 = true;
    }
  }
  QVERIFY2(hasVin, "Input voltage node present");
  QVERIFY2(hasN1, "Divider node present");

  qDebug() << "DC Operating Point test PASSED";
}

void TestSimulationTypes::testACFrequencyAnalysis()
{
  qDebug() << "\n=== Testing AC Frequency Analysis ===";

  // Create an AC circuit with voltage source and resistor
  // AC sweep from 1Hz to 100kHz, 50 points
  QStringList netlist;
  netlist << "AC Frequency Analysis Test";
  netlist << "V1 in 0 DC 0 AC 1";
  netlist << "R1 in out 1k";
  netlist << "R2 out 0 1k";
  netlist << ".ac lin 50 1 100k";
  netlist << ".end";

  int result = m_ngspice->sendCircuit(netlist);
  QCOMPARE(result, 0);

  result = m_ngspice->sendCommand("run");
  QCOMPARE(result, 0);

  QVERIFY2(waitForSimulation(3000), "AC simulation completed");

  // Verify AC plot exists
  QString plot = m_ngspice->currentPlot();
  QVERIFY2(!plot.isEmpty(), "AC analysis plot available");
  qDebug() << "AC plot:" << plot;

  // Get vectors
  QStringList vectors = m_ngspice->allVectors(plot);
  QVERIFY2(vectors.size() > 0, "AC analysis produced vectors");
  qDebug() << "AC vectors:" << vectors;

  // Verify frequency vector exists
  bool hasFrequency = false;
  for (const QString& vec : vectors) {
    if (vec.contains("frequency") || vec.toLower().contains("freq")) {
      hasFrequency = true;
      break;
    }
  }
  QVERIFY2(hasFrequency, "Frequency vector present in AC analysis");

  qDebug() << "AC Frequency Analysis test PASSED";
}

void TestSimulationTypes::testTransientAnalysis()
{
  qDebug() << "\n=== Testing Transient Time-Domain Analysis ===";

  // Create a simple circuit with pulse source
  // Transient simulation from 0 to 1ms
  QStringList netlist;
  netlist << "Transient Analysis Test";
  netlist << "V1 in 0 PULSE(0 5 0 1n 1n 0.5m 1m)";
  netlist << "R1 in out 1k";
  netlist << "C1 out 0 1u";
  netlist << ".tran 10u 1m";
  netlist << ".end";

  int result = m_ngspice->sendCircuit(netlist);
  QCOMPARE(result, 0);

  result = m_ngspice->sendCommand("run");
  QCOMPARE(result, 0);

  QVERIFY2(waitForSimulation(3000), "Transient simulation completed");

  // Verify transient plot exists
  QString plot = m_ngspice->currentPlot();
  QVERIFY2(!plot.isEmpty(), "Transient analysis plot available");
  qDebug() << "Transient plot:" << plot;

  // Get vectors
  QStringList vectors = m_ngspice->allVectors(plot);
  QVERIFY2(vectors.size() > 0, "Transient analysis produced vectors");
  qDebug() << "Transient vectors:" << vectors;

  // Verify time vector exists
  bool hasTime = false;
  for (const QString& vec : vectors) {
    if (vec.contains("time") || vec.toLower() == "t") {
      hasTime = true;
      break;
    }
  }
  QVERIFY2(hasTime, "Time vector present in transient analysis");

  qDebug() << "Transient Time-Domain Analysis test PASSED";
}

void TestSimulationTypes::testComplexCircuitAC()
{
  qDebug() << "\n=== Testing Complex Circuit AC Analysis (RC Filter) ===";

  // RC low-pass filter: input -> R(1k) -> output -> C(1u) -> ground
  // Corner frequency = 1/(2*pi*R*C) = 159 Hz
  QStringList netlist;
  netlist << "RC Low-Pass Filter AC Analysis";
  netlist << "V1 in 0 DC 0 AC 1";
  netlist << "R1 in out 1k";
  netlist << "C1 out 0 1u";
  netlist << ".ac dec 20 1 100k";
  netlist << ".end";

  int result = m_ngspice->sendCircuit(netlist);
  QCOMPARE(result, 0);

  result = m_ngspice->sendCommand("run");
  QCOMPARE(result, 0);

  QVERIFY2(waitForSimulation(3000), "RC filter AC simulation completed");

  QString plot = m_ngspice->currentPlot();
  QVERIFY2(!plot.isEmpty(), "RC filter AC plot available");

  QStringList vectors = m_ngspice->allVectors(plot);
  QVERIFY2(vectors.size() > 0, "RC filter produced vectors");
  qDebug() << "RC filter vectors:" << vectors;

  // Verify we have output voltage vector
  bool hasOutput = false;
  for (const QString& vec : vectors) {
    if (vec.contains("out") || vec.contains("v(out)")) {
      hasOutput = true;
      break;
    }
  }
  QVERIFY2(hasOutput, "Output voltage vector present");

  qDebug() << "Complex Circuit AC Analysis test PASSED";
}

void TestSimulationTypes::testTransientRCResponse()
{
  qDebug() << "\n=== Testing Transient RC Step Response ===";

  // RC circuit with step input: input(step 0->5V) -> R(1k) -> output -> C(1u) -> GND
  // Time constant tau = R*C = 1ms
  // Output should reach ~63% of final value at t=1ms
  QStringList netlist;
  netlist << "RC Step Response - Transient";
  netlist << "V1 in 0 PULSE(0 5 0 1n 1n 10m 20m)";
  netlist << "R1 in out 1k";
  netlist << "C1 out 0 1u";
  netlist << ".tran 50u 5m";
  netlist << ".end";

  int result = m_ngspice->sendCircuit(netlist);
  QCOMPARE(result, 0);

  result = m_ngspice->sendCommand("run");
  QCOMPARE(result, 0);

  QVERIFY2(waitForSimulation(3000), "RC step response simulation completed");

  QString plot = m_ngspice->currentPlot();
  QVERIFY2(!plot.isEmpty(), "RC step response plot available");

  QStringList vectors = m_ngspice->allVectors(plot);
  QVERIFY2(vectors.size() > 0, "RC step response produced vectors");
  qDebug() << "RC step response vectors:" << vectors;

  // Verify time vector and voltage vectors
  bool hasTime = false;
  bool hasOutput = false;
  for (const QString& vec : vectors) {
    QString lower = vec.toLower();
    if (lower.contains("time") || lower == "t") {
      hasTime = true;
    }
    if (lower.contains("out") || lower.contains("v(out)")) {
      hasOutput = true;
    }
  }
  QVERIFY2(hasTime, "Time vector present");
  QVERIFY2(hasOutput, "Output voltage vector present");

  qDebug() << "Transient RC Step Response test PASSED";
}

#endif // NGSPICE_SHARED

QTEST_MAIN(TestSimulationTypes)
#include "test_simulation_types.moc"
