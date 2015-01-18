/***************************************************************************
                               resistor.cpp
                              --------------
    begin                : Sat Aug 23 2003
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
#include "resistor.h"
#include "node.h"
#include "schematic.h"
#include "misc.h"


Resistor::Resistor(bool european)
{
  Description = QObject::tr("resistor");

  Props.append(new Property("R", "50 Ohm", true,
    QObject::tr("ohmic resistance in Ohms")));
  Props.append(new Property("Temp", "26.85", false,
    QObject::tr("simulation temperature in degree Celsius")));
  Props.append(new Property("Tc1", "0.0", false,
    QObject::tr("first order temperature coefficient")));
  Props.append(new Property("Tc2", "0.0", false,
    QObject::tr("second order temperature coefficient")));
  Props.append(new Property("Tnom", "26.85", false,
    QObject::tr("temperature at which parameters were extracted")));

  // this must be the last property in the list !!!
  Props.append(new Property("Symbol", "european", false,
        QObject::tr("schematic symbol")+" [european, US]"));
  if(!european)  Props.getLast()->Value = "US";

  createSymbol();
  tx = x1+4;
  ty = y2+4;
  Model = "R";
  SpiceModel = "R";
  Name  = "R";
}

// -------------------------------------------------------
Component* Resistor::newOne()
{
  return new Resistor(Props.getLast()->Value != "US");
}

QString Resistor::spice_netlist(bool isXyce)
{
    QString s=check_spice_refdes();

    s += QString(" %1 %2 ").arg(Ports.at(0)->Connection->Name)
            .arg(Ports.at(1)->Connection->Name); // output 2 nodes
    s.replace(" gnd ", " 0 ");

    QString unit;
    double R,fac;
    str2num(Props.at(0)->Value,R,unit,fac);
    s += QString(" %1\n").arg(R*fac);

    return s;
}

// -------------------------------------------------------
void Resistor::createSymbol()
{
  if(Props.getLast()->Value != "US") {
    Lines.append(new Line(-18, -9, 18, -9,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( 18, -9, 18,  9,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( 18,  9,-18,  9,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-18,  9,-18, -9,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-30,  0,-18,  0,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( 18,  0, 30,  0,QPen(Qt::darkBlue,2)));
  }
  else {
    Lines.append(new Line(-30,  0,-18,  0,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-18,  0,-15, -7,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-15, -7, -9,  7,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( -9,  7, -3, -7,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( -3, -7,  3,  7,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(  3,  7,  9, -7,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(  9, -7, 15,  7,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( 15,  7, 18,  0,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( 18,  0, 30,  0,QPen(Qt::darkBlue,2)));
  }

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -11;
  x2 =  30; y2 =  11;
}

// -------------------------------------------------------
Element* Resistor::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Resistor");
  BitmapFile = (char *) "resistor";

  if(getNewOne)  return new Resistor();
  return 0;
}

// -------------------------------------------------------
Element* Resistor::info_us(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Resistor US");
  BitmapFile = (char *) "resistor_us";

  if(getNewOne)  return new Resistor(false);
  return 0;
}
