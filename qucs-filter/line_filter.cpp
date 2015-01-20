/***************************************************************************
  copyright: (C) 2010 by Michael Margraf
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "line_filter.h"

#include <QString>
#include <QMessageBox>

// capacitive end-coupled, half-wavelength bandpass filter
Line_Filter::Line_Filter()
{
}

// -------------------------------------------------------------------------
QString* Line_Filter::createSchematic(tFilter *Filter, tSubstrate *Substrate, bool isMicrostrip)
{
  double Value, Value2, gap, len, dl, width, er_eff, Wh;
  double Omega = (Filter->Frequency2 + Filter->Frequency) / 2.0;
  double Bandwidth = fabs(Filter->Frequency2 - Filter->Frequency) / Omega;

  // get "width" and "er_eff"
  width = er_eff = 1.0;
  if(isMicrostrip)
    getMicrostrip(Filter->Impedance, Omega, Substrate, width, er_eff);
  Wh = width / Substrate->height;

  Omega *= 2.0*pi;  // angular frequency

  // create the Qucs schematic
  QString *s = new QString("<Qucs Schematic " PACKAGE_VERSION ">\n");

  int i, x, yc;
  x = 60;
  *s += "<Components>\n";
  *s += QString("<Pac P1 1 %1 320 18 -26 0 1 \"1\" 1 \"%2 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(x).arg(Filter->Impedance);
  *s += QString("<GND * 1 %1 350 0 0 0 0>\n").arg(x);

  x -= 30;
  yc = 180;
  Value2 = len = dl = 0.0;
  for(i = 0; i <= Filter->Order; i++) {
    x += 90;

    Value = getNormValue(i-1, Filter);
    if(Value > 1e30) {
      delete s;
      return NULL;
    }
    Value *= getNormValue(i, Filter);

	// characteristic admittance of J-inverter (normalized to Y0)
    if((i == 0) || (i == Filter->Order))
      Value = sqrt(0.5 * pi * Bandwidth / Value);
    else
      Value = 0.5 * pi * Bandwidth / sqrt(Value);

    // susceptance
    Value /= (1.0 - Value*Value);

    // gap capacitance
    gap = Value / Filter->Impedance / Omega;
    //if(gap < 1e-7) {
    if(gap < 0) {
      QMessageBox::critical(0, "Error", "Filter bandwidth is too large.");
      delete s;
      return NULL;
    }

    // calculate gap from gap capacitance
    if(isMicrostrip) {
      gap /= 0.04598e-12 * (0.03 + pow(Wh, 1.23)) * (0.272 + 0.07*Substrate->er);
      gap /= (1.0 + 4.19 * (1.0 - exp(-0.785/sqrt(Wh))));
      gap /= 500 * Substrate->height;
      gap = log(gap) * Substrate->height / -1.86;
    }

    if(i > 0) {
      len = LIGHTSPEED / sqrt(er_eff) / Omega
          * (pi - 0.5*(atan(2.0*Value) + atan(2.0*Value2)));
      len -= dl;
    }
    Value2 = Value;  // remember for next loop

    if(isMicrostrip) {
      if(gap < 1e-7) {
        QMessageBox::critical(0, "Error",
            "Filter can't be created.\n"
            "A small bandwidth of less than 3\% is possible only.\n"
            "Using a substrate with larger thickness or with smaller permitivity may also help a little bit.");
        delete s;
        return NULL;
      }

      dl = 0.107 * (Wh+9.0) * pow(gap/Substrate->height, 3.23)
         + 2.09*pow(gap/Substrate->height, 1.05) * (1.5+0.3*Wh) / (1.0+0.6*Wh);
      dl = (dl + 2.04e-5) / (dl + 1.0);
      dl *= getMicrostripOpen(Wh, Substrate->er, er_eff) * Substrate->height;
    }

    if(i > 0) {
      len -= dl;
      if(isMicrostrip)
        *s += QString("<MLIN MS1 1 %1 %2 -26 15 0 0 \"Subst1\" 0 \"%3\" 1 \"%4\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(x).arg(yc).arg(num2str(width)).arg(num2str(len));
      else
        *s += QString("<TLIN Line1 1 %1 %2 -26 20 0 0 \"%3\" 1 \"%4\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(x).arg(yc).arg(Filter->Impedance).arg(num2str(len));
      x += 90;
    }

    if(isMicrostrip)
      *s += QString("<MGAP MS1 1 %1 %2 -26 15 0 0 \"Subst1\" 0 \"%3\" 1 \"%4\" 1 \"%5\" 1 \"Hammerstad\" 0 \"Kirschning\" 0>\n").arg(x).arg(yc).arg(num2str(width)).arg(num2str(width)).arg(num2str(gap));
    else
      *s += QString("<C C1 1 %1 %2 -26 17 0 0 \"%3\" 1>\n").arg(x).arg(yc).arg(num2str(gap));
  }


  x += 80;
  *s += QString("<Pac P2 1 %1 320 18 -26 0 1 \"2\" 1 \"%2 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(x).arg(Filter->Impedance);
  *s += QString("<GND * 1 %1 350 0 0 0 0>\n").arg(x);

  Value  = 2.0 * Filter->Frequency  - Filter->Frequency2;
  Value2 = 2.0 * Filter->Frequency2 - Filter->Frequency;
  *s += QString("<.SP SP1 1 70 420 0 67 0 0 \"lin\" 1 \"%2Hz\" 1 \"%3Hz\" 1 \"300\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n").arg(num2str(Value)).arg(num2str(Value2));
  if(isMicrostrip)
    *s += QString("<SUBST Subst1 1 300 460 -30 24 0 0 \"%1\" 1 \"%2m\" 1 \"%3m\" 1 \"%4\" 1 \"%5\" 1 \"%6\" 1>\n").arg(Substrate->er).arg(num2str(Substrate->height)).arg(num2str(Substrate->thickness)).arg(Substrate->tand).arg(Substrate->resistivity).arg(Substrate->roughness);
  *s += QString("<Eqn Eqn1 1 450 520 -28 15 0 0 \"S21_dB=dB(S[2,1])\" 1 \"S11_dB=dB(S[1,1])\" 1 \"yes\" 0>\n");
  *s += "</Components>\n";

  *s += "<Wires>\n";

  // connect left source
  *s += QString("<60 180 60 290 \"\" 0 0 0>\n");
  *s += QString("<60 180 90 180 \"\" 0 0 0>\n");

  // connect right source
  *s += QString("<%1 180 %2 290 \"\" 0 0 0>\n").arg(x).arg(x);
  *s += QString("<%1 180 %2 180 \"\" 0 0 0>\n").arg(x-50).arg(x);

  // wires between components
  x = 150;
  for(i = 0; i < 2*Filter->Order; i++) {
    *s += QString("<%1 180 %2 180 \"\" 0 0 0>\n").arg(x).arg(x+30);
    x += 90;
  }

  *s += "</Wires>\n";

  *s += "<Diagrams>\n";
  *s += "</Diagrams>\n";

  *s += "<Paintings>\n";

  *s += QString("<Text 420 420 12 #000000 0 \"end-coupled, half-wavelength bandpass filter \\n ");
  switch(Filter->Type) {
    case TYPE_BESSEL:      *s += QString("Bessel"); break;
    case TYPE_BUTTERWORTH: *s += QString("Butterworth"); break;
    case TYPE_CHEBYSHEV:   *s += QString("Chebyshev"); break;
  }

  *s += QString(" %1Hz...%2Hz \\n ").arg(num2str(Filter->Frequency)).arg(num2str(Filter->Frequency2));
  *s += QString("impedance matching %3 Ohm\">\n").arg(Filter->Impedance);
  *s += "</Paintings>\n";

  return s;
}
