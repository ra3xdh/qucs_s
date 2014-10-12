/***************************************************************************
                                ac_sim.cpp
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
#include "ac_sim.h"
#include "main.h"


AC_Sim::AC_Sim()
{
  isSimulation = true;
  Description = QObject::tr("ac simulation");

  QString s = Description;
  int a = s.indexOf(" ");
  int b = s.findRev(" ");
  if (a != -1 && b != -1) {
    if (a > (int) s.length() - b)  b = a;
  }
  if (a < 8 || s.length() - b < 8) b = -1;
  if (b != -1) s[b] = '\n';

  Texts.append(new Text(0, 0, s.left(b), Qt::darkBlue, QucsSettings.largeFontSize));
  if (b != -1)
    Texts.append(new Text(0, 0, s.mid(b+1), Qt::darkBlue, QucsSettings.largeFontSize));

  x1 = -10; y1 = -9;
  x2 = x1+128; y2 = y1+41;

  tx = 0;
  ty = y2+1;
  Model = ".AC";
  SpiceModel = ".AC";
  Name  = "AC";

  // The index of the first 4 properties must not changed. Used in recreate().
  Props.append(new Property("Type", "lin", true,
			QObject::tr("sweep type")+" [lin, log, list, const]"));
  Props.append(new Property("Start", "1 GHz", true,
			QObject::tr("start frequency in Hertz")));
  Props.append(new Property("Stop", "10 GHz", true,
			QObject::tr("stop frequency in Hertz")));
  Props.append(new Property("Points", "19", true,
			QObject::tr("number of simulation steps")));
  Props.append(new Property("Noise", "no", false,
			QObject::tr("calculate noise voltages")+
			" [yes, no]"));
}

AC_Sim::~AC_Sim()
{
}

Component* AC_Sim::newOne()
{
  return new AC_Sim();
}

Element* AC_Sim::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("ac simulation");
  BitmapFile = (char *) "ac";

  if(getNewOne)  return new AC_Sim();
  return 0;
}

void AC_Sim::recreate(Schematic*)
{
  Property *pp = Props.first();
  if((pp->Value == "list") || (pp->Value == "const")) {
    // Call them "Symbol" to omit them in the netlist.
    pp = Props.next();
    pp->Name = "Symbol";
    pp->display = false;
    pp = Props.next();
    pp->Name = "Symbol";
    pp->display = false;
    Props.next()->Name = "Values";
  }
  else {
    Props.next()->Name = "Start";
    Props.next()->Name = "Stop";
    Props.next()->Name = "Points";
  }
}

QString AC_Sim::spice_netlist()
{
    QString s = SpiceModel + " ";
    if (Props.at(0)->Value=="log") {
        s += "DEC ";
        QString unit;
        QString tmp = Props.at(3)->Value;
        double Np,Fstart,Fstop,fac = 1.0;
        str2num(tmp,Np,unit,fac);
        Np = Np*fac;
        tmp = Props.at(1)->Value;
        str2num(tmp,Fstart,unit,fac);
        Fstart = Fstart*fac;
        tmp = Props.at(2)->Value;
        str2num(tmp,Fstop,unit,fac);
        Fstop = Fstop*fac;
        double Nd = ceil(log10(Fstop/Fstart));
        double Npd = ceil(Np/Nd);
        s += QString::number(Npd) + " ";
    } else {
        s += "LIN ";
        s += Props.at(3)->Value + " ";
    }
    QString start = Props.at(1)->Value;
    QString stop = Props.at(2)->Value;
    s += start.replace("M","Meg",Qt::CaseSensitive).remove(' ').remove("Hz").toUpper() + " ";
    s += stop.replace("M","Meg",Qt::CaseSensitive).remove(' ').remove("Hz").toUpper() + " ";
    s += " \n";
    return s;

}
