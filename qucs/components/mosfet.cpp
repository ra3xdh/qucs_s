/***************************************************************************
                                mosfet.cpp
                               ------------
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

#include "mosfet.h"
#include "node.h"
#include "misc.h"


MOSFET::MOSFET()
{
  // properties obtained from "Basic_MOSFET" in mosfet_sub.cpp
  Description = QObject::tr("MOS field-effect transistor");
  createSymbol();
  tx = x2+4;
  ty = y1+4;
  Model = "_MOSFET";
}

// -------------------------------------------------------
Component* MOSFET::newOne()
{
  MOSFET* p = new MOSFET();
  p->Props.first()->Value = Props.first()->Value;
  p->Props.next()->Value = Props.next()->Value;
  p->recreate(0);
  return p;
}

// -------------------------------------------------------
Element* MOSFET::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("n-MOSFET");
  BitmapFile = (char *) "nmosfet";

  if(getNewOne)  return new MOSFET();
  return 0;
}

// -------------------------------------------------------
Element* MOSFET::info_p(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("p-MOSFET");
  BitmapFile = (char *) "pmosfet";

  if(getNewOne) {
    MOSFET* p = new MOSFET();
    p->Props.first()->Value = "pfet";
    p->Props.next()->Value = "-1.0 V";
    p->recreate(0);
    return p;
  }
  return 0;
}

// -------------------------------------------------------
Element* MOSFET::info_depl(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("depletion MOSFET");
  BitmapFile = (char *) "dmosfet";

  if(getNewOne) {
    MOSFET* p = new MOSFET();
    p->Props.first();
    p->Props.next()->Value = "-1.0 V";
    p->recreate(0);
    return p;
  }
  return 0;
}

// -------------------------------------------------------
void MOSFET::createSymbol()
{
  Lines.append(new Line(-14,-13,-14, 13,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(-30,  0,-14,  0,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-10,-11,  0,-11,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0,-11,  0,-30,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-10, 11,  0, 11,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0,  0,  0, 30,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-10,  0,  0,  0,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-10,-16,-10, -7,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(-10,  7,-10, 16,QPen(Qt::darkBlue,3)));

  if(Props.first()->Value == "nfet") {
    Lines.append(new Line( -9,  0, -4, -5,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( -9,  0, -4,  5,QPen(Qt::darkBlue,2)));
  }
  else {
    Lines.append(new Line( -1,  0, -6, -5,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( -1,  0, -6,  5,QPen(Qt::darkBlue,2)));
  }

  if((Props.next()->Value.trimmed().at(0) == '-') ==
     (Props.first()->Value == "nfet"))
    Lines.append(new Line(-10, -8,-10,  8,QPen(Qt::darkBlue,3)));
  else
    Lines.append(new Line(-10, -4,-10,  4,QPen(Qt::darkBlue,3)));
  
  Ports.append(new Port(-30,  0));
  Ports.append(new Port(  0,-30));
  Ports.append(new Port(  0, 30));

  x1 = -30; y1 = -30;
  x2 =   4; y2 =  30;
}

// -------------------------------------------------------
QString MOSFET::netlist()
{
  QString s = "MOSFET:"+Name;

  // output all node names
  foreach(Port *p1, Ports)
    s += " "+p1->Connection->Name;   // node names
  s += " "+Ports.at(2)->Connection->Name;  // connect substrate to source

  // output all properties
  for(Property *p2 = Props.first(); p2 != 0; p2 = Props.next())
    s += " "+p2->Name+"=\""+p2->Value+"\"";

  return s + '\n';
}

QString MOSFET::spice_netlist(bool isXyce)
{
    QString s = check_spice_refdes();
    QList<int> pin_seq;
    pin_seq<<1<<0<<2<<2; // Pin sequence: DGS; coonect substrate to source
    // output all node names
    foreach(int pin, pin_seq) {
        QString nam = Ports.at(pin)->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam;   // node names
    }

    QStringList spice_incompat,spice_tr;
    spice_incompat<<"Type"<<"Temp"<<"L"<<"W"<<"Ad"<<"As"<<"Pd"<<"Ps"
                 <<"Rg"<<"N"<<"Tt"<<"Nrd"<<"Nrs"<<"Ffe";
                              // spice-incompatible parameters
    if (isXyce) {
        spice_tr<<"Vt0"<<"VtO"; // parameters that need convertion of names
    } else {
        spice_tr.clear();
    }


    QStringList check_defaults_list;
    QString unit;
    check_defaults_list<<"Nsub"<<"Nss";
    foreach (QString parnam,check_defaults_list) { // Check some parameters for default value (zero)
        double val,fac;   // And reduce parameter list
        str2num(getProperty(parnam)->Value,val,unit,fac);
        if ((val *= fac)==0.0) {
            spice_incompat.append(parnam);
        }
    }

    QString par_str = form_spice_param_list(spice_incompat,spice_tr);

    QString mosfet_type = getProperty("Type")->Value.at(0).toUpper();

    double l,w,as,ad,ps,pd,fac;
    str2num(getProperty("L")->Value,l,unit,fac);
    l *= fac;
    str2num(getProperty("W")->Value,w,unit,fac);
    w *= fac;
    str2num(getProperty("Ad")->Value,ad,unit,fac);
    ad *= fac;
    str2num(getProperty("As")->Value,as,unit,fac);
    as *= fac;
    str2num(getProperty("Pd")->Value,pd,unit,fac);
    pd *= fac;
    str2num(getProperty("Ps")->Value,ps,unit,fac);
    ps *= fac;

    s += QString(" MMOD_%1 L=%2 W=%3 Ad=%4 As=%5 Pd=%6 Ps=%7 Temp=%8\n")
            .arg(Name).arg(l).arg(w).arg(ad).arg(as).arg(pd).arg(ps).arg(getProperty("Temp")->Value);
    s += QString(".MODEL MMOD_%1 %2MOS (%3)\n").arg(Name).arg(mosfet_type).arg(par_str);

    return s;
}
