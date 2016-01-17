/***************************************************************************
                          sp_nutmeg.cpp  -  description
                             -------------------
    begin                : Fri May 22 2015
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
#include "sp_nutmeg.h"
#include "main.h"

#include <QFontMetrics>

NutmegEquation::NutmegEquation()
{
  isEquation = true;
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr("Nutmeg equation");

  QFont f = QucsSettings.font;
  f.setWeight(QFont::Light);
  f.setPointSizeF(12.0);
  QFontMetrics  metrics(f, 0);  // use the the screen-compatible metric
  QSize r = metrics.size(0, QObject::tr("Nutmeg"));
  int xb = r.width()  >> 1;
  int yb = r.height() >> 1;

  Lines.append(new Line(-xb, -yb, -xb,  yb,QPen(Qt::blue,2)));
  Lines.append(new Line(-xb,  yb,  xb+3,yb,QPen(Qt::blue,2)));
  Texts.append(new Text(-xb+4,  -yb-3, QObject::tr("Nutmeg"),
			QColor(0,0,0), 12.0));

  x1 = -xb-3;  y1 = -yb-5;
  x2 =  xb+9; y2 =  yb+3;

  tx = x1+4;
  ty = y2+4;
  Model = "NutmegEq";
  SpiceModel = "NutmegEq";
  Name  = "NutmegEq";

  Props.append(new Property("Simulation","ac",true,
                            "Used simulation [ac, tran, dc, disto, all]"));
  Props.append(new Property("y", "1", true));
}

NutmegEquation::~NutmegEquation()
{
}

Component* NutmegEquation::newOne()
{
  return new NutmegEquation();
}

Element* NutmegEquation::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Nutmeg Equation");
  BitmapFile = (char *) "sp_nutmeg";

  if(getNewOne)  return new NutmegEquation();
  return 0;
}

QString NutmegEquation::getEquations(QString sim, QStringList &dep_vars)
{
    if (isActive != COMP_IS_ACTIVE) return QString("");

    QString s;
    s.clear();
    if (Props.at(0)->Value==sim) {
        Property *pp = Props.first();
        pp = Props.next();
        for (;pp!=0;pp=Props.next()) {
            s += QString("let %1 = %2\n").arg(pp->Name).arg(pp->Value);
            dep_vars.append(pp->Name);
        }
    }
    return s;
}

