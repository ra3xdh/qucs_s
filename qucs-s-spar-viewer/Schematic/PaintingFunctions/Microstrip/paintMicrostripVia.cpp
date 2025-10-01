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

void Component::paintMicrostripVia(QPainter *painter) {
  if (Rotation != 0) {
    painter->rotate(Rotation);
  }
  
  // Define dimensions for microstrip via
  int viaRadius = 6;      // Radius of the via hole
  int padRadius = 10;     // Radius of the pad around via
  int lineWidth = 20;     // Width of microstrip lines
  int lineLength = 12;    // Length of line segments on each side
  
  // Draw left microstrip line (dark orange)
  QRect leftLine(-lineLength - padRadius, -lineWidth/2, lineLength, lineWidth);
  painter->fillRect(leftLine, QColor(255, 140, 0));
  
  // Draw right microstrip line (dark orange)
  QRect rightLine(padRadius, -lineWidth/2, lineLength, lineWidth);
  painter->fillRect(rightLine, QColor(255, 140, 0));
  
  // Draw via pad (larger circle) - light orange/copper color
  painter->setBrush(QColor(255, 180, 100));
  painter->setPen(QPen(Qt::black, 1));
  painter->drawEllipse(QPoint(0, 0), padRadius, padRadius);
  
  // Draw via hole (center circle) - darker color to show it's a hole
  painter->setBrush(QColor(150, 150, 150));
  painter->setPen(QPen(Qt::black, 2));
  painter->drawEllipse(QPoint(0, 0), viaRadius, viaRadius);
  
  // Draw connection lines
  painter->setPen(QPen(Qt::black, 1));
  
  // Left connector line
  painter->drawLine(QPoint(-25, 0), QPoint(-lineLength - padRadius, 0));
  
  // Right connector line
  painter->drawLine(QPoint(lineLength + padRadius, 0), QPoint(25, 0));
  
  // Draw microstrip line outlines
  // Left section
  painter->drawLine(QPoint(-lineLength - padRadius, -lineWidth/2), 
                    QPoint(-padRadius, -lineWidth/2));  // top edge
  painter->drawLine(QPoint(-lineLength - padRadius, lineWidth/2), 
                    QPoint(-padRadius, lineWidth/2));   // bottom edge
  painter->drawLine(QPoint(-lineLength - padRadius, -lineWidth/2), 
                    QPoint(-lineLength - padRadius, lineWidth/2));  // left edge
  
  // Right section
  painter->drawLine(QPoint(padRadius, -lineWidth/2), 
                    QPoint(lineLength + padRadius, -lineWidth/2));  // top edge
  painter->drawLine(QPoint(padRadius, lineWidth/2), 
                    QPoint(lineLength + padRadius, lineWidth/2));   // bottom edge
  painter->drawLine(QPoint(lineLength + padRadius, -lineWidth/2), 
                    QPoint(lineLength + padRadius, lineWidth/2));   // right edge
  
  // Draw small "X" marks inside via hole to indicate through-hole connection
  painter->setPen(QPen(Qt::black, 1));
  int xSize = 3;
  painter->drawLine(QPoint(-xSize, -xSize), QPoint(xSize, xSize));
  painter->drawLine(QPoint(-xSize, xSize), QPoint(xSize, -xSize));
  
  // Undo rotation for text
  if (Rotation != 0) {
    painter->rotate(-Rotation);
  }
  
  QPoint OriginText(-15, 15);
  if (Rotation != 0)
    OriginText.setX(-10), OriginText.setY(20);
  
  // Draw ID and diameter parameter text
  painter->drawText(QRect(OriginText, QPoint(100, 100)), QString("%1").arg(this->ID));
  painter->drawText(QRect(OriginText + QPoint(0, 10), QPoint(100, 100)), QString("D=%1").arg(Value["D"]));
}
