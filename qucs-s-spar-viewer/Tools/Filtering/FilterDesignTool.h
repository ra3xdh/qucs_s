/***************************************************************************
                                FilterDesignTool.h
                                ----------

    author                :  2019 Andres Martinez-Mera
    email                  :  andresmmera@protonmail.com
 ***************************************************************************/

/***************************************************************************
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 ***************************************************************************/
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
      *MinimumZLabel, *MaximumZLabel; // Zverev mode
  QStringList DefaultFilterResponses;
  struct FilterSpecifications Filter_SP; // User specifications
  QString netlist;
  SchematicContent SchContent;

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

public:
  void synthesize();

signals:
  void updateSchematic(SchematicContent);
  void updateSimulation(SchematicContent);
};

#endif // FILTERDESIGNTOOL_H
