/*
 *  Copyright (C) 2025 Andrés Martínez Mera - andresmmera@protonmail.com
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

#ifndef MATCHINGNETWORKDESIGNTOOL_H
#define MATCHINGNETWORKDESIGNTOOL_H

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QWidget>

#include "../../Schematic/Network.h"
#include "../../Schematic/SchematicContent.h"
#include "../../Schematic/component.h"

#include "LoadSpecificationWidget.h"
#include "MatchingNetworkDesigner.h" // Class to create the matching network
#include "MatchingNetworkParametersWidget.h" // Custom widget for the network setup data entry

class MatchingNetworkDesignTool : public QWidget {
  Q_OBJECT
public:
  MatchingNetworkDesignTool(QWidget* parent = nullptr);
  ~MatchingNetworkDesignTool();
  void design();

private slots:
  void UpdateDesignParameters();
  void AdjustOneTwoPortMatchingWidgetsVisibility();

private:
  QCheckBox* TwoPortCheckBox;

  MatchingNetworkParametersWidget*
      InputMatchingSetupWidget; // Input matching network data entry widget
  MatchingNetworkParametersWidget*
      OutputMatchingSetupWidget; // Output matching network data entry widget
  LoadSpecificationWidget* LoadSpecWidget;

  QLabel *Zout_Label, *Ohm_Zout_Label, *Zout_J;
  QDoubleSpinBox *ZoutISpinBox, *ZoutRSpinBox;

  QLabel* f_match_Label;

  // Two-port network widgets
  QGroupBox* two_port_GroupBox;
  QCheckBox* enter_S2P_file_CheckBox;
  QPushButton* browse_S2P_Button;
  QLabel* s2p_filename_Label;

  QLabel* input_format_Label;
  QComboBox* input_format_Combo;

  QLabel *S11_Label, *S12_Label, *S21_Label, *S22_Label;
  QLabel *S11_Separator_Label, *S12_Separator_Label, *S21_Separator_Label,
      *S22_Separator_Label;

  QDoubleSpinBox *S11_A_SpinBox, *S11_B_SpinBox;
  QDoubleSpinBox *S12_A_SpinBox, *S12_B_SpinBox;
  QDoubleSpinBox *S21_A_SpinBox, *S21_B_SpinBox;
  QDoubleSpinBox *S22_A_SpinBox, *S22_B_SpinBox;

  QDoubleSpinBox *f_match_Spinbox, *FreqEnd_Spinbox;
  QComboBox *f_match_Scale_Combo, *FreqEnd_Scale_Combo,
      *StubTermination_ComboBox;

  double getScaleFreq(int);

  SchematicContent SchContent; // Schematic representation

  // Add trace to simulate
  QLabel* traceNameLabel;
  QLineEdit* traceNameLineEdit;

signals:
  void updateSchematic(SchematicContent);
  void updateSimulation(SchematicContent);
};

#endif // MATCHINGNETWORKDESIGNTOOL_H
