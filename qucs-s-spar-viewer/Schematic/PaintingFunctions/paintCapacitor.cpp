/// @file paintCapacitor.cpp
/// @brief Capacitor painting method
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "./../component.h"

///
/// @brief Capacitor painting method
/// \param painter Painter object
///
void Component::paintCapacitor(QPainter *painter) {

  if (Rotation != 0) {
    painter->rotate(Rotation);
  }

  // The following lines draw the capacitor in a vertical position
  // Lines from ports to parallel plates
  painter->drawLine(QPoint(0, 25), QPoint(0, 4));
  painter->drawLine(QPoint(0, -25), QPoint(0, -4));

  // Parallel plates
  painter->drawLine(QPoint(-10, -4), QPoint(10, -4));
  painter->drawLine(QPoint(-10, 4), QPoint(10, 4));
  painter->setPen(QPen(Qt::black, 1));

  if (Rotation != 0) { // The rotation is undone to draw the text
    painter->rotate(-Rotation);
  }

  QPoint OriginText(2, 5);
  if (Rotation != 0) {
    OriginText.setX(-10), OriginText.setY(10);
  }

  painter->drawText(QRect(OriginText, QPoint(100, 100)),
                    QString("%1").arg(this->ID));
  painter->drawText(QRect(OriginText + QPoint(0, 10), QPoint(100, 100)),
                    QString("%1").arg(Value["C"]));
}
