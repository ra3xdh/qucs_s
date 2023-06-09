/***************************************************************************
                         vTRNOISE.cpp  -  description
                   --------------------------------------
    begin                    : Fri Mar 9 2007
    copyright              : (C) 2007 by Gunther Kraut
    email                     : gn.kraut@t-online.de
    spice4qucs code added  Tue. 31 March 2015
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
#include "vTRNOISE.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


vTRNOISE::vTRNOISE()
{
  Description = QObject::tr("SPICE V(TRNOISE): ");
  Simulator = spicecompat::simSpice;

  // normal voltage source symbol
  Arcs.append(new qucs::Arc(-12,-12, 24, 24, 0, 16*360,QPen(Qt::blue,3)));
   Texts.append(new Text(36, 4,"VTRN",Qt::blue,10.0,0.0,-1.0)); 
  Lines.append(new qucs::Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  
  Lines.append(new qucs::Line(-12,  1,  1,-12,QPen(Qt::darkGray,3)));
  Lines.append(new qucs::Line(-10,  6,  6,-10,QPen(Qt::darkGray,3)));
  Lines.append(new qucs::Line( -7, 10, 10, -7,QPen(Qt::darkGray,3)));
  Lines.append(new qucs::Line( -2, 12, 12, -2,QPen(Qt::darkGray,3)));

  
  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  40;

  tx = x1+4;
  ty = y2+4;
  Model = "vTRNOISE";
  SpiceModel = "V";
  Name  = "V";

  Props.append(new Property("Na",  "20n", true,
		QObject::tr(" Rms noise amplitude Gaussian)")));
  Props.append(new Property("Nt",  "0.5n", true,
		QObject::tr("Time step")));
  Props.append(new Property("Nalpha",  "1.1", true,
		QObject::tr("1/f exponent (0  < alpha < 2)")));
  Props.append(new Property("Namp",  "12p", true,
		QObject::tr("Amplitude (1/f)")));
   Props.append(new Property("Rtsam",  "0", true,
		QObject::tr("Amplitude (1/f)")));
  Props.append(new Property("Rtscapt",  "0", true,
		QObject::tr("Trap capture time")));
  Props.append(new Property("Rtsemt",  "0", true,
		QObject::tr("Trap emission time" )));

  rotate();  // fix historical flaw
}

vTRNOISE::~vTRNOISE()
{
}

Component* vTRNOISE::newOne()
{
  return new vTRNOISE();
}

Element* vTRNOISE::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("V(TRNOISE)");
  BitmapFile = (char *) "vTRNOISE";

  if(getNewOne)  return new vTRNOISE();
  return 0;
}

QString vTRNOISE::netlist()
{
    return QString("");
}

QString vTRNOISE::spice_netlist(bool)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    for (Port *p1 : Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam;   // node names
    }

   QString Na= spicecompat::normalize_value(Props.at(0)->Value);
   QString Nt= spicecompat::normalize_value(Props.at(1)->Value);
   QString Nalpha= spicecompat::normalize_value(Props.at(2)->Value);
   QString Namp = spicecompat::normalize_value(Props.at(3)->Value);
   QString Rtsam = spicecompat::normalize_value(Props.at(4)->Value);
   QString Rtscapt = spicecompat::normalize_value(Props.at(5)->Value);
   QString Rtsemt = spicecompat::normalize_value(Props.at(6)->Value);


    s += QString(" DC 0 AC 0 TRNOISE(%1 %2 %3 %4 %5  %6 %7) \n").arg(Na).arg(Nt).arg(Nalpha).arg(Namp).
                                arg(Rtsam).arg(Rtscapt).arg(Rtsemt);
    return s;
}
