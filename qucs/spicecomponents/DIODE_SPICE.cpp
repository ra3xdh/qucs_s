/***************************************************************************
                         DIODE_SPICE.cpp  -  description
                   --------------------------------------
    begin                    : Fri Mar 9 2007
    copyright              : (C) 2007 by Gunther Kraut
    email                     : gn.kraut@t-online.de
    spice4qucs code added  Wed. 27 May 2015
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
#include "DIODE_SPICE.h"
#include "node.h"
#include "extsimkernels/spicecompat.h"


DIODE_SPICE::DIODE_SPICE()
{
    Description = QObject::tr("SPICE D:\nMultiple line ngspice or Xyce D model specifications allowed using \"+\" continuation lines.\nLeave continuation lines blank when NOT in use.");
    Simulator = spicecompat::simSpice;

    Model = "DIODE_SPICE";
    SpiceModel = "D";
    Name  = "D";

    Props.append(new Property("D", "", true,"Param list and\n .model spec."));
    Props.append(new Property("D_Line 2", "", false,"+ continuation line 1"));
    Props.append(new Property("D_Line 3", "", false,"+ continuation line 2"));
    Props.append(new Property("D_Line 4", "", false,"+ continuation line 3"));
    Props.append(new Property("D_Line 5", "", false,"+ continuation line 4"));
    Props.append(new Property("Pins", "2", true,"[2,3] Pins count"));
    Props.append(new Property("Letter", "D", true,"[D,X,N] SPICE letter"));

    createSymbol();
    tx = x1+4;
    ty = y2+4;
}

void DIODE_SPICE::createSymbol()
{
    int Npins = getProperty("Pins")->Value.toInt();
    Lines.append(new qucs::Line(-30,  0, -20,  0,QPen(Qt::darkBlue,3)));
    Lines.append(new qucs::Line( -20, 0, -6,   0,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line(  6,  0, 20,   0,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line( 20,  0, 30,   0,QPen(Qt::darkBlue,3)));


    Lines.append(new qucs::Line( -6, -9, -6,  9,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line(  6, -9,  6,  9,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line( -6, -9,  6,  0,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line( -6,  9,  6,  0,QPen(Qt::darkRed,3)));

    if (Npins >= 3) {
        Lines.append(new qucs::Line(  0,  -5, 0,  -30,QPen(Qt::darkRed,2)));
    }

    Ports.append(new Port(-30, 0));
    Ports.append(new Port( 30, 0));

    if (Npins >= 3) Ports.append(new Port( 0, -30));

    x1 = -30; y1 = -11;
    x2 =  30; y2 =  11;

}

DIODE_SPICE::~DIODE_SPICE()
{
}

Component* DIODE_SPICE::newOne()
{
    auto p = new DIODE_SPICE();
    p->getProperty("Pins")->Value = getProperty("Pins")->Value;
    p->getProperty("Letter")->Value = getProperty("Letter")->Value;
    p->recreate();
    return p;
}

Element* DIODE_SPICE::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("D Diode");
  BitmapFile = (char *) "DIODE_SPICE";

  if(getNewOne)  return new DIODE_SPICE();
  return 0;
}

Element* DIODE_SPICE::info_DIODE3(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("D Diode 3 pin");
  BitmapFile = (char *) "DIODE_SPICE";

  if(getNewOne)  {
      auto p = new DIODE_SPICE();
      p->Props.at(5)->Value = "3";
      p->recreate();
      return p;
  }
  return 0;
}

QString DIODE_SPICE::netlist()
{
    return QString();
}

QString DIODE_SPICE::spice_netlist(spicecompat::SpiceDialect dialect /* = spicecompat::SPICEDefault */)
{
    Q_UNUSED(dialect);

    QString ltr = getProperty("Letter")->Value;
    QString s = spicecompat::check_refdes(Name,ltr);
    for (Port *p1 : Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam+" ";   // node names
    }

    QString D= Props.at(0)->Value;
    QString D_Line_2= Props.at(1)->Value;
    QString D_Line_3= Props.at(2)->Value;
    QString D_Line_4= Props.at(3)->Value;
    QString D_Line_5= Props.at(4)->Value;

    if(  D.length()  > 0)          s += QStringLiteral("%1").arg(D);
    if(  D_Line_2.length() > 0 )   s += QStringLiteral("\n%1").arg(D_Line_2);
    if(  D_Line_3.length() > 0 )   s += QStringLiteral("\n%1").arg(D_Line_3);
    if(  D_Line_4.length() > 0 )   s += QStringLiteral("\n%1").arg(D_Line_4);
    if(  D_Line_5.length() > 0 )   s += QStringLiteral("\n%1").arg(D_Line_5);

    s += "\n";

    return s;
}

QString DIODE_SPICE::cdl_netlist()
{
    if (Ports.size() == 2)
    {
        return spice_netlist(spicecompat::CDL);
    }

    return QString();
}
