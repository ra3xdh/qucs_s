/***************************************************************************
                          hb_sim.cpp  -  description
                             -------------------
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
#include "hb_sim.h"
#include "main.h"
#include "misc.h"


HB_Sim::HB_Sim()
{
  Description = QObject::tr("Harmonic balance simulation");

  QString  s = Description;
  int a = s.findRev(" ");
  if (a != -1) s[a] = '\n';    // break line before the word "simulation"

  Texts.append(new Text(0, 0, s.left(a), Qt::darkBlue, QucsSettings.largeFontSize));
  if (a != -1)
    Texts.append(new Text(0, 0, s.mid(a+1), Qt::darkBlue, QucsSettings.largeFontSize));

  x1 = -10; y1 = -9;
  x2 = x1+163; y2 = y1+59;

  tx = 0;
  ty = y2+1;
  Model = ".HB";
  Name  = "HB";
  SpiceModel = ".HB";
  isSimulation = true;

  Props.append(new Property("f", "1 GHz", false,
		QObject::tr("frequency in Hertz")));
  Props.append(new Property("n", "4", true,
		QObject::tr("number of harmonics")));
  Props.append(new Property("iabstol", "1 pA", false,
		QObject::tr("absolute tolerance for currents")));
  Props.append(new Property("vabstol", "1 uV", false,
		QObject::tr("absolute tolerance for voltages")));
  Props.append(new Property("reltol", "0.001", false,
		QObject::tr("relative tolerance for convergence")));
  Props.append(new Property("MaxIter", "150", false,
		QObject::tr("maximum number of iterations until error")));
}

HB_Sim::~HB_Sim()
{
}

Component* HB_Sim::newOne()
{
  return new HB_Sim();
}

Element* HB_Sim::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Harmonic balance");
  BitmapFile = (char *) "hb";

  if(getNewOne)  return new HB_Sim();
  return 0;
}

QString HB_Sim::spice_netlist(bool isXyce)
{
    QString s="";
    if (isXyce) {  // Only in Xyce
        double val,fac;
        QString unit;
        str2num(Props.at(1)->Value,val,unit,fac);
        s += QString(".options hbint numfreq=%1 STARTUPPERIODS=2\n").arg(val*fac);
        str2num(Props.at(0)->Value,val,unit,fac);
        s += QString(".HB %1\n").arg(val*fac);
    }
    return s;
}
