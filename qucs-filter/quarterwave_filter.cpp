/*
 * quarterwave_filter.cpp - Quarter wavelength filter implementation
 *
 * copyright (C) 2015 Andres Martinez-Mera <andresmartinezmera@gmail.com>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "quarterwave_filter.h"

#include <QString>
#include <QMessageBox>

QuarterWave_Filter::QuarterWave_Filter()
{
}

// -----------------------------------------------------------------------
QString* QuarterWave_Filter::createSchematic(tFilter *Filter, tSubstrate *Substrate, bool isMicrostrip)
{
  int i, x;
  double d_lamdba4;
  double Z;
  double er_eff=1, width=1;
  double fc = Filter->Frequency+0.5*(Filter->Frequency2-Filter->Frequency);
  double bw = (Filter->Frequency2-Filter->Frequency)/(fc);



 if (Filter->Class<2)
 {
     QMessageBox::warning(0, QObject::tr("Error"),
         QObject::tr("Quarter wave filters do not allow low-pass nor high-pass masks\n"));
         return NULL;
 }
 isMicrostrip ? d_lamdba4 = 0.25*LIGHTSPEED / (sqrt(Substrate->er)*(Filter->Frequency+0.5*(Filter->Frequency2-Filter->Frequency))):
                d_lamdba4 = 0.25*LIGHTSPEED / ((Filter->Frequency+0.5*(Filter->Frequency2-Filter->Frequency)));
  // create the Qucs schematic
  QString *s = new QString("<Qucs Schematic " PACKAGE_VERSION ">\n");

  x = 60;
  *s += "<Components>\n";
  *s += QString("<Pac P1 1 %1 330 18 -26 0 1 \"1\" 1 \"%2 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(x).arg(Filter->Impedance);
  *s += QString("<GND * 1 %1 360 0 0 0 0>\n").arg(x);

  x -= 30;
  for(i = 0; i < Filter->Order; i++) {
    x += 90;



    if(isMicrostrip)
    {
       TL_Filter::getMicrostrip(Filter->Impedance, fc, Substrate, width, er_eff);
      *s += QString("<MLIN MS1 1 %1 180 -26 15 0 0 \"Sub1\" 1 \"%2\" 1 \"%3\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(x).arg( width).arg(d_lamdba4);
        if (Filter->Class == CLASS_BANDPASS)
        {
            Z = (pi*Filter->Impedance*bw)/(4*getNormValue(i, Filter));
            TL_Filter::getMicrostrip(Z, fc, Substrate, width, er_eff);
            *s += QString("<MLIN MS1 1 %1 60 -26 20 0 0 \"Sub1\" 1 \"%2\" 1 \"%3\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(x+80).arg(width).arg(d_lamdba4);
            *s += QString("<GND * 1 %1 60 0 0 0 0>\n").arg(x+110);
        }
        if (Filter->Class == CLASS_BANDSTOP)
        {
            Z = (4*Filter->Impedance)/(pi*bw*getNormValue(i, Filter));
            TL_Filter::getMicrostrip(Z, fc, Substrate, width, er_eff);
            *s += QString("<MLIN MS1 1 %1 60 -26 20 0 0 \"Sub1\" 1 \"%2\" 1 \"%3\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(x+80).arg(width).arg(d_lamdba4);
        }
    }
    else
    {
      *s += QString("<TLIN Line1 1 %1 180 -26 20 0 0 \"%2\" 1 \"%3\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(x).arg(Filter->Impedance).arg(d_lamdba4);
      if (Filter->Class == CLASS_BANDPASS)
      {
          Z = (pi*Filter->Impedance*bw)/(4*getNormValue(i, Filter));
          *s += QString("<TLIN Line1 1 %1 80 -26 20 0 0 \"%2\" 1 \"%3\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(x+80).arg(Z).arg(d_lamdba4);
          *s += QString("<GND * 1 %1 80 0 0 0 0>\n").arg(x+110);
      }
      if (Filter->Class == CLASS_BANDSTOP)
      {
          Z = (4*Filter->Impedance)/(pi*bw*getNormValue(i, Filter));
          *s += QString("<TLIN Line1 1 %1 80 -26 20 0 0 \"%2\" 1 \"%3\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(x+80).arg(Z).arg(d_lamdba4);
      }
    }


  }
  x += 90;
  if (isMicrostrip)
  {
      TL_Filter::getMicrostrip(Filter->Impedance, fc, Substrate, width, er_eff);
      *s += QString("<MLIN MS1 1 %1 180 -26 15 0 0 \"Sub1\" 1 \"%2\" 1 \"%3\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(x).arg(num2str(width)).arg(d_lamdba4);
  }
  else
  {
   *s += QString("<TLIN Line1 1 %1 180 -26 20 0 0 \"%2\" 1 \"%3\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(x).arg(Filter->Impedance).arg(d_lamdba4);
  }
  x += 80;
  *s += QString("<Pac P2 1 %1 330 18 -26 0 1 \"2\" 1 \"%2 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(x).arg(Filter->Impedance);
  *s += QString("<GND * 1 %1 360 0 0 0 0>\n").arg(x);

  *s += QString("<.SP SP1 1 70 460 0 67 0 0 \"lin\" 1 \"%2Hz\" 1 \"%3Hz\" 1 \"300\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n").arg(num2str(0.1 * Filter->Frequency)).arg(num2str(10.0 * Filter->Frequency));
  if(isMicrostrip)
    *s += QString("<SUBST Sub1 1 300 500 -30 24 0 0 \"%1\" 1 \"%2m\" 1 \"%3m\" 1 \"%4\" 1 \"%5\" 1 \"%6\" 1>\n").arg(Substrate->er).arg(num2str(Substrate->height)).arg(num2str(Substrate->thickness)).arg(Substrate->tand).arg(Substrate->resistivity).arg(Substrate->roughness);
  *s += QString("<Eqn Eqn1 1 450 560 -28 15 0 0 \"S21_dB=dB(S[2,1])\" 1 \"S11_dB=dB(S[1,1])\" 1 \"yes\" 0>\n");
  *s += "</Components>\n";

  *s += "<Wires>\n";

  // connect left source
  *s += QString("<60 180 60 300 \"\" 0 0 0>\n");
  *s += QString("<60 180 90 180 \"\" 0 0 0>\n");

  // connect right source
  *s += QString("<%1 180 %2 300 \"\" 0 0 0>\n").arg(x).arg(x);
  *s += QString("<%1 180 %2 180 \"\" 0 0 0>\n").arg(x-50).arg(x);

  // wires between components
  x = 150;
  for(i = 1; i < Filter->Order; i++) {
    *s += QString("<%1 180 %2 180 \"\" 0 0 0>\n").arg(x).arg(x+30);
    isMicrostrip ? *s += QString("<%1 60 %2 180 \"\" 0 1 0>\n").arg(x+20).arg(x+20):
                   *s += QString("<%1 80 %2 180 \"\" 0 1 0>\n").arg(x+20).arg(x+20);
    x += 90;
  }
  *s += QString("<%1 180 %2 180 \"\" 0 0 0>\n").arg(x).arg(x+30);
  isMicrostrip ? *s += QString("<%1 60 %2 180 \"\" 0 1 0>\n").arg(x+20).arg(x+20):
                 *s += QString("<%1 80 %2 180 \"\" 0 1 0>\n").arg(x+20).arg(x+20);

  *s += "</Wires>\n";

  *s += "<Diagrams>\n";
  *s += "</Diagrams>\n";

  *s += "<Paintings>\n";

  *s += QString("<Text 420 460 12 #000000 0 \"Quarter wave bandpass filter \\n ");
  switch(Filter->Type) {
    case TYPE_BESSEL:      *s += QString("Bessel"); break;
    case TYPE_BUTTERWORTH: *s += QString("Butterworth"); break;
    case TYPE_CHEBYSHEV:   *s += QString("Chebyshev"); break;
  }

  *s += QString(" %1Hz...%2Hz \\n ").arg(num2str(Filter->Frequency)).arg(num2str(Filter->Frequency2));
  *s += QString("Impedance matching %3 Ohm\">\n").arg(Filter->Impedance);
  *s += "</Paintings>\n";

  return s;
}
