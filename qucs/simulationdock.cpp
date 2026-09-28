/// @file simulationdock.cpp
/// @brief Dock window hosting the simulation console (implementation)
/// @date September 28, 2026

#include "simulationdock.h"

SimulationDock::SimulationDock(QWidget* parent)
    : QDockWidget(tr("Simulation Console"), parent) {
  setAllowedAreas(Qt::BottomDockWidgetArea);
}
