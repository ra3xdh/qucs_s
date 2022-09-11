/***************************************************************************
                               fa1b
                              ------
    begin                : December 2008
    copyright            : (C) 2008 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/

/*
 * fa1b.cpp - device implementations for fa1b module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */
#include "fa1b.h"
#include "node.h"
#include "misc.h"

fa1b::fa1b()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("1bit full adder verilog device");

  Props.append (new Property ("TR", "6", false,
    QObject::tr ("transfer function high scaling factor")));
  Props.append (new Property ("Delay", "1 ns", false,
    QObject::tr ("output delay")
    +" ("+QObject::tr ("s")+")"));
 
  createSymbol ();
  tx = x1 + 19;
  ty = y2 + 4;
  Model = "fa1b";
  Name  = "Y";
}

Component * fa1b::newOne()
{
  fa1b * p = new fa1b();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * fa1b::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("1Bit FullAdder");
  BitmapFile = (char *) "fa1b";

  if(getNewOne) return new fa1b();
  return 0;
}

void fa1b::createSymbol()
{
  Lines.append(new qucs::Line(-30, -40, 30,-40,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30, -40, 30, 50,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30,  50,-30, 50,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-30,  50,-30,-40,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-50,-10,-30,-10,QPen(Qt::darkBlue,2)));  // A
  Lines.append(new qucs::Line(-50, 10,-30, 10,QPen(Qt::darkBlue,2)));  // B
  Lines.append(new qucs::Line(-50, 30,-30, 30,QPen(Qt::darkBlue,2)));  // CI
  Lines.append(new qucs::Line( 30, 10, 50, 10,QPen(Qt::darkBlue,2)));  // CO
  Lines.append(new qucs::Line( 30,-10, 50,-10,QPen(Qt::darkBlue,2)));  // S
  
  Texts.append(new Text(-25, 17, "CI", Qt::darkBlue, 12.0));
  Texts.append(new Text( 0,  -3, "CO", Qt::darkBlue, 12.0));

  Lines.append(new qucs::Line(-10,-35, 10, -35, QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-10,-35,  5, -25, QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  5,-25,-10, -15, QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-10,-15, 10, -15, QPen(Qt::darkBlue,2)));
 
  Ports.append(new Port(-50,-10));  // A
  Ports.append(new Port(-50, 10));  // B
  Ports.append(new Port(-50, 30));  // CI
  Ports.append(new Port( 50, 10));  // CO
  Ports.append(new Port( 50,-10));  // S

  x1 = -50; y1 = -44;
  x2 =  50; y2 =  54;
}

QString fa1b::vhdlCode( int )
{
  QString s="";

  QString td = Props.at(1)->Value;     // delay time
  if(!misc::VHDL_Delay(td, Name)) return td; // time has not VHDL format
  td += ";\n";

  QString A   = Ports.at(0)->Connection->Name;
  QString B   = Ports.at(1)->Connection->Name;
  QString CI  = Ports.at(2)->Connection->Name;
  QString CO  = Ports.at(3)->Connection->Name;
  QString S   = Ports.at(4)->Connection->Name;
 
  s = "\n  " + Name + ":process (" + A + ", " +  B + ", " + CI + ")\n" +
      "  begin\n" +
      "    " + CO + " <= (" + A + " and " + B +  ") or (" + CI + " and (" + A + " xor " + B + "))" + td +
      "    " + S  + " <= " + CI + " xor " + A + " xor " + B + td +
      "  end process;\n";
  return s;
}

QString fa1b::verilogCode( int )
{
  QString td = Props.at(1)->Value;        // delay time
  if(!misc::Verilog_Delay(td, Name)) return td; // time does not have VHDL format
  
  QString l = "";

  QString A   = Ports.at(0)->Connection->Name;
  QString B   = Ports.at(1)->Connection->Name;
  QString CI  = Ports.at(2)->Connection->Name;
  QString CO  = Ports.at(3)->Connection->Name;
  QString S   = Ports.at(4)->Connection->Name;

  QString COR = "CO_reg" + Name + CO;
  QString SR  = "S_reg"  + Name + S;

  l = "\n  // " + Name + " 1bit fulladder\n" +
      "  assign  " + CO + " = " + COR + ";\n" +
      "  reg     " + COR + " = 0;\n" +
      "  assign  " + S + " = " + SR + ";\n" +
      "  reg     " + SR + " = 0;\n" +
      "  always @ ("+ A + " or " + B + " or " + CI + ")\n" +
      "  begin\n" +
      "    " + COR + " <=" + td + " (" + A + " && " + B + ") || " + CI + " && " + "(" + A + " ^ " + B + ");\n" +
      "    " + SR + " <=" + td + " (" + CI + " ^ " + A + " ^ "  + B + ");\n" +
      "  end\n";

  return l;
}

