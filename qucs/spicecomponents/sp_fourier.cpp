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
#include "extsimkernels/spicecompat.h"


SpiceFourier::SpiceFourier()
{
  isSimulation = true;
  Description = QObject::tr("Fourier simulation");
  Simulator = spicecompat::simSpice;
  initSymbol(Description);
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

QString SpiceFourier::spice_netlist(bool isXyce, bool)
{
    QString s;
    QString f0 = spicecompat::normalize_value(Props.at(2)->Value);
    QString out = "spice4qucs." + Name.toLower() + ".four";
    if (!isXyce) {
        s = QStringLiteral("set nfreqs=%1\n").arg(Props.at(1)->Value);
        s += QStringLiteral("fourier %1 %2 > %3\n").arg(f0).arg(Props.at(3)->Value).arg(out);
    } else {
        s = QStringLiteral(".FOUR %1 %2\n").arg(f0).arg(Props.at(3)->Value);
    }

    return s;
}
