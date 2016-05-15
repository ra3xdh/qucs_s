/***************************************************************************
                          sp_nodeset.cpp  -  description
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
#include "sp_nodeset.h"
#include "main.h"

#include <QFontMetrics>

SpiceNodeset::SpiceNodeset()
{
  isEquation = true;
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr(".NODESET section");

  QFont f = QucsSettings.font;
  f.setWeight(QFont::Light);
  f.setPointSizeF(12.0);
  QFontMetrics  metrics(f, 0);  // use the the screen-compatible metric
  QSize r = metrics.size(0, QObject::tr(".NODESET"));
  int xb = r.width()  >> 1;
  int yb = r.height() >> 1;

  Lines.append(new Line(-xb, -yb, -xb,  yb,QPen(Qt::darkRed,2)));
  Lines.append(new Line(-xb,  yb,  xb+3,yb,QPen(Qt::darkRed,2)));
  Texts.append(new Text(-xb+4,  -yb-3, QObject::tr(".NODESET"),
			QColor(0,0,0), 12.0));

  x1 = -xb-3;  y1 = -yb-5;
  x2 =  xb+9; y2 =  yb+3;

  tx = x1+4;
  ty = y2+4;
  Model = "SpiceNodeset";
  Name  = "Nodeset";

  Props.append(new Property("v(node1)", "1", true));
}

SpiceNodeset::~SpiceNodeset()
{
}

Component* SpiceNodeset::newOne()
{
  return new SpiceNodeset();
}

Element* SpiceNodeset::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr(".NODESET Section");
  BitmapFile = (char *) "sp_nodeset";

  if(getNewOne)  return new SpiceNodeset();
  return 0;
}

QString SpiceNodeset::getExpression(bool)
{
    if (isActive != COMP_IS_ACTIVE) return QString("");

    QString s;
    s.clear();
    foreach (Property *pp, Props) {
        s += QString(".NODESET %1 = %2\n").arg(pp->Name).arg(pp->Value);
    }
    return s;
}

