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

///
/// @brief Microstrip open painting method
/// \param painter Painter object
///
void Component::paintMicrostripOpen(QPainter *painter) {
  if (Rotation != 0) {
    painter->rotate(Rotation);
  }

  int w = 15;      // Microstrip width (similar to line)
  int length = 22; // Shortened section for open

  // Fill rectangle for the microstrip segment
  QRect fillRect(-w / 2, -14, w, length);
  painter->fillRect(fillRect, QColor(255, 140, 0)); // dark orange

  // Draw outline and left port connector
  painter->setPen(QPen(Qt::black, 1));
  painter->drawLine(QPoint(0, -25), QPoint(0, -14));          // left connector
  painter->drawLine(QPoint(-w / 2, -14), QPoint(w / 2, -14)); // top edge
  painter->drawLine(QPoint(-w / 2, -14),
                    QPoint(-w / 2, length - 14)); // left edge
  painter->drawLine(QPoint(w / 2, -14),
                    QPoint(w / 2, length - 14)); // right edge
  painter->drawLine(QPoint(-w / 2, length - 14),
                    QPoint(w / 2, length - 14)); // bottom edge

  // Draw 4 parallel thin diagonal lines at the open end
  int edgeY = length - 14;
  int lineSpacing = 5;
  int lineLen = 4;
  int baseOffset = -7;

  painter->setPen(QPen(Qt::black, 1));
  painter->drawLine(
      QPoint(baseOffset + 0 * lineSpacing, edgeY),
      QPoint(baseOffset + 0 * lineSpacing + lineLen, edgeY + lineLen));
  painter->drawLine(
      QPoint(baseOffset + 1 * lineSpacing, edgeY),
      QPoint(baseOffset + 1 * lineSpacing + lineLen, edgeY + lineLen));
  painter->drawLine(
      QPoint(baseOffset + 2 * lineSpacing, edgeY),
      QPoint(baseOffset + 2 * lineSpacing + lineLen, edgeY + lineLen));
  painter->drawLine(
      QPoint(baseOffset + 3 * lineSpacing, edgeY),
      QPoint(baseOffset + 3 * lineSpacing + lineLen, edgeY + lineLen));

  // Undo rotation for parameter text
  if (Rotation != 0) {
    painter->rotate(-Rotation);
  }

  QPoint OriginText(10, -10);
  if (Rotation != 0) {
    OriginText.setX(-15), OriginText.setY(10);
  }

  // Draw ID and parameters: width only
  painter->drawText(QRect(OriginText, QPoint(100, 100)),
                    QString("%1").arg(this->ID));
  painter->drawText(QRect(OriginText + QPoint(0, 10), QPoint(100, 100)),
                    QString("W=%1").arg(Value["Width"]));
}
