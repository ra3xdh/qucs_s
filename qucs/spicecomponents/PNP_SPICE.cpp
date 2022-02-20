/***************************************************************************
                         PNP_SPICE.cpp  -  description
                   --------------------------------------
    begin                     : Fri Mar 9 2007
    copyright                 : (C) 2007 by Gunther Kraut
    email                     : gn.kraut@t-online.de
    spice4qucs code added  Thur. 28 May 2015
    copyright                 : (C) 2015 by Mike Brinson
    email                     : mbrin72043@yahoo.co.uk

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "PNP_SPICE.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


PNP_SPICE::PNP_SPICE()
{
    Description = QObject::tr("Q(PNP) BJT:\nMultiple line ngspice or Xyce Q model specifications allowed using \"+\" continuation lines.\nLeave continuation lines blank when NOT in use.");

  Lines.append(new qucs::Line(-10,-15,-10, 15,QPen(Qt::darkRed,3)));
  
  Lines.append(new qucs::Line(-30,  0,-20,  0,QPen(Qt::darkBlue,3)));
  Lines.append(new qucs::Line(-20,  0,-10,  0,QPen(Qt::darkRed,3)));
  
  Lines.append(new qucs::Line(-10, -5,  0,-15,QPen(Qt::darkRed,3)));
  
  Lines.append(new qucs::Line(  0,-15,  0,-20,QPen(Qt::darkRed,3)));
  Lines.append(new qucs::Line(  0,-20,  0,-30,QPen(Qt::darkBlue,3)));
    
  Lines.append(new qucs::Line(-10,  5,  0, 15,QPen(Qt::darkRed,3)));
  
  Lines.append(new qucs::Line(  0, 15,  0, 20,QPen(Qt::darkRed,3)));
  Lines.append(new qucs::Line(  0, 20,  0, 30,QPen(Qt::darkBlue,3)));

  
  Lines.append(new qucs::Line( -5, 10, -5, 16,QPen(Qt::darkRed,3)));
  Lines.append(new qucs::Line( -5, 10,  1, 10,QPen(Qt::darkRed,3)));
  
  Texts.append(new Text(30,12,"PNP",Qt::darkRed,10.0,0.0,-1.0));

 
  Ports.append(new Port(  0,-30));
  Ports.append(new Port(-30,  0));
  Ports.append(new Port(  0, 30));

  x1 = -30; y1 = -30;
  x2 =   4; y2 =  30;

    tx = x1+4;
    ty = y2+4;

    Model = "PNP_SPICE";
    SpiceModel = "Q";
    Name  = "Q";

    Props.append(new Property("Q", "", true,"Param list and\n .model spec."));
    Props.append(new Property("Q_Line 2", "", false,"+ continuation line 1"));
    Props.append(new Property("Q_Line 3", "", false,"+ continuation line 2"));
    Props.append(new Property("Q_Line 4", "", false,"+ continuation line 3"));
    Props.append(new Property("Q_Line 5", "", false,"+ continuation line 4"));

}

PNP_SPICE::~PNP_SPICE()
{
}

Component* PNP_SPICE::newOne()
{
  return new PNP_SPICE();
}

Element* PNP_SPICE::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Q(PNP) BJT");
  BitmapFile = (char *) "PNP_SPICE";

  if(getNewOne)  return new PNP_SPICE();
  return 0;
}

QString PNP_SPICE::netlist()
{
    return QString("");
}

QString PNP_SPICE::spice_netlist(bool)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam+" ";   // node names
    }
 
    QString Q= Props.at(0)->Value;
    QString Q_Line_2= Props.at(1)->Value;
    QString Q_Line_3= Props.at(2)->Value;
    QString Q_Line_4= Props.at(3)->Value;
    QString Q_Line_5= Props.at(4)->Value;

    if(  Q.length()  > 0)          s += QString("%1\n").arg(Q);
    if(  Q_Line_2.length() > 0 )   s += QString("%1\n").arg(Q_Line_2);
    if(  Q_Line_3.length() > 0 )   s += QString("%1\n").arg(Q_Line_3);
    if(  Q_Line_4.length() > 0 )   s += QString("%1\n").arg(Q_Line_4);
    if(  Q_Line_5.length() > 0 )   s += QString("%1\n").arg(Q_Line_5);
 
    return s;
}
