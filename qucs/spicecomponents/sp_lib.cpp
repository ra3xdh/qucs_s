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
#include "sp_lib.h"
#include "main.h"
#include "misc.h"
#include <QFontMetrics>

S4Q_Lib::S4Q_Lib()
{
  isEquation = false;
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr(".LIB directive\n");
  Simulator = spicecompat::simSpice;

  QFont f = QucsSettings.font;
  f.setWeight(QFont::Light);
  f.setPointSizeF(12.0);
  QFontMetrics  metrics(f, 0);  // use the the screen-compatible metric
  QSize r = metrics.size(0, QObject::tr(".LIB"));
  int xb = r.width()  >> 1;
  int yb = r.height() >> 1;

  Lines.append(new qucs::Line(-xb, -yb, -xb,  yb,QPen(Qt::darkRed,2)));
  Lines.append(new qucs::Line(-xb,  yb,  xb+3,yb,QPen(Qt::darkRed,2)));
  Texts.append(new Text(-xb+4,  -yb-3, QObject::tr(".LIB"),
			QColor(0,0,0), 12.0));

  x1 = -xb-3;  y1 = -yb-5;
  x2 =  xb+9; y2 =  yb+3;

  tx = x1+4;
  ty = y2+4;
  Model = "SpiceLib";
  Name  = "SpiceLib";
  SpiceModel = ".LIB";

  Props.append(new Property("File", "/home/user/library.inc", true,"SPICE file to include"));
  Props.append(new Property("Section", "mos1", true,"Library section name"));
}

S4Q_Lib::~S4Q_Lib()
{
}

Component* S4Q_Lib::newOne()
{
  return new S4Q_Lib();
}

Element* S4Q_Lib::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr(".Lib directive");
  BitmapFile = (char *) "sp_lib";

  if(getNewOne)  return new S4Q_Lib();
  return 0;
}

QString S4Q_Lib::getSpiceModel()
{
  if (isActive != COMP_IS_ACTIVE) return QString("");
  QString s;
  s.clear();

  QString file = getProperty("File")->Value;
  if ( !file.isEmpty() ){
    file = misc::properAbsFileName(file, containingSchematic);
    QString sec = getProperty("Section")->Value;
    s += QString("%1 \"%2\" %3\n").arg(SpiceModel).arg(file).arg(sec);
  }

  return s;
}

