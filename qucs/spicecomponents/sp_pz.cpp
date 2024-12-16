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
#include "sp_pz.h"
#include "extsimkernels/spicecompat.h"


SpicePZ::SpicePZ()
{
  isSimulation = true;
  Description = QObject::tr("Pole-Zero simulation");
  Simulator = spicecompat::simNgspice | spicecompat::simSpiceOpus;
  initSymbol(Description);
  Model = ".PZ";
  Name  = "PZ";
  SpiceModel = ".PZ";

  // The index of the first 4 properties must not changed. Used in recreate().
  Props.append(new Property("Input", "in 0", true,
            QObject::tr("Two input nodes list (space separated)")));
  Props.append(new Property("Output", "out 0", true,
            QObject::tr("Two output nodes list (space separated)")));
  Props.append(new Property("TF_type","vol",true,
            QObject::tr("Transfer function type (current/voltage)")+" [cur, vol]"));
  Props.append(new Property("PZ_mode","pz",true,
            QObject::tr("Analysis mode (Pole-Zero, Poles only, Zeros only)")+" [pz, pol, zer]"));

}

SpicePZ::~SpicePZ()
{
}

Component* SpicePZ::newOne()
{
  return new SpicePZ();
}

Element* SpicePZ::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Pole-Zero simulation");
  BitmapFile = (char *) "sp_pz";

  if(getNewOne)  return new SpicePZ();
  return 0;
}

QString SpicePZ::spice_netlist(bool isXyce, bool)
{
    QString s;
    QString out = "spice4qucs." + Name.toLower() + ".cir.pz";
    if (!isXyce) {
        s = QStringLiteral("pz %1 %2 %3 %4\n").arg(Props.at(0)->Value).arg(Props.at(1)->Value)
                .arg(Props.at(2)->Value).arg(Props.at(3)->Value);
        s += QStringLiteral("echo \"PZ analysis\" >> %1\n").arg(out);
        s += "let dummy_var = 0.0\n"; // To overcome featurebug of Ngspice when printing single variable
        s += QStringLiteral("print all >> %1\n").arg(out);
    } else {
        s.clear();
    }

    return s;
}
