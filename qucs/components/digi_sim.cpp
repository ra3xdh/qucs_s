/***************************************************************************
                                digi_sim.cpp
                               --------------
    begin                : Oct 3 2005
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
#include <QtGui>
#include "digi_sim.h"
#include "main.h"


Digi_Sim::Digi_Sim()
{
  Type = isDigitalComponent;
  Description = QObject::tr("digital simulation");

  QString  s = Description;
  int a = s.find(" ");
  if (a != -1) s[a] = '\n';  // break line before the word "simulation"

  Texts.append(new Text(0, 0, s.left(a), Qt::darkBlue, QucsSettings.largeFontSize));
  if (a != -1)
    Texts.append(new Text(0, 0, s.mid(a+1), Qt::darkBlue, QucsSettings.largeFontSize));

  x1 = -10; y1 = -9;
  x2 = x1+120; y2 = y1+59;

  tx = 0;
  ty = y2+1;
  Model = ".Digi";
  Name  = "Digi";

  // Property list must keeps its order !
  Props.append(new Property("Type", "TruthTable", true,
	QObject::tr("type of simulation")+" [TruthTable, TimeList]"));
  Props.append(new Property("time", "10 ns", false,
	QObject::tr("duration of TimeList simulation")));
  Props.append(new Property("Model", "VHDL", false,
	QObject::tr("netlist format")+" [VHDL, Verilog]"));
}

// -------------------------------------------------------
Digi_Sim::~Digi_Sim()
{
}

// -------------------------------------------------------
Component* Digi_Sim::newOne()
{
  return new Digi_Sim();
}

// -------------------------------------------------------
Element* Digi_Sim::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("digital simulation");
  BitmapFile = (char *) "digi";

  if(getNewOne)  return new Digi_Sim();
  return 0;
}
