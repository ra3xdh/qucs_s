/*
 *  Copyright (C) 2019-2025 Andrés Martínez Mera - andresmmera@protonmail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SIMULATIONSETUP_H
#define SIMULATIONSETUP_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include "../..//Misc/general.h"

class SimulationSetup : public QWidget {
  Q_OBJECT
public:
  SimulationSetup(QWidget *parent = nullptr);
  ~SimulationSetup();

  double getFstart();
  double getFstop();
  int getNpoints();

private:
  QDoubleSpinBox *fstartSpinBox, *fstopSpinBox;
  QComboBox *fstartScaleComboBox, *fstopScaleComboBox;
  QSpinBox *npointsSpinBox;

private slots:
  void update();

signals:
  void updateSimulation();
};

#endif // SIMULATIONSETUP_H
