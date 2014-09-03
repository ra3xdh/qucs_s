/***************************************************************************
                             gatedDlatch
                            -------------
    begin                : January 2009
    copyright            : (C) 2008 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/

/*
 * gatedDlatch.cpp - device implementations for gatedDlatch module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */
#include <QtGui>
#include "gatedDlatch.h"
#include "node.h"
#include "main.h"

gatedDlatch::gatedDlatch()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("gated D latch verilog device");

  Props.append (new Property ("TR_H", "6", false,
    QObject::tr ("cross coupled gate transfer function high scaling factor")));
  Props.append (new Property ("TR_L", "5", false,
    QObject::tr ("cross coupled gate transfer function low scaling factor")));
  Props.append (new Property ("Delay", "1 ns", false,
    QObject::tr ("cross coupled gate delay")
    +" ("+QObject::tr ("s")+")"));

  createSymbol ();
  tx = x1 + 19;
  ty = y2 + 4;
  Model = "gatedDlatch";
  Name  = "Y";
}

Component * gatedDlatch::newOne()
{
  gatedDlatch * p = new gatedDlatch();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * gatedDlatch::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Gated D-Latch");
  BitmapFile = (char *) "gatedDlatch";

  if(getNewOne) return new gatedDlatch();
  return 0;
}

void gatedDlatch::createSymbol()
{
  Lines.append(new Line(-30, -40, 30,-40,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30, -40, 30, 40,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  40,-30, 40,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30, 40,-30, -40,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-50,-20,-30,-20,QPen(Qt::darkBlue,2))); // D
  Lines.append(new Line(-50, 20,-30, 20,QPen(Qt::darkBlue,2))); // C
  Lines.append(new Line( 40, 20, 50, 20,QPen(Qt::darkBlue,2))); // QB
  Lines.append(new Line( 30,-20, 50,-20,QPen(Qt::darkBlue,2))); // Q

  Arcs.append(new Arc( 30, 15, 10, 10, 0, 16*360, QPen(Qt::darkBlue,2)));

  Texts.append(new Text(-25,-32, "D", Qt::darkBlue, 12.0));
  Texts.append(new Text(-25,  7, "C", Qt::darkBlue, 12.0));
  Texts.append(new Text( 11,-32, "Q", Qt::darkBlue, 12.0));
  Texts.append(new Text( 11,  7, "Q", Qt::darkBlue, 12.0));
  Texts.last()->over=true;

  Ports.append(new Port(-50,-20));  // D
  Ports.append(new Port(-50, 20));  // C
  Ports.append(new Port( 50, 20));  // QB
  Ports.append(new Port( 50,-20));  // Q

  x1 = -50; y1 = -44;
  x2 =  50; y2 =  44;
}

QString gatedDlatch::vhdlCode( int )
{
  QString s="";

  QString td = Props.at(2)->Value;     // delay time
  if(!VHDL_Delay(td, Name)) return td; // time has not VHDL format
  td += ";\n";

  QString D    = Ports.at(0)->Connection->Name;
  QString C    = Ports.at(1)->Connection->Name;
  QString QB   = Ports.at(2)->Connection->Name;
  QString Q    = Ports.at(3)->Connection->Name;

  s = "\n  "+Name+":process ("+D+", "+C+")\n"+
      "  begin\n" +
      "    if ("+C+" = '1') then\n"+
      "      "+Q+" <= "+D+td+
      "      "+QB+" <= not "+D+td+
      "    end if;\n"+
      "  end process;\n";
  return s;
}

QString gatedDlatch::verilogCode( int )
{
  QString td = Props.at(2)->Value;        // delay time
  if(!Verilog_Delay(td, Name)) return td; // time does not have VHDL format
  
  QString l = "";

  QString D    = Ports.at(0)->Connection->Name;
  QString C    = Ports.at(1)->Connection->Name;
  QString QB   = Ports.at(2)->Connection->Name;
  QString Q    = Ports.at(3)->Connection->Name;

  QString QR   = "Q_reg"  + Name + Q;
  QString QBR  = "QB_reg"  + Name + QB;

  l = "\n  // "+Name+" gated d latch\n"+
      "  assign  "+Q+" = "+QR+";\n"+
      "  reg     "+QR+" = 0;\n"+
      "  assign  "+QB+" = "+QBR+";\n"+
      "  reg     "+QBR+" = 0;\n"+
      "  always @ ("+D+" or "+C+")\n"+
      "  begin\n"+
      "    if ("+C+" == 1)\n"+
      "    begin\n"+
      "      " +QR+" <="+td+" "+D+";\n"+
      "      " +QBR+" <="+td+" ~"+D+";\n" +
      "    end\n"+
      "  end\n";

  return l;
}
