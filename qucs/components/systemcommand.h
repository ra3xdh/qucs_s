/// @file SystemCommand.h
/// @brief Block for calling a system command after simulation.
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Mar 20, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef SystemCommand_H
#define SystemCommand_H

#include "component.h"

/// @class SystemCommand
/// @brief This block allows to run system command once the simulation ends.
///
/// This block is placed on the schematic like any other component. When the simulation
/// finishes, Qucs-S reads the command stored in the @c cmd property and executes it via
/// the system shell. This makes it easy to trigger post-processing scripts (Python, Bash,
/// etc.) automatically without leaving the Qucs-S workflow.
/// @par Properties
/// - @c cmd     — the shell command (or multi-line script) to execute after simulation.
/// - @c console — if @c yes, the command is run inside a terminal emulator window so the
///                user can see live output.
/// - @c hold    — if @c yes, the terminal window stays open after the command finishes,
///                allowing the user to inspect the output before closing manually.
/// - @c terminal — the terminal emulator.
///                 If empty Qucs-S falls back to its built-in auto-detection logic.
///
/// @note The component is intentionally excluded from netlist generation so that no
///       simulation backend ever sees it.
class SystemCommand : public Component  {

public:
  SystemCommand();
  ~SystemCommand();
  /// @brief Returns a new instance (copy) of this component.
  Component* newOne();

  /// @brief Returns component metadata
  /// @param Name      Receives the component name.
  /// @param BitmapFile Receives the name of the icon bitmap (without extension).
  /// @param getNewOne  If @c true, a new instance is returned.
  /// @return A new @c SystemCommand instance if @p getNewOne is @c true, otherwise @c nullptr.
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
