/***************************************************************************
                              logical_inv.cpp
                             -----------------
    begin                : Wed Sep 28 2005
    copyright            : (C) 2005 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "logical_inv.h"
#include "schematic.h"
#include "node.h"
#include "misc.h"


Logical_Inv::Logical_Inv()
{
  Type = isComponent;   // both analog and digital
  Description = QObject::tr("logical inverter");

  // the list order must be preserved !!!
  Props.append(new Property("V", "1 V", false,
		QObject::tr("voltage of high level")));
  Props.append(new Property("t", "0", false,
		QObject::tr("delay time")));
  Props.append(new Property("TR", "10", false,
		QObject::tr("transfer function scaling factor")));

  // this must be the last property in the list !!!
  Props.append(new Property("Symbol", "old", false,
		QObject::tr("schematic symbol")+" [old, DIN40900]"));

  createSymbol();
  tx = x1+4;
  ty = y2+4;
  Model = "Inv";
  Name  = "Y";
}

// -------------------------------------------------------
QString Logical_Inv::vhdlCode(int NumPorts)
{
  QString s = "  " + Ports.first()->Connection->Name + " <= not " +
              Ports.last()->Connection->Name;

  if(NumPorts <= 0) { // no truth table simulation ?
    QString td = Props.at(1)->Value;
    if(!VHDL_Delay(td, Name)) return td;
    s += td;
  }

  s += ";\n";
  return s;
}

// -------------------------------------------------------
QString Logical_Inv::verilogCode(int NumPorts)
{
  bool synthesize = true;
  Port *pp = Ports.at(0);
  QString s ("");

  if (synthesize) {
    s = "  assign";

    if(NumPorts <= 0) { // no truth table simulation ?
      QString td = Props.at(1)->Value;
      if(!Verilog_Delay(td, Name)) return td;
      s += td;
    }
    s += " ";
    s += pp->Connection->Name + " = ";  // output port
    pp = Ports.at(1);
    s += "~" + pp->Connection->Name;   // input port
    s += ";\n";
  }
  else {
    s = "  not";

    if(NumPorts <= 0) { // no truth table simulation ?
      QString td = Props.at(1)->Value;
      if(!Verilog_Delay(td, Name))
	return td;    // time has not VHDL format
      s += td;
    }
    s += " " + Name + " (" + pp->Connection->Name;  // output port
    pp = Ports.at(1);
    s += ", " + pp->Connection->Name; // first input port
    s += ");\n";
  }
  return s;
}

// -------------------------------------------------------
void Logical_Inv::createSymbol()
{
  int xr;

  if(Props.getLast()->Value.at(0) == 'D') {  // DIN symbol
    Lines.append(new Line( 15,-20, 15, 20,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-15,-20, 15,-20,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-15, 20, 15, 20,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-15,-20,-15, 20,QPen(Qt::darkBlue,2)));

    Texts.append(new Text(-11,-17, "1", Qt::darkBlue, 15.0));
    xr =  15;
  }
  else {   // old symbol
    Lines.append(new Line(-10,-20,-10,20, QPen(Qt::darkBlue,2)));
    Arcs.append(new Arc(-30,-20, 40, 30,  0, 16*90,QPen(Qt::darkBlue,2)));
    Arcs.append(new Arc(-30,-10, 40, 30,  0,-16*90,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( 10,-5, 10, 5,QPen(Qt::darkBlue,2)));
    xr =  10;
  }

  Ellips.append(new Area(xr,-4, 8, 8,
                QPen(Qt::darkBlue,0), QBrush(Qt::darkBlue)));

  Lines.append(new Line( xr, 0, 30, 0, QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30, 0,-xr, 0, QPen(Qt::darkBlue,2)));
  Ports.append(new Port( 30, 0));
  Ports.append(new Port(-30, 0));

  x1 = -30; y1 = -23;
  x2 =  30; y2 =  23;
}

// -------------------------------------------------------
Component* Logical_Inv::newOne()
{
  Logical_Inv* p = new Logical_Inv();
  p->Props.getLast()->Value = Props.getLast()->Value;
  p->recreate(0);
  return p;
}

// -------------------------------------------------------
Element* Logical_Inv::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Inverter");
  BitmapFile = (char *) "inverter";

  if(getNewOne)  return new Logical_Inv();
  return 0;
}
