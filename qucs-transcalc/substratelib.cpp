/*
 * substratelib.cpp - minimal reader for Qucs substrate library files
 *
 * Copyright (C) 2026 Qucs-S team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "substratelib.h"

#include <QFile>
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>
#include <QStringList>
#include <QTextStream>

/* Length units understood by Transcalc (see TRANS_LENGTHS in qucstrans.h). */
static bool isSupportedLengthUnit(const QString& unit) {
  static const QStringList units = {"mil", "cm", "mm", "m", "um", "in", "ft"};
  return units.contains(unit);
}

bool parseLengthWithUnit(const QString& text, double& value, QString& unit) {
  const QString s = text.simplified();
  const int sep   = s.indexOf(' ');
  if (sep <= 0) {
    return false; // need a "<value> <unit>" pair
  }

  bool ok        = false;
  const double v = s.left(sep).toDouble(&ok); // QString::toDouble uses C locale
  if (!ok) {
    return false;
  }

  const QString u = s.mid(sep + 1).trimmed();
  if (!isSupportedLengthUnit(u)) {
    return false;
  }

  value = v;
  unit  = u;
  return true;
}

QList<SubstrateMaterial> readSubstrateLibrary(const QString& filePath) {
  QList<SubstrateMaterial> materials;

  QFile file(filePath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return materials; // no file -> behave exactly as before
  }

  QString content;
  {
    QTextStream stream(&file);
    content = stream.readAll();
  }
  file.close();

  const QString compOpen  = "<Component ";
  const QString compClose = "</Component>";
  const QRegularExpression quoted("\"([^\"]*)\"");

  int pos = 0;
  while ((pos = content.indexOf(compOpen, pos)) >= 0) {
    const int nameStart = pos + compOpen.length();
    const int nameEnd   = content.indexOf('>', nameStart);
    if (nameEnd < 0) {
      break; // truncated header, nothing usable left
    }

    const int blockEnd = content.indexOf(compClose, nameEnd);
    if (blockEnd < 0) {
      break; // truncated component, nothing usable left
    }

    // advance the cursor now so every "continue" below is safe
    const int nextPos = blockEnd + compClose.length();

    const QString name  = content.mid(nameStart, nameEnd - nameStart).trimmed();
    const QString block = content.mid(nameEnd + 1, blockEnd - nameEnd - 1);
    pos                 = nextPos;

    if (name.isEmpty()) {
      continue;
    }

    const int substStart = block.indexOf("<SUBST");
    if (substStart < 0) {
      continue; // not a substrate component
    }
    const int substEnd = block.indexOf('>', substStart);
    if (substEnd < 0) {
      continue;
    }
    const QString substLine = block.mid(substStart, substEnd - substStart);

    QStringList values;
    QRegularExpressionMatchIterator it = quoted.globalMatch(substLine);
    while (it.hasNext()) {
      values << it.next().captured(1);
    }
    if (values.count() < 6) {
      continue; // not enough positional values
    }

    SubstrateMaterial m;
    m.name = name;

    bool ok = false;
    m.er    = values.at(0).toDouble(&ok);
    if (!ok) {
      continue;
    }
    if (!parseLengthWithUnit(values.at(1), m.h, m.hUnit)) {
      continue;
    }
    if (!parseLengthWithUnit(values.at(2), m.t, m.tUnit)) {
      continue;
    }
    m.tand = values.at(3).toDouble(&ok);
    if (!ok) {
      continue;
    }
    m.rho = values.at(4).toDouble(&ok); // rho == 0 is valid; no division here
    if (!ok) {
      continue;
    }
    m.D = values.at(5).toDouble(&ok);
    if (!ok) {
      continue;
    }

    materials.append(m);
  }

  return materials;
}
