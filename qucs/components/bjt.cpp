/***************************************************************************
                                bjt.cpp
                               ---------
    begin                : Fri Jun 4 2004
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "bjt.h"
#include "node.h"
#include "extsimkernels/spicecompat.h"


BJT::BJT()
{
  // properties obtained from "Basic_BJT" in bjtsub.cpp
  Description = QObject::tr("bipolar junction transistor");
  createSymbol();
  tx = x2+4;
  ty = y1+4;
  Model = "_BJT";
  SpiceModel="Q";
}

// -------------------------------------------------------
Component* BJT::newOne()
{
  BJT* p = new BJT();
  p->Props.getFirst()->Value = Props.getFirst()->Value;
  p->recreate(0);
  return p;
}

QString BJT::spice_netlist(bool isXyce)
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    QList<int> pin_seq;
    pin_seq<<1<<0<<2; // Pin sequence: CBE
    // output all node names
    foreach(int pin, pin_seq) {
        QString nam = Ports.at(pin)->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam;   // node names
    }

    QStringList spice_incompat,spice_tr;
    spice_incompat<<"Type"<<"Area"<<"Temp"<<"Ffe"<<"Kb"<<"Ab"<<"Fb"; // spice-incompatible parameters
    spice_tr.clear(); // parameters that need convertion of names

    QString par_str = form_spice_param_list(spice_incompat,spice_tr);

    s += QString(" QMOD_%1 AREA=%2 TEMP=%3\n").arg(Name).arg(getProperty("Area")->Value)
            .arg(getProperty("Temp")->Value);
    s += QString(".MODEL QMOD_%1 %2 (%3)\n").arg(Name).arg(getProperty("Type")->Value).arg(par_str);

    return s;
}

// -------------------------------------------------------
Element* BJT::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("npn transistor");
  BitmapFile = (char *) "npn";

  if(getNewOne)  return new BJT();
  return 0;
}

// -------------------------------------------------------
Element* BJT::info_pnp(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("pnp transistor");
  BitmapFile = (char *) "pnp";

  if(getNewOne) {
    BJT* p = new BJT();
    p->Props.getFirst()->Value = "pnp";
    p->recreate(0);
    return p;
  }
  return 0;
}

// -------------------------------------------------------
void BJT::createSymbol()
{
  Lines.append(new Line(-10,-15,-10, 15,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(-30,  0,-10,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-10, -5,  0,-15,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0,-15,  0,-30,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-10,  5,  0, 15,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0, 15,  0, 30,QPen(Qt::darkBlue,2)));

  if(Props.getFirst()->Value == "npn") {
    Lines.append(new Line( -6, 15,  0, 15,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(  0,  9,  0, 15,QPen(Qt::darkBlue,2)));
  }
  else {
    Lines.append(new Line( -5, 10, -5, 16,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( -5, 10,  1, 10,QPen(Qt::darkBlue,2)));
  }

  Ports.append(new Port(-30,  0));
  Ports.append(new Port(  0,-30));
  Ports.append(new Port(  0, 30));

  x1 = -30; y1 = -30;
  x2 =   4; y2 =  30;
}

// -------------------------------------------------------
QString BJT::netlist()
{
  QString s = "BJT:"+Name;

  // output all node names
  foreach(Port *p1, Ports)
    s += " "+p1->Connection->Name;   // node names
  s += " "+Ports.at(1)->Connection->Name;  // connect substrate to collector

  // output all properties
  for(Property *p2 = Props.first(); p2 != 0; p2 = Props.next())
    s += " "+p2->Name+"=\""+p2->Value+"\"";

  return s + '\n';
}
