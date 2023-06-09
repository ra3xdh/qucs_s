/***************************************************************************
                         L_SPICE.cpp  -  description
                   --------------------------------------
    begin                    : Fri Mar 9 2007
    copyright              : (C) 2007 by Gunther Kraut
    email                     : gn.kraut@t-online.de
    spice4qucs code added  Sat. 4 April 2015
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
#include "L_SPICE.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


L_SPICE::L_SPICE()
{
    Description = QObject::tr("SPICE L:\nMultiple line ngspice or Xyce L specifications allowed using \"+\" continuation lines.\nLeave continuation lines blank when NOT in use.  ");
    Simulator = spicecompat::simSpice;

    Arcs.append(new qucs::Arc(-18, -6, 12, 12,  0, 16*180,QPen(Qt::darkRed,3)));
    Arcs.append(new qucs::Arc( -6, -6, 12, 12,  0, 16*180,QPen(Qt::darkRed,3)));
    Arcs.append(new qucs::Arc(  6, -6, 12, 12,  0, 16*180,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line(-30,  0,-18,  0,QPen(Qt::darkBlue,2)));
    Lines.append(new qucs::Line( 18,  0, 30,  0,QPen(Qt::darkBlue,2)));
    // DOT
      Arcs.append(new qucs::Arc(-18, -20,  5,  5,  0, 16*360,QPen(Qt::darkRed,3)));

    Ports.append(new Port(-30,  0));
    Ports.append(new Port( 30,  0));

    x1 = -30; y1 = -10;
    x2 =  30; y2 =   6;

    tx = x1+4;
    ty = y2+4;

    Model = "L_SPICE";
    SpiceModel = "L";
    Name  = "L";

    Props.append(new Property("L", "", true,"L param list and\n .model spec."));
    Props.append(new Property("L_Line 2", "", false,"+ continuation line 1"));
    Props.append(new Property("L_Line 3", "", false,"+ continuation line 2"));
    Props.append(new Property("L_Line 4", "", false,"+ continuation line 3"));
    Props.append(new Property("L_Line 5", "", false,"+ continuation line 4"));



    rotate();  // fix historical flaw
}

L_SPICE::~L_SPICE()
{
}

Component* L_SPICE::newOne()
{
  return new L_SPICE();
}

Element* L_SPICE::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("L Inductor");
  BitmapFile = (char *) "L_SPICE";

  if(getNewOne)  return new L_SPICE();
  return 0;
}

QString L_SPICE::netlist()
{
    return QString("");
}

QString L_SPICE::spice_netlist(bool)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    for (Port *p1 : Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam+" ";   // node names
    }
 
    QString L= Props.at(0)->Value;
    QString L_Line_2= Props.at(1)->Value;
    QString L_Line_3= Props.at(2)->Value;
    QString L_Line_4= Props.at(3)->Value;
    QString L_Line_5= Props.at(4)->Value;

    if(  L.length()  > 0)          s += QString("%1").arg(L);
    if(  L_Line_2.length() > 0 )   s += QString("\n%1").arg(L_Line_2);
    if(  L_Line_3.length() > 0 )   s += QString("\n%1").arg(L_Line_3);
    if(  L_Line_4.length() > 0 )   s += QString("\n%1").arg(L_Line_4);
    if( L_Line_5.length() >  0 )   s += QString("\n%1").arg(L_Line_5);
    s += "\n";

    return s;
}
