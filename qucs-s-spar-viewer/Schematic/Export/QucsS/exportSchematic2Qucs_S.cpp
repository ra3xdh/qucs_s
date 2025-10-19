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

// This is needed for catching the current Qucs-S version
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "./../../SchematicContent.h"

QString SchematicContent::export2QucsS() {
  if (Comps.isEmpty()) {
    return QString("");
  }

  QString qucsNetlist;

  // Qucs-S header
  qucsNetlist += QString("<Qucs Schematic %1>\n").arg(PACKAGE_VERSION);

  ///////////////////////////////////////////////////////////////
  // Process components
  qucsNetlist += QString("<Components>\n");

  // Coordinates of the bottom left (needed for putting the simulation box, etc.
  // there)
  int x_bottom = 1e6, y_bottom = 1e6;

  for (int i = 0; i < Comps.length(); i++) {

    // Update circuit bottom-left coordinate
    std::vector<double> coordinates = Comps[i].Coordinates;
    if (coordinates[0] < x_bottom) {
      x_bottom = coordinates[0];
    }
    if (coordinates[1] < y_bottom) {
      y_bottom = coordinates[1];
    }

    // Convert component to Qucs format
    QString componentLine;

    switch (Comps[i].Type) {
    case Resistor:
      componentLine = parseResistor_QucsS(Comps[i]);
      break;

    case Capacitor:
      componentLine = parseCapacitor_QucsS(Comps[i]);
      break;

    case Inductor:
      componentLine = parseInductor_QucsS(Comps[i]);
      break;

    case Term:
      componentLine = parseTerm_QucsS(Comps[i]);
      break;

    case GND:
      componentLine = parseGND_QucsS(Comps[i]);
      break;

    default:
      componentLine =
          QString(".* Unsupported component: %1\n").arg(Comps[i].ID);
      break;
    }

    if (!componentLine.isEmpty()) {
      qucsNetlist += componentLine;
    }
  }

  // Add S-parameter simulation box
  y_bottom += 150;
  qucsNetlist +=
      QString("<.SP SP1 1 %1 %2 0 60 0 0 \"log\" 1 \"%3\" 1 \"%4\" 1 "
              "\"%5\" 1 \"no\" 0 \"1\" 0 \"2\" 0 \"no\" 0 \"no\" 0>\n")
          .arg(x_bottom)
          .arg(y_bottom)
          .arg(f_start)
          .arg(f_stop)
          .arg(n_points);

  x_bottom += 200;

  // Add equations
  if (this->Type == QString("Power Combiner")) {
    qucsNetlist += QString("<Eqn Eqn1 1 %1 %2 -28 15 0 0 \"S11_dB=dB(S[1,1])\" "
                           "1 \"S21_dB=dB(S[2,1])\" 1 \"S31_dB=dB(S[3,1])\" 1 "
                           "\"S32_dB=dB(S[3,2])\" 1 \"yes\" 0>\n")
                       .arg(x_bottom)
                       .arg(y_bottom);

  } else if (this->Type == QString("Matching-1-port")) {
    qucsNetlist +=
        QString(
            "<Eqn Eqn1 1 %1 %2 -28 15 0 0 \"S11_dB=dB(S[1,1])\" 1 \"yes\" 0>\n")
            .arg(x_bottom)
            .arg(y_bottom);

  } else if (this->Type == QString("Matching-2-ports")) {
    qucsNetlist += QString("<Eqn Eqn1 1 %1 %2 -28 15 0 0 \"S21_dB=dB(S[2,1])\" "
                           "1 \"S11_dB=dB(S[1,1])\" 1 \"yes\" 0>\n")
                       .arg(x_bottom)
                       .arg(y_bottom);
  } else {
    // Filter
    qucsNetlist += QString("<Eqn Eqn1 1 %1 %2 -28 15 0 0 \"S21_dB=dB(S[2,1])\" "
                           "1 \"S11_dB=dB(S[1,1])\" 1 \"yes\" 0>\n")
                       .arg(x_bottom)
                       .arg(y_bottom);
  }

  qucsNetlist += QString("</Components>\n"); // Close the components section

  ///////////////////////////////////////////////////////////////

  qDebug() << qucsNetlist;
  return qucsNetlist;
}
