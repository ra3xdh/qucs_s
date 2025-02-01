/***************************************************************************
                                wirelabel.h
                               -------------
    begin                : Sun Feb 29 2004
    copyright            : (C) 2004 by Michael Margraf
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

#ifndef WIRELABEL_H
#define WIRELABEL_H

#include "conductor.h"

#include <QString>

class QPainter;


class WireLabel : public Element {
public:
  WireLabel(const QString& _Name=0, int _cx=0, int _cy=0,
            int _x1=0, int _y1=0, int _Type=isNodeLabel);

  bool getSelected(int, int);
  void setName(const QString& Name_);
  void setHighlighted (bool newval) { isHighlighted = newval; };


  Conductor* pOwner = nullptr;  // Wire or Node where label belongs to
  QString Name = "";
  QString initValue = "";

  void    paint(QPainter* painter) const;

  QString save();
  bool    load(const QString& s);
  bool    isHorizontal();
  void    getLabelBounding(int& _xmin, int& _ymin, int& _xmax, int& _ymax);

  /** Returns coordinates of a point where label sticks its stem into conductor */
  QPoint root() const noexcept;

  /** Moves root by dx and dy */
  void moveRoot(int dx, int dy) noexcept;

  /** Moves root to (x, y) */
  void moveRootTo(int x, int y) noexcept;

  /** Returns coordinates of label text center */
  QPoint center() const noexcept override;

  /** Moves label text center by dx and dy */
  void moveCenter(int dx, int dy) noexcept override;

  /** Rotates label around its root */
  void rotate() noexcept override;

  /** Same as rotate() */
  void rotate(int /*rcx*/, int /*rcy*/) noexcept override { rotate(); }

  /** Mirrors label vertically relative to its root */
  void mirrorX() noexcept override;

  /** Mirrors label horizontally relative to its root */
  void mirrorY() noexcept override;

  /** Same as mirrorX() */
   void mirrorX(int /*axis*/) noexcept override { mirrorX(); }

  /** Same as mirrorY() */
  void mirrorY(int /*axis*/) noexcept override { mirrorY(); }

  QRect boundingRect() const noexcept override;

private:
  bool isHighlighted = false;
  QSize textSize;
};

#endif
