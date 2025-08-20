/***************************************************************************
                                MatchingNetworkDesignTool.cpp
                                ----------
    copyright            :  QUCS team
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
#ifndef MATCHINGNETWORKDESIGNTOOL_H
#define MATCHINGNETWORKDESIGNTOOL_H

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QWidget>

#include "../../Schematic/Network.h"
#include "../../Schematic/SchematicContent.h"
#include "../../Schematic/component.h"

#include "Lsection.h"


class MatchingNetworkDesignTool : public QWidget {
  Q_OBJECT
public:
  MatchingNetworkDesignTool(QWidget *parent = nullptr);
  ~MatchingNetworkDesignTool();
  void design();

private slots:
  void UpdateDesignParameters();
  void on_TopoCombo_currentIndexChanged(int);

private:
  QLabel *Topology_Label, *Zin_Label, *Ohm_Zin_Label, *Zout_Label,
      *Ohm_Zout_Label, *FreqStart_Label, *FreqEnd_Label, *Zin_J, *Zout_J;
  QDoubleSpinBox *ZinRSpinBox, *ZoutRSpinBox, *ZinISpinBox, *ZoutISpinBox,
      *FreqStart_Spinbox, *FreqEnd_Spinbox;
  QComboBox *Topology_Combo, *FreqStart_Scale_Combo, *FreqEnd_Scale_Combo;
  QCheckBox *Broadband_Checkbox;
  QRadioButton *Solution1_RB, *Solution2_RB;
  double getScaleFreq(int);

  SchematicContent SchContent; // Schematic representation

  // Add trace to simulate
  QLabel *traceNameLabel;
  QLineEdit *traceNameLineEdit;

signals:
  void updateSchematic(SchematicContent);
  void updateSimulation(SchematicContent);
};

#endif // MATCHINGNETWORKDESIGNTOOL_H
