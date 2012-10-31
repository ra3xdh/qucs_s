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

#include "viewpainter.h"
#include "element.h"
#include "components/component.h"    // because of struct Port
#include "wirelabel.h"

#include <qpainter.h>
#include <qstring.h>
#include <q3ptrlist.h>


class Wire : public Conductor {
public:
  Wire(int _x1=0, int _y1=0, int _x2=0, int _y2=0, Node *n1=0, Node *n2=0);
 ~Wire();

  void paint(ViewPainter*);
  void paintScheme(QPainter*);
  void setCenter(int, int, bool relative=false);
  void getCenter(int&, int&);
  bool getSelected(int, int);
  void setName(const QString&, const QString&, int delta_=0, int x_=0, int y_=0);

  Node      *Port1, *Port2;

  void    rotate();
  QString save();
  bool    load(const QString&);
  bool    isHorizontal();
};

#endif
