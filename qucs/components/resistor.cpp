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
#include "extsimkernels/spicecompat.h"
#include "extsimkernels/verilogawriter.h"


Resistor::Resistor(bool european)
{
  Description = QObject::tr("resistor");

  Props.emplace_back( Property("R", "1 kOhm", true,
    QObject::tr("ohmic resistance in Ohms")));
  Props.emplace_back( Property("Temp", "26.85", false,
    QObject::tr("simulation temperature in degree Celsius (Qucsator only)")));
  Props.emplace_back( Property("Tc1", "0.0", false,
    QObject::tr("first order temperature coefficient")));
  Props.emplace_back( Property("Tc2", "0.0", false,
    QObject::tr("second order temperature coefficient")));
  Props.emplace_back( Property("Tnom", "26.85", false,
    QObject::tr("temperature at which parameters were extracted (Qucsator only)")));

  // this must be the last property in the list !!!
  Props.emplace_back( Property("Symbol", "european", false,
        QObject::tr("schematic symbol")+" [european, US]"));
  if(!european)  Props.back().Value = "US";

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
  return new Resistor(Props.back().Value != "US");
}

QString Resistor::spice_netlist(bool )
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);

    s += QString(" %1 %2 ").arg(port(0).getConnection()->Name)
            .arg(port(1).getConnection()->Name); // output 2 nodes
    s.replace(" gnd ", " 0 ");

    QString Tc1 = getProperty("Tc1")->Value;
    QString Tc2 = getProperty("Tc2")->Value;

    s += QString(" %1").arg(spicecompat::normalize_value(prop(0).Value));

    if (!Tc1.isEmpty()) {
        s += " tc1=" + Tc1;
    }

    if (!Tc2.isEmpty()) {
        s += " tc2=" + Tc2;
    }

    s += QString(" \n");

    return s;
}

QString Resistor::va_code()
{
    QString val = vacompat::normalize_value(prop(0).Value);
    QString valTemp = vacompat::normalize_value(prop(1).Value);
    QString plus = port(0).getConnection()->Name;
    QString minus = port(1).getConnection()->Name;
    QString s = "";
    QString Vpm = vacompat::normalize_voltage(plus,minus);
    QString Ipm = vacompat::normalize_current(plus,minus,true);
    
    if (plus=="gnd") s += QString("%1 <+ -(%2/( %3 ));\n").arg(Ipm).arg(Vpm).arg(val);
    else s+= QString("%1 <+ %2/( %3 );\n").arg(Ipm).arg(Vpm).arg(val);
    s += QString("%1 <+ white_noise( 4.0*`P_K*( %2 + 273.15) / ( %3 ), \"thermal\" );\n")
                 .arg(Ipm).arg(valTemp).arg(val);
                  
    return s;
}

// -------------------------------------------------------
void Resistor::createSymbol()
{
  if(Props.back().Value != "US") {
    Lines.emplace_back( qucs::Line(-18, -9, 18, -9,QPen(Qt::darkBlue,2)));
    Lines.emplace_back( qucs::Line( 18, -9, 18,  9,QPen(Qt::darkBlue,2)));
    Lines.emplace_back( qucs::Line( 18,  9,-18,  9,QPen(Qt::darkBlue,2)));
    Lines.emplace_back( qucs::Line(-18,  9,-18, -9,QPen(Qt::darkBlue,2)));
    Lines.emplace_back( qucs::Line(-30,  0,-18,  0,QPen(Qt::darkBlue,2)));
    Lines.emplace_back( qucs::Line( 18,  0, 30,  0,QPen(Qt::darkBlue,2)));
  }
  else {
    Lines.emplace_back( qucs::Line(-30,  0,-18,  0,QPen(Qt::darkBlue,2)));
    Lines.emplace_back( qucs::Line(-18,  0,-15, -7,QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::RoundCap)));
    Lines.emplace_back( qucs::Line(-15, -7, -9,  7,QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::RoundCap)));
    Lines.emplace_back( qucs::Line( -9,  7, -3, -7,QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::RoundCap)));
    Lines.emplace_back( qucs::Line( -3, -7,  3,  7,QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::RoundCap)));
    Lines.emplace_back( qucs::Line(  3,  7,  9, -7,QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::RoundCap)));
    Lines.emplace_back( qucs::Line(  9, -7, 15,  7,QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::RoundCap)));
    Lines.emplace_back( qucs::Line( 15,  7, 18,  0,QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::RoundCap)));
    Lines.emplace_back( qucs::Line( 18,  0, 30,  0,QPen(Qt::darkBlue,2)));
  }

  Ports.emplace_back( Port(-30,  0));
  Ports.emplace_back( Port( 30,  0));

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
