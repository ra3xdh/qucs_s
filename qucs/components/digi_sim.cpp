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
#include "digi_sim.h"


Digi_Sim::Digi_Sim()
{
  Type = isDigitalComponent;
  Description = QObject::tr("digital simulation");
  initSymbol(Description);
  Model = ".Digi";
  Name  = "Digi";

  // Property list must keeps its order !
  Props.emplace_back( Property("Type", "TruthTable", true,
	QObject::tr("type of simulation")+" [TruthTable, TimeList]"));
  Props.emplace_back( Property("time", "10 ns", false,
	QObject::tr("duration of TimeList simulation")));
  Props.emplace_back( Property("Model", "VHDL", false,
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
