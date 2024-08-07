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

#include <QPen>

namespace qucs {

class Rectangle : public Painting  {
public:
  Rectangle(bool _filled=false);
 ~Rectangle();

  void paintScheme(Schematic*);
  void getCenter(int&, int&);
  void setCenter(int, int, bool relative=false);

  Painting* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  static Element* info_filled(QString&, char* &, bool getNewOne=false);
  bool load(const QString&);
  QString save();
  QString saveCpp();
  QString saveJSON();
  void paint(QPainter* painter);
  void MouseMoving(Schematic*, int, int, int, int, Schematic*, int, int);
  bool MousePressing(Schematic *sch = 0);
  bool getSelected(float, float, float);
  bool resizeTouched(float, float, float);
  void MouseResizeMoving(int, int, Schematic*);

  void rotate(int, int);
  void mirrorX();
  void mirrorY();
  bool Dialog(QWidget *parent = 0);

  QPen  Pen;
  QBrush Brush;    // filling style/color
  bool  filled;    // filled or not (for "getSelected" etc.)
};

}

#endif
