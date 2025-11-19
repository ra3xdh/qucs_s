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

#include "./../QucsS/QucsSExporter.h"

QString QucsSExporter::parseIdealCoupler_QucsS(ComponentInfo Comp) {
  // Format: <Coupler ID status x y text_x text_y vertical_mirror rotation k
  // k_Visibility phi phi_Visibility Z0 Z0_Visibility>
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
  // k: Coupling factor (double [0, 1])
  // k_visibility: Visibility of the coupling factor. {0: Hide; 1: Show}
  // phi: Phase shift in the coupling path (deg)
  // phi_visibility: Visibility of phi
  // Z0: Characteristic impedance
  // Z0_visibility: Visitbilty of Z0 in the schematic

  // Example: <Coupler X1 1 1220 1450 -26 29 0 0 "0.7071" 1 "180" 1 "50 Ohm" 0>

  int status = 1;
  int x_pos = Comp.Coordinates.at(0) * scale_x + x_offset;
  int y_pos = Comp.Coordinates.at(1) * scale_y + y_offset;
  int x_text = 25;
  int y_text = -20;
  int rotation = static_cast<int>(Comp.Rotation / 90) + 1;

  // Parameters
  QString k = Comp.val["k"];
  int k_visibility = 1;

  QString phi = Comp.val["phase"];
  int phi_visibility = 0;

  QString Z0 = Comp.val["Z0"];
  int Z0_visibility = 0;

  // Adjust text position depending on orientation
  switch (rotation) {
  case 1: // Vertical orientation

    // Save pin position. This is needed for wiring later
    ComponentPinMap[Comp.ID][0] = QPoint(x_pos - 20, y_pos + 30); // Pin 1
    ComponentPinMap[Comp.ID][1] = QPoint(x_pos - 20, y_pos - 30); // Pin 2
    ComponentPinMap[Comp.ID][2] = QPoint(x_pos + 20, y_pos - 30); // Pin 3
    ComponentPinMap[Comp.ID][3] = QPoint(x_pos + 20, y_pos + 30); // Pin 4
    break;
  }

  QString componentLine = QString("<Coupler %1 %2 %3 %4 %5 %6 1 %7 \"%8\" %9 "
                                  "\"%10\" %11 \"%12\" %13 1 \"1\" 0 \"1\" 0 "
                                  "\"0 dB\" 0 \"0 dB\" 0 \"26.85\" 0>\n")
                              .arg(Comp.ID)
                              .arg(status)
                              .arg(x_pos)
                              .arg(y_pos)
                              .arg(x_text)
                              .arg(y_text)
                              .arg(rotation)
                              .arg(k)
                              .arg(k_visibility)
                              .arg(phi)
                              .arg(phi_visibility)
                              .arg(Z0)
                              .arg(Z0_visibility);

  return componentLine;
}
