/***************************************************************************
                                 irect.cpp
                                -----------
    begin                : Sat Sep 18 2004
    copyright            : (C) 2004 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
    spice4qucs code added  Wed. 22 March 2015
    copyright          : (C) 2015 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "irect.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


iRect::iRect()
{
  Description = QObject::tr("ideal rectangle current source");

  Arcs.append(new qucs::Arc(-12,-12, 24, 24,  0, 16*360,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( -7,  0,  7,  0,QPen(Qt::darkBlue,3)));
  Lines.append(new qucs::Line(  6,  0,  0, -4,QPen(Qt::darkBlue,3)));
  Lines.append(new qucs::Line(  6,  0,  0,  4,QPen(Qt::darkBlue,3)));

  // little rectangle symbol
  Lines.append(new qucs::Line( 19,  5, 19,  7,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 13,  7, 19,  7,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 13,  7, 13, 11,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 13, 11, 19, 11,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 19, 11, 19, 15,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 13, 15, 19, 15,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 13, 15, 13, 17,QPen(Qt::darkBlue,2)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  20;

  tx = x1+4;
  ty = y2+4;
  Model = "Irect";
  Name  = "I";
  SpiceModel = "I";

  Props.append(new Property("U", "1 mA", true,
		QObject::tr("current at high pulse")));
  Props.append(new Property("TH", "1 ms", true,
		QObject::tr("duration of high pulses")));
  Props.append(new Property("TL", "1 ms", true,
		QObject::tr("duration of low pulses")));
  Props.append(new Property("Tr", "1 ns", false,
		QObject::tr("rise time of the leading edge")));
  Props.append(new Property("Tf", "1 ns", false,
		QObject::tr("fall time of the trailing edge")));
  Props.append(new Property("Td", "0 ns", false,
		QObject::tr("initial delay time")));

  rotate();  // fix historical flaw
}

iRect::~iRect()
{
}

QString iRect::spice_netlist(bool)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);

    for (Port *p1 : Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam;   // node names
    }   

    double T, TL, TH, Trval, Tfval, fac;
    QString unit;

    QString U = spicecompat::normalize_value(Props.at(0)->Value); //VH
    QString Td = spicecompat::normalize_value(Props.at(5)->Value); // Td
    QString Tr = spicecompat::normalize_value(Props.at(3)->Value); // Tr
    QString Tf = spicecompat::normalize_value(Props.at(4)->Value); //Tf
    misc::str2num(Props.at(1)->Value,TH,unit,fac); //TH
    TH *= fac;    // TH = pw
    misc::str2num(Props.at(2)->Value,TL,unit,fac);
    T = TL*fac+TH;
   misc::str2num(Props.at(3)->Value,Trval,unit,fac); 
     T = Trval*fac+T;    
   misc::str2num(Props.at(4)->Value,Tfval,unit,fac); 
     T = Tfval*fac+T;    
     
    s += QString(" DC 0 PULSE( 0  -%1 %2 %3 %4 %5 %6)  AC 0\n").arg(U).arg(Td).arg(Tr).arg(Tf).arg(TH).arg(T);

    return s;
}

Component* iRect::newOne()
{
  return new iRect();
}

Element* iRect::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Rectangle Current");
  BitmapFile = (char *) "irect";

  if(getNewOne)  return new iRect();
  return 0;
}
