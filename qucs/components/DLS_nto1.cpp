/*
 * DLS_nto1.cpp - device implementations for DLS_nto1 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#include "DLS_nto1.h"

DLS_nto1::DLS_nto1()
{
  Description = QObject::tr ("data voltage level shifter (analogue to digital) verilog device");

  Props.append (new Property ("LEVEL", "5 V", false,
    QObject::tr ("voltage level")
    +" ("+QObject::tr ("V")+")"));
  Props.append (new Property ("Delay", "1 ns", false,
    QObject::tr ("time delay")
    +" ("+QObject::tr ("s")+")"));

  createSymbol ();
  tx = x1 + 14;
  ty = y2 + 4;
  Model = "DLS_nto1";
  Name  = "Y";
}

Component * DLS_nto1::newOne()
{
  DLS_nto1 * p = new DLS_nto1();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * DLS_nto1::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("A2D Level Shifter");
  BitmapFile = (char *) "DLS_nto1";

  if(getNewOne) return new DLS_nto1();
  return 0;
}

void DLS_nto1::createSymbol()
{
  Lines.append(new qucs::Line(-30, -30, 30,-30,QPen(Qt::darkRed,2)));
  Lines.append(new qucs::Line( 30, -30, 30, 30,QPen(Qt::darkRed,2)));
  Lines.append(new qucs::Line( 30, 30,-30, 30,QPen(Qt::darkRed,2)));
  Lines.append(new qucs::Line(-30, 30, -30, -30,QPen(Qt::darkRed,2)));
 
  Lines.append(new qucs::Line(-30, 30, 30, -30,QPen(Qt::darkRed,2)));

  Lines.append(new qucs::Line(-40,  0,-30,  0,QPen(Qt::darkRed,2)));  // Lin
  Lines.append(new qucs::Line( 30,  0, 40,  0,QPen(Qt::darkRed,2)));  // Lout
  
  Lines.append(new qucs::Line(-25, -20,-15, -20,QPen(Qt::darkRed,2)));
  Lines.append(new qucs::Line( 25,  20, 15,  20,QPen(Qt::darkRed,2)));
  
  Texts.append(new Text(-10,-32, "n", Qt::darkRed, 12.0));
  Texts.append(new Text(  0,  8, "1", Qt::darkRed, 12.0));
 
  Ports.append(new Port(-40, 0));  // Lin
  Ports.append(new Port( 40, 0));  // Lout


  x1 = -40; y1 = -34;
  x2 =  40; y2 =  34;
}
