/***************************************************************************
                               ellipsearc.h
                              --------------
    begin                : Thu Sep 9 2004
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

#ifndef ELLIPSEARC_H
#define ELLIPSEARC_H

#include "painting.h"

#include <QPen>


class EllipseArc : public Painting  {
public:
  EllipseArc();
 ~EllipseArc();

  void paintScheme(Schematic*);
  void getCenter(int&, int&);
  void setCenter(int, int, bool relative=false);

  Painting* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  bool load(const QString&);
  QString save();
  QString saveCpp();
  QString saveJSON();
  void paint(ViewPainter*);
  void MouseMoving(Schematic*, int, int, int, int, Schematic*, int, int, bool);
  bool MousePressing(Schematic *sch = 0);
  bool getSelected(float, float, float);
  bool resizeTouched(float, float, float);
  void MouseResizeMoving(int, int, Schematic*);

  void rotate(int, int);
  void mirrorX();
  void mirrorY();
  bool Dialog(QWidget *parent = 0);

  QPen  Pen;
  int   Angle, ArcLen;   // start angle and arc length
};

#endif
