/***************************************************************************
                            greytobinary4bit
                          -------------------
    begin                : December 2008
    copyright            : (C) 2008 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/

/*
 * greytobinary4bit.cpp - device implementations for greytobinary4bit module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */
#include "greytobinary4bit.h"
#include "node.h"
#include "misc.h"

greytobinary4bit::greytobinary4bit()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("4bit grey to binary converter verilog device");

  Props.append (new Property ("TR", "6", false,
    QObject::tr ("transfer function scaling factor")));
  Props.append (new Property ("Delay", "1 ns", false,
    QObject::tr ("output delay")
    +" ("+QObject::tr ("s")+")"));

  createSymbol ();
  tx = x1 + 19;
  ty = y2 + 4;
  Model = "greytobinary4bit";
  Name  = "Y";
}

Component * greytobinary4bit::newOne()
{
  greytobinary4bit * p = new greytobinary4bit();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * greytobinary4bit::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("4Bit Grey2Bin");
  BitmapFile = (char *) "greytobinary4bit";

  if(getNewOne) return new greytobinary4bit();
  return 0;
}

void greytobinary4bit::createSymbol()
{
  Lines.append(new Line(-30, -60, 30,-60,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30, -60, 30, 40,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  40,-30, 40,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30,  40,-30, -60,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-50,-30,-30,-30,QPen(Qt::darkBlue,2)));  // G0
  Lines.append(new Line(-50,-10,-30,-10,QPen(Qt::darkBlue,2)));  // G1
  Lines.append(new Line(-50, 10,-30, 10,QPen(Qt::darkBlue,2)));  // G2
  Lines.append(new Line(-50, 30,-30, 30,QPen(Qt::darkBlue,2)));  // G3

  Lines.append(new Line( 30, 30, 50, 30,QPen(Qt::darkBlue,2)));  // B3
  Lines.append(new Line( 30, 10, 50, 10,QPen(Qt::darkBlue,2)));  // B2
  Lines.append(new Line( 30,-10, 50,-10,QPen(Qt::darkBlue,2)));  // B1
  Lines.append(new Line( 30,-30, 50,-30,QPen(Qt::darkBlue,2)));  // B0
 
  Texts.append(new Text(-16,-59, "G", Qt::darkBlue, 12.0));
  Texts.append(new Text( -2,-59, "/", Qt::darkBlue, 12.0));
  Texts.append(new Text(  5,-59, "B", Qt::darkBlue, 12.0));

  Texts.append(new Text(-25,-43, "0", Qt::darkBlue, 12.0));
  Texts.append(new Text(-25,-23, "1", Qt::darkBlue, 12.0));
  Texts.append(new Text(-25, -3, "2", Qt::darkBlue, 12.0));
  Texts.append(new Text(-25, 17, "3", Qt::darkBlue, 12.0));

  Texts.append(new Text( 15,-43, "0", Qt::darkBlue, 12.0));
  Texts.append(new Text( 15,-23, "1", Qt::darkBlue, 12.0));
  Texts.append(new Text( 15, -3, "2", Qt::darkBlue, 12.0));
  Texts.append(new Text( 15, 17, "3", Qt::darkBlue, 12.0));

  Ports.append(new Port(-50,-30));  // G0
  Ports.append(new Port(-50,-10));  // G1
  Ports.append(new Port(-50, 10));  // G2
  Ports.append(new Port(-50, 30));  // G3

  Ports.append(new Port( 50, 30));  // B3
  Ports.append(new Port( 50, 10));  // B2
  Ports.append(new Port( 50,-10));  // B1
  Ports.append(new Port( 50,-30));  // B0

  x1 = -50; y1 = -64;
  x2 =  50; y2 =  44;
}

QString greytobinary4bit::vhdlCode( int )
{
  QString s="";

  QString td = Props.at(1)->Value;     // delay time
  if(!VHDL_Delay(td, Name)) return td; // time has not VHDL format
  td += ";\n";

  QString G0 = Ports.at(0)->Connection->Name;
  QString G1 = Ports.at(1)->Connection->Name;
  QString G2 = Ports.at(2)->Connection->Name;
  QString G3 = Ports.at(3)->Connection->Name;
  QString B3 = Ports.at(4)->Connection->Name;
  QString B2 = Ports.at(5)->Connection->Name;
  QString B1 = Ports.at(6)->Connection->Name;
  QString B0 = Ports.at(7)->Connection->Name;
 
  s = "\n  " + Name + ":process ("+G0+", "+G1+", "+G2+", "+G3+")\n"+
      "  begin\n"+
      "    "+B3+" <= "+G3+td+
      "    "+B2+" <= "+G3+" xor "+G2+td+
      "    "+B1+" <= "+G3+" xor "+G2+" xor "+G1+td+
      "    "+B0+" <= "+G3+" xor "+G2+" xor "+G1+" xor "+G0+td+
      "  end process;\n";
  return s;
}

QString greytobinary4bit::verilogCode( int )
{
  QString td = Props.at(1)->Value;        // delay time
  if(!Verilog_Delay(td, Name)) return td; // time does not have VHDL format

  QString G0 = Ports.at(0)->Connection->Name;
  QString G1 = Ports.at(1)->Connection->Name;
  QString G2 = Ports.at(2)->Connection->Name;
  QString G3 = Ports.at(3)->Connection->Name;
  QString B3 = Ports.at(4)->Connection->Name;
  QString B2 = Ports.at(5)->Connection->Name;
  QString B1 = Ports.at(6)->Connection->Name;
  QString B0 = Ports.at(7)->Connection->Name; 
 
  QString l = "";
 
  QString B0R = "net_reg" + Name + B0;
  QString B1R = "net_reg" + Name + B1;
  QString B2R = "net_reg" + Name + B2;
  QString B3R = "net_reg" + Name + B3;
  
  l = "\n  // " + Name + " 4bit grey to binary\n" +
      "  assign  " + B0 + " = " + B0R + ";\n" +
      "  reg     " + B0R + " = 0;\n" +
      "  assign  " + B1 + " = " + B1R + ";\n" +
      "  reg     " + B1R + " = 0;\n" +
      "  assign  " + B2 + " = " + B2R + ";\n" +
      "  reg     " + B2R + " = 0;\n" +
      "  assign  " + B3 + " = " + B3R + ";\n" +
      "  reg     " + B3R + " = 0;\n" +
      "  always @ ("+G0+" or "+G1+" or "+G2+" or "+G3+")\n" +
      "  begin\n"+
      "    "+B3R+" <="+td+" "+G3+";\n"+
      "    "+B2R+" <="+td+" "+G3+" ^ "+G2+";\n"+
      "    "+B1R+" <="+td+" "+G3+" ^ "+G2+" ^ "+G1+";\n"+
      "    "+B0R+" <="+td+" "+G3+" ^ "+G2+" ^ "+G1+" ^ "+G0+";\n"+
      "  end\n";
  return l;
}

