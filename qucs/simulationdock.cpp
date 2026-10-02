/// @file simulationdock.cpp
/// @brief Dock window hosting the simulation console (implementation)
/// @date September 28, 2026

#include "simulationdock.h"

#include <QVBoxLayout>

SimulationDock::SimulationDock(QWidget* parent)
    : QDockWidget(tr("Simulation Console"), parent) {
  setAllowedAreas(Qt::BottomDockWidgetArea);

  // Persistent container: sessions are replaced, the dock widget is not
  auto* container = new QWidget(this);
  a_layout        = new QVBoxLayout(container);
  a_layout->setContentsMargins(0, 0, 0, 0);
  setWidget(container);
}

void SimulationDock::setSession(QWidget* session) {
  if (session == a_session) {
    return;
  }
  if (a_session) {
    a_layout->removeWidget(a_session);
    a_session->hide();
  }
  a_session = session;
  if (session) {
    a_layout->addWidget(session);
    session->show();
  }
}
