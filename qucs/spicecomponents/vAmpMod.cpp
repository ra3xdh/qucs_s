/***************************************************************************
                         vAmpMod.cpp  -  description
                   --------------------------------------
    begin                    : Fri Mar 9 2007
    copyright              : (C) 2007 by Gunther Kraut
    email                     : gn.kraut@t-online.de
    spice4qucs code added  Tue. 23 March 2015
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
#include "vAmpMod.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


vAmpMod::vAmpMod()
{
  Description = QObject::tr("SPICE V(AM): ngspice only.");

  // normal voltage source symbol
  Arcs.append(new qucs::Arc(-12,-12, 24, 24,     0, 16*360,QPen(Qt::blue,3)));
   Texts.append(new Text(36, 4,"AM",Qt::blue,10.0,0.0,-1.0)); 
  Lines.append(new qucs::Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  
  Lines.append(new qucs::Line( 18,  -5, 18, -11,QPen(Qt::red,1)));
  Lines.append(new qucs::Line( 21,  -8, 15,  -8,QPen(Qt::red,1)));
  Lines.append(new qucs::Line(-18,  -5,-18, -11,QPen(Qt::black,1)));

 

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  40;

  tx = x1+4;
  ty = y2+4;
  Model = "VAmpMod";
  SpiceModel = "V";
  Name  = "V";

  Props.append(new Property("Va", "1 ", true,
		QObject::tr("voltage amplitude")));
  Props.append(new Property("Vo", "0 ", true,
		QObject::tr("offset voltage")));
  Props.append(new Property("Mf", "500", true,
		QObject::tr("modulation frequency")));
  Props.append(new Property("Fc", "10k", true,
		QObject::tr("carrier frequency")));
  Props.append(new Property("Td", "0", true,
		QObject::tr("signal delay")));

  rotate();  // fix historical flaw
}

vAmpMod::~vAmpMod()
{
}

Component* vAmpMod::newOne()
{
  return new vAmpMod();
}

Element* vAmpMod::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("V(AM)");
  BitmapFile = (char *) "vAmpMod";

  if(getNewOne)  return new vAmpMod();
  return 0;
}

QString vAmpMod::netlist()
{
    return QString("");
}

QString vAmpMod::spice_netlist(bool)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam;   // node names
    }

   QString Va= spicecompat::normalize_value(Props.at(0)->Value);
   QString Vo= spicecompat::normalize_value(Props.at(1)->Value);
   QString Mf= spicecompat::normalize_value(Props.at(2)->Value);
   QString Fc = spicecompat::normalize_value(Props.at(3)->Value);
   QString Td = spicecompat::normalize_value(Props.at(4)->Value);


    s += QString(" DC 0 AM(%1 %2 %3 %4 %5 ) AC 0\n").arg(Va).arg(Vo).arg(Mf).arg(Fc).arg(Td);
    return s;
}
