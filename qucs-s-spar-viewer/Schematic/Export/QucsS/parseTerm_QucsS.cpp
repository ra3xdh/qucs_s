/// @file parseTerm_QucsS.cpp
/// @brief Converts a termination/port component to Qucs-S format
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 4, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "./../QucsS/QucsSExporter.h"

QString QucsSExporter::parseTerm_QucsS(ComponentInfo Comp) {
  // Format: <Pac ID status x y text_x text_y Num Num_visibility Z0
  // Z0_visibility> ID: e.g. Lx, where x is a positive integer
  // status:
  //         0 -> opened
  //         1 -> active
  //         2 -> shorted
  // x: x-axis position
  // y: y-axis position
  // text_x: x-axis position of the texts showing the properties
  // text_y: y-axis position of the texts showing the properties
  // 0: Unidentified behaviour
  // rotation: Component's rotation {0, 1, 2, 3}
  // Num: Port number
  // Num_Visibility: Visibility of the port number {0: Hide; 1: Show}
  // Z0: Port impedance
  // Z0_visibility: Visibility of the port impedance {0: Hide; 1: Show}

  // Example: <Pac P1 1 210 170 18 -26 0 1 "1" 1 "50 Ohm" 1 "0 dBm" 0 "1
  // MHz" 0 "26.85" 0 "true" 0>

  int status = 1;

  int x_pos = Comp.Coordinates.at(0) * scale_x + x_offset;
  int y_pos =
      Comp.Coordinates.at(1) * scale_y + y_offset +
      50; // The term component needs extra adjusting (+50) because in the
          // synthesis tool the port size is much smaller than in Qucs-S

  int x_text = 25;
  int y_text = 0;
  int Num = Comp.ID.mid(1).toInt(); // Returns all but "T". Port number
  int Num_Visibility = 1;           // Visibility of the port number
  int rotation = 1;                 // Fixed. Always in vertical position
  QString Z0 = Comp.val["Z"];
  int Z0_visibility = 1;

  // Adjust term text's location depending on the number
  switch (Num) {
  case 1:
    x_text = -100;
    y_text = -20;
    break;

  default:
  case 2:
    x_text = 25;
    y_text = -20;
    break;
  }

  // Save pin position. This is needed for wiring later
  // The terms are always placed with vertical orientation
  ComponentPinMap[Comp.ID][0] = QPoint(x_pos, y_pos - 30); // Pin 1
  ComponentPinMap[Comp.ID][1] = QPoint(x_pos, y_pos + 30); // Pin 2

  QString componentLine =
      QString("<Pac %1 %2 %3 %4 %5 %6 0 %7 \"%8\" %9 \"%10\" %11 \"0 "
              "dBm\" 0 \"1 MHz\" 0>\n")
          .arg(Comp.ID)
          .arg(status)
          .arg(x_pos)
          .arg(y_pos)
          .arg(x_text)
          .arg(y_text)
          .arg(rotation)
          .arg(Num)
          .arg(Num_Visibility)
          .arg(Z0)
          .arg(Z0_visibility);

  componentLine += QString("<GND * %1 %2 %3 0 0 0 0>\n")
                       .arg(status)
                       .arg(x_pos)
                       .arg(y_pos + 30);

  return componentLine;
}
