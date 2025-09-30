/*
 *  Copyright (C) 2025 Andrés Martínez Mera - andresmmera@protonmail.com
 *  Based on msstep.cpp from Qucs - Copyright (C) 2004, 2007, 2008 Stefan Jahn
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

void Component::paintMicrostripStep(QPainter *painter) {
  if (Rotation != 0) {
    painter->rotate(Rotation);
  }

  // Define widths for the two sections
  int w1 = 24;  // Width of first (wider) section
  int w2 = 12;  // Width of second (narrower) section
  int h1 = 14;  // Height of first section
  int h2 = 16;  // Height of second section

  // Fill first (wider) rectangle with dark orange
  QRect fillRect1(-w1/2, -14, w1, h1);
  painter->fillRect(fillRect1, QColor(255, 140, 0)); // dark orange

  // Fill second (narrower) rectangle with dark orange
  QRect fillRect2(-w2/2, 0, w2, h2);
  painter->fillRect(fillRect2, QColor(255, 140, 0)); // dark orange

  // Draw outlines with black pen
  painter->setPen(QPen(Qt::black, 1));

  // Top connector line
  painter->drawLine(QPoint(0, -25), QPoint(0, -14));

  // First (wider) section outline
  painter->drawLine(QPoint(-w1/2, -14), QPoint(w1/2, -14));  // top edge
  painter->drawLine(QPoint(-w1/2, -14), QPoint(-w1/2, 0));   // left edge
  painter->drawLine(QPoint(w1/2, -14), QPoint(w1/2, 0));     // right edge

  // Step transition (sharp horizontal lines at the junction)
  painter->drawLine(QPoint(-w1/2, 0), QPoint(-w2/2, 0));     // left step
  painter->drawLine(QPoint(w2/2, 0), QPoint(w1/2, 0));       // right step

  // Second (narrower) section outline
  painter->drawLine(QPoint(-w2/2, 0), QPoint(-w2/2, 16));    // left edge
  painter->drawLine(QPoint(w2/2, 0), QPoint(w2/2, 16));      // right edge
  painter->drawLine(QPoint(-w2/2, 16), QPoint(w2/2, 16));    // bottom edge

  // Bottom connector line
  painter->drawLine(QPoint(0, 16), QPoint(0, 25));

  // Undo rotation for text
  if (Rotation != 0) {
    painter->rotate(-Rotation);
  }

  QPoint OriginText(15, -10);
  if (Rotation != 0)
    OriginText.setX(-15), OriginText.setY(10);

  // Draw ID and other parameters text
  painter->drawText(QRect(OriginText, QPoint(100, 100)), QString("%1").arg(this->ID));
  painter->drawText(QRect(OriginText + QPoint(0, 10), QPoint(100, 100)), QString("W1=%1").arg(Value["W1"]));
  painter->drawText(QRect(OriginText + QPoint(0, 20), QPoint(100, 100)), QString("W2=%1").arg(Value["W2"]));
}
