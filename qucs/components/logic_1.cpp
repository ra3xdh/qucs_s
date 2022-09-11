/***************************************************************************
                             logic_1
                            ---------
    begin                : December 2008
    copyright            : (C) 2008 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/

/*
 * logic_1.cpp - device implementations for logic_1 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */
#include "node.h"
#include "logic_1.h"

logic_1::logic_1()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("logic 1 verilog device");

 Props.append (new Property ("LEVEL", "1", false,
    QObject::tr ("logic 1 voltage level")
    +" ("+QObject::tr ("V")+")"));
  createSymbol ();
  tx = x1 + 4;
  ty = y2 + 4;
  Model = "logic_1";
  Name  = "S";
}

Component * logic_1::newOne()
{
  logic_1 * p = new logic_1();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;

}

Element * logic_1::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Logic 1");
  BitmapFile = (char *) "logic_1";

  if(getNewOne) return new logic_1();
  return 0;
}

void logic_1::createSymbol()
{

  Lines.append(new qucs::Line(-10,  0,  0,  0,QPen(Qt::darkGreen,2)));
  Lines.append(new qucs::Line(-20,-10,-10,  0,QPen(Qt::darkGreen,2)));
  Lines.append(new qucs::Line(-20, 10,-10,  0,QPen(Qt::darkGreen,2)));
  Lines.append(new qucs::Line(-35,-10,-20,-10,QPen(Qt::darkGreen,2)));
  Lines.append(new qucs::Line(-35, 10,-20, 10,QPen(Qt::darkGreen,2)));
  Lines.append(new qucs::Line(-35,-10,-35, 10,QPen(Qt::darkGreen,2)));

  Texts.append(new Text(-30,-12, "1", Qt::darkGreen, 12.0));

  Ports.append(new Port(  0,  0)); // L1

  x1 = -39; y1 = -14;
  x2 = 0;   y2 =  14;
}

QString logic_1::vhdlCode( int )
{
  QString s = "";

  QString L1 = Ports.at(0)->Connection->Name;
 
  s = "\n  " + Name + ":process\n" +
     "  begin\n    " +
     L1 + " <= '1';\n" + 
     "  end process;\n";
  return s;
}

QString logic_1::verilogCode( int )
{
   
  QString l = "";

  QString L1 = Ports.at(0)->Connection->Name;
  
  QString v = "net_reg" + Name + L1;
  
  l = "\n  // " + Name + " logic 1\n" +
      "  assign  " + L1 + " = " + v + ";\n" +
      "  reg     " + v + " = 1;\n" +
      "  initial\n" +
      "    " + v + " <= 1;\n";

  return l;
}



