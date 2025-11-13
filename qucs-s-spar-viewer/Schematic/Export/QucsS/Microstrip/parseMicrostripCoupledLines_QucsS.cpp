#include "./../../../SchematicContent.h"

QString
SchematicContent::parseMicrostripCoupledLines_QucsS(ComponentInfo Comp) {
  // Format: <MCOUPLED ID status x y text_x text_y vertical_mirror rotation
  // <properties>> ...>
  // status:
  //         0 -> opened
  //         1 -> active
  //         2 -> shorted
  // x: x-axis position
  // y: y-axis position
  // text_x: x-axis position of the texts showing the properties
  // text_y: y-axis position of the texts showing the properties
  // vertical_mirror: {0, 1}
  // rotation: Component's rotation {0, 1, 2, 3}

  // Example: <MCOUPLED MS1 1 410 -590 -26 37 0 0 "Subst1" 1 "1 mm" 1 "10 mm" 1
  // "1 mm" 1 "Kirschning" 0 "Kirschning" 0 "26.85" 0 "DC" 0>

  int status = 1;
  int x_pos = Comp.Coordinates.at(0) * scale_x_QucsS_export + x_offset_export;
  int y_pos = Comp.Coordinates.at(1) * scale_y_QucsS_export + y_offset_export;
  int x_text = 25;
  int y_text = 0;
  int rotation = static_cast<int>(Comp.Rotation / 90) + 1;

  // Add offsets to the schematic
  y_offset_export += 30;

  // Parameters
  QString Width = Comp.val["W"];
  int Width_visibility = 1;

  QString Length = Comp.val["L"];
  int Length_visibility = 1;

  QString Gap = Comp.val["S"];
  int Gap_visibility = 1;

  // Adjust text position depending on orientation
  switch (rotation) {
  case 2:
  case 0: // Horizontal orientation
    x_text = -30;
    y_text = -100;

    // Save pin position. This is needed for wiring later
    ComponentPinMap[Comp.ID][0] = QPoint(x_pos - 30, y_pos - 30); // Pin 1
    ComponentPinMap[Comp.ID][1] = QPoint(x_pos - 30, y_pos + 30); // Pin 2
    ComponentPinMap[Comp.ID][2] = QPoint(x_pos + 30, y_pos + 30); // Pin 3
    ComponentPinMap[Comp.ID][3] = QPoint(x_pos + 30, y_pos - 30); // Pin 4
    break;
  }

  QString componentLine =
      QString("<MCOUPLED %1 %2 %3 %4 %5 %6 1 %7 \"Subst1\" 0 \"%8\" %9 "
              "\"%10\" %11 \"%12\" %13 \"Hammerstad\" 0 \"Kirschning\" "
              "0 \"26.85\" 0 \"DC\" 0>\n")
          .arg(Comp.ID)
          .arg(status)
          .arg(x_pos)
          .arg(y_pos)
          .arg(x_text)
          .arg(y_text)
          .arg(rotation)
          .arg(Width)
          .arg(Width_visibility)
          .arg(Length)
          .arg(Length_visibility)
          .arg(Gap)
          .arg(Gap_visibility);

  return componentLine;
}
