#include "./../QucsS/QucsSExporter.h"

QString QucsSExporter::parseComplexImpedance_QucsS(ComponentInfo Comp,
                                                   double Z0) {
  // Format: <RFEDD ID status x y text_x text_y 0 rotation params>
  // ID: e.g. Rx, where x is a positive integer
  // status:
  //         0 -> opened
  //         1 -> active
  //         2 -> shorted
  // x: x-axis position
  // y: y-axis position
  // 0: Unidentified behaviour
  // rotation: Component's rotation {0, 1, 2, 3}
  // text_x: x-axis position of the texts showing the properties
  // text_y: y-axis position of the texts showing the properties

  // Example: <RFEDD RF1 1 210 250 -26 -38 0 0 "S" 0 "2" 0 "open" 0 "S11" 0
  // "S12" 0 "S21" 0 "S22" 0>

  int status = 1;
  int x_pos = Comp.Coordinates.at(0) * scale_x + x_offset;
  int y_pos = Comp.Coordinates.at(1) * scale_y + y_offset;
  int x_text = 25;
  int y_text = 0;
  int rotation = static_cast<int>(Comp.Rotation / 90) + 1;
  QString Zstr = Comp.val["Z"]; // R+jX

  // Process the string to get the resistance and the reactance
  std::complex<double> Z = Str2Complex(Zstr);

  // Calculate the ABCD parameters of a series impedance
  std::complex<double> A = 1;
  std::complex<double> B = Z;
  std::complex<double> C = 0;
  std::complex<double> D = 1;

  // Convert ABCD into S
  std::complex<double> K = (A + B / Z0 + C * Z0 + D);
  std::complex<double> S11 = (A + B / Z0 - C * Z0 - D) / K;
  std::complex<double> S12 = 2. * (A * D - B * C) / K;
  std::complex<double> S21 = 2. / K;
  std::complex<double> S22 = (-A + B / Z0 - C * Z0 + D) / K;

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
    x_text = 20;
    y_text = -10;

    // Save pin position. This is needed for wiring later
    ComponentPinMap[Comp.ID][0] = QPoint(x_pos, y_pos + 30); // Pin 1
    ComponentPinMap[Comp.ID][1] = QPoint(x_pos, y_pos - 30); // Pin 2
    break;
  }

  // Add RFEDD
  QString componentLine =
      QString("<RFEDD %1 %2 %3 %4 %5 %6 0 %7 \"S\" 0 \"2\" 0 \"open\" 0 "
              "\"%8\" 0  \"%9\" 0 \"%10\" 0 \"%11\" 0>\n")
          .arg(Comp.ID)
          .arg(status)
          .arg(x_pos)
          .arg(y_pos)
          .arg(x_text)
          .arg(y_text)
          .arg(rotation)
          .arg(num2str(S11, NoUnits))
          .arg(num2str(S12, NoUnits))
          .arg(num2str(S21, NoUnits))
          .arg(num2str(S22, NoUnits));

  return componentLine;
}
