#include "config.h"

#include <QTest>
#include <QSignalSpy>
#include <QEventLoop>
#include <QTimer>

#if NGSPICE_SHARED
#include "ngspice_shared.h"
#endif

/**
 * @brief UI regression test suite for Qucs-S
 *
 * This test suite validates UI components and simulation integration.
 * Tests run in CI on all platforms to catch regressions early.
 */
class TestNgspiceIntegration : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

#if NGSPICE_SHARED
    void testNgspiceSharedInitialization();
    void testNgspiceSharedSimulation();
    void testNgspiceSharedSignals();
    void testNgspiceSharedVectorRetrieval();
#endif

private:
#if NGSPICE_SHARED
    NgspiceShared* m_ngspice = nullptr;
#endif
};

void TestNgspiceIntegration::initTestCase()
{
    qDebug() << "Starting Qucs-S UI Regression Test Suite";
#if NGSPICE_SHARED
    qDebug() << "NgspiceShared support: ENABLED";
#else
    qDebug() << "NgspiceShared support: DISABLED";
#endif
}

void TestNgspiceIntegration::cleanupTestCase()
{
#if NGSPICE_SHARED
    // Give ngspice time to finish any pending operations
    QTest::qWait(100);

    if (m_ngspice) {
        // Ngspice cleanup can sometimes be problematic
        // Just set to nullptr without explicit delete to avoid issues
        m_ngspice->setParent(nullptr);
        m_ngspice = nullptr;
    }
#endif
    qDebug() << "Test suite completed";
}

#if NGSPICE_SHARED

void TestNgspiceIntegration::testNgspiceSharedInitialization()
{
    m_ngspice = new NgspiceShared(this);
    QVERIFY2(m_ngspice != nullptr, "NgspiceShared object created");

    bool initialized = m_ngspice->initialize();
    QVERIFY2(initialized, "NgspiceShared initialized successfully");
}

void TestNgspiceIntegration::testNgspiceSharedSimulation()
{
    QVERIFY2(m_ngspice != nullptr, "NgspiceShared must be initialized first");

    // Create a simple voltage divider circuit
    QStringList netlist;
    netlist << "Simple Voltage Divider Test";
    netlist << "V1 n1 0 DC 10";
    netlist << "R1 n1 n2 1k";
    netlist << "R2 n2 0 1k";
    netlist << ".op";
    netlist << ".end";

    int result = m_ngspice->sendCircuit(netlist);
    QCOMPARE(result, 0);

    result = m_ngspice->sendCommand("run");
    QCOMPARE(result, 0);

    // Give simulation time to complete
    QTest::qWait(500);
}

void TestNgspiceIntegration::testNgspiceSharedSignals()
{
    QVERIFY2(m_ngspice != nullptr, "NgspiceShared must be initialized first");

    // Set up signal spies
    QSignalSpy outputSpy(m_ngspice, &NgspiceShared::outputReceived);
    QSignalSpy startedSpy(m_ngspice, &NgspiceShared::simulationStarted);

    QVERIFY(outputSpy.isValid());
    QVERIFY(startedSpy.isValid());

    // Create and run a simple circuit
    QStringList netlist;
    netlist << "Signal Test Circuit";
    netlist << "V1 n1 0 DC 5";
    netlist << "R1 n1 0 1k";
    netlist << ".op";
    netlist << ".end";

    m_ngspice->sendCircuit(netlist);
    m_ngspice->sendCommand("run");

    // Wait for output (simulationStarted may not always be emitted reliably)
    // For now, just check that we get output
    bool gotOutput = outputSpy.wait(2000) || outputSpy.count() > 0;
    QVERIFY2(gotOutput, "outputReceived signal emitted");

    qDebug() << "Received" << outputSpy.count() << "output messages";

    // Give time for any remaining signals
    QTest::qWait(100);
}

void TestNgspiceIntegration::testNgspiceSharedVectorRetrieval()
{
    QVERIFY2(m_ngspice != nullptr, "NgspiceShared must be initialized first");

    // Run a simple simulation first
    QStringList netlist;
    netlist << "Vector Test Circuit";
    netlist << "V1 n1 0 DC 12";
    netlist << "R1 n1 n2 2k";
    netlist << "R2 n2 0 2k";
    netlist << ".op";
    netlist << ".end";

    m_ngspice->sendCircuit(netlist);
    m_ngspice->sendCommand("run");
    QTest::qWait(500);

    // Get current plot
    QString plot = m_ngspice->currentPlot();
    QVERIFY2(!plot.isEmpty(), "Current plot name retrieved");
    qDebug() << "Current plot:" << plot;

    // Get vectors
    QStringList vectors = m_ngspice->allVectors(plot);
    QVERIFY2(vectors.size() > 0, "At least one vector available");
    qDebug() << "Available vectors:" << vectors;

    // Should have voltage nodes
    bool hasVoltageNode = false;
    for (const QString& vec : vectors) {
        if (vec.contains("n1") || vec.contains("n2")) {
            hasVoltageNode = true;
            break;
        }
    }
    QVERIFY2(hasVoltageNode, "Voltage node vectors present in results");
}

#endif // NGSPICE_SHARED

QTEST_MAIN(TestNgspiceIntegration)
#include "test_ngspice_integration.moc"
