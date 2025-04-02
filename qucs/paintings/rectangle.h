/***************************************************************************
                               rectangle.h
                              -------------
    begin                : Sat Nov 22 2003
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

#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "painting.h"


namespace qucs {

class Rectangle : public Painting  {
public:
  Rectangle(bool _filled=false);

  void paint(QPainter* painter) override;
  void paintScheme(Schematic*) override;

  Painting* newOne() override;
  static Element* info(QString&, char* &, bool getNewOne=false);
  static Element* info_filled(QString&, char* &, bool getNewOne=false);

  bool load(const QString&) override;
  QString save() override;
  QString saveCpp() override;
  QString saveJSON() override;

  bool getSelected(const QPoint& click, int tolerance) override;
  bool resizeTouched(const QPoint& click, int tolerance) override;

  void MouseMoving(const QPoint& onGrid, Schematic* sch, const QPoint& cursor) override;
  bool MousePressing(Schematic* sch = nullptr) override;
  void MouseResizeMoving(int, int, Schematic*) override;

  bool rotate() noexcept override;
  bool rotate(int, int) noexcept override;

  bool Dialog(QWidget* parent = nullptr) override;

private:
  enum class State { idle, moving_top_left, moving_top_right, moving_bottom_right, moving_bottom_left };
  State resizeState = State::idle;

  QPen  pen;
  QBrush brush;    // filling style/color
  bool  filled;    // filled or not (for "getSelected" etc.)

  bool  isBeingDrawn = false;

  void   normalize() noexcept {
    if (x2 < x1) std::swap(x1, x2);
    if (y2 < y1) std::swap(y1, y2);
  }
};

}

#endif
