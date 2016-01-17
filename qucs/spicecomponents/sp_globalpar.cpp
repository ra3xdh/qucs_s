/***************************************************************************
                          equation.cpp  -  description
                             -------------------
    begin                : Sat Aug 23 2003
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
#include "sp_globalpar.h"
#include "main.h"

#include <QFontMetrics>

SpiceGlobalParam::SpiceGlobalParam()
{
  isEquation = true;
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr(".GLOBAL_PARAM section");

  QFont f = QucsSettings.font;
  f.setWeight(QFont::Light);
  f.setPointSizeF(12.0);
  QFontMetrics  metrics(f, 0);  // use the the screen-compatible metric
  QSize r = metrics.size(0, QObject::tr(".GLOBAL_PARAM"));
  int xb = r.width()  >> 1;
  int yb = r.height() >> 1;

  Lines.append(new Line(-xb, -yb, -xb,  yb,QPen(Qt::darkRed,2)));
  Lines.append(new Line(-xb,  yb,  xb+3,yb,QPen(Qt::darkRed,2)));
  Texts.append(new Text(-xb+4,  -yb-3, QObject::tr(".GLOBAL PARAM"),
			QColor(0,0,0), 12.0));

  x1 = -xb-3;  y1 = -yb-5;
  x2 =  xb+9; y2 =  yb+3;

  tx = x1+4;
  ty = y2+4;
  Model = "SpGlobPar";
  Name  = "SpGlobPar";

  Props.append(new Property("y", "1", true));
}

SpiceGlobalParam::~SpiceGlobalParam()
{
}

Component* SpiceGlobalParam::newOne()
{
  return new SpiceGlobalParam();
}

Element* SpiceGlobalParam::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr(".GLOBAL_PARAM Section");
  BitmapFile = (char *) "sp_globpar";

  if(getNewOne)  return new SpiceGlobalParam();
  return 0;
}


QString SpiceGlobalParam::getExpression(bool)
{
    if (isActive != COMP_IS_ACTIVE) return QString("");

    QString s;
    s.clear();
    foreach (Property *pp, Props) {
        s += QString(".GLOBAL_PARAM %1 = %2\n").arg(pp->Name).arg(pp->Value);
    }
    return s;
}

