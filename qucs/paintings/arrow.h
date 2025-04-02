/***************************************************************************
                                arrow.h
                               ---------
    begin                : Sun Nov 23 2003
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

#ifndef ARROW_H
#define ARROW_H

#include "painting.h"

#include <QPen>


class Arrow : public Painting  {
public:
  Arrow();

  void paint(QPainter* painter) override;
  void paintScheme(Schematic*) override;

  Painting* newOne() override;
  static Element* info(QString&, char* &, bool getNewOne=false);

  bool load(const QString&) override;
  QString save() override;
  QString saveCpp() override;
  QString saveJSON() override;

  bool getSelected(const QPoint& click, int tolerance) override;
  bool resizeTouched(const QPoint& click, int tolerance) override;

  void MouseMoving(const QPoint& onGrid, Schematic* sch, const QPoint& cursor) override;
  bool MousePressing(Schematic *sch = nullptr) override;
  void MouseResizeMoving(int, int, Schematic*) override;

  bool  rotate() noexcept override;
  bool  rotate(int, int) noexcept override;
  bool  mirrorX() noexcept override;
  bool  mirrorY() noexcept override;

  bool Dialog(QWidget* parent = nullptr) override;

private:
  QPen   pen;

  enum ArrowHeadStyle { empty = 0, filled };
  ArrowHeadStyle headStyle;

  // size of the arrow head
  double headHeight, headWidth;
  // calculated from height and width
  double headWingLength, headAngle;

  QPoint headLeftWing;
  QPoint headRightWing;

  // Helps in handling of mouse movements while resiging the line
  enum class State { idle, moving_head, moving_tail };
  State arrowState = State::idle;

  // Helps distinguish first and second mouse click while
  // drawing a line
  bool isBeingDrawn = false;

  // Calculates and updates coordinates of arrow wings
  void updateHead() noexcept;

protected:
  void afterMove(int /*dx*/, int /*dy*/) noexcept override { updateHead(); }
};

#endif
