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

#ifndef NETLISTSCRATCHPAD_H
#define NETLISTSCRATCHPAD_H

#include "../../CustomWidgets/codeeditor.h"
#include "../../Misc/general.h"
#include "../../Schematic/Network.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>

class NetlistScratchPad : public QWidget {
  Q_OBJECT
public:
  NetlistScratchPad(QWidget* parent = nullptr);
  ~NetlistScratchPad();
  QString getText();
  void setText(QString);

private:
  CodeEditor* Netlist_Editor_Widget;
  QLabel* traceNameLabel;
  QLineEdit* traceNameLineEdit;

private slots:
  void update();

signals:
  void updateSimulation(SchematicContent);
};

#endif // NETLISTSCRATCHPAD_h
