#include "./../../SchematicContent.h"

QString SchematicContent::parseGND_QucsS(ComponentInfo Comp) {
  // Format: <GND * status x y 0 0 0 0>
  // status:
  //         0 -> opened
  //         1 -> active
  //         2 -> shorted
  // x: x-axis position
  // y: y-axis position
  // 0's: Unidentified behaviour

  // Example: <GND * 1 60 350 0 0 0 0>

  int status = 1;
  int x_pos = Comp.Coordinates.at(0) * scale_x_QucsS_export;
  int y_pos = Comp.Coordinates.at(1) * scale_y_QucsS_export -
              20; // Additional correction needed: The GND in the internal
                  // schematic was a small wire connection
  int mirror = 0;

  if (y_pos < 0) {
    // if GND is above the "main circuit line" mirror the ground
    mirror = 1;
    y_pos += 20; // Correct y-axis position
  }

  QString componentLine = QString("<GND * %1 %2 %3 0 0 %4 0>\n")
                              .arg(status)
                              .arg(x_pos)
                              .arg(y_pos)
                              .arg(mirror);

  return componentLine;
}
