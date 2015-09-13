/***************************************************************************
                             S4Q_V.cpp
                              -------------
    begin                : Thu May 21 2015
    copyright            : (C) 2015 by Vadim Kuznetsov
    email                : ra3xdh@gmail.com
             
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

#include "S4Q_V.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


S4Q_V::S4Q_V()
{
   Description = QObject::tr("SPICE V:\nMultiple line ngspice or Xyce V specifications allowed using \"+\" continuation lines.\nLeave continuation lines blank when NOT in use.  ");

  Arcs.append(new Arc(-12,-12, 24, 24,     0, 16*360,QPen(Qt::darkRed,2)));

  Lines.append(new Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
 
  Lines.append(new Line( 18,  5, 18, 11,QPen(Qt::red,1)));
  Lines.append(new Line( 21,  8, 15,  8,QPen(Qt::red,1)));
  Lines.append(new Line(-18,  5,-18, 11,QPen(Qt::black,1)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = x1+4;
  ty = y2+4;
  Model = "S4Q_V";
  SpiceModel = "V";
  Name  = "V";

  Props.append(new Property("V", "", true,"Specification xpression"));
  Props.append(new Property("V_Line 2", "", false,"+ continuation line 1"));
  Props.append(new Property("V_Line 3", "", false,"+ continuation line 2"));
  Props.append(new Property("V_Line 4", "", false,"+ continuation line 3"));
  Props.append(new Property("V_Line 5", "", false,"+ continuation line 4"));

  rotate();  // fix historical flaw
}

S4Q_V::~S4Q_V()
{
}

Component* S4Q_V::newOne()
{
  return new S4Q_V();
}

Element* S4Q_V::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("V Source");
  BitmapFile = (char *) "S4Q_V";

  if(getNewOne)  return new S4Q_V();
  return 0;
}

QString S4Q_V::spice_netlist(bool)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam+" ";   // node names
    }

    QString l0= Props.at(0)->Value;
    QString l1= Props.at(1)->Value;
    QString l2= Props.at(2)->Value;
    QString l3= Props.at(3)->Value;
    QString l4= Props.at(4)->Value;

    if(l0.length()> 0)   s += QString("%1\n").arg(l0);
    if(l1.length()> 0)   s += QString("%1\n").arg(l1);
    if(l2.length()> 0)   s += QString("%1\n").arg(l2);
    if(l3.length()> 0)   s += QString("%1\n").arg(l3);
    if(l4.length()> 0)   s += QString("%1\n").arg(l4);
    return s;
}
