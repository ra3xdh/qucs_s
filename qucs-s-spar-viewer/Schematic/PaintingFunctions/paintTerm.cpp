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

#include "./../component.h"

///
/// @brief Port terminal painting method
/// \param painter Painter object
///
void Component::paintTerm(QPainter *painter) {
  if (Rotation != 0) {
    painter->rotate(Rotation);
  }

  QPainterPath path;
  path.moveTo(0, 0);
  path.lineTo(0, 6);
  path.lineTo(10, 0);
  path.lineTo(0, -6);
  painter->setPen(Qt ::NoPen);
  painter->fillPath(path, QBrush(QColor("red")));
  painter->setPen(QPen(Qt::black, 1));

  int OriginText_x = -25;
  if (Rotation != 0) {
    painter->rotate(-Rotation);
    OriginText_x = 5;
  }

  painter->drawText(QRect(OriginText_x, -10, 100, 100),
                    QString("%1").arg(this->ID));
  painter->drawText(
      QRect(OriginText_x, 0, 100, 100),
      QString("%1").arg(Value["Z"].replace("Ohm", QChar(0xa9, 0x03))));
}
