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

void Component::paintCoupler(QPainter* painter) {

  if (Rotation != 0) {
    painter->rotate(Rotation);
  }

  // Terms
  painter->drawLine(QPoint(-10, -25), QPoint(-10, -20));
  painter->drawLine(QPoint(-10, 20), QPoint(-10, 25));
  painter->drawLine(QPoint(10, -25), QPoint(10, -20));
  painter->drawLine(QPoint(10, 20), QPoint(10, 25));

  // Box
  painter->drawRect(QRect(-15, -20, 30, 40));

  painter->setPen(QPen(Qt::black, 1));
  // Draw lines
  painter->drawLine(QPoint(-10, -12), QPoint(-10, 12));
  painter->drawLine(QPoint(10, -12), QPoint(10, 12));

  // Draw couplings
  painter->setPen(QPen(Qt::gray, 1, Qt::DotLine));
  painter->drawLine(QPoint(-10, -12), QPoint(10, 12));
  painter->drawLine(QPoint(10, -12), QPoint(-10, 12));

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
