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
#include "xsp_cmlib.h"
#include "main.h"
#include "misc.h"

#include <QFontMetrics>

XSP_CMlib::XSP_CMlib()
{
  isEquation = false;
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr("XSPICE precompiled CodeModel library\n");

  QFont f = QucsSettings.font;
  f.setWeight(QFont::Light);
  f.setPointSizeF(12.0);
  QFontMetrics  metrics(f, 0);  // use the the screen-compatible metric
  QSize r = metrics.size(0, QObject::tr("Precompiled CM-library"));
  int xb = r.width()  >> 1;
  int yb = r.height() >> 1;

  Lines.append(new qucs::Line(-xb, -yb, -xb,  yb,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-xb,  yb,  xb+3,yb,QPen(Qt::darkBlue,2)));
  Texts.append(new Text(-xb+4,  -yb-3, QObject::tr("Precompiled CM-library"),
			QColor(0,0,0), 12.0));

  x1 = -xb-3;  y1 = -yb-5;
  x2 =  xb+9; y2 =  yb+3;

  tx = x1+4;
  ty = y2+4;
  Model = "XSP_CMlib";
  Name  = "XSP_CMlib";
  SpiceModel = "CMlib";

  Props.append(new Property("File", "/home/user/library.cm", true,"Precompiled *.cm file"));
  Props.append(new Property("File", "", false,"Precompiled *.cm file"));
  Props.append(new Property("File", "", false,"Precompiled *.cm file"));
  Props.append(new Property("File", "", false,"Precompiled *.cm file"));
  Props.append(new Property("File", "", false,"Precompiled *.cm file"));
}

XSP_CMlib::~XSP_CMlib()
{
}

Component* XSP_CMlib::newOne()
{
  return new XSP_CMlib();
}

Element* XSP_CMlib::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("XSPICE precompiled CM-library");
  BitmapFile = (char *) "xsp_cmlib";

  if(getNewOne)  return new XSP_CMlib();
  return 0;
}

QString XSP_CMlib::getSpiceInit()
{
    QString s;
    s.clear();
    for (Property *pp : Props) {
        if (!pp->Value.isEmpty()) {
            QString f = misc::properAbsFileName(pp->Value, containingSchematic);
            s += "codemodel " + f + "\n";
        }
    }
    return s;
}

QString XSP_CMlib::spice_netlist()
{
    return QString("");
}


