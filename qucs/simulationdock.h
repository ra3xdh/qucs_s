/// @file simulationdock.h
/// @brief Dock window hosting the simulation console (definition)
/// @date September 28, 2026

#ifndef SIMULATIONDOCK_H
#define SIMULATIONDOCK_H

#include <QDockWidget>
#include <QPointer>

class QVBoxLayout;

/// @brief Bottom dock of the main window that hosts the simulation
///        console (issue #235).
///
/// The dock is owned by QucsApp and lives for the whole application
/// lifetime. It starts hidden and its visibility is toggled from the View
/// menu.
class SimulationDock : public QDockWidget {
  Q_OBJECT
public:
  explicit SimulationDock(QWidget* parent = nullptr);

  /// @brief Shows @p session in the dock instead of the previous one.
  ///
  /// The session is reparented to the dock container. The previous session
  /// is only removed from the layout and hidden: deleting it is up to the
  /// caller.
  void setSession(QWidget* session);

private:
  QVBoxLayout* a_layout;
  QPointer<QWidget> a_session;
};

#endif // SIMULATIONDOCK_H
