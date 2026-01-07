/// @file netlistscratchpad.cpp
/// @brief Widget for editing netlists (implemenationt)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 6, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "netlistscratchpad.h"

NetlistScratchPad::NetlistScratchPad(QWidget *parent) : QWidget(parent) {
  QGridLayout *WidgetLayout = new QGridLayout();

  Netlist_Editor_Widget = new CodeEditor();
  Netlist_Editor_Widget->insertPlainText(
      QString("P1 1 50.0Ohm\nC1 0 1 3.76pF\nL1 2 1 14.5nH\nC2 0 2 3.76pF\nP2 2 "
              "50.0Ohm\n"));
  WidgetLayout->addWidget(Netlist_Editor_Widget, 0, 0, 1, 2);

  traceNameLabel = new QLabel("Trace name");
  traceNameLineEdit = new QLineEdit("Netlist1");
  WidgetLayout->addWidget(traceNameLabel, 1, 0);
  WidgetLayout->addWidget(traceNameLineEdit, 1, 1);

  this->setLayout(WidgetLayout);

  // Connect the text editor so that any changes in there rereuns the simulation
  connect(Netlist_Editor_Widget, &CodeEditor::textChanged, this,
          &NetlistScratchPad::update);
}

void NetlistScratchPad::update() {
  QString netlist = Netlist_Editor_Widget->getText();
  SchematicContent SC;

  SC.setNetlist(netlist);
  SC.Name = traceNameLineEdit->text();

  emit updateSimulation(SC);
}
