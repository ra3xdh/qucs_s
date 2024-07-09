/*
 * potentiometer.cpp - device implementations for potentiometer module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#include "potentiometer.h"
#include "extsimkernels/spicecompat.h"
#include "node.h"

potentiometer::potentiometer()
{
  Description = QObject::tr ("Potentiometer verilog device");
  Simulator = spicecompat::simAll;

  Props.emplace_back ( Property ("R_pot", "10k", true,
    QObject::tr ("nominal device resistance")
    +" ("+QObject::tr ("Ohm")+")"));
  Props.emplace_back ( Property ("Rotation", "120", true,
    QObject::tr ("shaft/wiper arm rotation")
    +" ("+QObject::tr ("degrees")+")"));
  Props.emplace_back ( Property ("Taper_Coeff", "0", false,
    QObject::tr ("resistive law taper coefficient")));
  Props.emplace_back ( Property ("LEVEL", "1", false,
    QObject::tr ("device type selector")+" [1, 2, 3]"));
  Props.emplace_back ( Property ("Max_Rotation", "240.0", true,
    QObject::tr ("maximum shaft/wiper rotation")
    +" ("+QObject::tr ("degrees")+")"));
  Props.emplace_back ( Property ("Conformity", "0.2", false,
    QObject::tr ("conformity error")
    +" ("+QObject::tr ("%")+")"));
  Props.emplace_back ( Property ("Linearity", "0.2", false,
    QObject::tr ("linearity error")
    +" ("+QObject::tr ("%")+")"));
  Props.emplace_back ( Property ("Contact_Res", "1", false,
    QObject::tr ("wiper arm contact resistance")
    +" ("+QObject::tr ("Ohm")+")"));
  Props.emplace_back ( Property ("Temp_Coeff", "100", false,
    QObject::tr ("resistance temperature coefficient")
    +" ("+QObject::tr ("PPM/Celsius")+")"));
  Props.emplace_back ( Property ("Tnom", "26.85", false,
    QObject::tr ("parameter measurement temperature")
    +" ("+QObject::tr ("Celsius")+")"));
  Props.emplace_back ( Property ("Temp", "26.85", false,
    QObject::tr ("simulation temperature")));

  createSymbol ();
  tx = x1 + 8;
  ty = y2 + 4;
  Model = "potentiometer";
  SpiceModel = "R";
  Name  = "POT";
}

Component * potentiometer::newOne()
{
  potentiometer * p = new potentiometer();
  p->Props.front().Value = Props.front().Value;
  p->recreate(0); 
  return p;
}

Element * potentiometer::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Potentiometer");
  BitmapFile = (char *) "potentiometer";

  if(getNewOne) return new potentiometer();
  return 0;
}

void potentiometer::createSymbol()
{
  // frame
  Lines.emplace_back( qucs::Line(-30,-13,-30, 10,QPen(Qt::darkBlue,2)));
  Lines.emplace_back( qucs::Line(-30, 10, 30, 10,QPen(Qt::darkBlue,2)));
  Lines.emplace_back( qucs::Line( 30, 10, 30,-13,QPen(Qt::darkBlue,2)));
  Lines.emplace_back( qucs::Line( 30,-13,-30,-13,QPen(Qt::darkBlue,2)));

  // resistor
  Lines.emplace_back( qucs::Line(-40,  0, -25, 0,QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::FlatCap)));
  Lines.emplace_back( qucs::Line(-25,  0, -20,-5,QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::RoundCap)));
  Lines.emplace_back( qucs::Line(-20, -5, -15, 0,QPen(Qt::darkBlue,2)));
  Lines.emplace_back( qucs::Line(-15,  0, -10,-5,QPen(Qt::darkBlue,2)));
  Lines.emplace_back( qucs::Line(-10, -5, -5,  0,QPen(Qt::darkBlue,2)));
  Lines.emplace_back( qucs::Line( -5,  0,  0, -5,QPen(Qt::darkBlue,2)));
  Lines.emplace_back( qucs::Line(  0, -5,  5,  0,QPen(Qt::darkBlue,2)));
  Lines.emplace_back( qucs::Line(  5,  0, 10, -5,QPen(Qt::darkBlue,2)));
  Lines.emplace_back( qucs::Line( 10, -5, 15,  0,QPen(Qt::darkBlue,2)));
  Lines.emplace_back( qucs::Line( 15,  0, 20, -5,QPen(Qt::darkBlue,2)));
  Lines.emplace_back( qucs::Line( 20, -5, 25,  0,QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::RoundCap)));
  Lines.emplace_back( qucs::Line( 25,  0, 40,  0,QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::FlatCap)));

  // arrow
  Lines.emplace_back( qucs::Line( -4, -9,  0, -5,QPen(Qt::darkBlue,2)));
  Lines.emplace_back( qucs::Line(  4, -9,  0, -5,QPen(Qt::darkBlue,2)));
  Lines.emplace_back( qucs::Line(  0, -5,  0,-20,QPen(Qt::darkBlue,2)));

  Texts.emplace_back( Text(-23,   0, QObject::tr("B"), Qt::black, 6.0, 1.0, 0.0));
  Texts.emplace_back( Text( 18,   0, QObject::tr("T"), Qt::black, 6.0, 1.0, 0.0));

  Ports.emplace_back( Port(-40,   0)); // B
  Ports.emplace_back( Port(  0, -20)); // M
  Ports.emplace_back( Port( 40,   0)); // T

  x1 = -40; y1 = -20;
  x2 =  40; y2 =  15;
}

QString potentiometer::spice_netlist(bool isXyce)
{
    Q_UNUSED(isXyce);
    QString s;
    QString R = getProperty("R_pot")->Value;
    QString rot = getProperty("Rotation")->Value;
    QString max_rot = getProperty("Max_Rotation")->Value;
    QString pin1 = spicecompat::normalize_node_name(port(0).getConnection()->Name);
    QString pin2 = spicecompat::normalize_node_name(port(1).getConnection()->Name);
    QString pin3 = spicecompat::normalize_node_name(port(2).getConnection()->Name);
    s += QString("R%1_1 %2 %3 R='(%4)*(%5)/(%6)'\n").arg(Name).arg(pin1).arg(pin2).arg(R).arg(rot).arg(max_rot);
    s += QString("R%1_2 %2 %3 R='(%4)*(1.0-(%5)/(%6))'\n").arg(Name).arg(pin2).arg(pin3).arg(R).arg(rot).arg(max_rot);
    return s;
}
