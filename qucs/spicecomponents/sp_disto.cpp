/***************************************************************************
                               sp_disto.cpp
                               ------------
    begin                : Wed May 20 2015
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
#include "sp_disto.h"
#include "main.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


SpiceDisto::SpiceDisto()
{
  isSimulation = true;
  Description = QObject::tr("Distortion simulation");
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
  Model = ".DISTO";
  Name  = "DISTO";
  SpiceModel = ".DISTO";

  // The index of the first 4 properties must not changed. Used in recreate().
  Props.append(new Property("Type", "lin", true,
            QObject::tr("sweep type")+" [lin, oct, dec]"));
  Props.append(new Property("Start", "1 Hz", true,
            QObject::tr("start frequency in Hertz")));
  Props.append(new Property("Stop", "10 kHz", true,
            QObject::tr("stop frequency in Hertz")));
  Props.append(new Property("Points", "100", true,
            QObject::tr("number of simulation steps")));
  Props.append(new Property("f2overf1","",false,
            QObject::tr("Second frequency parameter")));

}

SpiceDisto::~SpiceDisto()
{
}

Component* SpiceDisto::newOne()
{
  return new SpiceDisto();
}

Element* SpiceDisto::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Distortion simulation");
  BitmapFile = (char *) "sp_disto";

  if(getNewOne)  return new SpiceDisto();
  return 0;
}

QString SpiceDisto::spice_netlist(bool isXyce)
{
    QString s;
    if (!isXyce) {
        QString fstart = spicecompat::normalize_value(Props.at(1)->Value); // Start freq.
        QString fstop = spicecompat::normalize_value(Props.at(2)->Value); // Stop freq.
        s = QString("disto %1 %2 %3 %4").arg(Props.at(0)->Value).arg(Props.at(3)->Value)
                .arg(fstart).arg(fstop);
        if (Props.at(4)->Value.remove(' ').isEmpty()) s += "\n";
        else s += Props.at(4)->Value + "\n";
    } else {
        s.clear();
    }

    return s;
}
