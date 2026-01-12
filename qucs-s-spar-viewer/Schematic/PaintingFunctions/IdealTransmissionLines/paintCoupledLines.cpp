/// @file paintCoupledLines.cpp
/// @brief Coupled lines painting method
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "./../../component.h"

///
/// @brief Coupled transmission lines painting method
/// \param painter Painter object
///
void Component::paintCoupledLines(QPainter *painter) {

  if (Rotation != 0) {
    painter->rotate(Rotation);
  }

  int w = 15, shiftx = 10;

  // The coupled lines painting code is the same as the transmission line code,
  // but it is shifted by shiftx
  painter->drawLine(QPoint(-shiftx, -25), QPoint(-shiftx, -14));
  painter->drawLine(QPoint(-shiftx - 0.5 * w, -14),
                    QPoint(-shiftx + 0.5 * w, -14));
  painter->drawLine(QPoint(-shiftx - 0.5 * w, -14),
                    QPoint(-shiftx + 0.5 * w, -14));
  painter->drawLine(QPoint(-shiftx - 0.5 * w, -14),
                    QPoint(-shiftx - 0.5 * w, 16));
  painter->drawLine(QPoint(-shiftx + 0.5 * w, -14),
                    QPoint(-shiftx + 0.5 * w, 16));
  painter->drawLine(QPoint(-shiftx - 0.5 * w, 16),
                    QPoint(-shiftx + 0.5 * w, 16));
  painter->drawLine(QPoint(-shiftx, 16), QPoint(-shiftx, 25));

  painter->drawLine(QPoint(shiftx, -25), QPoint(shiftx, -14));
  painter->drawLine(QPoint(shiftx - 0.5 * w, -14),
                    QPoint(shiftx + 0.5 * w, -14));
  painter->drawLine(QPoint(shiftx - 0.5 * w, -14),
                    QPoint(shiftx + 0.5 * w, -14));
  painter->drawLine(QPoint(shiftx - 0.5 * w, -14),
                    QPoint(shiftx - 0.5 * w, 16));
  painter->drawLine(QPoint(shiftx + 0.5 * w, -14),
                    QPoint(shiftx + 0.5 * w, 16));
  painter->drawLine(QPoint(shiftx - 0.5 * w, 16), QPoint(shiftx + 0.5 * w, 16));
  painter->drawLine(QPoint(shiftx, 16), QPoint(shiftx, 25));
  painter->setPen(QPen(Qt::black, 1));

  if (Rotation != 0) { // The rotation is undone to draw the text
    painter->rotate(-Rotation);
  }

  QPoint OriginText(20, -10);
  if (Rotation != 0) {
    OriginText.setX(-20), OriginText.setY(20);
  }

  painter->drawText(QRect(OriginText, QPoint(100, 100)),
                    QString("%1").arg(this->ID));
  painter->drawText(
      QRect(OriginText + QPoint(0, 10), QPoint(100, 100)),
      QString("%1").arg(Value["Ze"].replace("Ohm", QChar(0xa9, 0x03))));
  painter->drawText(
      QRect(OriginText + QPoint(0, 20), QPoint(100, 100)),
      QString("%1").arg(Value["Zo"].replace("Ohm", QChar(0xa9, 0x03))));
  painter->drawText(QRect(OriginText + QPoint(0, 30), QPoint(100, 100)),
                    QString("%1").arg(Value["Length"]));
}
