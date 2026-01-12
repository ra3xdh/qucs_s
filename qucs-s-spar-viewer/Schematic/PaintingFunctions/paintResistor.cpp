/// @file paintResistor.cpp
/// @brief Resistor painting method
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "./../component.h"

///
/// @brief Resistor painting method
/// \param painter Painter object
///
void Component::paintResistor(QPainter *painter) {

  if (Rotation != 0) {
    painter->rotate(Rotation);
  }

  int w = 5;
  painter->drawLine(QPoint(0, -25), QPoint(0, -14));
  painter->drawLine(QPoint(0, -14), QPoint(w, -14 + 2.5));
  painter->drawLine(QPoint(w, -14 + 2.5), QPoint(-w, -14 + 7.5));
  painter->drawLine(QPoint(-w, -14 + 7.5), QPoint(w, -14 + 12.5));
  painter->drawLine(QPoint(w, -14 + 12.5), QPoint(-w, -14 + 17.5));
  painter->drawLine(QPoint(-w, -14 + 17.5), QPoint(w, -14 + 22.5));
  painter->drawLine(QPoint(w, -14 + 22.5), QPoint(-w, -14 + 27.5));
  painter->drawLine(QPoint(-w, -14 + 27.5), QPoint(0, 16));
  painter->drawLine(QPoint(0, 16), QPoint(0, 25));

  QPoint OriginText(10, -10);

  if (Rotation != 0) {
    painter->rotate(-Rotation);
    OriginText.setX(-10), OriginText.setY(10);
  }

  painter->setPen(QPen(Qt::black, 1));
  painter->drawText(QRect(OriginText, QPoint(100, 100)),
                    QString("%1").arg(this->ID));
  painter->drawText(
      QRect(OriginText + QPoint(0, 10), QPoint(100, 100)),
      QString("%1").arg(Value["R"].replace("Ohm", QChar(0xa9, 0x03))));
}
