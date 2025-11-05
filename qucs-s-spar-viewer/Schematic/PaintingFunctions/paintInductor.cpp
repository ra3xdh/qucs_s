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

void Component::paintInductor(QPainter* painter) {
  if (Rotation != 0) {
    painter->rotate(Rotation);
  }

  // The following lines draw the capacitor in a vertical position
  painter->drawLine(QPoint(0, -25), QPoint(0, -2 * 7));
  painter->drawArc(QRect(-2 * 2, -2 * 7, 2 * 5.0, 2 * 5.0), -90 * 16, 180 * 16);
  painter->drawArc(QRect(-2 * 2, -2 * 2, 2 * 5.0, 2 * 5.0), -90 * 16, 180 * 16);
  painter->drawArc(QRect(-2 * 2, 2 * 3, 2 * 5.0, 2 * 5.0), -90 * 16, 180 * 16);
  painter->drawLine(QPoint(0, 16), QPoint(0, 25));
  painter->setPen(QPen(Qt::black, 1));

  if (Rotation != 0) { // The rotation is undone to draw the text
    painter->rotate(-Rotation);
  }

  QPoint OriginText(10, -10);
  if (Rotation != 0) {
    OriginText.setX(-10), OriginText.setY(5);
  }

  painter->drawText(QRect(OriginText, QPoint(100, 100)),
                    QString("%1").arg(this->ID));
  painter->drawText(QRect(OriginText + QPoint(0, +10), QPoint(100, 100)),
                    QString("%1").arg(Value["L"]));
}
