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
#include <QWidget>

#include "CanonicalFilter.h"
/*#include "Filtering/CapacitivelyCoupledShuntResonatorsFilter.h"
#include "Filtering/CoupledLineBandpassFilter.h"
#include "Filtering/CoupledLineHarmonicRejectionSIRBandpassFilter.h"
#include "Filtering/DirectCoupledFilters.h"
#include "Filtering/EllipticFilter.h"
#include "Filtering/EndCoupled.h"
#include "Filtering/QuarterWaveFilters.h"
#include "Filtering/SteppedImpedanceFilter.h"*/
#include "../Schematic/Network.h"

#define LC_LADDER 0
#define LC_DIRECT_COUPLED 1
#define QUARTERWAVE 2
#define STEPPED_IMPEDANCE 3
#define END_COUPLED 4
#define CAPACITIVELY_COUPLED_RESONATORS 5
#define SEMILUMPED_ELLIPTIC 6
#define SEMILUMPED_CANONICAL 7
#define COUPLED_LINE 8
#define COUPLED_LINE_SIR 9

class FilterDesignTool : public QWidget {
  Q_OBJECT
public:
  FilterDesignTool(QWidget *parent = nullptr);
  ~FilterDesignTool();
  QString getQucsNetlist();
  SchematicContent getSchematic();
  SP_Analysis
      SPAR_Settings; // The simulation settings vary depending on the design
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
  void synthesize();

signals:
  void updateSchematic(SchematicContent);
};

#endif // FILTERDESIGNTOOL_H
