/***************************************************************************
                               logic_0
                              ---------
    begin                : December 2008
    copyright            : (C) 2008 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/

/*
 * logic_0.cpp - device implementations for logic_0 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */
#include "node.h"
#include "logic_0.h"

logic_0::logic_0()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("logic 0 verilog device");

  Props.append (new Property ("LEVEL", "0", false,
    QObject::tr ("logic 0 voltage level")
    +" ("+QObject::tr ("V")+")"));

  createSymbol ();
  tx = x1 + 4;
  ty = y2 + 4;
  Model = "logic_0";
  Name  = "S";
}

Component * logic_0::newOne()
{
  logic_0 * p = new logic_0();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * logic_0::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Logic 0");
  BitmapFile = (char *) "logic_0";

  if(getNewOne) return new logic_0();
  return 0;
}

void logic_0::createSymbol()
{
  Lines.append(new qucs::Line(-10,  0,  0,  0,QPen(Qt::darkGreen,2)));
  Lines.append(new qucs::Line(-20,-10,-10,  0,QPen(Qt::darkGreen,2)));
  Lines.append(new qucs::Line(-20, 10,-10,  0,QPen(Qt::darkGreen,2)));
  Lines.append(new qucs::Line(-35,-10,-20,-10,QPen(Qt::darkGreen,2)));
  Lines.append(new qucs::Line(-35, 10,-20, 10,QPen(Qt::darkGreen,2)));
  Lines.append(new qucs::Line(-35,-10,-35, 10,QPen(Qt::darkGreen,2)));

  Texts.append(new Text(-30,-12, "0", Qt::darkGreen, 12.0));

  Ports.append(new Port(  0,  0)); // L0

  x1 = -39; y1 = -14;
  x2 = 0;   y2 =  14;
}

QString logic_0::vhdlCode( int )
{
  QString s="";

  QString LO = Ports.at(0)->Connection->Name;
 
  s = "\n  " + Name + ":process\n" +
     "  begin\n    " +
     LO + " <= '0';\n" + 
     "  end process;\n";
  return s;
}

QString logic_0::verilogCode( int )
{
  QString l = "";

  QString LO = Ports.at(0)->Connection->Name;
  
  QString v = "net_reg" + Name + LO;
  
  l = "\n  // " + Name + " logic 0\n" +
      "  assign  " + LO + " = " + v + ";\n" +
      "  reg     " + v + " = 0;\n" +
      "  initial\n" +
      "    " + v + " <= 0;\n";

  return l;
}

