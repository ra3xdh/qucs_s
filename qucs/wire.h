/***************************************************************************
                                  wire.h
                                 --------
    begin                : Wed Sep 3 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef WIRE_H
#define WIRE_H

#include "conductor.h"
#include "wirelabel.h"

class Schematic;
class QPainter;
class QString;


class Wire : public Conductor {
public:
  Wire(int _x1=0, int _y1=0, int _x2=0, int _y2=0, Node *n1=0, Node *n2=0);
 ~Wire() override;

  void paint(QPainter* painter) const;
  void paintScheme(Schematic* sch) override;

  bool getSelected(int, int);
  void setName(int distFromPort1, int text_x, int text_y, const QString&, const QString&);
  void setName(const QString&, const QString&, int root_x=0, int root_y=0, int x_=0, int y_=0);

  Node *Port1, *Port2;

  void rotate() noexcept override;

  void mirrorX() noexcept override { std::swap(y1, y2); }
  void mirrorY() noexcept override { std::swap(x1, x2); }

  QRect boundingRect() const noexcept override;

  void moveCenter(int dx, int dy) noexcept override;

  QString save();
  bool    load(const QString&);
  bool    isHorizontal();

  void setP1(const QPoint& p);
  void setP2(const QPoint& p);
};

#endif
