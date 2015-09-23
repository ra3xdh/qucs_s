/***************************************************************************
                         isffm.cpp  -  description
                   --------------------------------------
    begin                    : Fri Mar 9 2007
    copyright              : (C) 2007 by Gunther Kraut
    email                     : gn.kraut@t-online.de
    spice4qucs code added  Sun. 22  March 2015
    copyright              : (C) 2015 by Mike Brinson
    email                    : mbrin72043@yahoo.co.uk

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "isffm.h"
#include "node.h"
#include "extsimkernels/spicecompat.h"


iSffm::iSffm()
{
  Description = QObject::tr("SPICE I(SFFM):");

  // normal current source symbol
  
   Arcs.append(new Arc(-12,-12, 24, 24,  0, 16*360,QPen(Qt::darkRed,3)));
   Texts.append(new Text(36, 4," SFFM",Qt::red,10.0,0.0,-1.0)); 
  Lines.append(new Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -7,  0,  7,  0,QPen(Qt::darkRed,3)));
  Lines.append(new Line(  6,  0,  0, -4,QPen(Qt::darkRed,3)));
  Lines.append(new Line(  6,  0,  0,  4,QPen(Qt::darkRed,3)));
 
  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  40;

  tx = x1+4;
  ty = y2+4;
  Model = "I";
  SpiceModel = "I";
  Name  = "I";

  Props.append(new Property("I0", "0 ", true,
		QObject::tr("offset current")));
  Props.append(new Property("Ia", "1 ", true,
		QObject::tr("carrier current amplitude")));
  Props.append(new Property("Fc", "1", true,
		QObject::tr("carrier signal frequency")));
  Props.append(new Property("Mdi", "10", true,
		QObject::tr("modulation index")));
  Props.append(new Property("Fs", "500", true,
		QObject::tr("modulating signal frequency")));

  rotate();  // fix historical flaw
}

iSffm::~iSffm()
{
}

Component* iSffm::newOne()
{
  return new iSffm();
}

Element* iSffm::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("I(SFFM)");
  BitmapFile = (char *) "isffm";

  if(getNewOne)  return new iSffm();
  return 0;
}

QString iSffm::netlist()
{
    return QString("");
}

QString iSffm::spice_netlist(bool)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam;   // node names
    }

   QString I0  = spicecompat::normalize_value(Props.at(0)->Value);
   QString Ia  = spicecompat::normalize_value(Props.at(1)->Value);
   QString Fc  = spicecompat::normalize_value(Props.at(2)->Value);
   QString Mdi = spicecompat::normalize_value(Props.at(3)->Value);
   QString Fs  = spicecompat::normalize_value(Props.at(4)->Value);


    s += QString(" DC 0 SFFM(%1 %2 %3 %4 %5 ) AC 0\n").arg(I0).arg(Ia).arg(Fc).arg(Mdi).arg(Fs);
    return s;
}
