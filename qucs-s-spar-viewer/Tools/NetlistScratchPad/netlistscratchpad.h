/// @file netlistscratchpad.h
/// @brief Widget for editing netlists (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 6, 2026
/// @copyright Copyright (C) 2019-2025 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef NETLISTSCRATCHPAD_H
#define NETLISTSCRATCHPAD_H

#include "../../UI/CustomWidgets/codeeditor.h"
#include "../../Misc/general.h"
#include "../../Schematic/Network.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>

/// @brief Widget for editing netlists
class NetlistScratchPad : public QWidget {
  Q_OBJECT
  public:
    /// Class constructor
    /// @param parent Parent widget
    NetlistScratchPad(QWidget* parent = nullptr);

    /// @brief Class destructor
    ~NetlistScratchPad() {}

    /// Get the current netlist text
    /// @return Netlist text as QString
    QString getText() {
      return Netlist_Editor_Widget->getText();
    }

    /// Set the netlist text
    /// @param netlist New netlist text to display
    void setText(QString netlist) {
      Netlist_Editor_Widget->clear();
      Netlist_Editor_Widget->insertPlainText(netlist);
    }

  private:
    CodeEditor* Netlist_Editor_Widget; ///< Text editor widget
    QLabel* traceNameLabel;            ///< Label with the trace name widget
    QLineEdit* traceNameLineEdit;      ///< Widget for entering the name of the trace which identify the simulation results for the netlist

  private slots:
    /// Update the simulation when netlist changes
    void update();

  signals:
    /// Signal emitted when simulation needs to be updated
    /// @param content Schematic content with updated netlist
    void updateSimulation(SchematicContent);
};

#endif // NETLISTSCRATCHPAD_h
