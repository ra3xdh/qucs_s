/***************************************************************************
                         S4Q_W.cpp  -  description
                   --------------------------------------
    begin                    : Fri Mar 9 2007
    copyright              : (C) 2007 by Gunther Kraut
    email                     : gn.kraut@t-online.de
    spice4qucs code added  Tue. 16 Sept. 2015
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
#include "S4Q_W.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


S4Q_W::S4Q_W()
{
    Description = QObject::tr("SPICE W:\nMultiple line ngspice or Xyce W specifications allowed using \"+\" continuation lines.\nLeave continuation lines blank when NOT in use.  ");

    Lines.append(new qucs::Line(-15, -15, -15, 15,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line(-15,  15,  15, 15,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line( 15,  15,  15,-15,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line( 15, -15, -15, -15,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line(-30,  0,  -15,  0,QPen(Qt::darkBlue,2)));
    Lines.append(new qucs::Line(-15,  0,   -5,  0,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line( 30,  0,   15,  0,QPen(Qt::darkBlue,3)));
    Lines.append(new qucs::Line( 15,  0,    5,  0,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line(  5,  0,    0,  8,QPen(Qt::darkRed,3)));

    Ports.append(new Port(-30,  0)); // Ps+
    Ports.append(new Port( 30,  0)); // Ps-


    x1 = -30; y1 = -15;
    x2 =  40; y2 =  15;

    tx = x1+4;
    ty = y2+4;

    Model = "S4Q_W";
    SpiceModel = "W";
    Name  = "W";

    Props.append(new Property("W", "", true,"W param list and\n .model spec."));
    Props.append(new Property("W_Line 2", "", false,"+ continuation line 1"));
    Props.append(new Property("W_Line 3", "", false,"+ continuation line 2"));
    Props.append(new Property("W_Line 4", "", false,"+ continuation line 3"));
    Props.append(new Property("W_Line 5", "", false,"+ continuation line 4"));



    rotate();  // fix historical flaw
}

S4Q_W::~S4Q_W()
{
}

Component* S4Q_W::newOne()
{
  return new S4Q_W();
}

Element* S4Q_W::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr(" W(CSW)");
  BitmapFile = (char *) "S4Q_W";

  if(getNewOne)  return new S4Q_W();
  return 0;
}

QString S4Q_W::spice_netlist(bool)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam+" ";   // node names
    }

    QString W = Props.at(0)->Value;
    QString W_Line_2= Props.at(1)->Value;
    QString W_Line_3= Props.at(2)->Value;
    QString W_Line_4= Props.at(3)->Value;
    QString W_Line_5= Props.at(4)->Value;

    if(  W.length()  > 0)          s += QString("%1\n").arg(W);
    if(  W_Line_2.length() > 0 )   s += QString("%1\n").arg(W_Line_2);
    if(  W_Line_3.length() > 0 )   s += QString("%1\n").arg(W_Line_3);
    if(  W_Line_4.length() > 0 )   s += QString("%1\n").arg(W_Line_4);
    if(  W_Line_5.length() > 0)    s += QString("%1\n").arg(W_Line_5);
 
    return s;
}
