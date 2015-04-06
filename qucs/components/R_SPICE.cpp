/***************************************************************************
                         R_SPICE.cpp  -  description
                   --------------------------------------
    begin                    : Fri Mar 9 2007
    copyright              : (C) 2007 by Gunther Kraut
    email                     : gn.kraut@t-online.de
    spice4qucs code added  Sun. 5 April 2015
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
#include "R_SPICE.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


R_SPICE::R_SPICE()
{
    Description = QObject::tr("R SPICE format");

    Lines.append(new Line(-18, -9, 18, -9,QPen(Qt::darkRed,4)));
    Lines.append(new Line( 18, -9, 18,  9,QPen(Qt::darkRed,4)));
    Lines.append(new Line( 18,  9,-18,  9,QPen(Qt::darkRed,4)));
    Lines.append(new Line(-18,  9,-18, -9,QPen(Qt::darkRed,4)));
    Lines.append(new Line(-30,  0,-18,  0,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( 18,  0, 30,  0,QPen(Qt::darkBlue,2)));
    
    Ports.append(new Port(-30,  0));
    Ports.append(new Port( 30,  0));

    x1 = -30; y1 = -11;
    x2 =  30; y2 =  11;

    tx = x1+4;
    ty = y2+4;

    Model = "R_SPICE";
    SpiceModel = "R";
    Name  = "R";

    Props.append(new Property("R", "", true,"Expression"));
    Props.append(new Property("R_Line 2", "", false,"Expression"));
    Props.append(new Property("R_Line 3", "", false,"Expression"));
    Props.append(new Property("R_Line 4", "", false,"Expression"));
    Props.append(new Property("R_Line 5", "", false,"Expression"));



    rotate();  // fix historical flaw
}

R_SPICE::~R_SPICE()
{
}

Component* R_SPICE::newOne()
{
  return new R_SPICE();
}

Element* R_SPICE::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Resistor");
  BitmapFile = (char *) "R_SPICE";

  if(getNewOne)  return new R_SPICE();
  return 0;
}

QString R_SPICE::netlist()
{
    return QString("");
}

QString R_SPICE::spice_netlist(bool isXyce)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam+" ";   // node names
    }
     Property *pp = Props.first();
    QString val = pp->Value;
    if (!val.isEmpty()) s += " "+val + "\n";
    for(pp = Props.next(); pp != 0; pp = Props.next()) {
        QString val = pp->Value;
        if (!val.isEmpty()) s += QString("%1\n").arg(val) ;
    }
    s += '\n';
 
    return s;
}
