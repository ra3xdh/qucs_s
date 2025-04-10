/*
 * pad2bit.cpp - device implementations for pad2bit module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */
#include "pad2bit.h"
#include "node.h"

pad2bit::pad2bit()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("2bit pattern generator verilog device");

  Props.append (new Property ("Number", "0", false,
    QObject::tr ("pad output value")));

  createSymbol ();
  tx = x1 + 4;
  ty = y2 + 4;
  icon_dx = 6;
  Model = "pad2bit";
  Name  = "Y";
}

Component * pad2bit::newOne()
{
  pad2bit * p = new pad2bit();
  p->Props.front()->Value = Props.front()->Value; 
  p->recreate();
  return p;
}

Element * pad2bit::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("2Bit Pattern");
  BitmapFile = (char *) "pad2bit";

  if(getNewOne) return new pad2bit();
  return 0;
}

void pad2bit::createSymbol()
{
  // Body
  Rects.append(new qucs::Rect(-60, -50, 90, 60, QPen(Qt::darkGreen,2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin)));

  // Pins
  // A
  Ports.append(new Port(40,-30));
  Lines.append(new qucs::Line( 40,-30, 30,-30,QPen(Qt::darkGreen,2)));
  // B
  Ports.append(new Port(40,-10));
  Lines.append(new qucs::Line( 40,-10, 30,-10,QPen(Qt::darkGreen,2)));

  Texts.append(new Text(-50,-38, "0", Qt::darkGreen, 12.0));
  Texts.append(new Text(-30,-38, "1", Qt::darkGreen, 12.0));
  Texts.append(new Text(-10,-38, "2", Qt::darkGreen, 12.0));
  Texts.append(new Text(10,-38, "3", Qt::darkGreen, 12.0));

  x1 = -64; y1 = -54;
  x2 =  40; y2 =  14;
}

QString pad2bit::vhdlCode( int )
{
  QString v = Props.at(0)->Value; ;
  QString s1, s2, s3, s ="";

  QString A    = Ports.at(0)->Connection->Name;
  QString B    = Ports.at(1)->Connection->Name;

  s1 = "\n  "+Name+":process\n"+
       "  variable n_" + Name + " : integer := " + v + ";\n" +
       "  begin\n";
  s2 = "    case n_" + Name + " is\n" +
       "      when 0 => "+A+" <= '0'; "+B+" <= '0';\n" +
       "      when 1 => "+A+" <= '0'; "+B+" <= '1';\n" +
       "      when 2 => "+A+" <= '1'; "+B+" <= '0';\n" +
       "      when 3 => "+A+" <= '1'; "+B+" <= '1';\n" +
       "      when others => null;\n" +
       "    end case;\n";
  s3 = "    wait for 1 ns;\n"
       "  end process;\n";
  s = s1+s2+s3;
  return s;
}

QString pad2bit::verilogCode( int )
{
  QString v = Props.at(0)->Value;

  QString l = "";
  QString l1, l2, l3;

  QString A   = Ports.at(0)->Connection->Name;
  QString B   = Ports.at(1)->Connection->Name;

  QString AR  = "A_reg"  + Name + A;
  QString BR  = "Y_reg"  + Name + B;
  

  l1 = "\n  // "+Name+" 2bit pattern generator\n"+
       "  assign  "+A+" = "+AR+";\n"+
       "  reg     "+AR+" = 0;\n"+
       "  assign  "+B+" = "+BR+";\n"+
       "  reg     "+BR+" = 0;\n"+
       "  initial\n";
  l2 = "  begin\n"
       "    case ("+v+")\n"+
       "      0 : begin "+AR+" = 0; "+BR+" = 0; end\n"+
       "      1 : begin "+AR+" = 0; "+BR+" = 1; end\n"+
       "      2 : begin "+AR+" = 1; "+BR+" = 0; end\n"+
       "      3 : begin "+AR+" = 1; "+BR+" = 1; end\n"+
       "    endcase\n";
  l3 = "  end\n";
  l = l1+l2+l3;
  return l;
}
