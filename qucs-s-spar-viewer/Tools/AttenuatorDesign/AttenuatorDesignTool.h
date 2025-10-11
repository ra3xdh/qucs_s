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

#ifndef ATTENUATORDESIGNTOOL_H
#define ATTENUATORDESIGNTOOL_H

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QWidget>

#include "../../Schematic//Network.h"
#include "AttenuatorDesigner.h"

class AttenuatorDesignTool : public QWidget {
  Q_OBJECT
public:
  AttenuatorDesignTool(QWidget* parent = nullptr);
  ~AttenuatorDesignTool();
  void design();
  void setPdiss(struct PdissAtt);

private slots:
  void UpdateDesignParameters();
  void UpdatePowerDissipationData();
  void on_TopoCombo_currentIndexChanged(int);

private:
  QLabel *Topology_Label, *Attenuation_Label, *dBLabelAtt, *Zin_Label,
      *Ohm_Zin_Label, *Zout_Label, *Ohm_Zout_Label, *Pin_Label, *Pdiss_R1_Label,
      *Pdiss_R2_Label, *Pdiss_R3_Label, *Pdiss_R4_Label, *freqLabel;
  QDoubleSpinBox *AttenuationSpinBox, *ZinSpinBox, *ZoutSpinBox, *Pin_SpinBox,
      *freqSpinBox;
  QComboBox *Topology_Combo, *Pin_units_Combo, *R1_Pdiss_Units_Combo,
      *R2_Pdiss_Units_Combo, *R3_Pdiss_Units_Combo, *R4_Pdiss_Units_Combo,
      *FreqScaleCombo;
  QLineEdit *Pdiss_R1_Lineedit, *Pdiss_R2_Lineedit, *Pdiss_R3_Lineedit,
      *Pdiss_R4_Lineedit;
  struct PdissAtt
      Pdiss; // Power dissipated by the resistors. It is calculated in the
             // specific design functions an copied here after the synthesis

  double getFreq();
  double getPowerW(double, unsigned int);
  double ConvertPowerFromW(double, unsigned int);

  // Transmission line implementation
  QLabel* TL_Implementation_Label;
  QComboBox* TL_Implementation_Combo;

  // Add trace to simulate
  QLabel* traceNameLabel;
  QLineEdit* traceNameLineEdit;

  QString netlist;
  SchematicContent SchContent;

signals:
  void updateSchematic(SchematicContent);
  void updateSimulation(SchematicContent);
};

#endif // ATTENUATORDESIGNTOOL_H
