/// @file paintInductor.cpp
/// @brief Inductor painting method
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "./../component.h"

///
/// @brief Inductor painting method
/// \param painter Painter object
///
void Component::paintInductor(QPainter *painter) {
  if (Rotation != 0) {
    painter->rotate(Rotation);
  }

  // The following lines draw the capacitor in a vertical position
  painter->drawLine(QPoint(0, -25), QPoint(0, -2 * 7));
  painter->drawArc(QRect(-2 * 2, -2 * 7, 2 * 5.0, 2 * 5.0), -90 * 16, 180 * 16);
  painter->drawArc(QRect(-2 * 2, -2 * 2, 2 * 5.0, 2 * 5.0), -90 * 16, 180 * 16);
  painter->drawArc(QRect(-2 * 2, 2 * 3, 2 * 5.0, 2 * 5.0), -90 * 16, 180 * 16);
  painter->drawLine(QPoint(0, 16), QPoint(0, 25));
  painter->setPen(QPen(Qt::black, 1));

  if (Rotation != 0) { // The rotation is undone to draw the text
    painter->rotate(-Rotation);
  }

  QPoint OriginText(10, -10);
  if (Rotation != 0) {
    OriginText.setX(-10), OriginText.setY(5);
  }

  painter->drawText(QRect(OriginText, QPoint(100, 100)),
                    QString("%1").arg(this->ID));
  painter->drawText(QRect(OriginText + QPoint(0, +10), QPoint(100, 100)),
                    QString("%1").arg(Value["L"]));
}
