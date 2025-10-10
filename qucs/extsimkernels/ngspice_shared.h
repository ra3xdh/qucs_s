/***************************************************************************
                          ngspice_shared.h
                          ----------------
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

#ifndef NGSPICE_SHARED_H
#define NGSPICE_SHARED_H

#include "config.h"

#if NGSPICE_SHARED

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QMutex>

// Include ngspice shared library header
extern "C" {
#include <ngspice/sharedspice.h>
}

/*!
  \file ngspice_shared.h
  \brief Declaration of the NgspiceShared class
*/

/*!
 * \brief The NgspiceShared class provides a wrapper around the ngspice
 *        shared library interface. It handles initialization, command
 *        execution, and data retrieval from ngspice via the shared library API.
 */
class NgspiceShared : public QObject {
  Q_OBJECT

public:
  explicit NgspiceShared(QObject *parent = nullptr);
  ~NgspiceShared();

  // Initialize ngspice library
  bool initialize();

  // Check if ngspice is initialized and ready
  bool isInitialized() const { return a_initialized; }

  // Send a command to ngspice
  int sendCommand(const QString &command);

  // Send a circuit (netlist) to ngspice
  int sendCircuit(const QStringList &netlist);

  // Get current plot name
  QString currentPlot() const;

  // Get all plot names
  QStringList allPlots() const;

  // Get all vector names in a plot
  QStringList allVectors(const QString &plotName) const;

  // Get vector data
  pvector_info getVectorInfo(const QString &vectorName) const;

  // Check if simulation is running
  bool isRunning() const { return a_is_running; }

  // Reset ngspice state
  void reset();

signals:
  // Emitted when ngspice produces output text
  void outputReceived(const QString &text);

  // Emitted when simulation status changes
  void statusUpdate(const QString &status, int percent);

  // Emitted when simulation finishes
  void simulationFinished(int exitCode);

  // Emitted when simulation starts
  void simulationStarted();

  // Emitted for simulation data (each time step)
  void dataReceived(pvecvaluesall vdata, int numstructs, int ident);

  // Emitted once before simulation with vector info
  void initDataReceived(pvecinfoall vinfoall, int ident);

  // Emitted when background thread status changes
  void backgroundThreadRunning(bool running);

private:
  // Callback functions (static, will call member functions)
  static int cbSendChar(char *output, int ident, void *userdata);
  static int cbSendStat(char *status, int ident, void *userdata);
  static int cbControlledExit(int exitstatus, bool immediate, bool quitexit,
                               int ident, void *userdata);
  static int cbSendData(pvecvaluesall vdata, int numstructs, int ident,
                        void *userdata);
  static int cbSendInitData(pvecinfoall vinfoall, int ident, void *userdata);
  static int cbBGThreadRunning(bool running, int ident, void *userdata);

  // Member function implementations of callbacks
  void handleSendChar(const QString &output);
  void handleSendStat(const QString &status);
  void handleControlledExit(int exitstatus, bool immediate, bool quitexit);
  void handleSendData(pvecvaluesall vdata, int numstructs);
  void handleSendInitData(pvecinfoall vinfoall);
  void handleBGThreadRunning(bool running);

  bool a_initialized;
  bool a_is_running;
  QMutex a_mutex;

  // Accumulated output buffer
  QString a_output_buffer;
};

#endif // NGSPICE_SHARED

#endif // NGSPICE_SHARED_H
