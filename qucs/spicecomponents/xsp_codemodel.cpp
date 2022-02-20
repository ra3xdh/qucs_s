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
#include "xsp_codemodel.h"
#include "main.h"

#include <QFontMetrics>

XSP_CodeModel::XSP_CodeModel()
{
  isEquation = false;
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr("XSPICE CodeModel: cfunc.mod and ifspec.ifs files pair\n");

  QFont f = QucsSettings.font;
  f.setWeight(QFont::Light);
  f.setPointSizeF(12.0);
  QFontMetrics  metrics(f, 0);  // use the the screen-compatible metric
  QSize r = metrics.size(0, QObject::tr("XSPICE CodeModel"));
  int xb = r.width()  >> 1;
  int yb = r.height() >> 1;

  Lines.append(new qucs::Line(-xb, -yb, -xb,  yb,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-xb,  yb,  xb+3,yb,QPen(Qt::darkBlue,2)));
  Texts.append(new Text(-xb+4,  -yb-3, QObject::tr("XSPICE CodeModel"),
			QColor(0,0,0), 12.0));

  x1 = -xb-3;  y1 = -yb-5;
  x2 =  xb+9; y2 =  yb+3;

  tx = x1+4;
  ty = y2+4;
  Model = "XSP_CMod";
  Name  = "XSP_CMod";
  SpiceModel = "CMod";

  Props.append(new Property("File", "cfunc.mod", true,"Model file *.mod"));
  Props.append(new Property("File", "ifspec.ifs", true,"Interface file *.ifs"));
}

XSP_CodeModel::~XSP_CodeModel()
{
}

Component* XSP_CodeModel::newOne()
{
  return new XSP_CodeModel();
}

Element* XSP_CodeModel::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("XSPICE CodeModel");
  BitmapFile = (char *) "xsp_cmod";

  if(getNewOne)  return new XSP_CodeModel();
  return 0;
}

QString XSP_CodeModel::spice_netlist()
{
    return QString("");
}


