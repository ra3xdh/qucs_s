/***************************************************************************
                          ngspice_shared.cpp
                          ------------------
    begin                : Mon Oct 7 2024
    copyright            : (C) 2024
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "ngspice_shared.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if NGSPICE_SHARED

#include <QDebug>
#include <QMutexLocker>
#include <QRegularExpression>

/*!
  \file ngspice_shared.cpp
  \brief Implementation of the NgspiceShared class
*/

/*!
 * \brief NgspiceShared::NgspiceShared Constructor
 * \param parent Parent QObject
 */
NgspiceShared::NgspiceShared(QObject *parent)
    : QObject(parent), a_initialized(false), a_is_running(false) {}

/*!
 * \brief NgspiceShared::~NgspiceShared Destructor
 */
NgspiceShared::~NgspiceShared() {
  if (a_initialized) {
    // Clean up ngspice
    ngSpice_Command("quit");
  }
}

/*!
 * \brief NgspiceShared::initialize Initialize the ngspice shared library
 * \return true if successful, false otherwise
 */
bool NgspiceShared::initialize() {
  if (a_initialized) {
    return true;
  }

  // Register callback functions with ngspice
  int ret = ngSpice_Init(cbSendChar, cbSendStat, cbControlledExit, cbSendData,
                         cbSendInitData, cbBGThreadRunning, this);

  if (ret != 0) {
    qCritical() << "NgspiceShared: Failed to initialize ngspice library. Error code:" << ret;
    return false;
  }

  a_initialized = true;
  qDebug() << "NgspiceShared: Successfully initialized ngspice shared library";
  return true;
}

/*!
 * \brief NgspiceShared::sendCommand Send a command to ngspice
 * \param command Command string to execute
 * \return 0 on success, non-zero on error
 */
int NgspiceShared::sendCommand(const QString &command) {
  if (!a_initialized) {
    qWarning() << "NgspiceShared: Cannot send command, not initialized";
    return -1;
  }

  // Reject background commands - they don't work properly with shared library mode
  // which expects synchronous execution
  QString trimmedCommand = command.trimmed();
  if (trimmedCommand.startsWith("bg_")) {
    qCritical() << "NgspiceShared: Background commands (bg_*) are not supported in shared library mode";
    qCritical() << "NgspiceShared: Use synchronous commands instead. Command rejected:" << command;
    return -1;
  }

  QByteArray cmdBytes = command.toLocal8Bit();
  int ret = ngSpice_Command(cmdBytes.data());

  if (ret != 0) {
    qWarning() << "NgspiceShared: Command failed:" << command << "Error:" << ret;
  }

  return ret;
}

/*!
 * \brief NgspiceShared::sendCircuit Send a circuit netlist to ngspice
 * \param netlist List of netlist lines
 * \return 0 on success, non-zero on error
 */
int NgspiceShared::sendCircuit(const QStringList &netlist) {
  if (!a_initialized) {
    qWarning() << "NgspiceShared: Cannot send circuit, not initialized";
    return -1;
  }

  // Convert QStringList to char** array
  // Need to allocate array with NULL terminator
  int lineCount = netlist.size();
  char **circArray = new char *[lineCount + 1];

  for (int i = 0; i < lineCount; ++i) {
    QByteArray lineBytes = netlist[i].toLocal8Bit();
    circArray[i] = new char[lineBytes.size() + 1];
    strcpy(circArray[i], lineBytes.constData());
  }
  circArray[lineCount] = nullptr; // NULL terminator

  int ret = ngSpice_Circ(circArray);

  // Free allocated memory
  for (int i = 0; i < lineCount; ++i) {
    delete[] circArray[i];
  }
  delete[] circArray;

  if (ret != 0) {
    qWarning() << "NgspiceShared: Failed to load circuit. Error:" << ret;
  }

  return ret;
}

/*!
 * \brief NgspiceShared::currentPlot Get the name of the current plot
 * \return Current plot name
 */
QString NgspiceShared::currentPlot() const {
  if (!a_initialized) {
    return QString();
  }

  char *plot = ngSpice_CurPlot();
  return plot ? QString::fromLatin1(plot) : QString();
}

/*!
 * \brief NgspiceShared::allPlots Get names of all plots
 * \return List of plot names
 */
QStringList NgspiceShared::allPlots() const {
  QStringList plots;

  if (!a_initialized) {
    return plots;
  }

  char **plotArray = ngSpice_AllPlots();
  if (plotArray) {
    for (int i = 0; plotArray[i] != nullptr; ++i) {
      plots.append(QString::fromLatin1(plotArray[i]));
    }
  }

  return plots;
}

/*!
 * \brief NgspiceShared::allVectors Get all vector names in a plot
 * \param plotName Name of the plot
 * \return List of vector names
 */
QStringList NgspiceShared::allVectors(const QString &plotName) const {
  QStringList vectors;

  if (!a_initialized) {
    return vectors;
  }

  QByteArray plotBytes = plotName.toLocal8Bit();
  char **vecArray = ngSpice_AllVecs(plotBytes.data());

  if (vecArray) {
    for (int i = 0; vecArray[i] != nullptr; ++i) {
      vectors.append(QString::fromLatin1(vecArray[i]));
    }
  }

  return vectors;
}

/*!
 * \brief NgspiceShared::getVectorInfo Get information about a specific vector
 * \param vectorName Name of the vector
 * \return Pointer to vector_info structure, or nullptr if not found
 */
pvector_info NgspiceShared::getVectorInfo(const QString &vectorName) const {
  if (!a_initialized) {
    return nullptr;
  }

  QByteArray vecBytes = vectorName.toLocal8Bit();
  return ngGet_Vec_Info(vecBytes.data());
}

/*!
 * \brief NgspiceShared::reset Reset ngspice state
 */
void NgspiceShared::reset() {
  if (a_initialized) {
    sendCommand("reset");
  }
}

// ============================================================================
// Callback function implementations
// ============================================================================

/*!
 * \brief NgspiceShared::cbSendChar Callback for character output from ngspice
 */
int NgspiceShared::cbSendChar(char *output, int ident, void *userdata) {
  Q_UNUSED(ident);

  if (userdata && output) {
    NgspiceShared *self = static_cast<NgspiceShared *>(userdata);
    self->handleSendChar(QString::fromLocal8Bit(output));
  }

  return 0;
}

/*!
 * \brief NgspiceShared::cbSendStat Callback for status updates from ngspice
 */
int NgspiceShared::cbSendStat(char *status, int ident, void *userdata) {
  Q_UNUSED(ident);

  if (userdata && status) {
    NgspiceShared *self = static_cast<NgspiceShared *>(userdata);
    self->handleSendStat(QString::fromLocal8Bit(status));
  }

  return 0;
}

/*!
 * \brief NgspiceShared::cbControlledExit Callback for controlled exit
 */
int NgspiceShared::cbControlledExit(int exitstatus, bool immediate,
                                     bool quitexit, int ident,
                                     void *userdata) {
  Q_UNUSED(ident);

  if (userdata) {
    NgspiceShared *self = static_cast<NgspiceShared *>(userdata);
    self->handleControlledExit(exitstatus, immediate, quitexit);
  }

  return exitstatus;
}

/*!
 * \brief NgspiceShared::cbSendData Callback for simulation data
 */
int NgspiceShared::cbSendData(pvecvaluesall vdata, int numstructs, int ident,
                               void *userdata) {
  if (userdata && vdata) {
    NgspiceShared *self = static_cast<NgspiceShared *>(userdata);
    self->handleSendData(vdata, numstructs);

    // Thread-safe signal emission: queue to main thread
    QMetaObject::invokeMethod(self, "dataReceived", Qt::QueuedConnection,
                              Q_ARG(pvecvaluesall, vdata),
                              Q_ARG(int, numstructs), Q_ARG(int, ident));
  }

  return 0;
}

/*!
 * \brief NgspiceShared::cbSendInitData Callback for initialization data
 */
int NgspiceShared::cbSendInitData(pvecinfoall vinfoall, int ident,
                                   void *userdata) {
  if (userdata && vinfoall) {
    NgspiceShared *self = static_cast<NgspiceShared *>(userdata);
    self->handleSendInitData(vinfoall);

    // Thread-safe signal emission: queue to main thread
    QMetaObject::invokeMethod(self, "initDataReceived", Qt::QueuedConnection,
                              Q_ARG(pvecinfoall, vinfoall), Q_ARG(int, ident));
  }

  return 0;
}

/*!
 * \brief NgspiceShared::cbBGThreadRunning Callback for background thread status
 */
int NgspiceShared::cbBGThreadRunning(bool running, int ident, void *userdata) {
  Q_UNUSED(ident);

  if (userdata) {
    NgspiceShared *self = static_cast<NgspiceShared *>(userdata);
    self->handleBGThreadRunning(running);
  }

  return 0;
}

// ============================================================================
// Member function handlers for callbacks
// ============================================================================

/*!
 * \brief NgspiceShared::handleSendChar Handle character output
 */
void NgspiceShared::handleSendChar(const QString &output) {
  QMutexLocker locker(&a_mutex);
  a_output_buffer.append(output);
  locker.unlock();

  // Thread-safe signal emission: queue to main thread
  QMetaObject::invokeMethod(this, "outputReceived", Qt::QueuedConnection,
                            Q_ARG(QString, output));
}

/*!
 * \brief NgspiceShared::handleSendStat Handle status updates
 */
void NgspiceShared::handleSendStat(const QString &status) {
  // Parse status string for percentage if present
  // Format is typically "status message" or may contain percentage
  int percent = 0;

  // Try to extract percentage from status string
  QRegularExpression percentRegex("(\\d+)%");
  QRegularExpressionMatch match = percentRegex.match(status);
  if (match.hasMatch()) {
    percent = match.captured(1).toInt();
  }

  // Thread-safe signal emission: queue to main thread
  QMetaObject::invokeMethod(this, "statusUpdate", Qt::QueuedConnection,
                            Q_ARG(QString, status), Q_ARG(int, percent));
}

/*!
 * \brief NgspiceShared::handleControlledExit Handle controlled exit
 */
void NgspiceShared::handleControlledExit(int exitstatus, bool immediate,
                                          bool quitexit) {
  Q_UNUSED(immediate);
  Q_UNUSED(quitexit);

  QMutexLocker locker(&a_mutex);
  a_is_running = false;
  locker.unlock();

  // Thread-safe signal emission: queue to main thread
  QMetaObject::invokeMethod(this, "simulationFinished", Qt::QueuedConnection,
                            Q_ARG(int, exitstatus));
}

/*!
 * \brief NgspiceShared::handleSendData Handle simulation data
 */
void NgspiceShared::handleSendData(pvecvaluesall vdata, int numstructs) {
  Q_UNUSED(vdata);
  Q_UNUSED(numstructs);
  // Data is emitted via signal, application can process it
}

/*!
 * \brief NgspiceShared::handleSendInitData Handle initialization data
 */
void NgspiceShared::handleSendInitData(pvecinfoall vinfoall) {
  Q_UNUSED(vinfoall);
  // Data is emitted via signal
  QMutexLocker locker(&a_mutex);
  a_is_running = true;
  locker.unlock();

  // Thread-safe signal emission: queue to main thread
  QMetaObject::invokeMethod(this, "simulationStarted", Qt::QueuedConnection);
}

/*!
 * \brief NgspiceShared::handleBGThreadRunning Handle background thread status
 */
void NgspiceShared::handleBGThreadRunning(bool running) {
  QMutexLocker locker(&a_mutex);
  a_is_running = running;
  locker.unlock();

  // Thread-safe signal emission: queue to main thread
  QMetaObject::invokeMethod(this, "backgroundThreadRunning",
                            Qt::QueuedConnection, Q_ARG(bool, running));

  // When background thread stops, the simulation has completed
  if (!running) {
    QMetaObject::invokeMethod(this, "simulationFinished",
                              Qt::QueuedConnection, Q_ARG(int, 0));
  }
}

#endif // NGSPICE_SHARED
