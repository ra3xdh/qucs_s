/// @file paintGND.cpp
/// @brief Ground painting method
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "./../component.h"

///
/// @brief Ground painting method
/// \param painter Painter object
///
void Component::paintGND(QPainter *painter) {
  if (Rotation != 0) {
    painter->rotate(Rotation);
  }

  painter->drawLine(QPoint(0, -10), QPoint(0, 0));
  painter->drawLine(QPoint(-10, 0), QPoint(10, 0));

  if (Rotation != 0) { // The rotation is undone to draw the text
    painter->rotate(-Rotation);
  }
}
