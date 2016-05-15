/***************************************************************************
                         PMF_MESFET_SPICE.cpp  -  description
                   --------------------------------------
    begin                    : Fri Mar 9 2007
    copyright              : (C) 2007 by Gunther Kraut
    email                     : gn.kraut@t-online.de
    spice4qucs code added  Mon. 25 May 2015
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
#include "PMF_MESFET_SPICE.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


PMF_MESFET_SPICE::PMF_MESFET_SPICE()
{
    Description = QObject::tr("Z(PMF) MESFET:\nMultiple line ngspice or Xyce Z model specifications allowed using \"+\" continuation lines.\nLeave continuation lines blank when NOT in use.");

  Lines.append(new Line(-10,-15,-10, 15,QPen(Qt::darkRed,3)));
  Lines.append(new Line(-30,  0,-10,  0,QPen(Qt::darkRed,3)));
  Lines.append(new Line(-10,-10,  0,-10,QPen(Qt::darkRed,3)));
  Lines.append(new Line(  0,-10,  0,-20,QPen(Qt::darkRed,3)));
  Lines.append(new Line(  0,-20,  0,-30,QPen(Qt::darkBlue,3)));
   
  Lines.append(new Line(-10, 10,  0, 10,QPen(Qt::darkRed,3)));
  Lines.append(new Line(  0, 10,  0, 20,QPen(Qt::darkRed,3)));
  Lines.append(new Line(  0, 20,  0, 30,QPen(Qt::darkBlue,3)));

  Texts.append(new Text(30,12,"PMF",Qt::darkRed,10.0,0.0,-1.0));
  
  Ports.append(new Port(  0,-30));
  Ports.append(new Port(-30,  0));
  Ports.append(new Port(  0, 30));

  x1 = -30; y1 = -30;
  x2 =   4; y2 =  30;

    tx = x1+4;
    ty = y2+4;

    Model = "PMF_MESFET_SPICE";
    SpiceModel = "Z";
    Name  = "Z";

    Props.append(new Property("Z", "", true,"Param list and\n .model spec."));
    Props.append(new Property("Z_Line 2", "", false,"+ continuation line 1"));
    Props.append(new Property("Z_Line 3", "", false,"+ continuation line 2"));
    Props.append(new Property("Z_Line 4", "", false,"+ continuation line 3"));
    Props.append(new Property("Z_Line 5", "", false,"+ continuation line 4"));

}

PMF_MESFET_SPICE::~PMF_MESFET_SPICE()
{
}

Component* PMF_MESFET_SPICE::newOne() 
{
  return new PMF_MESFET_SPICE();
}

Element* PMF_MESFET_SPICE::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Z(PMF)");
  BitmapFile = (char *) "PMF_MESFET_SPICE";

  if(getNewOne)  return new PMF_MESFET_SPICE();
  return 0;
}

QString PMF_MESFET_SPICE::netlist()
{
    return QString("");
}

QString PMF_MESFET_SPICE::spice_netlist(bool)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam+" ";   // node names
    }
 
    QString Z= Props.at(0)->Value;
    QString Z_Line_2= Props.at(1)->Value;
    QString Z_Line_3= Props.at(2)->Value;
    QString Z_Line_4= Props.at(3)->Value;
    QString Z_Line_5= Props.at(4)->Value;

    if(  Z.length()  > 0)          s += QString("%1\n").arg(Z);
    if(  Z_Line_2.length() > 0 )   s += QString("%1\n").arg(Z_Line_2);
    if(  Z_Line_3.length() > 0 )   s += QString("%1\n").arg(Z_Line_3);
    if(  Z_Line_4.length() > 0 )   s += QString("%1\n").arg(Z_Line_4);
    if(  Z_Line_5.length() >  0 )  s += QString("%1\n").arg(Z_Line_5);
 
    return s;
}
