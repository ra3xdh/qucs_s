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

#include "netlistscratchpad.h"

NetlistScratchPad::NetlistScratchPad(QWidget* parent) : QWidget(parent) {
  QGridLayout* WidgetLayout = new QGridLayout();

  Netlist_Editor_Widget = new CodeEditor();
  Netlist_Editor_Widget->insertPlainText(
      QString("P1 1 50.0Ohm\nC1 0 1 3.76pF\nL1 2 1 14.5nH\nC2 0 2 3.76pF\nP2 2 "
              "50.0Ohm\n"));
  WidgetLayout->addWidget(Netlist_Editor_Widget, 0, 0, 1, 2);

  traceNameLabel    = new QLabel("Trace name");
  traceNameLineEdit = new QLineEdit("Netlist1");
  WidgetLayout->addWidget(traceNameLabel, 1, 0);
  WidgetLayout->addWidget(traceNameLineEdit, 1, 1);

  this->setLayout(WidgetLayout);

  // Connect the text editor so that any changes in there rereuns the simulation
  connect(Netlist_Editor_Widget, SIGNAL(textChanged()), this, SLOT(update()));
}

NetlistScratchPad::~NetlistScratchPad() {}

void NetlistScratchPad::update() {
  QString netlist = Netlist_Editor_Widget->getText();
  SchematicContent SC;

  SC.setNetlist(netlist);
  SC.Name = traceNameLineEdit->text();

  emit updateSimulation(SC);
}

QString NetlistScratchPad::getText() {
  return Netlist_Editor_Widget->getText();
}

void NetlistScratchPad::setText(QString netlist) {
  Netlist_Editor_Widget->clear();
  Netlist_Editor_Widget->insertPlainText(netlist);
}
