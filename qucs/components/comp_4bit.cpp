/***************************************************************************
                              comp_4bit
                             -----------
    begin                : December 2008
    copyright            : (C) 2008 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/

/*
 * comp_4bit.cpp - device implementations for comp_4bit module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */
#include "comp_4bit.h"
#include "node.h"
#include "misc.h"

comp_4bit::comp_4bit()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("4bit comparator verilog device");

  Props.append (new Property ("TR", "6", false,
    QObject::tr ("transfer function high scaling factor")));
  Props.append (new Property ("Delay", "1 ns", false,
    QObject::tr ("output delay")
    +" ("+QObject::tr ("s")+")"));

  createSymbol ();
  tx = x1 + 19;
  ty = y2 + 4;
  Model = "comp_4bit";
  Name  = "Y";
}

Component * comp_4bit::newOne()
{
  comp_4bit * p = new comp_4bit();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * comp_4bit::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("4Bit Comparator");
  BitmapFile = (char *) "comp_4bit";

  if(getNewOne) return new comp_4bit();
  return 0;
}

void comp_4bit::createSymbol()
{
  Lines.append(new Line(-40, -90, 40,-90,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 40, -90, 40,100,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 40, 100,-40,100,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-40, 100,-40,-90,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-60,-50,-40,-50,QPen(Qt::darkBlue,2)));  // X0
  Lines.append(new Line(-60,-30,-40,-30,QPen(Qt::darkBlue,2)));  // X1
  Lines.append(new Line(-60,-10,-40,-10,QPen(Qt::darkBlue,2)));  // X2
  Lines.append(new Line(-60, 10,-40, 10,QPen(Qt::darkBlue,2)));  // X3
  Lines.append(new Line(-60, 30,-40, 30,QPen(Qt::darkBlue,2)));  // Y0
  Lines.append(new Line(-60, 50,-40, 50,QPen(Qt::darkBlue,2)));  // Y1
  Lines.append(new Line(-60, 70,-40, 70,QPen(Qt::darkBlue,2)));  // Y2
  Lines.append(new Line(-60, 90,-40, 90,QPen(Qt::darkBlue,2)));  // Y3
  Lines.append(new Line( 40, 30, 60, 30,QPen(Qt::darkBlue,2)));  // L
  Lines.append(new Line( 40, 10, 60, 10,QPen(Qt::darkBlue,2)));  // G
  Lines.append(new Line( 40,-10, 60,-10,QPen(Qt::darkBlue,2)));  // E

  Texts.append(new Text(-25,-85, "COMP", Qt::darkBlue, 12.0));

  Texts.append(new Text(-25,-40,   "{",  Qt::darkBlue, 16.0));
  Texts.append(new Text(-15,-35,   "X",  Qt::darkBlue, 12.0));
  Texts.append(new Text(-35,-63,   "0",  Qt::darkBlue, 12.0));
  Texts.append(new Text(-35,-43,   "1",  Qt::darkBlue, 12.0));
  Texts.append(new Text(-35,-23,   "2",  Qt::darkBlue, 12.0));
  Texts.append(new Text(-35, -3,   "3",  Qt::darkBlue, 12.0));
  Texts.append(new Text(-25, 42,   "{",  Qt::darkBlue, 16.0));
  Texts.append(new Text(-15, 47,   "Y",  Qt::darkBlue, 12.0));
  Texts.append(new Text(-35, 17,   "0",  Qt::darkBlue, 12.0));
  Texts.append(new Text(-35, 37,   "1",  Qt::darkBlue, 12.0));
  Texts.append(new Text(-35, 57,   "2",  Qt::darkBlue, 12.0));
  Texts.append(new Text(-35, 77,   "3",  Qt::darkBlue, 12.0));
  Texts.append(new Text(  5, 17, "X<Y",  Qt::darkBlue, 12.0));
  Texts.append(new Text(  5, -3, "X>Y",  Qt::darkBlue, 12.0));
  Texts.append(new Text(  5,-23, "X=Y",  Qt::darkBlue, 12.0));

  Ports.append(new Port(-60,-50));  // X0
  Ports.append(new Port(-60,-30));  // X1
  Ports.append(new Port(-60,-10));  // X2
  Ports.append(new Port(-60, 10));  // X3
  Ports.append(new Port(-60, 30));  // Y0
  Ports.append(new Port(-60, 50));  // Y1
  Ports.append(new Port(-60, 70));  // Y2
  Ports.append(new Port(-60, 90));  // Y3
  Ports.append(new Port( 60, 30));  // L
  Ports.append(new Port( 60, 10));  // G
  Ports.append(new Port( 60,-10));  // E

  x1 = -60; y1 = -94;
  x2 =  60; y2 =  104;
}

QString comp_4bit::vhdlCode( int )
{
  QString s="";

  QString td = Props.at(1)->Value;     // delay time
  if(!VHDL_Delay(td, Name)) return td; // time has not VHDL format
  td += ";\n";

  QString X0    = Ports.at(0)->Connection->Name;
  QString X1    = Ports.at(1)->Connection->Name;
  QString X2    = Ports.at(2)->Connection->Name;
  QString X3    = Ports.at(3)->Connection->Name;
  QString Y0    = Ports.at(4)->Connection->Name;
  QString Y1    = Ports.at(5)->Connection->Name;
  QString Y2    = Ports.at(6)->Connection->Name;
  QString Y3    = Ports.at(7)->Connection->Name;
  QString L     = Ports.at(8)->Connection->Name;
  QString G     = Ports.at(9)->Connection->Name;
  QString E     = Ports.at(10)->Connection->Name;
 
  s = "\n  "+Name+":process ("+X0+", "+X1+", "+X2+", "+X3+", "+Y0+", "+Y1+", "+Y2+", "+Y3+")\n"+
      "  variable P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11 : std_logic;\n"+
      "  begin\n"+
      "    P0  := not ("+X0+" xor "+Y0+");\n"+
      "    P1  := not ("+X1+" xor "+Y1+");\n"+
      "    P2  := not ("+X2+" xor "+Y2+");\n"+
      "    P3  := not ("+X3+" xor "+Y3+");\n"+
      "    P4  := (not "+X0+") and "+Y0+";\n"+
      "    P5  := (not "+X1+") and "+Y1+";\n"+
      "    P6  := (not "+X2+") and "+Y2+";\n"+
      "    P7  := (not "+X3+") and "+Y3+";\n"+
      "    P8  := "+X0+" and (not "+Y0+");\n"+
      "    P9  := "+X1+" and (not "+Y1+");\n"+
      "    P10 := "+X2+" and (not "+Y2+");\n"+
      "    P11 := "+X3+" and (not "+Y3+");\n"+
      "    "+L+" <= P7 or (P3 and P6) or (P3 and P2 and P5) or (P3 and P2 and P1 and P4)"+td+
      "    "+G+" <= P11 or (P3 and P10) or (P3 and P2 and P9) or (P3 and P2 and P1 and P8)"+td+
      "    "+E+" <= P3 and P2 and P1 and P0"+td+  
      "  end process;\n";
  return s;
}

QString comp_4bit::verilogCode( int )
{
  QString l="";

  QString td = Props.at(1)->Value;        // delay time
  if(!Verilog_Delay(td, Name)) return td; // time does not have VHDL format

  QString X0    = Ports.at(0)->Connection->Name;
  QString X1    = Ports.at(1)->Connection->Name;
  QString X2    = Ports.at(2)->Connection->Name;
  QString X3    = Ports.at(3)->Connection->Name;
  QString Y0    = Ports.at(4)->Connection->Name;
  QString Y1    = Ports.at(5)->Connection->Name;
  QString Y2    = Ports.at(6)->Connection->Name;
  QString Y3    = Ports.at(7)->Connection->Name;
  QString L     = Ports.at(8)->Connection->Name;
  QString G     = Ports.at(9)->Connection->Name;
  QString E     = Ports.at(10)->Connection->Name; 

  QString LR  = "L_reg"  + Name + L;
  QString GR  = "G_reg"  + Name + G;
  QString ER  = "E_reg"  + Name + E;

  l = "\n  // "+Name+" 4bit comparator\n"+
      "  assign  "+L+" = "+LR+";\n"+
      "  reg     "+LR+" = 0;\n"+
      "  assign  "+G+" = "+GR+";\n"+
      "  reg     "+GR+" = 0;\n"+
      "  assign  "+E+" = "+ER+";\n"+
      "  reg     "+ER+" = 0;\n"+
      "  reg     P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11;\n"+
      "  always @ ("+X0+" or "+X1+" or "+X2+" or "+X3+" or "+Y0+" or "+Y1+" or "+Y2+" or "+Y3+")\n"+
      "  begin\n"+
      "    P0  = ~("+X0+" ^ "+Y0+");\n"+
      "    P1  = ~("+X1+" ^ "+Y1+");\n"+
      "    P2  = ~("+X2+" ^ "+Y2+");\n"+
      "    P3  = ~("+X3+" ^ "+Y3+");\n"+
      "    P4  = (~"+X0+") && "+Y0+";\n"+
      "    P5  = (~"+X1+") && "+Y1+";\n"+
      "    P6  = (~"+X2+") && "+Y2+";\n"+
      "    P7  = (~"+X3+") && "+Y3+";\n"+
      "    P8  = "+X0+" && (~"+Y0+");\n"+
      "    P9  = "+X1+" && (~"+Y1+");\n"+
      "    P10 = "+X2+" && (~"+Y2+");\n"+
      "    P11 = "+X3+" && (~"+Y3+");\n"+
      "    "+LR+" <="+td+" P7 || (P3 && P6) || (P3 && P2 && P5) || (P3 && P2 && P1 && P4)"+";\n"+
      "    "+GR+" <="+td+" P11 || (P3 && P10) || (P3 && P2 && P9) || (P3 && P2 && P1 && P8)"+";\n"+
      "    "+ER+" <="+td+" P3 && P2 && P1 && P0"+";\n"+
      "  end\n";

  return l;
}
