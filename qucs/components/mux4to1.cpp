/***************************************************************************
                              mux4to1
                             ---------
    begin                : December 2008
    copyright            : (C) 2008 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/

/*
 * mux4to1.cpp - device implementations for mux4to1 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */
#include "mux4to1.h"
#include "node.h"
#include "misc.h"

mux4to1::mux4to1()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("4to1 multiplexer verilog device");

  Props.append (new Property ("TR", "6", false,
    QObject::tr ("transfer function high scaling factor")));
  Props.append (new Property ("Delay", "1 ns", false,
    QObject::tr ("output delay")
    +" ("+QObject::tr ("s")+")"));
 
  createSymbol ();
  tx = x1 + 19;
  ty = y2 + 4;
  Model = "mux4to1";
  Name  = "Y";
}

Component * mux4to1::newOne()
{
  mux4to1 * p = new mux4to1();
  p->Props.front()->Value = Props.front()->Value; 
  p->recreate();
  return p;
}

Element * mux4to1::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("4to1 Mux");
  BitmapFile = (char *) "mux4to1";

  if(getNewOne) return new mux4to1();
  return 0;
}

void mux4to1::createSymbol()
{
  Rects.append(new qucs::Rect(-30, -80, 60, 180, QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin)));

  Lines.append(new qucs::Line(-50,-40,-40,-40,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-50,-20,-30, -20,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-50, 0, -30, 0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-50, 30, -30, 30,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-50, 50,-30, 50,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-50, 70, -30, 70,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-50, 90, -30, 90,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line( 30, 10, 50, 10,QPen(Qt::darkBlue,2)));

  Ellipses.append(new qucs::Ellips( -40, -45, 10, 10, QPen(Qt::darkBlue,2)));
 
  Texts.append(new Text(-14,-75, "MUX", Qt::darkBlue, 12.0));

  Texts.append(new Text(-25,-48, "En", Qt::darkBlue, 12.0));
  Texts.append(new Text(-14,-16, "G", Qt::darkBlue, 12.0));
  Texts.append(new Text(-1, -22, "}", Qt::darkBlue, 20.0));
  Texts.append(new Text( 12,-26, "0", Qt::darkBlue, 12.0));
  Texts.append(new Text( 12, -6, "3", Qt::darkBlue, 12.0));

  Texts.append(new Text(-25,-28, "0", Qt::darkBlue, 12.0));
  Texts.append(new Text(-25, -8, "1", Qt::darkBlue, 12.0));

  Texts.append(new Text(-25, 22, "0", Qt::darkBlue, 12.0));
  Texts.append(new Text(-25, 42, "1", Qt::darkBlue, 12.0));
  Texts.append(new Text(-25, 62, "2", Qt::darkBlue, 12.0));
  Texts.append(new Text(-25, 82, "3", Qt::darkBlue, 12.0));

  Lines.append(new qucs::Line(11, -8, 23, -8, QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-50,-40));  // En
  Ports.append(new Port(-50,-20));  // A
  Ports.append(new Port(-50,  0));  // B
  Ports.append(new Port(-50, 30));  // D0
  Ports.append(new Port(-50, 50));  // D1
  Ports.append(new Port(-50, 70));  // D2
  Ports.append(new Port(-50, 90));  // D3
  Ports.append(new Port( 50, 10));  // Y

  x1 = -50; y1 = -84;
  x2 =  50; y2 =  104;
}

QString mux4to1::vhdlCode( int )
{
  QString s="";

  QString td = Props.at(1)->Value;     // delay time
  if(!misc::VHDL_Delay(td, Name)) return td; // time has not VHDL format
  td += ";\n";

  QString En = Ports.at(0)->Connection->Name;
  QString A  = Ports.at(1)->Connection->Name;
  QString B  = Ports.at(2)->Connection->Name;
  QString D0 = Ports.at(3)->Connection->Name;
  QString D1 = Ports.at(4)->Connection->Name;
  QString D2 = Ports.at(5)->Connection->Name;
  QString D3 = Ports.at(6)->Connection->Name;
  QString y  = Ports.at(7)->Connection->Name;

  s = "\n  " + Name + ":process (" + En + ", " +  A + ", " + B + ", " +
                              D0 + ", " +  D1 + ", " + D2 + ", " + D3 + ")\n"  +
     "  begin\n    " +
     y + " <= " +  "(not " + En + ") and ((" + D3 + " and " + B + " and " +  A + ") or\n" + 
         "                 (" + D2 + " and " + B + " and not " + A + ") or\n" +
         "                 (" + D1 + " and not " + B + " and " + A + ") or\n" +
         "                 (" + D0 + " and not " + B + " and not " + A + ")" + ")" + td +
     "  end process;\n";
  return s;
}

QString mux4to1::verilogCode( int )
{
  QString td = Props.at(1)->Value;        // delay time
  if(!misc::Verilog_Delay(td, Name)) return td; // time does not have VHDL format
  
  QString l = "";

  QString En = Ports.at(0)->Connection->Name;
  QString A  = Ports.at(1)->Connection->Name;
  QString B  = Ports.at(2)->Connection->Name;
  QString D0 = Ports.at(3)->Connection->Name;
  QString D1 = Ports.at(4)->Connection->Name;
  QString D2 = Ports.at(5)->Connection->Name;
  QString D3 = Ports.at(6)->Connection->Name;
  QString y  = Ports.at(7)->Connection->Name;

  QString v = "net_reg" + Name + y;
  
  l = "\n  // " + Name + " 4to1 mux\n" +
      "  assign  " + y + " = " + v + ";\n" +
      "  reg     " + v + " = 0;\n" +
      "  always @ (" + En + " or " + A + " or " + B + " or " 
                     + D0 + " or " + D1 +  " or " + D2 + " or " + D0 + ")\n" +
      "    " + v + " <=" + td + " (" + D3 + " && " + B + " && " + A  +")" + " ||\n" +
      "                         (" + D2 + " && " + B + " && ~"+ A  +")" + " ||\n" +
      "                         (" + D1 + " && ~"+ B + " && " + A  +")" + " ||\n" +
      "                         (" + D0 + " && ~"+ B + " && ~"+ A  +");\n";

  return l;
}

