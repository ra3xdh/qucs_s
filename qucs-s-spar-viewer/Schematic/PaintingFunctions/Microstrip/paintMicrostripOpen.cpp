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

void Component::paintMicrostripOpen(QPainter *painter) {
  if (Rotation != 0) {
    painter->rotate(Rotation);
  }
  
  // Define dimensions for microstrip open end (horizontal orientation)
  int width = 24;   // Width of the microstrip line
  int length = 20;  // Length of the microstrip section
  
  // Fill the microstrip line with dark orange - horizontal section ending at open
  QRect fillRect(-20, -width/2, length, width);
  painter->fillRect(fillRect, QColor(255, 140, 0)); // dark orange
  
  // Draw outlines with black pen
  painter->setPen(QPen(Qt::black, 1));
  
  // Left connector line (input port)
  painter->drawLine(QPoint(-25, 0), QPoint(-20, 0));
  
  // Microstrip line outline
  painter->drawLine(QPoint(-20, -width/2), QPoint(-20, width/2));  // left edge
  painter->drawLine(QPoint(-20, -width/2), QPoint(0, -width/2));   // top edge
  painter->drawLine(QPoint(-20, width/2), QPoint(0, width/2));     // bottom edge
  
  // Open end (right edge) - emphasized with thicker line
  painter->setPen(QPen(Qt::black, 2));
  painter->drawLine(QPoint(0, -width/2), QPoint(0, width/2));      // open end edge
  
  // Draw small capacitance symbol to indicate the open end effect
  painter->setPen(QPen(Qt::black, 1));
  int capOffset = 3;
  int capHeight = 8;
  painter->drawLine(QPoint(capOffset, -capHeight), QPoint(capOffset, capHeight));  // capacitor plate
  
  // Undo rotation for text
  if (Rotation != 0) {
    painter->rotate(-Rotation);
  }
  
  QPoint OriginText(-15, 15);
  if (Rotation != 0)
    OriginText.setX(-10), OriginText.setY(20);
  
  // Draw ID and width parameter text
  painter->drawText(QRect(OriginText, QPoint(100, 100)), QString("%1").arg(this->ID));
  painter->drawText(QRect(OriginText + QPoint(0, 10), QPoint(100, 100)), QString("W=%1").arg(Value["W"]));
}
