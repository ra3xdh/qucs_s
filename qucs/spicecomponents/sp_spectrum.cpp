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
#include "sp_spectrum.h"
#include "main.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"


SpiceFFT::SpiceFFT()
{
  isSimulation = true;
  Description = QObject::tr("Spectrum analysis");

  QString  s = Description;
  int a = s.indexOf(" ");
  if (a != -1) s[a] = '\n';

  Texts.append(new Text(0, 0, s.left(a), Qt::darkBlue, QucsSettings.largeFontSize));
  if (a != -1)
    Texts.append(new Text(0, 0, s.mid(a+1), Qt::darkBlue, QucsSettings.largeFontSize));

  x1 = -10; y1 = -9;
  x2 = x1+104; y2 = y1+59;

  tx = 0;
  ty = y2+1;
  Model = ".FFT";
  Name  = "FFT";
  SpiceModel = ".FFT";

  // The index of the first 4 properties must not changed. Used in recreate().
  Props.append(new Property("BW","1MHz",true,"Bandwidth"));
  Props.append(new Property("dF","10kHz",true,"Frequency step"));
  Props.append(new Property("Window","hanning", true, "Window type "
                                     "[none,rectangular,bartlet,blackman,hanning,hamming,gaussian,flattop]"));
  Props.append(new Property("Order","2",false,"Order of the Gaussian window"));

}

SpiceFFT::~SpiceFFT()
{
}

Component* SpiceFFT::newOne()
{
  return new SpiceFFT();
}

Element* SpiceFFT::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Spectrum analysis");
  BitmapFile = (char *) "sp_fft";

  if(getNewOne)  return new SpiceFFT();
  return 0;
}

QString SpiceFFT::spice_netlist(bool isXyce)
{
    Q_UNUSED(isXyce);

    QString s;
    QString unit;
    double num,fac;
    misc::str2num(getProperty("BW")->Value,num,unit,fac);
    double bw = num*fac;
    misc::str2num(getProperty("dF")->Value,num,unit,fac);
    double df = num*fac;
    double tstop = 1.0/df;
    double tstep = 1.0/(2*bw);
    QString win = getProperty("Window")->Value;
    s =  QString("tran %1 %2 0\n").arg(tstep).arg(tstop);
    s += QString("set specwindow=%1\n").arg(win);
    if (win == "gaussian") {
        s += QString("set specwindoworder=%1\n")
                .arg(getProperty("Order")->Value);
    }

    return s;
}
