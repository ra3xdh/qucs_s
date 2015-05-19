/***************************************************************************
                               sp_fourier.cpp
                               ------------
    begin                : Sun May 17 2015
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
#include "sp_fourier.h"
#include "main.h"
#include "misc.h"


SpiceFourier::SpiceFourier()
{
  isSimulation = true;
  Description = QObject::tr("Fourier simulation");

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
  Model = ".FOURIER";
  Name  = "FOUR";
  SpiceModel = ".FOURIER";

  // The index of the first 4 properties must not changed. Used in recreate().
  Props.append(new Property("Sim","TR1",true,"Transient simulation name"));
  Props.append(new Property("numfreq","10",true,"Number of harmonics"));
  Props.append(new Property("F0","1kHz", true, "First harmonic frequency"));
  Props.append(new Property("Vars","V(1)",true,"Output expressions"));

}

SpiceFourier::~SpiceFourier()
{
}

Component* SpiceFourier::newOne()
{
  return new SpiceFourier();
}

Element* SpiceFourier::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Fourier simulation");
  BitmapFile = (char *) "sp_fourier";

  if(getNewOne)  return new SpiceFourier();
  return 0;
}

QString SpiceFourier::spice_netlist(bool isXyce)
{
    QString s;
    if (!isXyce) {
        s = QString("set nfreqs=%1\n").arg(Props.at(1)->Value);
        s += QString("FOURIER %1 %2 > spice4qucs.four\n").arg(Props.at(2)->Value).arg(Props.at(3)->Value);
    } else {
        s = QString(".FOUR %1 %2\n").arg(Props.at(2)->Value).arg(Props.at(3)->Value);
    }

    return s;
}
