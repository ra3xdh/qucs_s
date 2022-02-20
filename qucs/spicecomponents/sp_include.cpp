/***************************************************************************
                          sp_include.cpp  -  description
                             -------------------
    begin                : Mon Dec 07 2015
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
#include "sp_include.h"
#include "main.h"

#include <QFontMetrics>

S4Q_Include::S4Q_Include()
{
  isEquation = false;
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr(".INCLUDE statement\n");

  QFont f = QucsSettings.font;
  f.setWeight(QFont::Light);
  f.setPointSizeF(12.0);
  QFontMetrics  metrics(f, 0);  // use the the screen-compatible metric
  QSize r = metrics.size(0, QObject::tr(".INCLUDE"));
  int xb = r.width()  >> 1;
  int yb = r.height() >> 1;

  Lines.append(new qucs::Line(-xb, -yb, -xb,  yb,QPen(Qt::darkRed,2)));
  Lines.append(new qucs::Line(-xb,  yb,  xb+3,yb,QPen(Qt::darkRed,2)));
  Texts.append(new Text(-xb+4,  -yb-3, QObject::tr(".INCLUDE"),
			QColor(0,0,0), 12.0));

  x1 = -xb-3;  y1 = -yb-5;
  x2 =  xb+9; y2 =  yb+3;

  tx = x1+4;
  ty = y2+4;
  Model = "SpiceInclude";
  Name  = "SpiceInclude";
  SpiceModel = ".INCLUDE";

  Props.append(new Property("File", "~/home/user/library.inc", true,"SPICE file to include"));
  Props.append(new Property("File", "", false,"SPICE file to include"));
  Props.append(new Property("File", "", false,"SPICE file to include"));
  Props.append(new Property("File", "", false,"SPICE file to include"));
  Props.append(new Property("File", "", false,"SPICE file to include"));
}

S4Q_Include::~S4Q_Include()
{
}

Component* S4Q_Include::newOne()
{
  return new S4Q_Include();
}

Element* S4Q_Include::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr(".INCLUDE statement");
  BitmapFile = (char *) "sp_include";

  if(getNewOne)  return new S4Q_Include();
  return 0;
}

QString S4Q_Include::getSpiceModel()
{
    if (isActive != COMP_IS_ACTIVE) return QString("");
    QString s;
    s.clear();

    foreach (Property *pp, Props) {
        QString val = pp->Value;
        if (!val.isEmpty()) {
            val = spicecompat::convert_relative_filename(val);
            switch (QucsSettings.DefaultSimulator) {
            case spicecompat::simSpiceOpus: // Spice Opus doesn't support quotes
                s += QString("%1 %2\n").arg(SpiceModel).arg(val);
                break;
            default:
                s += QString("%1 \"%2\"\n").arg(SpiceModel).arg(val);
                break;
            }
        }
    }

    return s;
}

