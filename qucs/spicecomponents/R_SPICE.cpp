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
    Description = QObject::tr("SPICE R:\nMultiple line ngspice or Xyce R specifications allowed using \"+\" continuation lines.\nLeave continuation lines blank when NOT in use.  ");
    Simulator = spicecompat::simSpice;

    Model = "R_SPICE";
    SpiceModel = "R";
    Name  = "R";

    Props.append(new Property("R", "", true,"R param list and\n .model spec."));
    Props.append(new Property("R_Line 2", "", false,"+ continuation line 1"));
    Props.append(new Property("R_Line 3", "", false,"+ continuation line 2"));
    Props.append(new Property("R_Line 4", "", false,"+ continuation line 3"));
    Props.append(new Property("R_Line 5", "", false,"+ continuation line 4"));
    Props.append(new Property("Pins", "2", true,"[2,3] Pins count"));
    Props.append(new Property("Letter", "R", true,"[R,X,N] SPICE letter"));

    createSymbol();

    tx = x1+4;
    ty = y2+4;

    // rotate();  // fix historical flaw

}

void R_SPICE::createSymbol()
{
    int Npins = getProperty("Pins")->Value.toInt();
    Lines.append(new qucs::Line(-18, -9, 18, -9,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line( 18, -9, 18,  9,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line( 18,  9,-18,  9,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line(-18,  9,-18, -9,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line(-30,  0,-18,  0,QPen(Qt::darkBlue,2)));
    Lines.append(new qucs::Line( 18,  0, 30,  0,QPen(Qt::darkBlue,2)));

    if (Npins >= 3) {
        Lines.append(new qucs::Line(  0,  -10, 0,  -30,QPen(Qt::darkRed,2)));
    }

    Ports.append(new Port(-30,  0));
    Ports.append(new Port( 30,  0));

    if (Npins >= 3) Ports.append(new Port( 0, -30));

    x1 = -30; y1 = -11;
    x2 =  30; y2 =  11;

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
  Name = QObject::tr(" R Resistor");
  BitmapFile = (char *) "R_SPICE";

  if(getNewOne)  return new R_SPICE();
  return 0;
}

Element* R_SPICE::info_R3(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("R Resistor 3 pin");
  BitmapFile = (char *) "R_SPICE";

  if(getNewOne)  {
      auto p = new R_SPICE();
      p->Props.at(5)->Value = "3";
      p->recreate(0);
      return p;
  }
  return 0;
}

QString R_SPICE::netlist()
{
    return QString("");
}

QString R_SPICE::spice_netlist(bool)
{
    QString ltr = getProperty("Letter")->Value;
    QString s = spicecompat::check_refdes(Name,ltr);
    for (Port *p1 : Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " " + nam + " ";   // node names
    }

    QString R= Props.at(0)->Value;
    QString R_Line_2= Props.at(1)->Value;
    QString R_Line_3= Props.at(2)->Value;
    QString R_Line_4= Props.at(3)->Value;
    QString R_Line_5= Props.at(4)->Value;

    if(  R.length()  > 0)          s += QString("%1").arg(R);
    if(  R_Line_2.length() > 0 )   s += QString("\n%1").arg(R_Line_2);
    if(  R_Line_3.length() > 0 )   s += QString("\n%1").arg(R_Line_3);
    if(  R_Line_4.length() > 0 )   s += QString("\n%1").arg(R_Line_4);
    if(  R_Line_5.length() > 0)    s += QString("\n%1").arg(R_Line_5);
    s += "\n";

    return s;
}
