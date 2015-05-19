/***************************************************************************
                                tr_sim.cpp
                               ------------
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
#include "sp_customsim.h"
#include "main.h"
#include "misc.h"


SpiceCustomSim::SpiceCustomSim()
{
  isSimulation = true;
  Description = QObject::tr("Ngspice custom simulation");

  QString  s = Description;
  int a = s.indexOf(" ");
  if (a != -1) s[a] = '\n';

  Texts.append(new Text(0, 0, s.left(a), Qt::darkRed, QucsSettings.largeFontSize));
  if (a != -1)
    Texts.append(new Text(0, 0, s.mid(a+1), Qt::darkRed, QucsSettings.largeFontSize));

  x1 = -10; y1 = -9;
  x2 = x1+104; y2 = y1+59;

  tx = 0;
  ty = y2+1;
  Model = ".CUSTOMSIM";
  Name  = "CUSTOM";
  SpiceModel = "CUSTOM";

  // The index of the first 4 properties must not changed. Used in recreate().
  Props.append(new Property("SpiceCode", "\n"
                            "AC LIN 2000 100 10MEG\n"
                            "let K=V(1)/V(2)\n", true,
                                         "Insert spice code here"));
  Props.append(new Property("Vars","V(1);V(2)",false,"Vars to plot"));
  Props.append(new Property("Outputs","Custom_ac.txt;Custom_tran.txt",false,"Extra outputs to parse"));

}

SpiceCustomSim::~SpiceCustomSim()
{
}

Component* SpiceCustomSim::newOne()
{
  return new SpiceCustomSim();
}

Element* SpiceCustomSim::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Ngspice custom simulation");
  BitmapFile = (char *) "tran";

  if(getNewOne)  return new SpiceCustomSim();
  return 0;
}

QString SpiceCustomSim::spice_netlist(bool isXyce)
{
    QString s = "";
    if (isXyce) return s;
    s = Props.at(0)->Value;
    return s;
}
