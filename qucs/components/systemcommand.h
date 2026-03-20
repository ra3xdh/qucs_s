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
/// It is useful for running postprocessing scripts in the language of choice of the user
class SystemCommand : public Component  {

public:
  SystemCommand();
  ~SystemCommand();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
