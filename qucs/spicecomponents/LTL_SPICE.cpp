/***************************************************************************
                         LTL_SPICE.cpp  -  description
                   --------------------------------------
    spice4qucs code : Wed. 13 May 2015
    copyright       : (C) 2015 by Mike Brinson
    email           : mbrin72043@yahoo.co.uk

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "LTL_SPICE.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


LTL_SPICE::LTL_SPICE()
{
  Description = QObject::tr("SPICE T:");
  
  Arcs.append(new qucs::Arc(-28,-40, 18, 38,16*232, 16*33,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(-28,  2, 18, 38, 16*95, 16*33,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-20,-2, 20,-2,QPen(Qt::darkRed,3)));
  Lines.append(new qucs::Line(-20, 2, 20, 2,QPen(Qt::darkRed,3)));

  Arcs.append(new qucs::Arc( 10,-40, 18, 38,16*270, 16*40,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc( 10,  2, 18, 38, 16*50, 16*40,QPen(Qt::darkBlue,2)));

  Arcs.append(new qucs::Arc(-38,-10, 16, 28, 16*45, 16*45,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(-38,-18, 16, 28,16*270, 16*45,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc( 22,-10, 16, 28, 16*90, 16*45,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc( 22,-18, 16, 28,16*225, 16*45,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-30,-10));
  Ports.append(new Port(-30, 10));
  Ports.append(new Port( 30,-10));
  Ports.append(new Port( 30, 10)); 
  
  x1 = -30; y1 =-12;
  x2 =  30; y2 = 12;

  tx = x1+4;
  ty = y2+4;

  Model = "LTL_SPICE";
  SpiceModel = "T";
  Name  = "T";

  Props.append(new Property("Z0", "50 ", true,     QObject::tr("Characteristic impedance")));
  Props.append(new Property("Td", "0.25n ", true,  QObject::tr("Transmission delay")));
  Props.append(new Property("F", "1e9", true,      QObject::tr("Frequency")));
  Props.append(new Property("Nl", "0.25", true,    QObject::tr("Normalised length at given frequency")));
  Props.append(new Property("V1", "0", true,       QObject::tr("Initial voltage at end 1")));
  Props.append(new Property("I1", "0", true,       QObject::tr("Initial current at end 1")));
  Props.append(new Property("V2", "0", true,       QObject::tr("Initial voltage at end 2")));
  Props.append(new Property("I2", "0", true,       QObject::tr("Initial current at end 2")));  

}

LTL_SPICE::~LTL_SPICE()
{
}

Component* LTL_SPICE::newOne()
{
  return new LTL_SPICE();
}

Element* LTL_SPICE::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("T");
  BitmapFile = (char *) "LTL_SPICE";

  if(getNewOne)  return new LTL_SPICE();
  return 0;
}

QString LTL_SPICE::netlist()
{
    return QString("");
}

QString LTL_SPICE::spice_netlist(bool)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    foreach(Port *p1, Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam;   // node names
    }

   QString Z0 = spicecompat::normalize_value(Props.at(0)->Value);
   QString Td = spicecompat::normalize_value(Props.at(1)->Value);
   QString Freq = spicecompat::normalize_value(Props.at(2)->Value);
   QString Nl = spicecompat::normalize_value(Props.at(3)->Value);
   QString V1 = spicecompat::normalize_value(Props.at(4)->Value);
   QString I1 = spicecompat::normalize_value(Props.at(5)->Value);
   QString V2 = spicecompat::normalize_value(Props.at(6)->Value);
   QString I2 = spicecompat::normalize_value(Props.at(7)->Value);



    s += QString(" Z0=%1 Td=%2 F=%3 NL=%4 IC=%5, %6, %7, %8 \n").arg(Z0).arg(Td).arg(Freq).arg(Nl).arg(V1).arg(I1).arg(V2).arg(I2);
    return s;
}
