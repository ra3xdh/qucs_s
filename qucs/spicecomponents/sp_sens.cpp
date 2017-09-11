/***************************************************************************
                               sp_noise.cpp
                               ------------
    begin                : Tue May 26 2015
    copyright            : (C) 2015 by Vadim Kuznetsov
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
#include "sp_sens.h"
#include "main.h"
#include "extsimkernels/spicecompat.h"


SpiceSENS::SpiceSENS()
{
  isSimulation = true;
  Description = QObject::tr("Pole-Zero simulation");

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
  Model = ".SENS";
  Name  = "SENS";
  SpiceModel = ".SENS";

  // The index of the first 4 properties must not changed. Used in recreate().
  Props.append(new Property("Output", "v(out)", true,
            QObject::tr("Output variable")));
  Props.append(new Property("Mode","dc",true,
            QObject::tr("Sensitivity analysis mode (DC/AC)")+" [dc, ac]"));
  Props.append(new Property("Type", "lin", true,
            QObject::tr("sweep type (for AC mode only)")+" [lin, dec, oct]"));
  Props.append(new Property("Start", "1 Hz", true,
            QObject::tr("start frequency in Hertz (for AC mode only)")));
  Props.append(new Property("Stop", "1000 Hz", true,
            QObject::tr("stop frequency in Hertz (for AC mode only)")));
  Props.append(new Property("Points", "10", true,
            QObject::tr("number of simulation steps (for AC mode only)")));
}

SpiceSENS::~SpiceSENS()
{
}

Component* SpiceSENS::newOne()
{
  return new SpiceSENS();
}

Element* SpiceSENS::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Sensitivity simulation");
  BitmapFile = (char *) "sp_sens";

  if(getNewOne)  return new SpiceSENS();
  return 0;
}

QString SpiceSENS::spice_netlist(bool isXyce)
{
    QString s;
    s.clear();
    if (!isXyce) {
        if (Props.at(1)->Value=="dc") {
            s = QString("sens %1\n").arg(Props.at(0)->Value);
        } else {
            QString fstart = spicecompat::normalize_value(Props.at(3)->Value); // Start freq.
            QString fstop = spicecompat::normalize_value(Props.at(4)->Value); // Stop freq.
            s = QString("sens %1 ac %2 %3 %4 %5\n")
                    .arg(Props.at(0)->Value).arg(Props.at(2)->Value).arg(Props.at(5)->Value)
                    .arg(fstart).arg(fstop);
        }
        s += "write spice4qucs.sens.prn all\n";
    }

    return s;
}
