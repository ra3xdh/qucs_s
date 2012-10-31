/*
 * DLS_1ton.cpp - device implementations for DLS_1ton module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#include "DLS_1ton.h"

DLS_1ton::DLS_1ton()
{
  Description = QObject::tr ("data voltage level shifter (digital to analogue) verilog device");

  Props.append (new Property ("LEVEL", "5 V", false,
    QObject::tr ("voltage level")));
  Props.append (new Property ("Delay", "1 ns", false,
    QObject::tr ("time delay")
    +" ("+QObject::tr ("s")+")"));

  createSymbol ();
  tx = x1 + 14;
  ty = y2 + 4;
  Model = "DLS_1ton";
  Name  = "Y";
}

Component * DLS_1ton::newOne()
{
  DLS_1ton * p = new DLS_1ton();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * DLS_1ton::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("D2A Level Shifter");
  BitmapFile = (char *) "DLS_1ton";

  if(getNewOne) return new DLS_1ton();
  return 0;
}

void DLS_1ton::createSymbol()
{
  Lines.append(new Line(-30, -30, 30,-30,QPen(Qt::darkRed,2)));
  Lines.append(new Line( 30, -30, 30, 30,QPen(Qt::darkRed,2)));
  Lines.append(new Line( 30, 30,-30, 30,QPen(Qt::darkRed,2)));
  Lines.append(new Line(-30, 30, -30, -30,QPen(Qt::darkRed,2)));
 
  Lines.append(new Line(-30, 30, 30, -30,QPen(Qt::darkRed,2)));

  Lines.append(new Line(-40,  0,-30,  0,QPen(Qt::darkRed,2))); // Lin
  Lines.append(new Line( 30,  0, 40,  0,QPen(Qt::darkRed,2))); // Lout
  
  Lines.append(new Line(-25, -20,-15, -20,QPen(Qt::darkRed,2)));
  Lines.append(new Line( 25,  20, 15,  20,QPen(Qt::darkRed,2)));
  
  Texts.append(new Text(-10,-32, "1", Qt::darkRed, 12.0));
  Texts.append(new Text(  0,  8, "n", Qt::darkRed, 12.0));
 
  Ports.append(new Port(-40, 0));  // Lin
  Ports.append(new Port( 40, 0));  // Lout

  x1 = -40; y1 = -34;
  x2 =  40; y2 =  34;
}
