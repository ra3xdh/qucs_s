/// @file parseMicrostripVia_QucsS.cpp
/// @brief Converts a microstrip via component to Qucs-S format
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 4, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "./../../QucsS/QucsSExporter.h"

QString QucsSExporter::parseMicrostripVia_QucsS(ComponentInfo Comp) {
  // Format: <MVIA ID status x y text_x text_y vertical_mirror rotation
  // substrate substrate_visibility diameter diameter_visibility
  //  MS via example
  // <MVIA MS4 1 -660 1290 -85 -40 0 2 "Sub1" 0 "0.5 mm" 1 "26.85" 0>

  int status = 5; // Active. Name hidden

  int x_pos = Comp.Coordinates.at(0) * scale_x + x_offset - 20;
  int y_pos = Comp.Coordinates.at(1) * scale_y + y_offset;

  int x_text = 25;
  int y_text = 0;

  // Parameters
  QString Diameter = Comp.val["D"];
  int Diameter_visibility = 0;

  // Number of vias
  // Ideally, the export code should be able to put an array of vias. Currently,
  // it's not possible.
  // int N_vias = Comp.val["N"].toInt();

  x_text = 0;
  y_text = 30;

  // Save pin position. This is needed for wiring later
  ComponentPinMap[Comp.ID][0] = QPoint(x_pos + 20, y_pos); // Pin 1

  QString componentLine =
      QString("<MVIA %1 %2 %3 %4 %5 %6 1 2 \"Subst1\" 0 \"%7\" %8 "
              "\"Hammerstad\" 0 \"Kirschning\" 0 \"Kirschning\" 0>\n")
          .arg(Comp.ID)
          .arg(status)
          .arg(x_pos)
          .arg(y_pos)
          .arg(x_text)
          .arg(y_text)
          .arg(Diameter)
          .arg(Diameter_visibility);

  return componentLine;
}
