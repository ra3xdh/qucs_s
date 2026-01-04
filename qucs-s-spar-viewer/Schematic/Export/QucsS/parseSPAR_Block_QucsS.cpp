/// @file parseSPAR_Block_QucsS.cpp
/// @brief Converts an S-parameter block component to Qucs-S format
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 4, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "./../QucsS/QucsSExporter.h"

QString QucsSExporter::parseSPAR_Block_QucsS(ComponentInfo Comp) {
  // Format: <SPfile ID status x y text_x text_y 0 rotation C "path_file"
  // file_visibility "interpolation" interpolation_visibility> ID: e.g. Rx,
  // where x is a positive integer status:
  //         0 -> opened
  //         1 -> active
  //         2 -> shorted
  // x: x-axis position
  // y: y-axis position
  // 0: Unidentified behaviour
  // rotation: Component's rotation {0, 1, 2, 3}
  // text_x: x-axis position of the texts showing the properties
  // text_y: y-axis position of the texts showing the properties

  // Example: <SPfile X1 1 380 2420 -26 -55 0 0 "test.s2p" 1 "rectangular" 0
  // "linear" 0 "open" 0 "2" 0>

  int status = 1;
  int x_pos = Comp.Coordinates.at(0) * scale_x + x_offset;
  int y_pos = Comp.Coordinates.at(1) * scale_y + y_offset;
  int x_text = 25;
  int y_text = 0;
  int rotation = static_cast<int>(Comp.Rotation / 90);

  // Adjust text position depending on orientation
  switch (rotation) {
  case 2:
  case 0: // Horizontal orientation
    x_text = -30;
    y_text = -50;

    // Save pin position. This is needed for wiring later
    ComponentPinMap[Comp.ID][0] = QPoint(x_pos - 30, y_pos); // Pin 1
    ComponentPinMap[Comp.ID][1] = QPoint(x_pos + 30, y_pos); // Pin 2
    break;
  case 1: // Vertical orientation
    x_text = 10;
    y_text = -10;

    // Save pin position. This is needed for wiring later
    ComponentPinMap[Comp.ID][0] = QPoint(x_pos, y_pos + 30); // Pin 1
    ComponentPinMap[Comp.ID][1] = QPoint(x_pos, y_pos - 30); // Pin 2
    break;
  }

  QString componentLine =
      QString("<SPfile %1 %2 %3 %4 %5 %6 0 %7 \"file.s2p\" 1 \"rectangular\" 0 "
              "\"linear\" 0 \"open\" 0 \"2\" 0>\n")
          .arg(Comp.ID)
          .arg(status)
          .arg(x_pos)
          .arg(y_pos)
          .arg(x_text)
          .arg(y_text)
          .arg(rotation);

  // The S-parameter file needs a GND refererence
  QString GNDline =
      QString("<GND * 1 %1 %2 0 0 0 0>\n").arg(x_pos).arg(y_pos + 30);

  componentLine += GNDline;

  return componentLine;
}
