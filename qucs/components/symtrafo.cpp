/***************************************************************************
                          symtrafo.cpp  -  description
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

#include "node.h"
#include "main.h"
#include "symtrafo.h"
#include "extsimkernels/spicecompat.h"

#include<QFontMetrics>


symTrafo::symTrafo()
{
  Description = QObject::tr("ideal symmetrical transformer");
  Simulator = spicecompat::simAll;
  QFont Font(QucsSettings.font); // default application font
  // symbol text is smaller (10 pt default)
  Font.setPointSize(10); 
  // get the small font size; use the screen-compatible metric
  QFontMetrics  smallmetrics(Font, 0);

  int w;
  QString stmp;

  Arcs.append(new qucs::Arc(-16,-58,12,12, 16*270,16*180, QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(-16,-46,12,12, 16*270,16*180, QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(-16,-34,12,12, 16*270,16*180, QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(-16, 46,12,12, 16*270,16*180, QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(-16, 34,12,12, 16*270,16*180, QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(-16, 22,12,12, 16*270,16*180, QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(  4,-18,12,12,  16*90,16*180, QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(  4, -6,12,12,  16*90,16*180, QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(  4,  6,12,12,  16*90,16*180, QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-10,-58,-10,-70,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-10,-70,-30,-70,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 10,-18, 10,-30,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 10,-30, 30,-30,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-10, 58,-10, 70,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-10, 70,-30, 70,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 10, 18, 10, 30,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 10, 30, 30, 30,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-10,-10,-30,-10,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-10,-22,-10,-10,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-10, 10,-30, 10,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-10, 10,-10, 22,QPen(Qt::darkBlue,2)));

  // core lines
  Lines.append(new qucs::Line( -1,-57, -1, 57,QPen(Qt::darkBlue,1)));
  Lines.append(new qucs::Line(  1,-57,  1, 57,QPen(Qt::darkBlue,1)));

  stmp = "T1"; w = smallmetrics.boundingRect(stmp).width(); // compute width to right-align
  Texts.append(new Text(-13-w,-57,stmp));
  stmp = "T2"; w = smallmetrics.boundingRect(stmp).width(); // compute width to right-align
  Texts.append(new Text(-13-w, 22,stmp));

  // mark the turn direction
  Arcs.append(new qucs::Arc(-21,-64,  5,  5,  0, 16*360,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(-21, 15,  5,  5,  0, 16*360,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc( 15,-24,  5,  5,  0, 16*360,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-30,-70));
  Ports.append(new Port( 30,-30));
  Ports.append(new Port( 30, 30));
  Ports.append(new Port(-30, 70));
  Ports.append(new Port(-30, 10));
  Ports.append(new Port(-30,-10));

  x1 = -33; y1 = -74;
  x2 =  33; y2 =  74;

  tx = x1+4;
  ty = y2+4;
  Model = "sTr";
  Name  = "Tr";
  SpiceModel = "X";

  Props.append(new Property("T1", "1", true,
		QObject::tr("voltage transformation ratio of coil 1")));
  Props.append(new Property("T2", "1", true,
		QObject::tr("voltage transformation ratio of coil 2")));
}

symTrafo::~symTrafo()
{
}

Component* symTrafo::newOne()
{
  return new symTrafo();
}

Element* symTrafo::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("symmetric Transformer");
  BitmapFile = (char *) "symtrans";

  if(getNewOne)  return new symTrafo();
  return 0;
}

QString symTrafo::spice_netlist(spicecompat::SpiceDialect dialect)
{
  Q_UNUSED(dialect);

  QString p1 = Ports.at(1)->Connection->Name;
  QString p2 = Ports.at(2)->Connection->Name;

  QString s1p = Ports.at(4)->Connection->Name;
  QString s1m = Ports.at(3)->Connection->Name;

  QString s2p = Ports.at(0)->Connection->Name;
  QString s2m = Ports.at(5)->Connection->Name;

  QString s;
  s = "X_" + Name + "_W1 ";
  s += p1 + " ";  s += p2 + " ";
  s += s1p + " "; s += s1m + " ";
  s += " XFMR RATIO=" + spicecompat::normalize_value(Props.at(0)->Value);
  s += "\n";

  s += "X_" + Name + "_W2 ";
  s += p1 + " ";  s += p2 + " ";
  s += s2p + " "; s += s2m + " ";
  s += " XFMR RATIO=" + spicecompat::normalize_value(Props.at(1)->Value);
  s += "\n";
  return s;
}


QString symTrafo::getSpiceLibrary()
{
  QString f = spicecompat::getSpiceLibPath("xfmr.cir");
  QString s = QString (".INCLUDE \"%1\"\n").arg(f);
  return s;
}
