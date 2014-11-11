/***************************************************************************
                              dmux4to16
                             -----------
    begin                : December 2008
    copyright            : (C) 2008 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/

/*
 * dmux4to16.cpp - device implementations for dmux4to16 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */
#include "dmux4to16.h"
#include "node.h"
#include "main.h"
#include "misc.h"

dmux4to16::dmux4to16()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("4to16 demultiplexer verilog device");

  Props.append (new Property ("TR", "6", false,
    QObject::tr ("transfer function high scaling factor")));
  Props.append (new Property ("Delay", "1 ns", false,
    QObject::tr ("output delay")
    +" ("+QObject::tr ("s")+")"));

  createSymbol ();
  tx = x1 + 19;
  ty = y2 + 4;
  Model = "dmux4to16";
  Name  = "Y";
}

Component * dmux4to16::newOne()
{
  dmux4to16 * p = new dmux4to16();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * dmux4to16::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("4to16 Demux");
  BitmapFile = (char *) "dmux4to16";

  if(getNewOne) return new dmux4to16();
  return 0;
}

void dmux4to16::createSymbol()
{
  Lines.append(new Line(-30, -90, 30,-90,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30, -90, 30, 110,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  110,-30, 110,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30,  110,-30, -90,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-50,-50,-40,-50,QPen(Qt::darkBlue,2)));  // EN
  Lines.append(new Line(-50,-30,-30,-30,QPen(Qt::darkBlue,2)));  // A
  Lines.append(new Line(-50,-10,-30,-10,QPen(Qt::darkBlue,2)));  // B
  Lines.append(new Line(-50, 10,-30, 10,QPen(Qt::darkBlue,2)));  // C
  Lines.append(new Line(-50, 30,-30, 30,QPen(Qt::darkBlue,2)));  // D

  Lines.append(new Line( 30, 100, 50,100,QPen(Qt::darkBlue,2)));  // Y15
  Lines.append(new Line( 30,  90, 50, 90,QPen(Qt::darkBlue,2)));  // Y14
  Lines.append(new Line( 30,  80, 50, 80,QPen(Qt::darkBlue,2)));  // Y13
  Lines.append(new Line( 30,  70, 50, 70,QPen(Qt::darkBlue,2)));  // Y12
  Lines.append(new Line( 30,  60, 50, 60,QPen(Qt::darkBlue,2)));  // Y11
  Lines.append(new Line( 30,  50, 50, 50,QPen(Qt::darkBlue,2)));  // Y10
  Lines.append(new Line( 30,  40, 50, 40,QPen(Qt::darkBlue,2)));  // Y9
  Lines.append(new Line( 30,  30, 50, 30,QPen(Qt::darkBlue,2)));  // Y8
  Lines.append(new Line( 30,  20, 50, 20,QPen(Qt::darkBlue,2)));  // Y7
  Lines.append(new Line( 30,  10, 50, 10,QPen(Qt::darkBlue,2)));  // Y6
  Lines.append(new Line( 30,   0, 50,  0,QPen(Qt::darkBlue,2)));  // Y5
  Lines.append(new Line( 30, -10, 50,-10,QPen(Qt::darkBlue,2)));  // Y4
  Lines.append(new Line( 30, -20, 50,-20,QPen(Qt::darkBlue,2)));  // Y3
  Lines.append(new Line( 30, -30, 50,-30,QPen(Qt::darkBlue,2)));  // Y2
  Lines.append(new Line( 30, -40, 50,-40,QPen(Qt::darkBlue,2)));  // Y1
  Lines.append(new Line( 30, -50, 50,-50,QPen(Qt::darkBlue,2)));  // Y0

  Arcs.append(new Arc( -40, -55, 10, 10, 0, 16*360, QPen(Qt::darkBlue,2)));
 
  Texts.append(new Text(-25,-85, "DMUX", Qt::darkBlue, 12.0));

  Texts.append(new Text(-25,-63, "En", Qt::darkBlue, 12.0));
  Texts.append(new Text(-26,-15, "G", Qt::darkBlue, 12.0));
  Texts.append(new Text(-13,-20, "}", Qt::darkBlue, 16.0));
  Texts.append(new Text( -5,-20, "0", Qt::darkBlue, 12.0));
  Texts.append(new Text( -8,  0, "15", Qt::darkBlue, 12.0));

  Texts.append(new Text(-25,-43, "0", Qt::darkBlue, 12.0));
  Texts.append(new Text(-25, 17, "3", Qt::darkBlue, 12.0));

  Texts.append(new Text( 15,-59, "0", Qt::darkBlue, 9.0));
  Texts.append(new Text( 15,-49, "1", Qt::darkBlue, 9.0));
  Texts.append(new Text( 15,-39, "2", Qt::darkBlue, 9.0));
  Texts.append(new Text( 15,-29, "3", Qt::darkBlue, 9.0));
  Texts.append(new Text( 15,-19, "4", Qt::darkBlue, 9.0));
  Texts.append(new Text( 15, -9, "5", Qt::darkBlue, 9.0));
  Texts.append(new Text( 15,  1, "6", Qt::darkBlue, 9.0));
  Texts.append(new Text( 15, 11, "7", Qt::darkBlue, 9.0));
  Texts.append(new Text( 15, 21, "8", Qt::darkBlue, 9.0));
  Texts.append(new Text( 15, 31, "9", Qt::darkBlue, 9.0));
  Texts.append(new Text(  8, 41, "10", Qt::darkBlue, 9.0));
  Texts.append(new Text(  8, 51, "11", Qt::darkBlue, 9.0));
  Texts.append(new Text(  8, 61, "12", Qt::darkBlue, 9.0));
  Texts.append(new Text(  8, 71, "13", Qt::darkBlue, 9.0));
  Texts.append(new Text(  8, 81, "14", Qt::darkBlue, 9.0));
  Texts.append(new Text(  8, 91, "15", Qt::darkBlue, 9.0));

  Lines.append(new Line(-6,  2, 9,  2, QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-50,-50));  // En
  Ports.append(new Port(-50,-30));  // A
  Ports.append(new Port(-50,-10));  // B
  Ports.append(new Port(-50, 10));  // C
  Ports.append(new Port(-50, 30));  // D
  Ports.append(new Port( 50,100));  // Y15
  Ports.append(new Port( 50, 90));  // Y14
  Ports.append(new Port( 50, 80));  // Y13
  Ports.append(new Port( 50, 70));  // Y12
  Ports.append(new Port( 50, 60));  // Y11
  Ports.append(new Port( 50, 50));  // Y10
  Ports.append(new Port( 50, 40));  // Y9
  Ports.append(new Port( 50, 30));  // Y8
  Ports.append(new Port( 50, 20));  // Y7
  Ports.append(new Port( 50, 10));  // Y6
  Ports.append(new Port( 50,  0));  // Y5
  Ports.append(new Port( 50,-10));  // Y4
  Ports.append(new Port( 50,-20));  // Y3
  Ports.append(new Port( 50,-30));  // Y2
  Ports.append(new Port( 50,-40));  // Y1
  Ports.append(new Port( 50,-50));  // Y0

  x1 = -50; y1 = -94;
  x2 =  50; y2 = 114;
}

QString dmux4to16::vhdlCode( int )
{
  QString s="";

  QString td = Props.at(1)->Value;     // delay time
  if(!VHDL_Delay(td, Name)) return td; // time has not VHDL format
  td += ";\n";

  QString En = Ports.at(0)->Connection->Name;
  QString A  = Ports.at(1)->Connection->Name;
  QString B  = Ports.at(2)->Connection->Name;
  QString C  = Ports.at(3)->Connection->Name;
  QString D  = Ports.at(4)->Connection->Name;
  QString Y15 = Ports.at(5)->Connection->Name;
  QString Y14 = Ports.at(6)->Connection->Name;
  QString Y13 = Ports.at(7)->Connection->Name;
  QString Y12 = Ports.at(8)->Connection->Name;
  QString Y11 = Ports.at(9)->Connection->Name;
  QString Y10 = Ports.at(10)->Connection->Name;
  QString Y9 = Ports.at(11)->Connection->Name;
  QString Y8 = Ports.at(12)->Connection->Name;
  QString Y7 = Ports.at(13)->Connection->Name;
  QString Y6 = Ports.at(14)->Connection->Name;
  QString Y5 = Ports.at(15)->Connection->Name;
  QString Y4 = Ports.at(16)->Connection->Name;
  QString Y3 = Ports.at(17)->Connection->Name;
  QString Y2 = Ports.at(18)->Connection->Name;
  QString Y1 = Ports.at(19)->Connection->Name;
  QString Y0 = Ports.at(20)->Connection->Name;

  s = "\n  "+Name+":process ("+En+", "+A+", "+B+", "+C+", "+D+")\n"+
      "  begin\n" +
      "    "+Y0+" <= "+"(not "+En+") and (not "+D+") and (not "+C+") and (not "+B+") and (not "+A+")"+td+ 
      "    "+Y1+" <= "+"(not "+En+") and (not "+D+") and (not "+C+") and (not "+B+") and "+A+td+
      "    "+Y2+" <= "+"(not "+En+") and (not "+D+") and (not "+C+") and "+B+" and (not "+A+")"+td+
      "    "+Y3+" <= "+"(not "+En+") and (not "+D+") and (not "+C+") and "+B+" and "+A+td+  
      "    "+Y4+" <= "+"(not "+En+") and (not "+D+") and "+C+" and (not "+B+") and (not "+A+")"+td+ 
      "    "+Y5+" <= "+"(not "+En+") and (not "+D+") and "+C+" and (not "+B+") and "+A+td+
      "    "+Y6+" <= "+"(not "+En+") and (not "+D+") and "+C+" and "+B+" and (not "+A+")"+td+
      "    "+Y7+" <= "+"(not "+En+") and (not "+D+") and "+C+" and "+B+" and "+A+td+  
      "    "+Y8+" <= "+"(not "+En+") and "+D+" and (not "+C+") and (not "+B+") and (not "+A+")"+td+ 
      "    "+Y9+" <= "+"(not "+En+") and "+D+" and (not "+C+") and (not "+B+") and "+A+td+
      "    "+Y10+" <= "+"(not "+En+") and "+D+" and (not "+C+") and "+B+" and (not "+A+")"+td+
      "    "+Y11+" <= "+"(not "+En+") and "+D+" and (not "+C+") and "+B+" and "+A+td+  
      "    "+Y12+" <= "+"(not "+En+") and "+D+" and "+C+" and (not "+B+") and (not "+A+")"+td+ 
      "    "+Y13+" <= "+"(not "+En+") and "+D+" and "+C+" and (not "+B+") and "+A+td+
      "    "+Y14+" <= "+"(not "+En+") and "+D+" and "+C+" and "+B+" and (not "+A+")"+td+
      "    "+Y15+" <= "+"(not "+En+") and "+D+" and "+C+" and "+B+" and "+A+td+
         
      "  end process;\n";
  return s;
}

QString dmux4to16::verilogCode( int )
{
  QString td = Props.at(1)->Value;        // delay time
  if(!Verilog_Delay(td, Name)) return td; // time does not have VHDL format
  
  QString l = "";

  QString En = Ports.at(0)->Connection->Name;
  QString A  = Ports.at(1)->Connection->Name;
  QString B  = Ports.at(2)->Connection->Name;
  QString C  = Ports.at(3)->Connection->Name;
  QString D  = Ports.at(4)->Connection->Name;
  QString Y15 = Ports.at(5)->Connection->Name;
  QString Y14 = Ports.at(6)->Connection->Name;
  QString Y13 = Ports.at(7)->Connection->Name;
  QString Y12 = Ports.at(8)->Connection->Name;
  QString Y11 = Ports.at(9)->Connection->Name;
  QString Y10 = Ports.at(10)->Connection->Name;
  QString Y9 = Ports.at(11)->Connection->Name;
  QString Y8 = Ports.at(12)->Connection->Name;
  QString Y7 = Ports.at(13)->Connection->Name;
  QString Y6 = Ports.at(14)->Connection->Name;
  QString Y5 = Ports.at(15)->Connection->Name;
  QString Y4 = Ports.at(16)->Connection->Name;
  QString Y3 = Ports.at(17)->Connection->Name;
  QString Y2 = Ports.at(18)->Connection->Name;
  QString Y1 = Ports.at(19)->Connection->Name;
  QString Y0 = Ports.at(20)->Connection->Name;

  QString Y15R = "net_reg" + Name + Y15;
  QString Y14R = "net_reg" + Name + Y14;
  QString Y13R = "net_reg" + Name + Y13;
  QString Y12R = "net_reg" + Name + Y12;  
  QString Y11R = "net_reg" + Name + Y11;
  QString Y10R = "net_reg" + Name + Y10;
  QString Y9R = "net_reg" + Name + Y9;
  QString Y8R = "net_reg" + Name + Y8;
  QString Y7R = "net_reg" + Name + Y7;
  QString Y6R = "net_reg" + Name + Y6;
  QString Y5R = "net_reg" + Name + Y5;
  QString Y4R = "net_reg" + Name + Y4;  
  QString Y3R = "net_reg" + Name + Y3;
  QString Y2R = "net_reg" + Name + Y2;
  QString Y1R = "net_reg" + Name + Y1;
  QString Y0R = "net_reg" + Name + Y0;

  l = "\n  // " + Name + " 4to16 demux\n" +
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
      "  assign  " + Y8 + " = " + Y8R + ";\n" +
      "  reg     " + Y8R + " = 0;\n" +
      "  assign  " + Y9 + " = " + Y9R + ";\n" +
      "  reg     " + Y9R + " = 0;\n" +
      "  assign  " + Y10 + " = " + Y10R + ";\n" +
      "  reg     " + Y10R + " = 0;\n" +
      "  assign  " + Y11 + " = " + Y11R + ";\n" +
      "  reg     " + Y11R + " = 0;\n" +
      "  assign  " + Y12 + " = " + Y12R + ";\n" +
      "  reg     " + Y12R + " = 0;\n" +
      "  assign  " + Y13 + " = " + Y13R + ";\n" +
      "  reg     " + Y13R + " = 0;\n" +
      "  assign  " + Y14 + " = " + Y14R + ";\n" +
      "  reg     " + Y14R + " = 0;\n" +
      "  assign  " + Y15 + " = " + Y15R + ";\n" +
      "  reg     " + Y15R + " = 0;\n" +

      "  always @ ("+En+" or "+A+" or "+B+" or "+C+" or "+D+")\n" +
      "  begin\n"+
      "    "+Y0R+" <="+td+" (~"+En+") && (~"+D+") && (~"+C+") && (~"+B+" ) && (~"+A+");\n"+ 
      "    "+Y1R+" <="+td+" (~"+En+") && (~"+D+") && (~"+C+") && (~"+B+" ) && "+A+";\n"+
      "    "+Y2R+" <="+td+" (~"+En+") && (~"+D+") && (~"+C+") && "+B+" && (~"+A+");\n"+
      "    "+Y3R+" <="+td+" (~"+En+") && (~"+D+") && (~"+C+") && "+B+" && "+A+";\n"+  
      "    "+Y4R+" <="+td+" (~"+En+") && (~"+D+") && "+C+" && (~"+B+" ) && (~"+A+");\n"+ 
      "    "+Y5R+" <="+td+" (~"+En+") && (~"+D+") && "+C+" && (~"+B+" ) && "+A+";\n"+
      "    "+Y6R+" <="+td+" (~"+En+") && (~"+D+") && "+C+" && "+B+" && ( ~"+A+");\n"+
      "    "+Y7R+" <="+td+" (~"+En+") && (~"+D+") && "+C+" && "+B+" && "+A+";\n"+ 
      "    "+Y8R+" <="+td+" (~"+En+") && "+D+" && (~"+C+") && ( ~"+B+") && (~"+A+");\n"+ 
      "    "+Y9R+" <="+td+" (~"+En+") && "+D+" && (~"+C+") && ( ~"+B+") && "+A+";\n"+
      "    "+Y10R+" <="+td+" (~"+En+") && "+D+" && (~"+C+") && "+B+" && (~"+A+");\n"+
      "    "+Y11R+" <="+td+" (~"+En+") && "+D+" && (~"+C+") && "+B+" && "+A+";\n"+  
      "    "+Y12R+" <="+td+" (~"+En+") && "+D+" && "+C+" && (~"+B+") && (~"+A+");\n"+ 
      "    "+Y13R+" <="+td+" (~"+En+") && "+D+" && "+C+" && (~"+B+") && "+A+";\n"+
      "    "+Y14R+" <="+td+" (~"+En+") && "+D+" && "+C+" && "+B+" && (~"+A+");\n"+
      "    "+Y15R+" <="+td+" (~"+En+") && "+D+" && "+C+" && "+B+" && "+A+";\n"+ 
            
      "  end\n";

  return l;
}
