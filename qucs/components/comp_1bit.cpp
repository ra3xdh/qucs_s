/***************************************************************************
                              comp_1bit
                             -----------
    begin                : December 2008
    copyright            : (C) 2008 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/

/*
 * comp_1bit.cpp - device implementations for comp_1bit module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */
#include <QtGui>
#include "comp_1bit.h"
#include "node.h"
#include "main.h"

comp_1bit::comp_1bit()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("1bit comparator verilog device");

  Props.append (new Property ("TR", "6", false,
    QObject::tr ("transfer function high scaling factor")));
  Props.append (new Property ("Delay", "1 ns", false,
    QObject::tr ("output delay")
    +" ("+QObject::tr ("s")+")"));
 
  createSymbol ();
  tx = x1 + 19;
  ty = y2 + 4;
  Model = "comp_1bit";
  Name  = "Y";
}

Component * comp_1bit::newOne()
{
  comp_1bit * p = new comp_1bit();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * comp_1bit::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("1Bit Comparator");
  BitmapFile = (char *) "comp_1bit";

  if(getNewOne) return new comp_1bit();
  return 0;
}

void comp_1bit::createSymbol()
{
  Lines.append(new Line(-30, -60, 30,-60,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30, -60, 30, 30,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  30,-30, 30,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30,  30,-30, -60,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-50,-10,-30,-10,QPen(Qt::darkBlue,2)));  // X
  Lines.append(new Line(-50, 10,-30, 10,QPen(Qt::darkBlue,2)));  // Y
  Lines.append(new Line( 30, 20, 50, 20,QPen(Qt::darkBlue,2)));  // L
  Lines.append(new Line( 30,  0, 50,  0,QPen(Qt::darkBlue,2)));  // G
  Lines.append(new Line( 30,-20, 50,-20,QPen(Qt::darkBlue,2)));  // E

  Texts.append(new Text(-25,-55, "COMP", Qt::darkBlue, 12.0));

  Texts.append(new Text(-25,-23,   "X",  Qt::darkBlue, 12.0));
  Texts.append(new Text(-25, -3,   "Y",  Qt::darkBlue, 12.0));
  Texts.append(new Text( -5,  7, "X<Y",  Qt::darkBlue, 12.0));
  Texts.append(new Text( -5,-13, "X>Y", Qt::darkBlue, 12.0));
  Texts.append(new Text( -5,-33, "X=Y", Qt::darkBlue, 12.0));

  Ports.append(new Port(-50,-10));  // X
  Ports.append(new Port(-50, 10));  // Y
  Ports.append(new Port( 50, 20));  // L
  Ports.append(new Port( 50,  0));  // G
  Ports.append(new Port( 50,-20));  // E

  x1 = -50; y1 = -64;
  x2 =  50; y2 =  34;
}

QString comp_1bit::vhdlCode( int )
{
  QString s="";

  QString td = Props.at(1)->Value;     // delay time
  if(!VHDL_Delay(td, Name)) return td; // time has not VHDL format
  td += ";\n";
 
  QString X    = Ports.at(0)->Connection->Name;
  QString Y    = Ports.at(1)->Connection->Name;
  QString L    = Ports.at(2)->Connection->Name;
  QString G    = Ports.at(3)->Connection->Name;
  QString E    = Ports.at(4)->Connection->Name;
 
  s = "\n  "+Name+":process ("+X+", "+Y+")\n"+
      "  begin\n"+
      "    "+L+" <= (not "+X+") and "+Y+td+
      "    "+G+" <= "+X+" and (not "+Y+")"+td+
      "    "+E+" <= not ("+X+" xor "+Y+")"+td+  
      "  end process;\n";
  return s;
}

QString comp_1bit::verilogCode( int )
{
  QString l="";

  QString td = Props.at(1)->Value;        // delay time
  if(!Verilog_Delay(td, Name)) return td; // time does not have VHDL format

  QString X    = Ports.at(0)->Connection->Name;
  QString Y    = Ports.at(1)->Connection->Name;
  QString L    = Ports.at(2)->Connection->Name;
  QString G    = Ports.at(3)->Connection->Name;
  QString E    = Ports.at(4)->Connection->Name;

  QString LR  = "L_reg"  + Name + L;
  QString GR  = "G_reg"  + Name + G;
  QString ER  = "E_reg"  + Name + E;

  l = "\n  // "+Name+" 1bit comparator\n"+
      "  assign  "+L+" = "+LR+";\n"+
      "  reg     "+LR+" = 0;\n"+
      "  assign  "+G+" = "+GR+";\n"+
      "  reg     "+GR+" = 0;\n"+
      "  assign  "+E+" = "+ER+";\n"+
      "  reg     "+ER+" = 0;\n"+
      "  always @ ("+X+" or "+Y+")\n"+
      "  begin\n"+
      "    "+LR+" <="+td+" (~"+X+") && "+Y+";\n"+
      "    "+GR+" <="+td+" "+X+" && (~"+Y+");\n"+
      "    "+ER+" <="+td+" ~("+X+" ^ "+Y+");\n"+
      "  end\n";

  return l;
}
