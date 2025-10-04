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

#ifndef FILTERDESIGNTOOL_H
#define FILTERDESIGNTOOL_H
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QPushButton>
#include <QWidget>

#include "CanonicalFilter.h"
#include "EllipticFilter.h"
#include "DirectCoupledFilters.h"
#include "SteppedImpedanceFilter.h"
#include "QuarterWaveFilters.h"
#include "CapacitivelyCoupledShuntResonatorsFilter.h"
#include "CoupledLineBandpassFilter.h"
#include "EndCoupled.h"
/*#include "Filtering/CoupledLineHarmonicRejectionSIRBandpassFilter.h"*/


#include "../../Schematic/Network.h"

#define LC_LADDER 0
#define LC_DIRECT_COUPLED 1
#define STEPPED_IMPEDANCE 2
#define QUARTERWAVE 3
#define CAPACITIVELY_COUPLED_RESONATORS 4
#define COUPLED_LINES 5
#define END_COUPLED 6
#define SEMILUMPED 7

#define COUPLED_LINE_SIR 9

class FilterDesignTool : public QWidget {
  Q_OBJECT
public:
  FilterDesignTool(QWidget *parent = nullptr);
  ~FilterDesignTool();
  void design();

private slots:
  void UpdateDesignParameters();
  void ResposeComboChanged();
  void ImplementationComboChanged(int);
  void EllipticTypeChanged();
  void ChangeRL_CLC_LCL_mode();

private:
  // ************************** FILTER DESIGN ***************************
  QWidget *SetupFilterDesignGUI();
  QComboBox *FilterClassCombo, *FilterResponseTypeCombo,
      *FilterImplementationCombo, *FC_ScaleCombobox, *BW_ScaleCombobox,
      *SemiLumpedImplementationCombo;
  QComboBox *EllipticType, *DC_CouplingTypeCombo;
  QDoubleSpinBox *FCSpinbox, *BWSpinbox, *RippleSpinbox, *StopbandAttSpinbox,
      *MinimumZ_Spinbox, *MaximumZ_Spinbox, *ImpedanceRatio_Spinbox;
  QSpinBox *OrderSpinBox;

  QLineEdit *SourceImpedanceLineEdit;
  QRadioButton *CLCRadioButton, *LCLRadioButton;
  QLabel *StopbandAttLabel, *StopbandAttdBLabel, *EllipticTypeLabel,
      *RippleLabel, *RippledBLabel, *DC_CouplingLabel, *MaximumZ_Unit_Label,
      *MinimumZ_Unit_Label, *SemiLumpedImplementationLabel,
      *ImpedanceRatio_Label;
  QLabel *RLlabel, *RLlabelOhm, *PhaseErrorLabel, *PhaseErrorLabelDeg,
      *MinimumZLabel, *MaximumZLabel, *FC_Label, *BW_Label; // Zverev mode
  QStringList DefaultFilterResponses;

  // Transmission line implementation
  QLabel *TL_Implementation_Label;
  QComboBox *TL_Implementation_Combo;


  // Filter specs
  struct FilterSpecifications Filter_SP;
  QString netlist;
  SchematicContent SchContent;

  // Substrate
  MS_Substrate MS_Subs;

  QStringList setItemsResponseTypeCombo();
  double getScale(QString); // Gets the scale from the comboboxes

  // Add trace to simulate
  QLabel *traceNameLabel;
  QLineEdit *traceNameLineEdit;

  // Set UI settings depending on filter topology
  void setSettings_LC_Ladder();
  void setSettings_LC_Direct_Coupled();
  void setSettings_Stepped_Z_LPF();
  void setSettings_Quarterwavelength_BPF_BSF();
  void setSettings_EndCoupled_BPF();
  void setSettings_CCoupledShuntResonators_BPF();
  void setSettings_Semilumped();
  void setSettings_SideCoupled_BPF();

  void adjustRelativeBW(double max_rel_bw);

public:
  void synthesize();
  void set_MS_Subs(MS_Substrate MS_Subs);

signals:
  void updateSchematic(SchematicContent);
  void updateSimulation(SchematicContent);
};

#endif // FILTERDESIGNTOOL_H
