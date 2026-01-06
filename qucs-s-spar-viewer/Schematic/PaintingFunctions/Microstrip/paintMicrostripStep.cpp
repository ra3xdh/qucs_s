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
/// @brief Microstrip step painting method
/// \param painter Painter object
///
void Component::paintMicrostripStep(QPainter *painter) {
  if (Rotation != 0) {
    painter->rotate(Rotation);
  }

  // Define widths for the two sections (now horizontal orientation)
  int w1 = 24;   // Width of first (wider) section
  int w2 = 12;   // Width of second (narrower) section
  int len1 = 14; // Length of first section
  int len2 = 16; // Length of second section

  // Fill first (wider) rectangle with dark orange - horizontal left section
  QRect fillRect1(-14, -w1 / 2, len1, w1);
  painter->fillRect(fillRect1, QColor(255, 140, 0)); // dark orange

  // Fill second (narrower) rectangle with dark orange - horizontal right
  // section
  QRect fillRect2(0, -w2 / 2, len2, w2);
  painter->fillRect(fillRect2, QColor(255, 140, 0)); // dark orange

  // Draw outlines with black pen
  painter->setPen(QPen(Qt::black, 1));

  // Left connector line
  painter->drawLine(QPoint(-25, 0), QPoint(-14, 0));

  // First (wider) section outline
  painter->drawLine(QPoint(-14, -w1 / 2), QPoint(-14, w1 / 2)); // left edge
  painter->drawLine(QPoint(-14, -w1 / 2), QPoint(0, -w1 / 2));  // top edge
  painter->drawLine(QPoint(-14, w1 / 2), QPoint(0, w1 / 2));    // bottom edge

  // Step transition (sharp vertical lines at the junction)
  painter->drawLine(QPoint(0, -w1 / 2), QPoint(0, -w2 / 2)); // top step
  painter->drawLine(QPoint(0, w2 / 2), QPoint(0, w1 / 2));   // bottom step

  // Second (narrower) section outline
  painter->drawLine(QPoint(0, -w2 / 2), QPoint(16, -w2 / 2)); // top edge
  painter->drawLine(QPoint(0, w2 / 2), QPoint(16, w2 / 2));   // bottom edge
  painter->drawLine(QPoint(16, -w2 / 2), QPoint(16, w2 / 2)); // right edge

  // Right connector line
  painter->drawLine(QPoint(16, 0), QPoint(25, 0));

  // Undo rotation for text
  if (Rotation != 0) {
    painter->rotate(-Rotation);
  }

  QPoint OriginText(-15, 15);
  if (Rotation != 0) {
    OriginText.setX(-10), OriginText.setY(20);
  }

  // Draw ID and other parameters text
  painter->drawText(QRect(OriginText, QPoint(100, 100)),
                    QString("%1").arg(this->ID));
  painter->drawText(QRect(OriginText + QPoint(0, 10), QPoint(100, 100)),
                    QString("W1=%1").arg(Value["W1"]));
  painter->drawText(QRect(OriginText + QPoint(0, 20), QPoint(100, 100)),
                    QString("W2=%1").arg(Value["W2"]));
}
