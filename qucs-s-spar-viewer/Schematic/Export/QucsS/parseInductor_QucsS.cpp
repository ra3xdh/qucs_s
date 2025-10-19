#include "./../../SchematicContent.h"

QString SchematicContent::parseInductor_QucsS(ComponentInfo Comp) {
  // Format: <L ID status x y text_x text_y L L_visibility>
  // ID: e.g. Lx, where x is a positive integer
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
  // L: Inductance value (double)
  // L_visibility: Visibility of L. {0: Hide; 1: Show}

  // Example: <L L1 1 250 310 -26 10 0 0 "1 mH" 1 "" 0>

  int status = 1;
  int x_pos = Comp.Coordinates.at(0) * scale_x_QucsS_export;
  int y_pos = Comp.Coordinates.at(1) * scale_y_QucsS_export;
  int x_text = 25;
  int y_text = 0;
  int rotation = static_cast<int>(Comp.Rotation / 90) + 1;
  QString L = Comp.val["L"];
  int L_visibility = 1;

  // Adjust text position depending on orientation
  switch (rotation) {
  case 0: // Horizontal orientation
    x_text = -30;
    y_text = -50;
    break;
  case 1: // Vertical orientation
    x_text = 20;
    y_text = -10;
    break;
  }

  QString componentLine = QString("<L %1 %2 %3 %4 %5 %6 0 %7 \"%8\" %9>\n")
                              .arg(Comp.ID)
                              .arg(status)
                              .arg(x_pos)
                              .arg(y_pos)
                              .arg(x_text)
                              .arg(y_text)
                              .arg(rotation)
                              .arg(L)
                              .arg(L_visibility);

  return componentLine;
}
