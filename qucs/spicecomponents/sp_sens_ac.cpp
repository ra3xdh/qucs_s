/***************************************************************************
                               sp_sens_ac.cpp
                               ------------
    begin                : Mon Sep 18 2017
    copyright            : (C) 2017 by Vadim Kuznetsov
    email                : ra3xdh@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "sp_sens_ac.h"
#include "main.h"
#include "extsimkernels/spicecompat.h"


SpiceSENS_AC::SpiceSENS_AC()
{
  isSimulation = true;
  Description = QObject::tr("AC sensitivity simulation");
  Simulator = spicecompat::simNgspice | spicecompat::simSpiceOpus;

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
  Model = ".SENS_AC";
  Name  = "SENS";
  SpiceModel = ".SENS";

  // The index of the first 4 properties must not changed. Used in recreate().
  Props.append(new Property("Output", "v(out)", true,
            QObject::tr("Output variable")));
  Props.append(new Property("Type", "lin", true,
            QObject::tr("sweep type")+" [lin, dec, oct]"));
  Props.append(new Property("Start", "1 Hz", true,
            QObject::tr("start frequency in Hertz")));
  Props.append(new Property("Stop", "1000 Hz", true,
            QObject::tr("stop frequency in Hertz")));
  Props.append(new Property("Points", "10", true,
            QObject::tr("number of simulation steps")));
}

SpiceSENS_AC::~SpiceSENS_AC()
{
}

Component* SpiceSENS_AC::newOne()
{
  return new SpiceSENS_AC();
}

Element* SpiceSENS_AC::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("AC sensitivity simulation");
  BitmapFile = (char *) "sp_sens_ac";

  if(getNewOne)  return new SpiceSENS_AC();
  return 0;
}

QString SpiceSENS_AC::spice_netlist(bool isXyce)
{
    QString s;
    s.clear();
    if (!isXyce) {
        QString fstart = spicecompat::normalize_value(Props.at(2)->Value); // Start freq.
        QString fstop = spicecompat::normalize_value(Props.at(3)->Value); // Stop freq.
        s = QString("sens %1 ac %2 %3 %4 %5\n")
                .arg(Props.at(0)->Value).arg(Props.at(1)->Value).arg(Props.at(4)->Value)
                .arg(fstart).arg(fstop);
        s += "write spice4qucs.sens.prn all\n";
    }

    return s;
}
