/***************************************************************************
                              sp_sens_tr_xyce.cpp
                               ------------
    begin                : Wed Sep 27 2017
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
#include "sp_sens_tr_xyce.h"
#include "main.h"
#include "extsimkernels/spicecompat.h"


SpiceSENS_TR_Xyce::SpiceSENS_TR_Xyce()
{
  isSimulation = true;
  Description = QObject::tr("Transient .SENS analysis with Xyce");

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
  Model = ".SENS_TR_XYCE";
  Name  = "TSENS";
  SpiceModel = ".SENS";

  // The index of the first 4 properties must not changed. Used in recreate().
  Props.append(new Property("Objfunc", "v(out)", true,
            QObject::tr("Output expressions")));
  Props.append(new Property("RefParam", "R1:R", true,
        QObject::tr("Reference parameter for .SENS analysis")));
  Props.append(new Property("Mode", "direct", true,
        QObject::tr("Analysis mode ")+"[direct,adjoint]"));
  Props.append(new Property("Start", "0", true,
    QObject::tr("start time in seconds")));
  Props.append(new Property("Stop", "1 ms", true,
    QObject::tr("stop time in seconds")));
  Props.append(new Property("Step", "5u", true,
    QObject::tr("simulation tim step")));
  Props.append(new Property("initialDC", "yes", true,
    QObject::tr("perform an initial DC analysis")+" [yes, no]"));
}

SpiceSENS_TR_Xyce::~SpiceSENS_TR_Xyce()
{
}

Component* SpiceSENS_TR_Xyce::newOne()
{
  return new SpiceSENS_TR_Xyce();
}

Element* SpiceSENS_TR_Xyce::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Transient sensitivity analysis");
  BitmapFile = (char *) "sp_sens_tr_xyce";
  if(getNewOne)  return new SpiceSENS_TR_Xyce();
  return 0;
}

QString SpiceSENS_TR_Xyce::spice_netlist(bool isXyce)
{
    QString s;
    s.clear();
    if (isXyce) {
        QString start = spicecompat::normalize_value(Props.at(3)->Value);
        QString stop = spicecompat::normalize_value(Props.at(4)->Value);
        QString step = spicecompat::normalize_value(Props.at(5)->Value);
        s = QString(".tran %1 %2 %3").arg(start).arg(stop).arg(step);
        if (Props.at(5)->Value=="yes") s +="\n";
        else s += " uic\n";
        if (Props.at(2)->Value=="direct") s += ".options sensitivity direct=1 adjoint=0\n";
        else s += ".options sensitivity direct=0 adjoint=1\n";
        s += QString(".sens objfunc={%1} param=%2\n"
                    ".print sens\n").arg(Props.at(0)->Value).arg(Props.at(1)->Value);
    }

    return s;
}
