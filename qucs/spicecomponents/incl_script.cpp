/***************************************************************************
                              xyce_script.cpp
                               ------------
    begin                : Wed Jun 22 2016
    copyright            : (C) 2016 by Vadim Kuznetsov
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
#include "incl_script.h"
#include "main.h"
#include <QFontMetrics>


InclScript::InclScript()
{
    isEquation = false;
    Type = isComponent; // Analogue and digital component.
    Description = QObject::tr("Include script before simulation");
    Simulator = spicecompat::simSpice;

    QFont f = QucsSettings.font;
    f.setWeight(QFont::Light);
    f.setPointSizeF(12.0);
    QFontMetrics  metrics(f, 0);  // use the the screen-compatible metric
    QSize r = metrics.size(0, QObject::tr(".INCLUDE SCRIPT"));
    int xb = r.width()  >> 1;
    int yb = r.height() >> 1;

    Lines.append(new qucs::Line(-xb, -yb, -xb,  yb,QPen(Qt::darkRed,2)));
    Lines.append(new qucs::Line(-xb,  yb,  xb+3,yb,QPen(Qt::darkRed,2)));
    Texts.append(new Text(-xb+4,  -yb-3, QObject::tr(".INCLUDE SCRIPT"),
                          QColor(0,0,0), 12.0));

    x1 = -xb-3;  y1 = -yb-5;
    x2 =  xb+9; y2 =  yb+3;

    tx = x1+4;
    ty = y2+4;
    Model = "INCLSCR";
    Name  = "INCLSCR";
    SpiceModel = "INCLSCR";

    // The index of the first 4 properties must not changed. Used in recreate().
    Props.append(new Property("SpiceCode", "\n"
                              ".PARAM rp = 1k\n"
                              ".FUNC prod(x,y) = {x*y}", true,
                              "Insert spice code here"));
    Props.append(new Property("","",false,"Extra property"));
    Props.append(new Property("","",false,"Extra property"));

}

InclScript::~InclScript()
{
}

Component* InclScript::newOne()
{
  return new InclScript();
}

Element* InclScript::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Include script");
  BitmapFile = (char *) "incl_script";

  if(getNewOne)  return new InclScript();
  return 0;
}

QString InclScript::getExpression(bool)
{
    return Props.at(0)->Value+"\n";
}
