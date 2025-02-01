/***************************************************************************
                                painting.h
                               ------------
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

#ifndef PAINTING_H
#define PAINTING_H

#include "element.h"


class Painting : public Element  {
public:
  Painting() { Type = isPainting; }
 ~Painting() override = default;

  virtual void paint(QPainter* p) = 0;
  virtual Painting* newOne() = 0;

  virtual bool load(const QString& /*input*/) = 0;
  virtual QString save() = 0;
  virtual QString saveCpp() = 0;
  virtual QString saveJSON() = 0;

  virtual bool getSelected(const QPoint& /*click*/, int /*tolerance*/) { return false; };
  virtual bool resizeTouched(const QPoint& /*click*/, int /*tolerance*/) { return false; };

  virtual void MouseMoving(const QPoint& /*onGrid*/, Schematic* /*sch*/, const QPoint& /*cursor*/) {};
  virtual bool MousePressing(Schematic* sch = nullptr) { Q_UNUSED(sch) return false; };

  virtual void MouseResizeMoving(int, int, Schematic*) {};

  void  moveCenterTo(int x, int y) noexcept override;
  void  moveCenter(int dx, int dy) noexcept override;

  QRect boundingRect() const noexcept override;

  virtual bool Dialog(QWidget* parent = nullptr) { Q_UNUSED(parent) return false; };
  QString Name; // name of painting, e.g. for saving

protected:
  QString toPenString (int);
  QString toBrushString (int);
  void updateCenter() noexcept;
  virtual void afterMove([[maybe_unused]] int dx, [[maybe_unused]] int dy) noexcept {};
};

#endif
