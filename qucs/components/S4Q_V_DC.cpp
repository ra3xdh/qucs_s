/***************************************************************************
                               S4Q_V_DC.cpp
                              -------------
    begin                : Sat Aug 23 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
     
    SPICE Version         : Friday Sept 11 2015
    copyright            : (C) 2015 Mike Brinson
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

#include "S4Q_V_DC.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


S4Q_V_DC::S4Q_V_DC()
{
  Description = QObject::tr("Battery");

  Lines.append(new Line(  4,-13,  4, 13,QPen(Qt::darkRed,2)));
  Lines.append(new Line( -4, -6, -4,  6,QPen(Qt::darkRed,4))); 
  Lines.append(new Line( 30,  0,  15,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 15,  0,   4,  0,QPen(Qt::darkRed,2)));
  
   Lines.append(new Line(-30,  0,  -15,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -15,  0,   -4,  0,QPen(Qt::darkRed,2)));

  Lines.append(new Line( 11,  5, 11, 11,QPen(Qt::red,1)));
  Lines.append(new Line( 14,  8,  8,  8,QPen(Qt::red,1)));
  Lines.append(new Line(-11,  5,-11, 11,QPen(Qt::black,1)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = x1+4;
  ty = y2+4;
  Model = "S4Q_V_DC";
  Name  = "V";
  SpiceModel = "V";

  Props.append(new Property("U", "1 V", true,
		QObject::tr("voltage in Volts")));

  rotate();  // fix historical flaw
}

S4Q_V_DC::~S4Q_V_DC()
{
}

Component* S4Q_V_DC::newOne()
{
  return new S4Q_V_DC();
}

QString S4Q_V_DC::spice_netlist(bool)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam;   // node names
    }

    s += QString(" DC %1\n").arg(spicecompat::normalize_value(Props.at(0)->Value));
    return s;
}

Element* S4Q_V_DC::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Battery");
  BitmapFile = (char *) "S4Q_V_DC";

  if(getNewOne)  return new S4Q_V_DC();
  return 0;
}
