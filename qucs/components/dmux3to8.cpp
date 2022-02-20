/***************************************************************************
                              dmux3to8
                             ----------
    begin                : December 2008
    copyright            : (C) 2008 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/

/*
 * dmux3to8.cpp - device implementations for dmux3to8 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */
#include "dmux3to8.h"
#include "node.h"
#include "misc.h"


dmux3to8::dmux3to8()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("3to8 demultiplexer verilog device");

  Props.append (new Property ("TR", "6", false,
    QObject::tr ("transfer function high scaling factor")));
  Props.append (new Property ("Delay", "1 ns", false,
    QObject::tr ("output delay")
    +" ("+QObject::tr ("s")+")"));

  createSymbol ();
  tx = x1 + 19;
  ty = y2 + 4;
  Model = "dmux3to8";
  Name  = "Y";
}

Component * dmux3to8::newOne()
{
  dmux3to8 * p = new dmux3to8();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * dmux3to8::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("3to8 Demux");
  BitmapFile = (char *) "dmux3to8";

  if(getNewOne) return new dmux3to8();
  return 0;
}

void dmux3to8::createSymbol()
{
  Lines.append(new qucs::Line(-30, -90, 30,-90,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30, -90, 30, 100,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30,  100,-30, 100,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-30,  100,-30, -90,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-50,-50,-40,-50,QPen(Qt::darkBlue,2)));  // EN
  Lines.append(new qucs::Line(-50,-30,-30,-30,QPen(Qt::darkBlue,2)));  // A
  Lines.append(new qucs::Line(-50,-10,-30,-10,QPen(Qt::darkBlue,2)));  // B
  Lines.append(new qucs::Line(-50, 10,-30, 10,QPen(Qt::darkBlue,2)));  // C

  Lines.append(new qucs::Line( 30, 90, 50, 90,QPen(Qt::darkBlue,2)));  // Y7
  Lines.append(new qucs::Line( 30, 70, 50, 70,QPen(Qt::darkBlue,2)));  // Y6
  Lines.append(new qucs::Line( 30, 50, 50, 50,QPen(Qt::darkBlue,2)));  // Y5
  Lines.append(new qucs::Line( 30, 30, 50, 30,QPen(Qt::darkBlue,2)));  // Y4
  Lines.append(new qucs::Line( 30, 10, 50, 10,QPen(Qt::darkBlue,2)));  // Y3
  Lines.append(new qucs::Line( 30,-10, 50,-10,QPen(Qt::darkBlue,2)));  // Y2
  Lines.append(new qucs::Line( 30,-30, 50,-30,QPen(Qt::darkBlue,2)));  // Y1
  Lines.append(new qucs::Line( 30,-50, 50,-50,QPen(Qt::darkBlue,2)));  // Y0

  Arcs.append(new qucs::Arc( -40, -55, 10, 10, 0, 16*360, QPen(Qt::darkBlue,2)));
 
  Texts.append(new Text(-25,-85, "DMUX", Qt::darkBlue, 12.0));

  Texts.append(new Text(-25,-63, "En", Qt::darkBlue, 12.0));
  Texts.append(new Text(-20,-25, "G", Qt::darkBlue, 12.0));
  Texts.append(new Text(-8, -30, "}", Qt::darkBlue, 16.0));
  Texts.append(new Text( 2, -32, "0", Qt::darkBlue, 12.0));
  Texts.append(new Text( 2, -13, "7", Qt::darkBlue, 12.0));

  Texts.append(new Text(-25,-43, "0", Qt::darkBlue, 12.0));
  Texts.append(new Text(-25, -3, "2", Qt::darkBlue, 12.0));

  Texts.append(new Text( 15,-63, "0", Qt::darkBlue, 12.0));
  Texts.append(new Text( 15,-43, "1", Qt::darkBlue, 12.0));
  Texts.append(new Text( 15,-23, "2", Qt::darkBlue, 12.0));
  Texts.append(new Text( 15, -3, "3", Qt::darkBlue, 12.0));
  Texts.append(new Text( 15, 17, "4", Qt::darkBlue, 12.0));
  Texts.append(new Text( 15, 37, "5", Qt::darkBlue, 12.0));
  Texts.append(new Text( 15, 57, "6", Qt::darkBlue, 12.0));
  Texts.append(new Text( 15, 77, "7", Qt::darkBlue, 12.0));

  Lines.append(new qucs::Line(0, -11, 12, -11, QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-50,-50));  // En
  Ports.append(new Port(-50,-30));  // A
  Ports.append(new Port(-50,-10));  // B
  Ports.append(new Port(-50, 10));  // C
  Ports.append(new Port( 50, 90));  // Y7
  Ports.append(new Port( 50, 70));  // Y6
  Ports.append(new Port( 50, 50));  // Y5
  Ports.append(new Port( 50, 30));  // Y4
  Ports.append(new Port( 50, 10));  // Y3
  Ports.append(new Port( 50,-10));  // Y2
  Ports.append(new Port( 50,-30));  // Y1
  Ports.append(new Port( 50,-50));  // Y0

  x1 = -50; y1 = -94;
  x2 =  50; y2 = 104;
}

QString dmux3to8::vhdlCode( int )
{
  QString s="";

  QString td = Props.at(1)->Value;     // delay time
  if(!misc::VHDL_Delay(td, Name)) return td; // time has not VHDL format
  td += ";\n";

  QString En = Ports.at(0)->Connection->Name;
  QString A  = Ports.at(1)->Connection->Name;
  QString B  = Ports.at(2)->Connection->Name;
  QString C  = Ports.at(3)->Connection->Name;
  QString Y7 = Ports.at(4)->Connection->Name;
  QString Y6 = Ports.at(5)->Connection->Name;
  QString Y5 = Ports.at(6)->Connection->Name;
  QString Y4 = Ports.at(7)->Connection->Name;
  QString Y3 = Ports.at(8)->Connection->Name;
  QString Y2 = Ports.at(9)->Connection->Name;
  QString Y1 = Ports.at(10)->Connection->Name;
  QString Y0 = Ports.at(11)->Connection->Name;

  s = "\n  "+Name+":process ("+En+", "+A+", "+B+", "+C+")\n"+
      "  begin\n" +
      "    "+Y0+" <= "+"(not "+En+") and (not "+C+") and (not "+B+") and (not "+A+")"+td+
      "    "+Y1+" <= "+"(not "+En+") and (not "+C+") and (not "+B+") and "+A+td+
      "    "+Y2+" <= "+"(not "+En+") and (not "+C+") and "+B+" and (not "+A+")"+td+
      "    "+Y3+" <= "+"(not "+En+") and (not "+C+") and "+B+" and "+A+td+
      "    "+Y4+" <= "+"(not "+En+") and "+C+" and (not "+B+") and (not "+A+")"+td+
      "    "+Y5+" <= "+"(not "+En+") and "+C+" and (not "+B+") and "+A+td+
      "    "+Y6+" <= "+"(not "+En+") and "+C+" and "+B+" and (not "+A+")"+td+
      "    "+Y7+" <= "+"(not "+En+") and "+C+" and "+B+" and "+A+td+
      "  end process;\n";
  return s;
}

QString dmux3to8::verilogCode( int )
{
  QString td = Props.at(1)->Value;        // delay time
  if(!misc::Verilog_Delay(td, Name)) return td; // time does not have VHDL format
  
  QString l = "";

  QString En = Ports.at(0)->Connection->Name;
  QString A  = Ports.at(1)->Connection->Name;
  QString B  = Ports.at(2)->Connection->Name;
  QString C  = Ports.at(3)->Connection->Name;
  QString Y7 = Ports.at(4)->Connection->Name;
  QString Y6 = Ports.at(5)->Connection->Name;
  QString Y5 = Ports.at(6)->Connection->Name;
  QString Y4 = Ports.at(7)->Connection->Name;
  QString Y3 = Ports.at(8)->Connection->Name;
  QString Y2 = Ports.at(9)->Connection->Name;
  QString Y1 = Ports.at(10)->Connection->Name;
  QString Y0 = Ports.at(11)->Connection->Name;

  QString Y7R = "net_reg" + Name + Y7;
  QString Y6R = "net_reg" + Name + Y6;
  QString Y5R = "net_reg" + Name + Y5;
  QString Y4R = "net_reg" + Name + Y4;  
  QString Y3R = "net_reg" + Name + Y3;
  QString Y2R = "net_reg" + Name + Y2;
  QString Y1R = "net_reg" + Name + Y1;
  QString Y0R = "net_reg" + Name + Y0;

  l = "\n  // " + Name + " 3to8 demux\n" +
      "  assign  " + Y0 + " = " + Y0R + ";\n" +
      "  reg     " + Y0R + " = 0;\n" +
      "  assign  " + Y1 + " = " + Y1R + ";\n" +
      "  reg     " + Y1R + " = 0;\n" +
      "  assign  " + Y2 + " = " + Y2R + ";\n" +
      "  reg     " + Y2R + " = 0;\n" +
      "  assign  " + Y3 + " = " + Y3R + ";\n" +
      "  reg     " + Y3R + " = 0;\n" +
      "  assign  " + Y4 + " = " + Y4R + ";\n" +
      "  reg     " + Y4R + " = 0;\n" +
      "  assign  " + Y5 + " = " + Y5R + ";\n" +
      "  reg     " + Y5R + " = 0;\n" +
      "  assign  " + Y6 + " = " + Y6R + ";\n" +
      "  reg     " + Y6R + " = 0;\n" +
      "  assign  " + Y7 + " = " + Y7R + ";\n" +
      "  reg     " + Y7R + " = 0;\n" +


      "  always @ ("+En+" or "+A+" or "+B+" or "+C+")\n" +
      "  begin\n"+
      "    "+Y0R+" <="+td+" (~"+En+") && (~"+C+") && (~"+B+") && (~"+A+");\n"+ 
      "    "+Y1R+" <="+td+" (~"+En+") && (~"+C+") && (~"+B+") && "+A+";\n"+
      "    "+Y2R+" <="+td+" (~"+En+") && (~"+C+") && "+B+" && (~"+A+");\n"+
      "    "+Y3R+" <="+td+" (~"+En+") && (~"+C+") && "+B+" && "+A+";\n"+  
      "    "+Y4R+" <="+td+" (~"+En+") && "+C+" && (~"+B+") && (~"+A+");\n"+ 
      "    "+Y5R+" <="+td+" (~"+En+") && "+C+" && (~"+B+") && "+A+";\n"+
      "    "+Y6R+" <="+td+" (~"+En+") && "+C+" && "+B+" && (~"+A+");\n"+
      "    "+Y7R+" <="+td+" (~"+En+") && "+C+" && "+B+" && "+A+";\n"+
      "  end\n";

  return l;
}
