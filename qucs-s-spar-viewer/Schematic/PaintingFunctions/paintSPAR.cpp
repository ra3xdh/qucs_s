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

#include "./../component.h"

void Component::paintSPAR(QPainter* painter) {
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
