/*
 * pad3bit.cpp - device implementations for pad3bit module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */
#include <stdlib.h>
#include "pad3bit.h"
#include "node.h"
#include "main.h"

pad3bit::pad3bit()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("3bit pattern generator verilog device");

  Props.append (new Property ("Number", "0", false,
    QObject::tr ("pad output value")));
 
  createSymbol ();
  tx = x1 + 4;
  ty = y2 + 4;
  Model = "pad3bit";
  Name  = "Y";
}

Component * pad3bit::newOne()
{
  pad3bit * p = new pad3bit();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * pad3bit::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("3Bit Pattern");
  BitmapFile = (char *) "pad3bit";

  if(getNewOne) return new pad3bit();
  return 0;
}

void pad3bit::createSymbol()
{
  Lines.append(new Line(-60, -50, 30,-50,QPen(Qt::darkGreen,2)));
  Lines.append(new Line( 30, -50, 30, 30,QPen(Qt::darkGreen,2)));
  Lines.append(new Line( 30,  30,-60, 30,QPen(Qt::darkGreen,2)));
  Lines.append(new Line(-60,  30,-60,-50,QPen(Qt::darkGreen,2)));

  Lines.append(new Line( 40,-30, 30,-30,QPen(Qt::darkGreen,2)));  // A
  Lines.append(new Line( 40,-10, 30,-10,QPen(Qt::darkGreen,2)));  // B
  Lines.append(new Line( 40, 10, 30, 10,QPen(Qt::darkGreen,2))); // C

  Texts.append(new Text(-58,-33, " 0   1   2    3", Qt::darkGreen, 12.0));
  Texts.append(new Text(-58, -8, " 4   5   6    7", Qt::darkGreen, 12.0));

  Ports.append(new Port(40, 10));  // C
  Ports.append(new Port(40,-10));  // B
  Ports.append(new Port(40,-30));  // A

  x1 = -64; y1 = -54;
  x2 =  40; y2 =  34;
}

QString pad3bit::vhdlCode( int )
{
  QString v = Props.at(0)->Value;
  QString s1, s2, s3, s ="";

  QString A    = Ports.at(0)->Connection->Name;
  QString B    = Ports.at(1)->Connection->Name;
  QString C    = Ports.at(2)->Connection->Name;

  s1 = "\n  "+Name+":process\n"+
       "  variable n_" + Name + " : integer := " + v + ";\n" +
       "  begin\n";
  s2 = "    case n_" + Name + " is\n" +
       "      when 0 => "+A+" <= '0'; "+B+" <= '0'; "+C+" <= '0';\n"+
       "      when 1 => "+A+" <= '0'; "+B+" <= '0'; "+C+" <= '1';\n"+
       "      when 2 => "+A+" <= '0'; "+B+" <= '1'; "+C+" <= '0';\n"+
       "      when 3 => "+A+" <= '0'; "+B+" <= '1'; "+C+" <= '1';\n"+
       "      when 4 => "+A+" <= '1'; "+B+" <= '0'; "+C+" <= '0';\n"+
       "      when 5 => "+A+" <= '1'; "+B+" <= '0'; "+C+" <= '1';\n"+
       "      when 6 => "+A+" <= '1'; "+B+" <= '1'; "+C+" <= '0';\n"+
       "      when 7 => "+A+" <= '1'; "+B+" <= '1'; "+C+" <= '1';\n"+
       "      when others => null;\n" +
       "    end case;\n";
  s3 = "  end process;\n";
  s = s1+s2+s3;
  return s;
}

QString pad3bit::verilogCode( int )
{
  QString v = Props.at(0)->Value;

  QString l = "";
  QString l1, l2, l3;

  QString A   = Ports.at(0)->Connection->Name;
  QString B   = Ports.at(1)->Connection->Name;
  QString C   = Ports.at(2)->Connection->Name;

  QString AR  = "A_reg"  + Name + A;
  QString BR  = "Y_reg"  + Name + B;
  QString CR  = "Y_reg"  + Name + C;
  
  l1 = "\n  // "+Name+" 3bit pattern generator\n"+
       "  assign  "+A+" = "+AR+";\n"+
       "  reg     "+AR+" = 0;\n"+
       "  assign  "+B+" = "+BR+";\n"+
       "  reg     "+BR+" = 0;\n"+
       "  assign  "+C+" = "+CR+";\n"+
       "  reg     "+CR+" = 0;\n"+
       "  initial\n" ;
  l2 = "  begin\n"
       "    case ("+v+")\n"+
       "      0 : begin "+AR+" = 0; "+BR+" = 0; "+CR+" = 0; end\n"+
       "      1 : begin "+AR+" = 0; "+BR+" = 0; "+CR+" = 1; end\n"+
       "      2 : begin "+AR+" = 0; "+BR+" = 1; "+CR+" = 0; end\n"+
       "      3 : begin "+AR+" = 0; "+BR+" = 1; "+CR+" = 1; end\n"+
       "      4 : begin "+AR+" = 1; "+BR+" = 0; "+CR+" = 0; end\n"+
       "      5 : begin "+AR+" = 1; "+BR+" = 0; "+CR+" = 1; end\n"+
       "      6 : begin "+AR+" = 1; "+BR+" = 1; "+CR+" = 0; end\n"+
       "      7 : begin "+AR+" = 1; "+BR+" = 1; "+CR+" = 1; end\n"+
       "    endcase\n";
  l3 = "  end\n";
  l = l1+l2+l3;
  return l;
}
