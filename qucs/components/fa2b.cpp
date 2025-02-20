/***************************************************************************
                               fa2b
                              ------
    begin                : December 2008
    copyright            : (C) 2008 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/

/*
 * fa2b.cpp - device implementations for fa2b module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */
#include "fa2b.h"
#include "node.h"
#include "misc.h"

fa2b::fa2b()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("2bit full adder verilog device");

  Props.append (new Property ("TR", "6", false,
    QObject::tr ("transfer function high scaling factor")));
  Props.append (new Property ("Delay", "1 ns", false,
    QObject::tr ("output delay")
    +" ("+QObject::tr ("s")+")"));
 
  createSymbol ();
  tx = x1 + 19;
  ty = y2 + 4;
  icon_dy = 0;
  Model = "fa2b";
  Name  = "Y";
}

Component * fa2b::newOne()
{
  fa2b * p = new fa2b();
  p->Props.front()->Value = Props.front()->Value; 
  p->recreate();
  return p;
}

Element * fa2b::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("2Bit FullAdder");
  BitmapFile = (char *) "fa2b";

  if(getNewOne) return new fa2b();
  return 0;
}

void fa2b::createSymbol()
{
  Rects.append(new qucs::Rect(-40, -60, 80, 150, QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin)));
  Texts.append(new Text(-9.5, -60, "Σ", Qt::darkBlue, 29.0));

  // left-side pins
  // X0
  Ports.append(new Port(-60,-10));  // X0 -> D
  Lines.append(new qucs::Line(-60,-10,-40,-10,QPen(Qt::darkBlue,2)));
  Texts.append(new Text(-35,-18,   "0",  Qt::darkBlue, 12.0));
  // X1
  Ports.append(new Port(-60, 10));  // X1 -> C
  Lines.append(new qucs::Line(-60, 10,-40, 10,QPen(Qt::darkBlue,2)));
  Texts.append(new Text(-35, 2,   "1",  Qt::darkBlue, 12.0));

  Texts.append(new Text(-25,-11,   "{",  Qt::darkBlue, 16.0));
  Texts.append(new Text(-15,-8,   "X",  Qt::darkBlue, 12.0));

  // Y0
  Ports.append(new Port(-60, 30));  // Y0 -> B
  Lines.append(new qucs::Line(-60, 30,-40, 30,QPen(Qt::darkBlue,2)));
  Texts.append(new Text(-35, 22,   "0",  Qt::darkBlue, 12.0));
  // Y1
  Ports.append(new Port(-60, 50));  // Y1 -> A
  Lines.append(new qucs::Line(-60, 50,-40, 50,QPen(Qt::darkBlue,2)));
  Texts.append(new Text(-35, 42,   "1",  Qt::darkBlue, 12.0));

  Texts.append(new Text(-25, 29,   "{",  Qt::darkBlue, 16.0));
  Texts.append(new Text(-15, 32,   "Y",  Qt::darkBlue, 12.0));

  // CI
  Ports.append(new Port(-60, 70));  // CI -> E
  Lines.append(new qucs::Line(-60, 70,-40, 70,QPen(Qt::darkBlue,2)));
  Texts.append(new Text(-35, 62,  "CI",  Qt::darkBlue, 12.0));


  // right-side pins
  // C0
  Ports.append(new Port( 60, 30));  // CO
  Lines.append(new qucs::Line( 40, 30, 60, 30,QPen(Qt::darkBlue,2)));
  Texts.append(new Text( 15, 22,  "CO",  Qt::darkBlue, 12.0));
  // S1
  Ports.append(new Port( 60, 10));  // S1
  Lines.append(new qucs::Line( 40, 10, 60, 10,QPen(Qt::darkBlue,2)));
  Texts.append(new Text( 28, 2,   "1",  Qt::darkBlue, 12.0));
  // S0
  Ports.append(new Port( 60,-10));  // S0
  Lines.append(new qucs::Line( 40,-10, 60,-10,QPen(Qt::darkBlue,2)));
  Texts.append(new Text( 28,-18,   "0",  Qt::darkBlue, 12.0));

  Texts.append(new Text( 17,-11,   "}",  Qt::darkBlue, 16.0));
  Texts.append(new Text( 3, -8,   "S",  Qt::darkBlue, 12.0));

  x1 = -60; y1 = -64;
  x2 =  60; y2 =  94;
}

QString fa2b::vhdlCode( int )
{
  QString s="";

  QString td = Props.at(1)->Value;     // delay time
  if(!misc::VHDL_Delay(td, Name)) return td; // time has not VHDL format
  td += ";\n";

  QString D    = Ports.at(0)->Connection->Name;
  QString C    = Ports.at(1)->Connection->Name;
  QString B    = Ports.at(2)->Connection->Name;
  QString A    = Ports.at(3)->Connection->Name;
  QString E   = Ports.at(4)->Connection->Name;
  QString CO   = Ports.at(5)->Connection->Name;
  QString S1   = Ports.at(6)->Connection->Name;
  QString S0   = Ports.at(7)->Connection->Name; 

  s = "\n  "+Name+":process ("+A+", "+B+", "+C+", "+D+", "+E+ ")\n"+
      "  begin\n" +
      "    "+CO+" <= ("+A+" and "+C+") or (("+A+" or "+C+") and (("+B+" and "+D+") or ("+E+" and "+B+") or ("+E+" and "+ D +")))"+td+ 
      "    "+S1+" <= (("+B+" and "+D+") or ("+E+" and "+B+") or ("+E+" and "+D+"))"+" xor ("+A+" xor "+C+")"+td+
      "    "+S0+" <= "+E+" xor ("+B+" xor "+D+")"+td+ 
      "  end process;\n";
  return s;
}

QString fa2b::verilogCode( int )
{
  QString td = Props.at(1)->Value;        // delay time
  if(!misc::Verilog_Delay(td, Name)) return td; // time does not have VHDL format

  QString l = "";

  QString D    = Ports.at(0)->Connection->Name;
  QString C    = Ports.at(1)->Connection->Name;
  QString B    = Ports.at(2)->Connection->Name;
  QString A    = Ports.at(3)->Connection->Name;
  QString E   = Ports.at(4)->Connection->Name;
  QString CO   = Ports.at(5)->Connection->Name;
  QString S1   = Ports.at(6)->Connection->Name;
  QString S0   = Ports.at(7)->Connection->Name; 

  QString COR  = "CO_reg" + Name + CO;
  QString S1R  = "S1_reg" + Name + S1;
  QString S0R  = "S0_reg" + Name + S0;

  l = "\n  // "+Name+" 2bit fulladder\n"+
      "  assign  "+CO+" = "+COR+";\n"+
      "  reg     "+COR+" = 0;\n"+
      "  assign  "+S1+" = "+S1R+";\n"+
      "  reg     "+S1R+" = 0;\n"+
      "  assign  "+S0+" = "+S0R+";\n"+
      "  reg     "+S0R+" = 0;\n"+
      "  always @ ("+A+" or "+B+" or "+C+" or "+D+" or "+E+")\n"+
      "  begin\n" +
      "    " +COR+" <="+td+" ("+A+" && "+C+") || ("+A+" || "+C+") && ("+B+" && "+D+" || "+E+" && "+B+" || "+E+" && "+D+");\n"+
      "    " +S1R+" <="+td+" ("+B+" && "+D+" || "+E+" && "+B+" || "+E+" && "+D+") ^ ("+A+" ^ "+C+");\n" +
      "    " +S0R+" <="+td+" "+E+" ^ ("+B+" ^ "+D+");\n"+
      "  end\n";

  return l;
}

