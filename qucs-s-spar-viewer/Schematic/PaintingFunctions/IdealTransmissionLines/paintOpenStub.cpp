/// @file paintOpenStub.cpp
/// @brief Open stub painting method
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "./../../component.h"

///
/// @brief Open stub painting method
/// \param painter Painter object
///
void Component::paintOpenStub(QPainter *painter) {
  if (Rotation != 0) {
    painter->rotate(Rotation);
  }
  int w = 15;
  painter->drawLine(QPoint(0, -30), QPoint(0, -14));
  painter->drawLine(QPoint(-0.5 * w, -14), QPoint(0.5 * w, -14));
  painter->drawLine(QPoint(-0.5 * w, -14), QPoint(0.5 * w, -14));
  painter->drawLine(QPoint(-0.5 * w, -14), QPoint(-0.5 * w, 16));
  painter->drawLine(QPoint(0.5 * w, -14), QPoint(0.5 * w, 16));
  painter->drawLine(QPoint(-0.5 * w, 16), QPoint(0.5 * w, 16));
  painter->setPen(QPen(Qt::black, 1));

  if (Rotation != 0) { // The rotation is undone to draw the text
    painter->rotate(-Rotation);
  }

  QPoint OriginText(10, -10);
  if (Rotation != 0) {
    OriginText.setX(-10), OriginText.setY(10);
  }

  painter->drawText(QRect(OriginText, QPoint(100, 100)),
                    QString("%1").arg(this->ID));
  painter->drawText(
      QRect(OriginText + QPoint(0, 10), QPoint(100, 100)),
      QString("%1").arg(Value["Z0"].replace("Ohm", QChar(0xa9, 0x03))));
  painter->drawText(QRect(OriginText + QPoint(0, 20), QPoint(100, 100)),
                    QString("%1").arg(Value["Length"]));
}
