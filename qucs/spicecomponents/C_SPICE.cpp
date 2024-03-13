/***************************************************************************
                         C_SPICE.cpp  -  description
                   --------------------------------------
    begin                    : Fri Mar 9 2007
    copyright              : (C) 2007 by Gunther Kraut
    email                     : gn.kraut@t-online.de
    spice4qucs code added  Fri. 3 April 2015
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
#include "C_SPICE.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


C_SPICE::C_SPICE()
{
    Description = QObject::tr("SPICE C:\nMultiple line ngspice or Xyce C specifications allowed using \"+\" continuation lines.\nLeave continuation lines blank when NOT in use.");
    Simulator = spicecompat::simSpice;

    Model = "C_SPICE";
    SpiceModel = "C";
    Name  = "C";

    Props.append(new Property("C", "", true,"C param list and\n .model spec."));
    Props.append(new Property("C_Line 2", "", false,"+ continuation line 1"));
    Props.append(new Property("C_Line 3", "", false,"+ continuation line 2"));
    Props.append(new Property("C_Line 4", "", false,"+ continuation line 3"));
    Props.append(new Property("C_Line 5", "", false,"+ continuation line 4"));
    Props.append(new Property("Pins", "2", true,"[2,3] Pins count"));
    Props.append(new Property("Letter", "C", true,"[C,X,N] SPICE letter"));

    createSymbol();
    tx = x1+4;
    ty = y2+4;

    // rotate();  // fix historical flaw
}

void C_SPICE::createSymbol()
{
    int Npins = getProperty("Pins")->Value.toInt();
    Lines.append(new qucs::Line( -4,-11, -4, 11,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line(  4,-11,  4, 11, QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line(-30,  0, -4,  0,  QPen(Qt::darkBlue,2)));
    Lines.append(new qucs::Line(  4,  0, 30,  0,   QPen(Qt::darkBlue,2)));

    if (Npins >= 3) {
        Lines.append(new qucs::Line(  0,  -15, 0,  -30,QPen(Qt::darkRed,2)));
    }

    Ports.append(new Port( 30,  0));
    Ports.append(new Port(-30,  0));

    if (Npins >= 3) Ports.append(new Port( 0, -30));
    x1 = -30; y1 = -13;
    x2 =  30; y2 =  13;
}


C_SPICE::~C_SPICE()
{
}

Component* C_SPICE::newOne()
{
    auto p = new C_SPICE();
    p->getProperty("Pins")->Value = getProperty("Pins")->Value;
    p->getProperty("Letter")->Value = getProperty("Letter")->Value;
    p->recreate(0);
    return p;
}

Element* C_SPICE::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("C Capacitor");
  BitmapFile = (char *) "C_SPICE";

  if(getNewOne)  return new C_SPICE();
  return 0;
}

Element* C_SPICE::info_C3(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("C Capacitor 3 pin");
  BitmapFile = (char *) "C_SPICE";

  if(getNewOne)  {
      auto p = new C_SPICE();
      p->Props.at(5)->Value = "3";
      p->recreate(0);
      return p;
  }
  return 0;
}

QString C_SPICE::netlist()
{
    return QString("");
}

QString C_SPICE::spice_netlist(bool)
{
    QString ltr = getProperty("Letter")->Value;
    QString s = spicecompat::check_refdes(Name,ltr);
    for (Port *p1 : Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam+" ";   // node names
    }

    QString C= Props.at(0)->Value;
    QString C_Line_2= Props.at(1)->Value;
    QString C_Line_3= Props.at(2)->Value;
    QString C_Line_4= Props.at(3)->Value;
    QString C_Line_5= Props.at(4)->Value;

    if(  C.length()  > 0)          s += QString("%1").arg(C);
    if(  C_Line_2.length() > 0 )   s += QString("\n%1").arg(C_Line_2);
    if(  C_Line_3.length() > 0 )   s += QString("\n%1").arg(C_Line_3);
    if(  C_Line_4.length() > 0 )   s += QString("\n%1").arg(C_Line_4);
    if(  C_Line_5.length() > 0 )   s += QString("\n%1").arg(C_Line_5);
    s += "\n";
    return s;
}
