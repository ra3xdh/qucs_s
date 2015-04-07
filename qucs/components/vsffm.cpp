/***************************************************************************
                         vsffm.cpp  -  description
                   --------------------------------------
    begin                    : Fri Mar 9 2007
    copyright              : (C) 2007 by Gunther Kraut
    email                     : gn.kraut@t-online.de
    spice4qucs code added  Wed. 18 March 2015
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
#include "vsffm.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


vSffm::vSffm()
{
  Description = QObject::tr("Single frequency FM modulated source");

  // normal voltage source symbol
  Arcs.append(new Arc(-12,-12, 24, 24,     0, 16*360,QPen(Qt::red,3)));
  Texts.append(new Text(26, 4,"SFFM",Qt::red,10.0,0.0,-1.0)); 
  Lines.append(new Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  
  Lines.append(new Line( 18,  -5, 18, -11,QPen(Qt::red,1)));
  Lines.append(new Line( 21,  -8, 15,  -8,QPen(Qt::red,1)));
  Lines.append(new Line(-18,  -5,-18, -11,QPen(Qt::black,1)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  40;

  tx = x1+4;
  ty = y2+4;
  Model = "VSffm";
  SpiceModel = "V";
  Name  = "V";

  Props.append(new Property("Vo", "0 V", true,
		QObject::tr("offset volage")));
  Props.append(new Property("Va", "1 V", true,
		QObject::tr("carrier amplitude")));
  Props.append(new Property("Fc", "1k", true,
		QObject::tr("carrier signal frequency")));
  Props.append(new Property("Mdi", "10", true,
		QObject::tr("modulation index")));
  Props.append(new Property("Fs", "500", false,
		QObject::tr("modulating signal frequency")));

  rotate();  // fix historical flaw
}

vSffm::~vSffm()
{
}

Component* vSffm::newOne()
{
  return new vSffm();
}

Element* vSffm::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("SFFM");
  BitmapFile = (char *) "vsffm";

  if(getNewOne)  return new vSffm();
  return 0;
}

QString vSffm::netlist()
{
    return QString("");
}

QString vSffm::spice_netlist(bool isXyce)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam;   // node names
    }

   QString Vo= spicecompat::normalize_value(Props.at(0)->Value);
   QString Va= spicecompat::normalize_value(Props.at(1)->Value);
   QString Fc= spicecompat::normalize_value(Props.at(2)->Value);
   QString Mdi = spicecompat::normalize_value(Props.at(3)->Value);
   QString Fs = spicecompat::normalize_value(Props.at(4)->Value);


    s += QString(" DC 0 SFFM(%1 %2 %3 %4 %5 ) AC 0\n").arg(Vo).arg(Va).arg(Fc).arg(Mdi).arg(Fs);
    return s;
}
