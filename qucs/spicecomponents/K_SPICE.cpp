/***************************************************************************
                                K-SPICE.cpp
                              --------------
    begin                : Sat Aug 23 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
    spice4qucs code added  Wed. 8 April 2015
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
#include "K_SPICE.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"



K_SPICE::K_SPICE()
{
    Description = QObject::tr("SPICE K:\nEnter the names of the coupled inductances and their coupling factor.");

    Lines.append(new qucs::Line(-10,  0, 10,  0,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line(-10,  0, -6,   4,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line(-10,  0, -6,  -4,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line( 10,  0,   6,  4,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line( 10,  0,   6,  -4,QPen(Qt::darkRed,3)));
   
 
    x1 = -30; y1 = -13;
    x2 =  30; y2 =  13;

    tx = x1+4;
    ty = y2+4;
    Model = "K_SPICE";
    SpiceModel = "K";
    Name  = "K";

    
      Props.append(new Property("Ind1", "L1", true, "Inductance 1 name"));
      Props.append(new Property("Ind2", "L2", true, "Inductance 2 name"));
      Props.append(new Property("K",   "0.1", true, QObject::tr("Coupling factor ( 0  <  K  <= 1)")));
  
    rotate();  // fix historical flaw
}

K_SPICE::~K_SPICE()
{
}

Component* K_SPICE::newOne()
{
  return new K_SPICE();
}

Element* K_SPICE::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("K coupling");
  BitmapFile = (char *) "K_SPICE";

  if(getNewOne)  return new K_SPICE();
  return 0;
}

QString K_SPICE::netlist()
{
    return QString("");
}

QString K_SPICE::spice_netlist(bool)
{
	QString s = spicecompat::check_refdes(Name,SpiceModel);
    for (Port *p1 : Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam+" ";   // node names
}
   QString Ind1 = Props.at(0) ->Value;
   QString Ind2 = Props.at(1) ->Value;
   QString K = spicecompat::normalize_value(Props.at(2)->Value);
   
   s+= QString(" %1 %2 %3 \n").arg(Ind1).arg(Ind2).arg(K);
    
    return s;
}

