/***************************************************************************
                                ecvs.cpp
                               -----------
    copyright            : (C) 2013 by Richard Crozier
    email                : richard dot crozier at yahoo dot co dot uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "ecvs.h"
#include "schematic.h"
#include "extsimkernels/spicecompat.h"

ecvs::ecvs()
{
  Description = QObject::tr("externally controlled voltage source");
  Simulator = spicecompat::simQucsator;

//  Arcs.append(new Arc( -3, -7,  7,  7,16*270, 16*180,QPen(Qt::darkBlue,2)));
//  Arcs.append(new Arc( -3,  0,  7,  7, 16*90, 16*180,QPen(Qt::darkBlue,2)));

  // Circle in middle
  Arcs.append(new qucs::Arc(-12,-12, 24, 24,     0, 16*360,QPen(Qt::darkBlue,2)));
  // The 'E' symbol in middle of circle
  // horizontal lines (actually drawn vertically here)
  Lines.append(new qucs::Line(4, -3, 4, 3,QPen(Qt::darkGreen,2)));
  Lines.append(new qucs::Line(0, -3, 0, 2,QPen(Qt::darkGreen,2)));
  Lines.append(new qucs::Line(-4, -3, -4, 3,QPen(Qt::darkGreen,2)));
  // Vertical Line
  Lines.append(new qucs::Line(4, -3, -4, -3,QPen(Qt::darkGreen,2)));

  // Wires at top and bottom
  Lines.append(new qucs::Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  // positive symbol
  Lines.append(new qucs::Line( 18,  5, 18, 11,QPen(Qt::red,1)));
  Lines.append(new qucs::Line( 21,  8, 15,  8,QPen(Qt::red,1)));
  // negative symbol
  Lines.append(new qucs::Line(-18,  5,-18, 11,QPen(Qt::black,1)));

//  Lines.append(new Line( -6,-17, -6,-21,QPen(Qt::darkBlue,1)));
//  Lines.append(new Line( -8,-17, -8,-21,QPen(Qt::darkBlue,1)));
//  Lines.append(new Line(-10,-17,-10,-21,QPen(Qt::darkBlue,1)));
//  Lines.append(new Line( -3,-15, -3,-23,QPen(Qt::darkBlue,2)));
//  Lines.append(new Line(-13,-15,-13,-23,QPen(Qt::darkBlue,2)));
//  Lines.append(new Line( -3,-23,-13,-23,QPen(Qt::darkBlue,2)));
//  Lines.append(new Line( -3,-15,-13,-15,QPen(Qt::darkBlue,2)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = x1+4;
  ty = y2+4;
  Model = "ECVS";
  Name  = "ECVS";

  Props.append(new Property("U", "0 V", true,
		QObject::tr("voltage in Volts")));
//  Props.append(new Property("Interpolator", "linear", false,
//		QObject::tr("interpolation type")+" [hold, linear, cubic]"));

  rotate();  // fix historical flaw
}

ecvs::~ecvs()
{
}

Component* ecvs::newOne()
{
  return new ecvs();
}

Element* ecvs::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Externally Controlled Voltage Source");
  BitmapFile = (char *) "ecvs";

  if(getNewOne)  return new ecvs();
  return 0;
}


// -------------------------------------------------------
QString ecvs::netlist()
{
  QString s = Model+":"+Name;

  // output all node names
  for (Port *p1 : Ports)
    s += " "+p1->Connection->Name;   // node names

  // output all properties
  for(Property *p2 = Props.first(); p2 != 0; p2 = Props.next())
    s += " "+p2->Name+"=\""+p2->Value+"\"";

  return s + "\n";
}

