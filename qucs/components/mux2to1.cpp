/***************************************************************************
                             mux2to1
                            ---------
    begin                : December 2008
    copyright            : (C) 2008 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/

/*
 * mux2to1.cpp - device implementations for mux2to1 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */
#include "mux2to1.h"
#include "node.h"
#include "misc.h"

mux2to1::mux2to1()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("2to1 multiplexer verilog device");

  Props.append (new Property ("TR", "6", false,
    QObject::tr ("transfer function high scaling factor")));
  Props.append (new Property ("Delay", "1 ns", false,
    QObject::tr ("output delay")
    +" ("+QObject::tr ("s")+")"));
 
  createSymbol ();
  tx = x1 + 19;
  ty = y2 + 4;
  Model = "mux2to1";
  Name  = "Y";
}

Component * mux2to1::newOne()
{
  mux2to1 * p = new mux2to1();
  p->Props.front()->Value = Props.front()->Value; 
  p->recreate(0); 
  return p;
}

Element * mux2to1::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("2to1 Mux");
  BitmapFile = (char *) "mux2to1";

  if(getNewOne) return new mux2to1();
  return 0;
}

void mux2to1::createSymbol()
{
  // put in here symbol drawing code and terminal definitions
  Rects.append(new qucs::Rect(-30, -60, 60, 110, QPen(Qt::darkBlue,2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin)));

  Lines.append(new qucs::Line(-50,-20,-40,-20,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-50,  0,-30,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-50, 20,-30, 20,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-50, 40,-30, 40,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line( 30, 0, 50, 0,QPen(Qt::darkBlue,2)));

  Ellipses.append(new qucs::Ellips(-40, -25, 10, 10, QPen(Qt::darkBlue,2)));
 
  Texts.append(new Text(-14,-55, "MUX", Qt::darkBlue, 12.0));

  Texts.append(new Text(-25,-28, "En", Qt::darkBlue, 12.0));
  Texts.append(new Text(-25, -8, "0", Qt::darkBlue, 12.0));

  Texts.append(new Text(-15,-8, "G", Qt::darkBlue, 12.0));
  Texts.append(new Text( -1,-14, "}", Qt::darkBlue, 20.0));
  Texts.append(new Text( 12,-18, "0", Qt::darkBlue, 12.0));
  Texts.append(new Text( 12,  2, "1", Qt::darkBlue, 12.0));

  Lines.append(new qucs::Line(11, 0, 23, 0, QPen(Qt::darkBlue,2)));

  Texts.append(new Text(-25, 12, "0", Qt::darkBlue, 12.0));
  Texts.append(new Text(-25, 32, "1", Qt::darkBlue, 12.0));

  Ports.append(new Port(-50,-20));  // En
  Ports.append(new Port(-50,  0));  // A
  Ports.append(new Port(-50, 20));  // D0
  Ports.append(new Port(-50, 40));  // D1
  Ports.append(new Port( 50, 0 ));  // Y

  x1 = -50; y1 = -64;
  x2 =  50; y2 =  54;
}

QString mux2to1::vhdlCode( int )
{
  QString s="";

  QString td = Props.at(1)->Value;
  if(!misc::VHDL_Delay(td, Name))
    return td;      // Time does not have VHDL format.
  td += ";\n";

  QString En = Ports.at(0)->Connection->Name;
  QString A  = Ports.at(1)->Connection->Name;
  QString D0 = Ports.at(2)->Connection->Name;
  QString D1 = Ports.at(3)->Connection->Name;
  QString y  = Ports.at(4)->Connection->Name;

  s = "\n  " + Name + ":process (" + En + ", " +  A + ", " + D0 + ", " +  D1 + ")\n" +
     "  begin\n" +
     "    " + y + " <= " +  "(not " + En + ") and ((" + D1 + " and " + A + ") or " + 
                  "(" + D0 + " and " + "(not " + A + ")))" + td +
     "  end process;\n";
  return s;
}

QString mux2to1::verilogCode( int )
{
  QString td = Props.at(1)->Value;
  if(!misc::Verilog_Delay(td, Name))
    return td;      // Time does not have VHDL format.
  td += " ";
  
  QString l = "";

  QString En = Ports.at(0)->Connection->Name;
  QString A  = Ports.at(1)->Connection->Name;
  QString D0 = Ports.at(2)->Connection->Name;
  QString D1 = Ports.at(3)->Connection->Name;
  QString y  = Ports.at(4)->Connection->Name;

  QString v = "net_reg" + Name + y;
  
  l = "\n  // " + Name + " 2to1 mux\n" +
      "  assign  " + y + " = " + v + ";\n" +
      "  reg     " + v + " = 0;\n" +
      "  always @ (" + En + " or " + A + " or "
                     + D0 + " or " + D1 +  ")\n" +
      "    " + v + " <=" + td + "(" + D1 + " && " + A + ")" + " || " +
               "(" + D0 + " && (~" + A + "));\n" ;

  return l;
}

