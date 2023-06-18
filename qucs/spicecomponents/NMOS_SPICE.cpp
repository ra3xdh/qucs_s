/***************************************************************************
                         NMOS_SPICE.cpp  -  description
                   --------------------------------------
    begin                     : Fri Mar 9 2007
    copyright                 : (C) 2007 by Gunther Kraut
    email                     : gn.kraut@t-online.de
    spice4qucs code added  Sat. 30 May 2015
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
#include "NMOS_SPICE.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


NMOS_SPICE::NMOS_SPICE()
{
    Description = QObject::tr("M(NMOS) MOS:\nMultiple line ngspice or Xyce M model specifications allowed using \"+\" continuation lines.\nLeave continuation lines blank when NOT in use.");
    Simulator = spicecompat::simSpice;

  Lines.append(new qucs::Line(-14,-13,-14, 13,QPen(Qt::darkRed,2)));

  Lines.append(new qucs::Line(-30,  0,-20,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-20,  0,-14,  0,QPen(Qt::darkRed,2)));
  
  Lines.append(new qucs::Line(-10,-11,  0,-11,QPen(Qt::darkRed,2)));

  Lines.append(new qucs::Line(  0,-11,  0,-20,QPen(Qt::darkRed,2)));
  Lines.append(new qucs::Line(  0,-20,  0,-30,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-10, 11,  0, 11,QPen(Qt::darkRed,2)));
  Lines.append(new qucs::Line(  0, 11,  0, 20,QPen(Qt::darkRed,2)));
  Lines.append(new qucs::Line(  0, 20,  0, 30,QPen(Qt::darkBlue,2)));
  
  Lines.append(new qucs::Line(-10,  0, 10,  0,QPen(Qt::darkRed,2)));
  Lines.append(new qucs::Line( 10,  0, 20,  0,QPen(Qt::darkBlue,2)));
  
  Lines.append(new qucs::Line(-10,-16,-10, -7,QPen(Qt::darkRed,2)));
  Lines.append(new qucs::Line(-10,  7,-10, 16,QPen(Qt::darkRed,2)));

  Lines.append(new qucs::Line( -9,  0, -4, -5,QPen(Qt::darkRed,2)));
  Lines.append(new qucs::Line( -9,  0, -4,  5,QPen(Qt::darkRed,2)));
  
  Lines.append(new qucs::Line(-10, -8,-10,  8,QPen(Qt::darkRed,2)));
  Lines.append(new qucs::Line( -4, 24,  4, 20,QPen(Qt::darkRed,2)));
    
  //Texts.append(new Text(30,12,"NMOS",Qt::darkRed,10.0,0.0,-1.0));
 
  Ports.append(new Port(  0,-30)); //D
  Ports.append(new Port(-30,  0)); //G
  Ports.append(new Port(  0, 30)); //S
  Ports.append(new Port( 20,  0)); //B

  x1 = -30; y1 = -30;
  x2 =   4; y2 =  30;

    tx = x1+4;
    ty = y2+4;

    Model = "NMOS_SPICE";
    SpiceModel = "M";
    Name  = "M";

    Props.append(new Property("M", "", true,"Param list and\n .model spec."));
    Props.append(new Property("M_Line 2", "", false,"+ continuation line 1"));
    Props.append(new Property("M_Line 3", "", false,"+ continuation line 2"));
    Props.append(new Property("M_Line 4", "", false,"+ continuation line 3"));
    Props.append(new Property("M_Line 5", "", false,"+ continuation line 4"));

}

NMOS_SPICE::~NMOS_SPICE()
{
}

Component* NMOS_SPICE::newOne()
{
  return new NMOS_SPICE();
}

Element* NMOS_SPICE::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("M(NMOS)");
  BitmapFile = (char *) "NMOS_SPICE";

  if(getNewOne)  return new NMOS_SPICE();
  return 0;
}

QString NMOS_SPICE::netlist()
{
    return QString("");
}

QString NMOS_SPICE::spice_netlist(bool)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    for (Port *p1 : Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam+" ";   // node names
    }
 
    QString M= Props.at(0)->Value;
    QString M_Line_2= Props.at(1)->Value;
    QString M_Line_3= Props.at(2)->Value;
    QString M_Line_4= Props.at(3)->Value;
    QString M_Line_5= Props.at(4)->Value;

    if(  M.length()  > 0)          s += QString("%1").arg(M);
    if(  M_Line_2.length() > 0 )   s += QString("\n%1").arg(M_Line_2);
    if(  M_Line_3.length() > 0 )   s += QString("\n%1").arg(M_Line_3);
    if(  M_Line_4.length() > 0 )   s += QString("\n%1").arg(M_Line_4);
    if(  M_Line_5.length() > 0 )   s += QString("\n%1").arg(M_Line_5);
    s += "\n";

    return s;
}
