/***************************************************************************
                             andor4x3
                            ----------
    begin                : December 2008
    copyright            : (C) 2008 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/

/*
 * andor4x3.cpp - device implementations for andor4x3 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */
#include "andor4x3.h"
#include "node.h"
#include "main.h"
#include "misc.h"

andor4x3::andor4x3()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("4x3 andor verilog device");

  Props.append (new Property ("TR", "6", false,
    QObject::tr ("transfer function high scaling factor")));
  Props.append (new Property ("Delay", "1 ns", false,
    QObject::tr ("output delay")
    +" ("+QObject::tr ("s")+")"));
 
  createSymbol ();
  tx = x1 + 19;
  ty = y2 + 4;
  Model = "andor4x3";
  Name  = "Y";
}

Component * andor4x3::newOne()
{
  andor4x3 * p = new andor4x3();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * andor4x3::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("4x3 AndOr");
  BitmapFile = (char *) "andor4x3";

  if(getNewOne) return new andor4x3();
  return 0;
}

void andor4x3::createSymbol()
{
  Lines.append(new Line(-30, -60, 30,-60,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30, -60, 30, 100,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  100,-30,100,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30,  100,-30,-60,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-30, -20,  0,-20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30,  20,  0, 20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30,  60,  0, 60,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0, -60,  0,100,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-50,-50,-30,-50,QPen(Qt::darkBlue,2)));   //A11
  Lines.append(new Line(-50,-40,-30,-40,QPen(Qt::darkBlue,2)));   //A12
  Lines.append(new Line(-50,-30,-30,-30,QPen(Qt::darkBlue,2)));   //A13

  Lines.append(new Line(-50,-10,-30,-10,QPen(Qt::darkBlue,2)));   //A21
  Lines.append(new Line(-50,  0,-30,  0,QPen(Qt::darkBlue,2)));   //A22
  Lines.append(new Line(-50, 10,-30, 10,QPen(Qt::darkBlue,2)));   //A23

  Lines.append(new Line(-50, 30, -30, 30,QPen(Qt::darkBlue,2)));  //A31
  Lines.append(new Line(-50, 40, -30, 40,QPen(Qt::darkBlue,2)));  //A32
  Lines.append(new Line(-50, 50,-30,  50,QPen(Qt::darkBlue,2)));   //A33

  Lines.append(new Line(-50, 70, -30, 70,QPen(Qt::darkBlue,2)));  //A41
  Lines.append(new Line(-50, 80, -30, 80,QPen(Qt::darkBlue,2)));  //A42
  Lines.append(new Line(-50, 90,-30,  90,QPen(Qt::darkBlue,2)));   //A43

  Lines.append(new Line( 30,  20, 50, 20,QPen(Qt::darkBlue,2)));  //Y

  Texts.append(new Text( -20, -60, "&", Qt::darkBlue, 12.0));
  Texts.append(new Text( -20, -20, "&", Qt::darkBlue, 12.0));
  Texts.append(new Text( -20,  20, "&", Qt::darkBlue, 12.0));
  Texts.append(new Text( -20,  60, "&", Qt::darkBlue, 12.0));
 
  Lines.append(new Line(  7, -45, 17, -40,QPen(Qt::darkBlue,2))); 
  Lines.append(new Line(  7, -35, 17, -40,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  7, -30, 17, -35,QPen(Qt::darkBlue,2))); 
  Lines.append(new Line( 22, -30, 22, -45,QPen(Qt::darkBlue,2)));  

  Ports.append(new Port(-50,-50));  // A11
  Ports.append(new Port(-50,-40));  // A12
  Ports.append(new Port(-50,-30));  // A13

  Ports.append(new Port(-50,-10));  // A21
  Ports.append(new Port(-50,  0));  // A22
  Ports.append(new Port(-50, 10));  // A23

  Ports.append(new Port(-50, 30));  // A31
  Ports.append(new Port(-50, 40));  // A32
  Ports.append(new Port(-50, 50));  // A33

  Ports.append(new Port(-50, 70));  // A41
  Ports.append(new Port(-50, 80));  // A42
  Ports.append(new Port(-50, 90));  // A43

  Ports.append(new Port( 50, 20));  // Y

  x1 = -50; y1 = -64;
  x2 =  50; y2 =  104;

}

QString andor4x3::vhdlCode( int )
{
  QString s="";

  QString td = Props.at(1)->Value;     // delay time
  if(!VHDL_Delay(td, Name)) return td; // time has not VHDL format
  td += ";\n";

  QString a11 = Ports.at(0)->Connection->Name;
  QString a12 = Ports.at(1)->Connection->Name;
  QString a13 = Ports.at(2)->Connection->Name;
  QString a21 = Ports.at(3)->Connection->Name;
  QString a22 = Ports.at(4)->Connection->Name;
  QString a23 = Ports.at(5)->Connection->Name;
  QString a31 = Ports.at(6)->Connection->Name;
  QString a32 = Ports.at(7)->Connection->Name;
  QString a33 = Ports.at(8)->Connection->Name;
  QString a41 = Ports.at(9)->Connection->Name;
  QString a42 = Ports.at(10)->Connection->Name;
  QString a43 = Ports.at(11)->Connection->Name;
  QString y   = Ports.at(12)->Connection->Name;

  s = "\n  " + Name + ":process (" +
      a11 + ", " +  a12 + ", " + a13 + ", " +
      a21 + ", " +  a22 + ", " + a23 + ", " +
      a31 + ", " +  a32 + ", " + a33 + ", " +
      a41 + ", " +  a42 + ", " + a43 +")\n"  +
     "  begin\n    " +
     y + " <= " + "(" + a11 + " and " + a12 + " and " + a13 + ") or " + 
                  "(" + a21 + " and " + a22 + " and " + a23 + ") or\n" +
     "                 (" + a31 + " and " + a32 + " and " + a33 + ") or " + 
                 "(" + a41 + " and " + a42 + " and " + a43 + ")" + td  +
     "  end process;\n";
  return s;
}

QString andor4x3::verilogCode( int )
{
  QString td = Props.at(1)->Value;        // delay time
  if(!Verilog_Delay(td, Name)) return td; // time does not have VHDL format
  
  QString l = "";

  QString a11 = Ports.at(0)->Connection->Name;
  QString a12 = Ports.at(1)->Connection->Name;
  QString a13 = Ports.at(2)->Connection->Name;
  QString a21 = Ports.at(3)->Connection->Name;
  QString a22 = Ports.at(4)->Connection->Name;
  QString a23 = Ports.at(5)->Connection->Name;
  QString a31 = Ports.at(6)->Connection->Name;
  QString a32 = Ports.at(7)->Connection->Name;
  QString a33 = Ports.at(8)->Connection->Name;
  QString a41 = Ports.at(9)->Connection->Name;
  QString a42 = Ports.at(10)->Connection->Name;
  QString a43 = Ports.at(11)->Connection->Name;
  QString y   = Ports.at(12)->Connection->Name;
 
  QString v = "net_reg" + Name + y;
  
  l = "\n  // " + Name + " 4x3 andor\n" +
      "  assign  " + y + " = " + v + ";\n" +
      "  reg     " + v + " = 0;\n" +
      "  always @ (" + a11 + " or " + a12 + " or " + a13 + " or "
                     + a21 + " or " + a22 + " or " + a23 + " or "
		     + a31 + " or " + a32 + " or " + a33 + " or "
                     + a41 + " or " + a42 + " or " + a43 + ")\n  " +
      "  " + v + " <=" + td + " (" + a11 + " && " + a12 + " && " + a13 +")" + " || " +
                               "(" + a21 + " && " + a22 + " && " + a23 +")" + " ||\n" +
                 "                         (" + a31 + " && " + a32 + " && " + a33 +")" + " || " +
                               "(" + a41 + " && " + a42 + " && " + a43 +");\n " ;

  return l;
}

