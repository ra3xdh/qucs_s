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
#include "sp_noise.h"
#include "main.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


SpiceNoise::SpiceNoise()
{
  isSimulation = true;
  Description = QObject::tr("Noise simulation");

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
  Model = ".NOISE";
  Name  = "NOISE";
  SpiceModel = ".NOISE";

  // The index of the first 4 properties must not changed. Used in recreate().
  Props.append(new Property("Type", "lin", true,
            QObject::tr("sweep type")+" [lin, oct, dec]"));
  Props.append(new Property("Start", "1 Hz", true,
            QObject::tr("start frequency in Hertz")));
  Props.append(new Property("Stop", "10 kHz", true,
            QObject::tr("stop frequency in Hertz")));
  Props.append(new Property("Points", "100", true,
            QObject::tr("number of simulation steps")));
  Props.append(new Property("Output","v(node1)",true,
            QObject::tr("Node at which the total output is desired")));
  Props.append(new Property("Source","V1",true,
            QObject::tr("Independent source to which input noise is referred.")));

}

SpiceNoise::~SpiceNoise()
{
}

Component* SpiceNoise::newOne()
{
  return new SpiceNoise();
}

Element* SpiceNoise::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Noise simulation");
  BitmapFile = (char *) "sp_noise";

  if(getNewOne)  return new SpiceNoise();
  return 0;
}

QString SpiceNoise::spice_netlist(bool isXyce)
{
    QString s;
    QString fstart = spicecompat::normalize_value(Props.at(1)->Value); // Start freq.
    QString fstop = spicecompat::normalize_value(Props.at(2)->Value); // Stop freq.
    QString swp = spicecompat::convert_sweep_type(Props.at(0)->Value); // Sweep mode (lin,dec,etc.)

    QString points;
    if (swp=="dec") { // convert points number for spice compatibility
        double Np,Fstart,Fstop,fac = 1.0;
        QString unit;
        misc::str2num(Props.at(3)->Value,Np,unit,fac); // Points number
        Np *= fac;
        misc::str2num(Props.at(1)->Value,Fstart,unit,fac);
        Fstart *= fac;
        misc::str2num(Props.at(2)->Value,Fstop,unit,fac);
        Fstop *= fac;
        double Nd = ceil(log10(Fstop/Fstart)); // number of decades
        double Npd = ceil(Np/Nd); // points per decade
        points = QString::number(Npd);
    } else {
        points = Props.at(3)->Value;
    }

    s = QString("noise %1 %2 %3 %4 %5 %6\n").arg(Props.at(4)->Value).arg(Props.at(5)->Value)
            .arg(swp).arg(points).arg(fstart).arg(fstop);
    if (!isXyce) {
        s += QString("print inoise_total onoise_total >> spice4qucs.cir.noise\n");
    } else {
        s.insert(0,'.');
    }

    return s;
}
