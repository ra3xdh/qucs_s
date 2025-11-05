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

#include "SParameterCalculator.h"

double SParameterCalculator::parseScaledValue(const QString &input,
                                              QString unit_type) {
  // Regex to split number from unit
  static const QRegularExpression regex(
      "^\\s*([+-]?(?:\\d+(?:\\.\\d*)?|\\.\\d+)(?:[eE][+-]?\\d+)?)\\s*([a-zA-"
      "ZÂµ]*)\\s*$");
  QRegularExpressionMatch match = regex.match(input);

  if (!match.hasMatch()) {
    return 0.0;
  }

  double value = match.captured(1).toDouble();
  QString unit = match.captured(2).trimmed();

  if (unit_type == QString("Length")) {
    // Handle explicit length units FIRST (before SI prefixes)
    static const QMap<QString, double> lengthUnits = {
        {"nm", 1e-9},     // nanometers to meters
        {"µ", 1e-6},      // micrometers to meters
        {"um", 1e-6},     // micrometers to meters
        {"mm", 1e-3},     // millimeters to meters
        {"cm", 1e-2},     // centimeters to meters
        {"dm", 1e-1},     // decimeters to meters
        {"m", 1.0},       // meters (base unit)
        {"km", 1e3},      // kilometers to meters
        {"mil", 25.4e-6}, // mils to meters
        {"in", 0.0254},   // inches to meters
        {"ft", 0.3048}    // feet to meters
    };

    // Check for exact length unit match first
    if (lengthUnits.contains(unit)) {
      return value * lengthUnits[unit];
    }
  } else {

    // SI prefixes for electrical units (capacitance, inductance, etc.)
    static const QMap<QString, double> scaleMap = {
        {"f", 1e-15}, // femto
        {"p", 1e-12}, // pico
        {"n", 1e-9},  // nano
        {"u", 1e-6},  // micro
        {"µ", 1e-6},  // micro (Unicode)
        {"m", 1e-3},  // mili
        {"", 1.0},    // no prefix
        {"k", 1e3},   // kilo
        {"K", 1e3},   // kilo
        {"M", 1e6},   // mega
        {"G", 1e9},   // giga
        {"T", 1e12}   // tera
    };

    // For electrical units, extract first character as prefix
    QString prefix;
    if (!unit.isEmpty()) {
      prefix = QString(unit.at(0));
    }
    double scale = scaleMap.value(prefix, 1.0);
    return value * scale;
  }
  return -1;
}
