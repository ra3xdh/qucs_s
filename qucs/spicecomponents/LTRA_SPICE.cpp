/***************************************************************************
                         LTRA_SPICE.cpp  -  description
                   --------------------------------------
    begin                  : Fri Mar 9 2007
    copyright              : (C) 2007 by Gunther Kraut
    email                  : gn.kraut@t-online.de
    spice4qucs code added  : Tue. 19 May 2015
    copyright              : (C) 2015 by Mike Brinson
    email                  : mbrin72043@yahoo.co.uk

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "LTRA_SPICE.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


LTRA_SPICE::LTRA_SPICE()
{
  Description = QObject::tr("SPICE O(LTRA):");

  Lines.append(new qucs::Line(-60,  -20,-45,  -20,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-45,  -20, 45,  -20,QPen(Qt::darkRed,4))); 
  Lines.append(new qucs::Line( 45,  -20, 60,  -20,QPen(Qt::darkBlue,2)));
  
  Lines.append(new qucs::Line(-60,   20,-45,   20,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-45,   20, 45,   20,QPen(Qt::darkRed,4))); 
  Lines.append(new qucs::Line( 45,   20, 60,   20,QPen(Qt::darkBlue,2)));
  
  Lines.append(new qucs::Line(-45,    0,  -37,  0,QPen(Qt::darkRed,4)));
  Lines.append(new qucs::Line(-37,    0,  -30, -10,QPen(Qt::darkRed,4)));
  Lines.append(new qucs::Line(-30,  -10,  -15,  10,QPen(Qt::darkRed,4)));
  Lines.append(new qucs::Line(-15,   10,   0,  -10,QPen(Qt::darkRed,4)));
  Lines.append(new qucs::Line(  0,  -10,   15,  10,QPen(Qt::darkRed,4)));
  Lines.append(new qucs::Line(  15,  10,   30, -10,QPen(Qt::darkRed,4))); 
  Lines.append(new qucs::Line(  30, -10,  37,   0,QPen(Qt::darkRed,4))); 
  Lines.append(new qucs::Line(  37,   0,  45,   0,QPen(Qt::darkRed,4)));   

 
    
  Ports.append(new Port(  -60, -20));  // P1
  Ports.append(new Port(  -60,  20));  // P2
  Ports.append(new Port(   60, -20));  // P3
  Ports.append(new Port(   60,  20));  // P4
  
  x1 = -40; y1 = -20;
  x2 =  40; y2 =  30;

    tx = x1+4;
    ty = y2+4;

    Model = "LTRA_SPICE";
    SpiceModel = "O";
    Name  = "O";

    Props.append(new Property("O",        "", true,"LTRL specification"));
    Props.append(new Property("O_Line 2", "", false,"+ continuation line 1"));
    Props.append(new Property("O_Line 3", "", false,"+ continuation line 2"));
    Props.append(new Property("O_Line 4", "", false,"+ continuation line 3"));
    Props.append(new Property("O_Line 5", "", false,"+ continuation line 4"));

//    rotate();  // fix historical flaw
}

LTRA_SPICE::~LTRA_SPICE()
{
}

Component* LTRA_SPICE::newOne()
{
  return new LTRA_SPICE();
}

Element* LTRA_SPICE::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("O(LTRA)");
  BitmapFile = (char *) "LTRA_SPICE";

  if(getNewOne)  return new LTRA_SPICE();
  return 0;
}

QString LTRA_SPICE::netlist()
{
    return QString("");
}

QString LTRA_SPICE::spice_netlist(bool)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam+" ";   // node names
    }
 
    QString O= Props.at(0)->Value;
    QString O_Line_2= Props.at(1)->Value;
    QString O_Line_3= Props.at(2)->Value;
    QString O_Line_4= Props.at(3)->Value;
    QString O_Line_5= Props.at(4)->Value;

    if(  O.length()  > 0)          s += QString("%1").arg(O);
    if(  O_Line_2.length() > 0 )   s += QString("\n%1").arg(O_Line_2);
    if(  O_Line_3.length() > 0 )   s += QString("\n%1").arg(O_Line_3);
    if(  O_Line_4.length() > 0 )   s += QString("\n%1").arg(O_Line_4);
    if(  O_Line_5.length() >  0 )  s += QString("\n%1").arg(O_Line_5);
    s += "\n";

    return s;
}
