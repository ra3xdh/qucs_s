/// @file simulationdock.h
/// @brief Dock window hosting the simulation console (definition)
/// @date September 28, 2026

#ifndef SIMULATIONDOCK_H
#define SIMULATIONDOCK_H

#include <QDockWidget>

/// @brief Bottom dock of the main window that will host the simulation
///        console (issue #235).
///
/// The dock is owned by QucsApp and lives for the whole application
/// lifetime. It starts hidden and its visibility is toggled from the View
/// menu.
class SimulationDock : public QDockWidget {
  Q_OBJECT
public:
  explicit SimulationDock(QWidget* parent = nullptr);
};

#endif // SIMULATIONDOCK_H
