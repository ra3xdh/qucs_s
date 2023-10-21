/***************************************************************************
                              mux8to1
                             ---------
    begin                : December 2008
    copyright            : (C) 2008 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/

/*
 * mux8to1.cpp - device implementations for mux8to1 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */
#include "mux8to1.h"
#include "node.h"
#include "misc.h"

mux8to1::mux8to1()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("8to1 multiplexer verilog device");

  Props.append (new Property ("TR", "6", false,
    QObject::tr ("transfer function high scaling factor")));
  Props.append (new Property ("Delay", "1 ns", false,
    QObject::tr ("output delay")
    +" ("+QObject::tr ("s")+")"));
 
  createSymbol ();
  tx = x1 + 19;
  ty = y2 + 4;
  icon_dy = -112;
  Model = "mux8to1";
  Name  = "Y";
}

Component * mux8to1::newOne()
{
  mux8to1 * p = new mux8to1();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * mux8to1::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("8to1 Mux");
  BitmapFile = (char *) "mux8to1";

  if(getNewOne) return new mux8to1();
  return 0;
}

void mux8to1::createSymbol()
{
  Lines.append(new qucs::Line(-30, -80, 30,-80,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30, -80, 30, 190,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30, 190,-30, 190,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-30, 190,-30, -80,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(-50,-40,-40,-40,QPen(Qt::darkBlue,2)));  //EN
  Lines.append(new qucs::Line(-50,-20,-30, -20,QPen(Qt::darkBlue,2))); //A
  Lines.append(new qucs::Line(-50, 0, -30, 0,QPen(Qt::darkBlue,2)));   //B
  Lines.append(new qucs::Line(-50, 20, -30, 20,QPen(Qt::darkBlue,2))); //C

  Lines.append(new qucs::Line(-50, 40, -30, 40,QPen(Qt::darkBlue,2))); //D0
  Lines.append(new qucs::Line(-50, 60, -30, 60,QPen(Qt::darkBlue,2))); //D1
  Lines.append(new qucs::Line(-50, 80, -30, 80,QPen(Qt::darkBlue,2))); //D2
  Lines.append(new qucs::Line(-50, 100,-30,100,QPen(Qt::darkBlue,2))); //D3
  Lines.append(new qucs::Line(-50, 120,-30,120,QPen(Qt::darkBlue,2))); //D4
  Lines.append(new qucs::Line(-50, 140,-30,140,QPen(Qt::darkBlue,2))); //D5
  Lines.append(new qucs::Line(-50, 160,-30,160,QPen(Qt::darkBlue,2))); //D6
  Lines.append(new qucs::Line(-50, 180,-30,180,QPen(Qt::darkBlue,2))); //D7
  Lines.append(new qucs::Line( 30, 60, 50, 60,QPen(Qt::darkBlue,2)));  // Y

  Arcs.append(new qucs::Arc( -40, -45, 10, 10, 0, 16*360, QPen(Qt::darkBlue,2)));
 
  Texts.append(new Text(-17,-75, "MUX", Qt::darkBlue, 12.0));

  Texts.append(new Text(-25,-53, "En", Qt::darkBlue, 12.0));
  Texts.append(new Text(-14,-13, "G", Qt::darkBlue, 12.0));
  Texts.append(new Text(-1, -18, "}", Qt::darkBlue, 16.0));
  Texts.append(new Text( 12,-22, "0", Qt::darkBlue, 12.0));
  Texts.append(new Text( 12, -2, "7", Qt::darkBlue, 12.0));

  Texts.append(new Text(-25,-31, "0", Qt::darkBlue, 12.0));
  Texts.append(new Text(-25,  7, "2", Qt::darkBlue, 12.0));

  Texts.append(new Text(-25, 27, "0", Qt::darkBlue, 12.0));
  Texts.append(new Text(-25, 47, "1", Qt::darkBlue, 12.0));
  Texts.append(new Text(-25, 67, "2", Qt::darkBlue, 12.0));
  Texts.append(new Text(-25, 87, "3", Qt::darkBlue, 12.0));
  Texts.append(new Text(-25,107, "4", Qt::darkBlue, 12.0));
  Texts.append(new Text(-25,127, "5", Qt::darkBlue, 12.0));
  Texts.append(new Text(-25,147, "6", Qt::darkBlue, 12.0));
  Texts.append(new Text(-25,167, "7", Qt::darkBlue, 12.0));

  Lines.append(new qucs::Line(11, 0, 23, 0, QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-50,-40));  // En
  Ports.append(new Port(-50,-20));  // A
  Ports.append(new Port(-50,  0));  // B
  Ports.append(new Port(-50, 20));  // C
  Ports.append(new Port(-50, 40));  // D0
  Ports.append(new Port(-50, 60));  // D1
  Ports.append(new Port(-50, 80));  // D2
  Ports.append(new Port(-50,100));  // D3
  Ports.append(new Port(-50,120));  // D4
  Ports.append(new Port(-50,140));  // D5
  Ports.append(new Port(-50,160));  // D6
  Ports.append(new Port(-50,180));  // D7
  Ports.append(new Port( 50, 60));  // Y

  x1 = -50; y1 = -84;
  x2 =  50; y2 =  194;
}

QString mux8to1::vhdlCode( int )
{
  QString s="";

  QString td = Props.at(1)->Value;     // delay time
  if(!misc::VHDL_Delay(td, Name)) return td; // time has not VHDL format
  td += ";\n";

  QString En = Ports.at(0)->Connection->Name;
  QString A  = Ports.at(1)->Connection->Name;
  QString B  = Ports.at(2)->Connection->Name;
  QString C  = Ports.at(3)->Connection->Name;
  QString D0 = Ports.at(4)->Connection->Name;
  QString D1 = Ports.at(5)->Connection->Name;
  QString D2 = Ports.at(6)->Connection->Name;
  QString D3 = Ports.at(7)->Connection->Name;
  QString D4 = Ports.at(8)->Connection->Name;
  QString D5 = Ports.at(9)->Connection->Name;
  QString D6 = Ports.at(10)->Connection->Name;
  QString D7 = Ports.at(11)->Connection->Name;
  QString y  = Ports.at(12)->Connection->Name;

  s = "\n  " + Name + ":process (" + En + ", " +  A + ", " + B + ", " + C + ", " +
                              D0 + ", " +  D1 + ", " + D2 + ", " + D3 + ", " +
                              D4 + ", " +  D5 + ", " + D6 + ", " + D7 + ")\n"  +
     "  begin\n    " +
     y + " <= " +  "(not " + En + ") and ((" + D7 + " and "  + C + " and " + B   + " and " +  A + ") or\n" + 
         "                 (" + D6 + " and " + C + " and " + B + " and not "+ A + ") or\n" +
         "                 (" + D5 + " and "  + C + " and not "+ B + " and " + A + ") or\n" +
         "                 (" + D4 + " and "  + C + " and not " + B + " and not "+A + ") or\n" + 
         "                 (" + D3 + " and not "  + C + " and " + B   +  " and " +  A + ") or\n" + 
         "                 (" + D2 + " and not "  + C + " and " + B   +  " and not "+ A + ") or\n" +
         "                 (" + D1 + " and not "  + C + " and not "+ B   +  " and " +  A + ") or\n" +
         "                 (" + D0 + " and not "  + C + " and not " + B +  " and not "+A + "))" + td +
     "  end process;\n";
  return s;
}

QString mux8to1::verilogCode( int )
{
  QString td = Props.at(1)->Value;        // delay time
  if(!misc::Verilog_Delay(td, Name)) return td; // time does not have VHDL format
  
  QString l = "";

  QString En = Ports.at(0)->Connection->Name;
  QString A  = Ports.at(1)->Connection->Name;
  QString B  = Ports.at(2)->Connection->Name;
  QString C  = Ports.at(3)->Connection->Name;
  QString D0 = Ports.at(4)->Connection->Name;
  QString D1 = Ports.at(5)->Connection->Name;
  QString D2 = Ports.at(6)->Connection->Name;
  QString D3 = Ports.at(7)->Connection->Name;
  QString D4 = Ports.at(8)->Connection->Name;
  QString D5 = Ports.at(9)->Connection->Name;
  QString D6 = Ports.at(10)->Connection->Name;
  QString D7 = Ports.at(11)->Connection->Name;
  QString y  = Ports.at(12)->Connection->Name;

  QString v = "net_reg" + Name + y;
  
  l = "\n  // " + Name + " 8to1 mux\n" +
      "  assign  " + y + " = " + v + ";\n" +
      "  reg     " + v + " = 0;\n" +
      "  always @ (" + En + " or " + A + " or " + B + " or " + C + " or " +
                       D0 + " or " + D1 + " or " + D2 + " or " + D3 +  " or " +
                       D4 + " or " + D5 + " or " + D6 + " or " + D7 + ")\n" +
      "    " + v + " <=" + td + " (" + D7 + " && "  + C + " && " + B + " && " + A  +")" + " ||\n" +
      "                         (" + D6 + " && "  + C + " && " + B + " && ~"+ A  +")" + " ||\n" +
      "                         (" + D5 + " && "  + C + " && ~"+ B + " && " + A  +")" + " ||\n" +
      "                         (" + D4 + " && "  + C + " && ~"+ B + " && ~"+ A  +")" + " ||\n" +
      "                         (" + D3 + " && ~" + C + " && " + B + " && " + A  +")" + " ||\n" +
      "                         (" + D2 + " && ~" + C + " && " + B + " && ~"+ A  +")" + " ||\n" +
      "                         (" + D1 + " && ~" + C + " && ~"+ B + " && " + A  +")" + " ||\n" +
      "                         (" + D0 + " && ~" + C + " && ~"+ B + " && ~"+ A  +")" + ";\n" ;

  return l;
}
