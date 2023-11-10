/***************************************************************************
                                switch.cpp
                               ------------
    begin                : Sat Feb 25 2006
    copyright            : (C) 2006 by Michael Margraf
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
#include "switch.h"
#include "node.h"
#include "schematic.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"
#include <QDebug>

Switch::Switch()
{
  Description = QObject::tr("switch (time controlled)");

  Props.append(new Property("init", "off", false,
		QObject::tr("initial state")+" [on, off]"));
  Props.append(new Property("time", "1 ms", false,
		QObject::tr("time when state changes (semicolon separated list possible, even numbered lists are repeated)")));
  Props.append(new Property("Ron", "1e-9", false,
		QObject::tr("resistance of \"on\" state in ohms")));
  Props.append(new Property("Roff", "1e12", false,
		QObject::tr("resistance of \"off\" state in ohms")));
  Props.append(new Property("Temp", "26.85", false,
        QObject::tr("simulation temperature in degree Celsius (Qucsator only)")));
  Props.append(new Property("MaxDuration", "1e-6", false,
		QObject::tr("Max possible switch transition time (transition time 1/100 smallest value in 'time', or this number)")));
  Props.append(new Property("Transition", "spline", false,
        QObject::tr("Resistance transition shape (Qucsator only)")+" [abrupt, linear, spline]"));

  createSymbol();
  tx = x1+4;
  ty = y2+4;
  Model = "Switch";
  Name  = "S";
  SpiceModel = "S";
}

// -------------------------------------------------------
Component* Switch::newOne()
{
  Switch *p = new Switch();
  p->Props.getFirst()->Value = Props.getFirst()->Value;
  p->recreate(0);
  return p;
}

// -------------------------------------------------------
Element* Switch::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Switch");
  BitmapFile = (char *) "switch";

  if(getNewOne)  return new Switch();
  return 0;
}

// -------------------------------------------------------
QString Switch::netlist()
{
  QString s = Model+":"+Name;

  // output all node names
  s += " "+Ports.at(0)->Connection->Name;
  s += " "+Ports.at(1)->Connection->Name;

  // output all properties
  Property *p2 = Props.first();
  s += " "+p2->Name+"=\""+p2->Value+"\"";
  p2 = Props.next();
  s += " "+p2->Name+"=\"["+p2->Value+"]\"";
  for(p2 = Props.next(); p2 != 0; p2 = Props.next())
    s += " "+p2->Name+"=\""+p2->Value+"\"";

  return s + '\n';
}

QString Switch::spice_netlist(bool)
{
  QString s = spicecompat::check_refdes(Name,SpiceModel);
  QString port1 = spicecompat::normalize_node_name(Ports.at(0)->Connection->Name);
  QString port2 = spicecompat::normalize_node_name(Ports.at(1)->Connection->Name);

  s += QString(" %1 %2 control_net%3 0 switch_model%3\n").arg(port1).arg(port2).arg(Name);

  QString init = spicecompat::normalize_value(getProperty("init")->Value);
  QString times = spicecompat::normalize_value(getProperty("time")->Value);
  QStringList timesList = times.split(";");
  QString Ron = spicecompat::normalize_value(getProperty("Ron")->Value);
  QString Roff = spicecompat::normalize_value(getProperty("Roff")->Value);
  QString Max_duration = spicecompat::normalize_value(getProperty("MaxDuration")->Value);

  double fac, timeValue, changingTime, maxDuration, firstTimeVal, time = 0.0;
  QString unit;

  misc::str2num(timesList[0].toLower(), maxDuration,unit,fac);
  maxDuration *= fac / 100;
  misc::str2num(timesList[0].toLower(),firstTimeVal,unit,fac);
  firstTimeVal *= fac / 100;

  if (firstTimeVal > maxDuration){
    changingTime = maxDuration;
  }
  else {
    changingTime = firstTimeVal;
  }

  QString oddValue, evenValue;
  if (init == "{OFF}") {
    oddValue = "0";
    evenValue = "1";
  } else {
    oddValue = "1";
    evenValue = "0";
  }

  s += QString("V%1 control_net%1 0 DC %2 PWL(0 %2").arg(Name).arg(oddValue);

  for (int i = 0; i < timesList.size(); i++) {
    QString timeStep = timesList[i].toLower();
    misc::str2num(timeStep,timeValue,unit,fac);
    timeValue *= fac;

    if (i == 0) {
        time += timeValue - changingTime;
        s += QString(" %1 %2").arg(time).arg(oddValue);
        time += changingTime;
        s += QString(" %1 %2").arg(time).arg(evenValue);

    } else {
        if (i % 2 == 1) {
            time += timeValue- changingTime;
            s += QString(" %1 %2").arg(time).arg(evenValue);
            time += changingTime;
            s += QString(" %1 %2").arg(time).arg(oddValue);
        }
        else{
            time += timeValue- changingTime;
            s += QString(" %1 %2").arg(time).arg(oddValue);
            time += changingTime;
            s += QString(" %1 %2").arg(time).arg(evenValue);
        }
    }
  }

  s += ")\n";

  s += QString(".model switch_model%1 sw vt =0.5 ron =%2 roff =%3\n").arg(Name).arg(Ron).arg(Roff);
  return s;
}

// -------------------------------------------------------
void Switch::createSymbol()
{
  if(Props.getFirst()->Value != "on") {
    Lines.append(new qucs::Line(-15,  0, 15,-15,QPen(Qt::darkBlue,2)));
    y1 = -17;
  }
  else {
    Lines.append(new qucs::Line(-15,  0, 16,-5,QPen(Qt::darkBlue,2)));
    y1 = -7;
  }

  Lines.append(new qucs::Line(-30,  0,-15,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 17,  0, 30,  0,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc( 12, -3, 5, 5, 0, 16*360,QPen(Qt::darkBlue,2)));
  Ellips.append(new qucs::Area(-18, -3, 6, 6, QPen(Qt::darkBlue,2),
                QBrush(Qt::darkBlue, Qt::SolidPattern)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

  x1 = -30;
  x2 =  30; y2 =   7;
}
