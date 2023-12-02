/***************************************************************************
                         NPN_SPICE.cpp  -  description
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
#include "BJT_SPICE.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


BJT_SPICE::BJT_SPICE()
{
    Description = QObject::tr("Q(NPN) BJT:\nMultiple line ngspice or Xyce Q model specifications allowed using \"+\" continuation lines.\nLeave continuation lines blank when NOT in use.");
    Simulator = spicecompat::simSpice;

    x1 = -30; y1 = -30;
    x2 =   4; y2 =  30;

    tx = x1+4;
    ty = y2+4;

    Model = "BJT_SPICE";
    SpiceModel = "Q";
    Name  = "Q";

    Props.append(new Property("Pins", "4", true,"[3,4,5] Pins count"));
    Props.append(new Property("type", "npn", true,"[npn,pnp] BJT type"));
    Props.append(new Property("Letter", "Q", true,"[npn,pnp] BJT type"));
    Props.append(new Property("Model", "", true,"Param list and\n .model spec."));
    Props.append(new Property("Model_Line 2", "", false,"+ continuation line 1"));
    Props.append(new Property("Model_Line 3", "", false,"+ continuation line 2"));
    Props.append(new Property("Model_Line 4", "", false,"+ continuation line 3"));
    Props.append(new Property("Model_Line 5", "", false,"+ continuation line 4"));

    createSymbol();

}


void BJT_SPICE::createSymbol()
{
    int Npins = getProperty("Pins")->Value.toInt();

    Lines.append(new qucs::Line(-10,-15,-10, 15,QPen(Qt::darkRed,3)));

    Lines.append(new qucs::Line(-30,  0,-20,  0,QPen(Qt::darkBlue,3)));
    Lines.append(new qucs::Line(-20,  0,-10,  0,QPen(Qt::darkRed,3)));

    Lines.append(new qucs::Line(-10, -5,  0,-15,QPen(Qt::darkRed,3)));

    Lines.append(new qucs::Line(  0,-15,  0,-20,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line(  0,-20,  0,-30,QPen(Qt::darkBlue,3)));

    Lines.append(new qucs::Line(-10,  5,  0, 15,QPen(Qt::darkRed,3)));

    Lines.append(new qucs::Line(  0, 15,  0, 20,QPen(Qt::darkRed,3)));
    Lines.append(new qucs::Line(  0, 20,  0, 30,QPen(Qt::darkBlue,3)));

    QString type = getProperty("type")->Value;
    if (type == "npn") {
        Lines.append(new qucs::Line( -6, 15,  0, 15,QPen(Qt::darkRed,3)));
        Lines.append(new qucs::Line(  0,  9,  0, 15,QPen(Qt::darkRed,3)));
    } else if (type == "pnp") {
        Lines.append(new qucs::Line( -5, 10, -5, 16,QPen(Qt::darkRed,3)));
        Lines.append(new qucs::Line( -5, 10,  1, 10,QPen(Qt::darkRed,3)));
    }

    // substrate node
    if (Npins >= 4) {
        Lines.append(new qucs::Line(  9,  0, 30,  0,QPen(Qt::darkRed,2)));
        Lines.append(new qucs::Line(  9, -7,  9,  7,QPen(Qt::darkRed,3)));
    }
    // thermal node
    if (Npins >= 5) {
        Lines.append(new qucs::Line(-30, 20,-20, 20,QPen(Qt::darkRed,2)));
        Lines.append(new qucs::Line(-20, 17,-20, 23,QPen(Qt::darkRed,2)));
    }

    Ports.append(new Port(0, -30));
    Ports.append(new Port(-30, 0));
    Ports.append(new Port(  0, 30));
    if (Npins >= 4) Ports.append(new Port( 30,  0)); // substrate
    if (Npins >= 5) Ports.append(new Port(-30, 20)); // thermal node
}

BJT_SPICE::~BJT_SPICE()
{
}

Component* BJT_SPICE::newOne()
{
  return new BJT_SPICE();
}

Element* BJT_SPICE::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Q(NPN) BJT");
  BitmapFile = (char *) "NPN_SPICE";

  if(getNewOne)  return new BJT_SPICE();
  return 0;
}

Element* BJT_SPICE::infoNPN4(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Q(NPN) 4 pin");
  BitmapFile = (char *) "NPN4_SPICE";

  if(getNewOne)  {
      auto p = new BJT_SPICE();
      p->Props.at(0)->Value = "4";
      p->Props.at(1)->Value = "npn";
      p->recreate(0);
      return p;
  }
  return 0;
}

Element* BJT_SPICE::infoPNP4(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Q(PNP) 4 pin");
  BitmapFile = (char *) "PNP4_SPICE";

  if(getNewOne)  {
      auto p = new BJT_SPICE();
      p->Props.at(0)->Value = "4";
      p->Props.at(1)->Value = "pnp";
      p->recreate(0);
      return p;
  }
  return 0;
}


Element* BJT_SPICE::infoNPN5(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Q(NPN) 5 pin");
  BitmapFile = (char *) "NPN5_SPICE";

  if(getNewOne)  {
      auto p = new BJT_SPICE();
      p->Props.at(0)->Value = "5";
      p->Props.at(1)->Value = "npn";
      p->recreate(0);
      return p;
  }
  return 0;
}

Element* BJT_SPICE::infoPNP5(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Q(PNP) 5 pin");
  BitmapFile = (char *) "PNP5_SPICE";

  if(getNewOne)  {
      auto p = new BJT_SPICE();
      p->Props.at(0)->Value = "5";
      p->Props.at(1)->Value = "pnp";
      p->recreate(0);
      return p;
  }
  return 0;
}

QString BJT_SPICE::netlist()
{
    return QString("");
}

QString BJT_SPICE::spice_netlist(bool)
{
    QString ltr =getProperty("Letter")->Value;
    QString s = spicecompat::check_refdes(Name,ltr);
    for (Port *p1 : Ports) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam+" ";   // node names
    }
 
    QString Q= Props.at(3)->Value;
    QString Q_Line_2= Props.at(4)->Value;
    QString Q_Line_3= Props.at(5)->Value;
    QString Q_Line_4= Props.at(6)->Value;
    QString Q_Line_5= Props.at(7)->Value;

    if(  Q.length()  > 0)          s += QString("%1").arg(Q);
    if(  Q_Line_2.length() > 0 )   s += QString("\n%1").arg(Q_Line_2);
    if(  Q_Line_3.length() > 0 )   s += QString("\n%1").arg(Q_Line_3);
    if(  Q_Line_4.length() > 0 )   s += QString("\n%1").arg(Q_Line_4);
    if(  Q_Line_5.length() > 0 )   s += QString("\n%1").arg(Q_Line_5);
    s += "\n";

    return s;
}
