/***************************************************************************
                          sp_ic.cpp  -  description
                             -------------------
    begin                : Mon May 25 2015
    copyright            : (C) 2015 by Vadim Kuznetsov
    email                : ra3xdh@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "sp_ic.h"
#include "main.h"

#include <QFontMetrics>

SpiceIC::SpiceIC()
{
  isEquation = true;
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr(".IC section");

  QFont f = QucsSettings.font;
  f.setWeight(QFont::Light);
  f.setPointSizeF(12.0);
  QFontMetrics  metrics(f, 0);  // use the the screen-compatible metric
  QSize r = metrics.size(0, QObject::tr(".IC"));
  int xb = r.width()  >> 1;
  int yb = r.height() >> 1;

  Lines.append(new Line(-xb, -yb, -xb,  yb,QPen(Qt::darkRed,2)));
  Lines.append(new Line(-xb,  yb,  xb+3,yb,QPen(Qt::darkRed,2)));
  Texts.append(new Text(-xb+4,  -yb-3, QObject::tr(".IC"),
			QColor(0,0,0), 12.0));

  x1 = -xb-3;  y1 = -yb-5;
  x2 =  xb+9; y2 =  yb+3;

  tx = x1+4;
  ty = y2+4;
  Model = "SpiceIC";
  Name  = "SpiceIC";

  Props.append(new Property("v(node1)", "1", true));
}

SpiceIC::~SpiceIC()
{
}

Component* SpiceIC::newOne()
{
  return new SpiceIC();
}

Element* SpiceIC::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr(".IC Section");
  BitmapFile = (char *) "sp_ic";

  if(getNewOne)  return new SpiceIC();
  return 0;
}

QString SpiceIC::getExpression(bool)
{
    if (isActive != COMP_IS_ACTIVE) return QString("");

    QString s;
    s.clear();
    foreach (Property *pp, Props) {
        s += QString(".IC %1 = %2\n").arg(pp->Name).arg(pp->Value);
    }
    return s;
}

