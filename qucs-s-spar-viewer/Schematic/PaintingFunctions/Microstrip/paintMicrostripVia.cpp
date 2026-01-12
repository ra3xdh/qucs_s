/// @file paintMicrostripVia.cpp
/// @brief Microstrip via painting method
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "./../../component.h"

///
/// @brief Microstrip via painting method
/// \param painter Painter object
///
void Component::paintMicrostripVia(QPainter *painter) {
  if (Rotation != 0) {
    painter->rotate(Rotation);
  }

  // Define dimensions - similar to GND symbol but with orange rectangle
  int rectWidth = 8;        // Width of the vertical orange rectangle
  int rectHeight = 10;      // Height of the vertical orange rectangle
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
