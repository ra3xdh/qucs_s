/// @file paintSPAR.cpp
/// @brief S-parameter block painting method
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "./../component.h"

///
/// @brief S-parameter block painting method
/// \param painter Painter object
///
void Component::paintSPAR(QPainter *painter) {
  painter->save();
  if (Rotation != 0) {
    painter->rotate(Rotation);
  }
  // Draw rectangle for two-port device
  QRect rect(-15, -15, 30, 30);
  painter->setPen(QPen(Qt::black, 1));
  painter->drawRect(rect);
  // Terms
  painter->drawLine(QPoint(-20, 0), QPoint(-15, 0));
  painter->drawLine(QPoint(15, 0), QPoint(20, 0));
  painter->restore(); // Restore to unrotated state

  // Draw big red "[S]" in the center (unrotated)
  QFont font = painter->font();
  font.setBold(true);
  font.setPointSize(10);
  painter->setFont(font);
  painter->setPen(QPen(Qt::red, 2));
  painter->drawText(rect, Qt::AlignCenter, "[S]");

  // Restore pen for further drawing
  painter->setPen(QPen(Qt::black, 1));
}
