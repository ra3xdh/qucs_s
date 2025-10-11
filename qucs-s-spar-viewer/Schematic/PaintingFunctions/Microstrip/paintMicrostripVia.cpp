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

void Component::paintMicrostripVia(QPainter* painter) {
  if (Rotation != 0) {
    painter->rotate(Rotation);
  }

  // Define dimensions - similar to GND symbol but with orange rectangle
  int rectWidth       = 8;  // Width of the vertical orange rectangle
  int rectHeight      = 10; // Height of the vertical orange rectangle
  int groundLineWidth = 20; // Width of the ground line at bottom

  // Draw vertical rectangle filled with orange (represents the via)
  QRect viaRect(-rectWidth / 2, -rectHeight, rectWidth, rectHeight);
  painter->fillRect(viaRect, QColor(255, 140, 0)); // dark orange
  painter->setPen(QPen(Qt::black, 1));
  painter->drawRect(viaRect);

  // Draw horizontal ground line at bottom (like GND symbol)
  painter->drawLine(QPoint(-groundLineWidth / 2, 0),
                    QPoint(groundLineWidth / 2, 0));

  // Undo rotation for text
  if (Rotation != 0) {
    painter->rotate(-Rotation);
  }

  QPoint OriginText(-15, 5);
  if (Rotation != 0) {
    OriginText.setX(-10), OriginText.setY(20);
  }

  // Draw ID and diameter parameter text
  painter->drawText(QRect(OriginText, QPoint(100, 100)),
                    QString("%1").arg(this->ID));
  painter->drawText(QRect(OriginText + QPoint(0, 10), QPoint(100, 100)),
                    QString("D=%1").arg(Value["D"]));
  painter->drawText(QRect(OriginText + QPoint(0, 20), QPoint(100, 100)),
                    QString("N=%1").arg(Value["N"]));
}
