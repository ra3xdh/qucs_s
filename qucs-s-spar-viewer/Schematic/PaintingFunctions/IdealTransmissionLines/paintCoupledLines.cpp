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

#include "./../../component.h"

void Component::paintCoupledLines(QPainter* painter) {

  if (Rotation != 0) {
    painter->rotate(Rotation);
  }

  int w = 15, shiftx = 10;

  // The coupled lines painting code is the same as the transmission line code,
  // but it is shifted by shiftx
  painter->drawLine(QPoint(-shiftx, -25), QPoint(-shiftx, -14));
  painter->drawLine(QPoint(-shiftx - 0.5 * w, -14),
                    QPoint(-shiftx + 0.5 * w, -14));
  painter->drawLine(QPoint(-shiftx - 0.5 * w, -14),
                    QPoint(-shiftx + 0.5 * w, -14));
  painter->drawLine(QPoint(-shiftx - 0.5 * w, -14),
                    QPoint(-shiftx - 0.5 * w, 16));
  painter->drawLine(QPoint(-shiftx + 0.5 * w, -14),
                    QPoint(-shiftx + 0.5 * w, 16));
  painter->drawLine(QPoint(-shiftx - 0.5 * w, 16),
                    QPoint(-shiftx + 0.5 * w, 16));
  painter->drawLine(QPoint(-shiftx, 16), QPoint(-shiftx, 25));

  painter->drawLine(QPoint(shiftx, -25), QPoint(shiftx, -14));
  painter->drawLine(QPoint(shiftx - 0.5 * w, -14),
                    QPoint(shiftx + 0.5 * w, -14));
  painter->drawLine(QPoint(shiftx - 0.5 * w, -14),
                    QPoint(shiftx + 0.5 * w, -14));
  painter->drawLine(QPoint(shiftx - 0.5 * w, -14),
                    QPoint(shiftx - 0.5 * w, 16));
  painter->drawLine(QPoint(shiftx + 0.5 * w, -14),
                    QPoint(shiftx + 0.5 * w, 16));
  painter->drawLine(QPoint(shiftx - 0.5 * w, 16), QPoint(shiftx + 0.5 * w, 16));
  painter->drawLine(QPoint(shiftx, 16), QPoint(shiftx, 25));
  painter->setPen(QPen(Qt::black, 1));

  if (Rotation != 0) { // The rotation is undone to draw the text
    painter->rotate(-Rotation);
  }

  QPoint OriginText(20, -10);
  if (Rotation != 0) {
    OriginText.setX(-20), OriginText.setY(20);
  }

  painter->drawText(QRect(OriginText, QPoint(100, 100)),
                    QString("%1").arg(this->ID));
  painter->drawText(
      QRect(OriginText + QPoint(0, 10), QPoint(100, 100)),
      QString("%1").arg(Value["Ze"].replace("Ohm", QChar(0xa9, 0x03))));
  painter->drawText(
      QRect(OriginText + QPoint(0, 20), QPoint(100, 100)),
      QString("%1").arg(Value["Zo"].replace("Ohm", QChar(0xa9, 0x03))));
  painter->drawText(QRect(OriginText + QPoint(0, 30), QPoint(100, 100)),
                    QString("%1").arg(Value["Length"]));
}
