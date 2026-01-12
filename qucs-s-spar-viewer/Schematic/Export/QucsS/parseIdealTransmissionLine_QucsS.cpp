/// @file parseIdealTransmissionLine_QucsS.cpp
/// @brief Converts an ideal transmission line component to Qucs-S format
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 4, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "./../QucsS/QucsSExporter.h"

QString QucsSExporter::parseIdealTransmissionLine_QucsS(ComponentInfo Comp) {
  // Format: <TLIN ID status x y text_x text_y vertical_mirror rotation Z0
  // Z0_Visibility Length Length_Visibility Alpha Alpha_Visibility Temp
  // Temp_Visibility> ID: e.g. Lx, where x is a positive integer status:
  //         0 -> opened
  //         1 -> active
  //         2 -> shorted
  // x: x-axis position
  // y: y-axis position
  // text_x: x-axis position of the texts showing the properties
  // text_y: y-axis position of the texts showing the properties
  // vertical_mirror: {0, 1}
  // rotation: Component's rotation {0, 1, 2, 3}
  // Z0: Characteristic impednace (double)
  // Z0_visibility: Visibility of Z0. {0: Hide; 1: Show}
  // The same with the other params

  // Example: <TLIN Line1 1 -680 1980 -26 20 1 0 "50 Ohm" 1 "1 mm" 1 "0 dB" 0
  // "26.85" 0>

  int status = 1;
  int x_pos = Comp.Coordinates.at(0) * scale_x + x_offset;
  int y_pos = Comp.Coordinates.at(1) * scale_y + y_offset;
  int x_text = 25;
  int y_text = 0;
  int rotation = static_cast<int>(Comp.Rotation / 90) + 1;

  // Parameters
  QString Z0 = Comp.val["Z0"];
  int Z0_visibility = 1;

  QString Length = Comp.val["Length"];
  int Length_visibility = 1;

  // Adjust text position depending on orientation
  switch (rotation) {
  case 2:
  case 0: // Horizontal orientation
    x_text = -30;
    y_text = -60;

    // Save pin position. This is needed for wiring later
    ComponentPinMap[Comp.ID][0] = QPoint(x_pos - 30, y_pos); // Pin 1
    ComponentPinMap[Comp.ID][1] = QPoint(x_pos + 30, y_pos); // Pin 2
    break;
  case 1: // Vertical orientation
    x_text = 10;
    y_text = -20;

    // Save pin position. This is needed for wiring later
    ComponentPinMap[Comp.ID][0] = QPoint(x_pos, y_pos + 30); // Pin 1
    ComponentPinMap[Comp.ID][1] = QPoint(x_pos, y_pos - 30); // Pin 2
    break;
  }

  QString componentLine = QString("<TLIN %1 %2 %3 %4 %5 %6 1 %7 \"%8\" %9 "
                                  "\"%10\" %11 \"0 dB\" 0 \"26.85\" 0>\n")
                              .arg(Comp.ID)
                              .arg(status)
                              .arg(x_pos)
                              .arg(y_pos)
                              .arg(x_text)
                              .arg(y_text)
                              .arg(rotation)
                              .arg(Z0)
                              .arg(Z0_visibility)
                              .arg(Length)
                              .arg(Length_visibility);

  return componentLine;
}
