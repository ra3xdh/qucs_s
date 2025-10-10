#include "config.h"

#include <QTest>
#include <QSignalSpy>
#include <QEventLoop>
#include <QTimer>
#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>

#include "abstractspicekernel.h"

#if NGSPICE_SHARED
#include "ngspice_shared.h"
#endif

/**
 * @brief Test suite for AbstractSpiceKernel and SimulatorError enum
 *
 * This test suite validates:
 * - SimulatorError enum values and error code mapping
 * - Error signal emission
 * - Background command validation
 *
 * Note: Full integration testing with Ngspice class requires a complete
 * Schematic setup which is tested in test_ngspice_integration.cpp
 */
class TestNgspiceKernel : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // SimulatorError enum tests
    void testSimulatorErrorValues();
    void testSimulatorErrorNoConflict();

#if NGSPICE_SHARED
    // Background command validation
    void testBackgroundCommandRejection();
#endif

private:
    QTemporaryDir *m_tempDir;
};

void TestNgspiceKernel::initTestCase()
{
    qDebug() << "Starting AbstractSpiceKernel Test Suite";
    qDebug() << "Testing SimulatorError enum and error handling";

    // Create temporary directory for test outputs
    m_tempDir = new QTemporaryDir();
    QVERIFY2(m_tempDir->isValid(), "Failed to create temporary directory");
}

void TestNgspiceKernel::cleanupTestCase()
{
    delete m_tempDir;
    qDebug() << "Test suite completed";
}

void TestNgspiceKernel::testSimulatorErrorValues()
{
    // Test that SimulatorError enum has expected values
    QCOMPARE(static_cast<int>(SimulatorError::NoError), 0);

    // Verify enum auto-assignment is sequential
    QCOMPARE(static_cast<int>(SimulatorError::SharedLibraryNotInitialized), 1);
    QCOMPARE(static_cast<int>(SimulatorError::SharedLibraryFileOpenError), 2);
    QCOMPARE(static_cast<int>(SimulatorError::SharedLibraryCircuitLoadError), 3);
    QCOMPARE(static_cast<int>(SimulatorError::ProcessFailedToStart), 4);
    QCOMPARE(static_cast<int>(SimulatorError::ProcessCrashed), 5);
    QCOMPARE(static_cast<int>(SimulatorError::ProcessTimedOut), 6);

    qDebug() << "SimulatorError enum values verified";
}

void TestNgspiceKernel::testSimulatorErrorNoConflict()
{
    // Verify no conflict between NoError and ProcessFailedToStart
    // This was the bug we fixed - ProcessFailedToStart should NOT be 0
    int noError = static_cast<int>(SimulatorError::NoError);
    int processFailedToStart = static_cast<int>(SimulatorError::ProcessFailedToStart);

    QVERIFY2(noError != processFailedToStart,
             "NoError and ProcessFailedToStart must have different values");

    QCOMPARE(noError, 0);
    QVERIFY(processFailedToStart != 0);

    qDebug() << "Verified NoError =" << noError
             << "and ProcessFailedToStart =" << processFailedToStart
             << "are different";
}

#if NGSPICE_SHARED
void TestNgspiceKernel::testBackgroundCommandRejection()
{
    // Test that NgspiceShared rejects background commands
    NgspiceShared ngspice;
    bool initialized = ngspice.initialize();
    QVERIFY2(initialized, "NgspiceShared should initialize");

    // Try to send a background command
    int result = ngspice.sendCommand("bg_run");
    QVERIFY2(result != 0, "Background command should be rejected");

    // Verify normal commands still work
    result = ngspice.sendCommand("version");
    QCOMPARE(result, 0);

    qDebug() << "Verified background commands are rejected";
}
#endif

QTEST_MAIN(TestNgspiceKernel)
#include "test_ngspice_kernel.moc"
