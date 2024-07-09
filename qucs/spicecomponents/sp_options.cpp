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
#include "sp_options.h"
#include "main.h"

#include <QFontInfo>
#include <QFontMetrics>

SpiceOptions::SpiceOptions()
{
  isEquation = true;
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr(".OPTIONS section");
  Simulator = spicecompat::simSpice;

  QFont f = QucsSettings.font;
  f.setWeight(QFont::Light);
  f.setPointSizeF(12.0);
  QFontMetrics  metrics(f, 0);  // use the the screen-compatible metric
  QSize r = metrics.size(0, QObject::tr(".OPTIONS"));
  int xb = r.width()  >> 1;
  int yb = r.height() >> 1;

  Lines.emplace_back( qucs::Line(-xb, -yb, -xb,  yb,QPen(Qt::darkRed,2)));
  Lines.emplace_back( qucs::Line(-xb,  yb,  xb+3,yb,QPen(Qt::darkRed,2)));
  Texts.emplace_back( Text(-xb+4,  -yb-3, QObject::tr(".OPTIONS"),
			QColor(0,0,0), QFontInfo(f).pixelSize()));

  x1 = -xb-3;  y1 = -yb-5;
  x2 =  xb+9; y2 =  yb+3;

  tx = x1+4;
  ty = y2+4;
  Model = "SpiceOptions";
  Name  = "SpiceOptions";

  Props.emplace_back( Property("XyceOptionPackage", "DEVICE", false,
        QObject::tr("Xyce option package name")));
  Props.emplace_back( Property("GMIN", "1e-12", true));
}

SpiceOptions::~SpiceOptions()
{
}

Component* SpiceOptions::newOne()
{
  return new SpiceOptions();
}

Element* SpiceOptions::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr(".OPTIONS Section");
  BitmapFile = (char *) "sp_options";

  if(getNewOne)  return new SpiceOptions();
  return 0;
}

QString SpiceOptions::getExpression(bool isXyce)
{
    if (isActive != COMP_IS_ACTIVE) return QString("");

    QString s;
    s.clear();
    if (isXyce) {
        s += QString(".OPTIONS %1 ").arg(prop(0).Value);
        for (unsigned int i=1;i<Props.size();i++) {
            s += QString(" %1 = %2 ").arg(prop(i).Name).arg(prop(i).Value);
        }
        s += "\n";
    } else {
        for (unsigned int i=1;i<Props.size();i++) {
            s += QString(".OPTION %1 = %2\n").arg(prop(i).Name).arg(prop(i).Value);
        }
    }
    return s;
}

