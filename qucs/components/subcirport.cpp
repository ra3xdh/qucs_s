/***************************************************************************
                               subcirport.cpp
                              ----------------
    begin                : Sat Aug 23 2003
    copyright            : (C) 2003 by Michael Margraf
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
#include "subcirport.h"
#include "node.h"
#include "schematic.h"


SubCirPort::SubCirPort()
{
  Type = isComponent;   // both analog and digital
  Description = QObject::tr("port of a subcircuit");

  // This property must be the first one !
  Props.append(new Property("Num", "1", true,
		QObject::tr("number of the port within the subcircuit")));
  // This property must be the second one !
  Props.append(new Property("Type", "analog", false,
		QObject::tr("type of the port (for digital simulation only)")
		+" [analog, in, out, inout]"));
  Props.append(new Property("XSPICE_Type","v",false,"Allowed XSPICE types: [v,i,vd,id,h,g,hd,gd]"));
  Props.append(new Property("Conj","",false,
        QObject::tr("Conjugated port for XSPICE differential ports")));

  createSymbol();
  tx = x1+4;
  ty = y2+4;
  Model = "Port";
  Name  = "P";
  SpiceModel = "*";
}

// -------------------------------------------------------
void SubCirPort::createSymbol()
{
  x1 = -27; y1 = -8;
  x2 =   0; y2 =  8;

  if(Props.at(1)->Value.at(0) == 'a') {
    Arcs.append(new Arc(-25, -6, 12, 12,  0, 16*360,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-13,  0,  0,  0,QPen(Qt::darkBlue,2)));
  }
  else {
    Lines.append(new Line( -9,  0,  0,  0,QPen(Qt::darkBlue,2)));
    if(Props.at(1)->Value == "out") {
      Lines.append(new Line(-20, -5,-25,  0,QPen(Qt::red,2)));
      Lines.append(new Line(-20,  5,-25,  0,QPen(Qt::red,2)));
      Lines.append(new Line(-20, -5, -9, -5,QPen(Qt::red,2)));
      Lines.append(new Line(-20,  5, -9,  5,QPen(Qt::red,2)));
      Lines.append(new Line( -9, -5, -9,  5,QPen(Qt::red,2)));
    }
    else {
      Lines.append(new Line(-14, -5, -9,  0,QPen(Qt::darkGreen,2)));
      Lines.append(new Line(-14,  5, -9,  0,QPen(Qt::darkGreen,2)));
      if(Props.at(1)->Value == "in") {
        Lines.append(new Line(-25, -5,-14, -5,QPen(Qt::darkGreen,2)));
        Lines.append(new Line(-25,  5,-14,  5,QPen(Qt::darkGreen,2)));
        Lines.append(new Line(-25, -5,-25,  5,QPen(Qt::darkGreen,2)));
      }
      else {
        x1 = -30;
        Lines.append(new Line(-18, -5,-14, -5,QPen(Qt::darkGreen,2)));
        Lines.append(new Line(-18,  5,-14,  5,QPen(Qt::darkGreen,2)));
        Lines.append(new Line(-23, -5,-28,  0,QPen(Qt::red,2)));
        Lines.append(new Line(-23,  5,-28,  0,QPen(Qt::red,2)));
        Lines.append(new Line(-23, -5,-18, -5,QPen(Qt::red,2)));
        Lines.append(new Line(-23,  5,-18,  5,QPen(Qt::red,2)));
      }
    }
  }

  Ports.append(new Port(  0,  0));
}

// -------------------------------------------------------
Component* SubCirPort::newOne()
{
  return new SubCirPort();
}

// -------------------------------------------------------
Element* SubCirPort::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Subcircuit Port");
  BitmapFile = (char *) "subport";

  if(getNewOne)  return new SubCirPort();
  return 0;
}

// -------------------------------------------------------
QString SubCirPort::netlist()
{
  return QString("");
}

// -------------------------------------------------------
QString SubCirPort::spice_netlist(bool)
{
    return QString("");
}

// -------------------------------------------------------
QString SubCirPort::vhdlCode(int)
{
  if(Props.at(1)->Value != "out")
    return QString("");

  // Insert dummy buffer to avoid reading from an output port.
  QString s = "  net_out";
  Node *pn = Ports.first()->Connection;
  s += pn->Name + " <= ";
  s += pn->Name + ";\n";
  return s;
}

// -------------------------------------------------------
QString SubCirPort::verilogCode(int)
{
  return QString("");
}
