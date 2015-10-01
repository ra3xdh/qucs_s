/***************************************************************************
                          sp_model.cpp  -  description
                             -------------------
    begin                : Thu Oct 01 2015
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
#include "sp_model.h"
#include "main.h"

#include <QFontMetrics>

S4Q_Model::S4Q_Model()
{
  isEquation = false;
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr(".MODEL section\n"
                            "Multiple line ngspice or Xyce .MODEL allowed using \"+\" continuation lines.\n"
                            "Leave continuation lines blank when NOT in use.");

  QFont f = QucsSettings.font;
  f.setWeight(QFont::Light);
  f.setPointSizeF(12.0);
  QFontMetrics  metrics(f, 0);  // use the the screen-compatible metric
  QSize r = metrics.size(0, QObject::tr(".MODEL"));
  int xb = r.width()  >> 1;
  int yb = r.height() >> 1;

  Lines.append(new Line(-xb, -yb, -xb,  yb,QPen(Qt::darkRed,2)));
  Lines.append(new Line(-xb,  yb,  xb+3,yb,QPen(Qt::darkRed,2)));
  Texts.append(new Text(-xb+4,  -yb-3, QObject::tr(".MODEL"),
			QColor(0,0,0), 12.0));

  x1 = -xb-3;  y1 = -yb-5;
  x2 =  xb+9; y2 =  yb+3;

  tx = x1+4;
  ty = y2+4;
  Model = "SpiceModel";
  Name  = "SpiceModel";
  SpiceModel = ".MODEL";

  Props.append(new Property("Line_1 ", ".MODEL DIODE1 D(BF=50 Is=1e-13 Vbf = 50)", true,"Model statement"));
  Props.append(new Property("Line_2", "", false,"+ continuation line 1"));
  Props.append(new Property("Line_3", "", false,"+ continuation line 2"));
  Props.append(new Property("Line_4", "", false,"+ continuation line 3"));
  Props.append(new Property("Line_5", "", false,"+ continuation line 4"));
}

S4Q_Model::~S4Q_Model()
{
}

Component* S4Q_Model::newOne()
{
  return new S4Q_Model();
}

Element* S4Q_Model::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr(".MODEL Section");
  BitmapFile = (char *) "sp_model";

  if(getNewOne)  return new S4Q_Model();
  return 0;
}

QString S4Q_Model::getSpiceModel()
{
    if (isActive != COMP_IS_ACTIVE) return QString("");

    QString s;
    s.clear();
    foreach (Property *pp, Props) {
        if (!pp->Value.isEmpty())
            s += pp->Value + "\n";
    }
    return s;
}

