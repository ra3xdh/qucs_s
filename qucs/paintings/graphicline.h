/***************************************************************************
                               graphicline.h
                              ---------------
    begin                : Mon Nov 24 2003
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

#ifndef GRAPHICLINE_H
#define GRAPHICLINE_H

#include "painting.h"


class GraphicLine : public Painting  {
public:
  GraphicLine(int ax = 0, int ay = 0, int bx = 0, int by = 0,
              QPen pen = QPen(QColor()));

  void paint(QPainter* painter) override;
  void paintScheme(Schematic*) override;

  Painting* newOne() override;
  static Element* info(QString&, char* &, bool getNewOne=false);

  bool    load(const QString&) override;
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
  bool mirrorX() noexcept override;
  bool mirrorY() noexcept override;

  bool Dialog(QWidget* parent = nullptr) override;

private:
  QPen pen;

  // Helps in handling of mouse movements while resiging the line
  enum class State { Idle, Moving_End1, Moving_End2 };
  State lineState = State::Idle;

  // Helps distinguish first and second mouse click while
  // drawing a line
  bool isBeingDrawn = false;
};

#endif
