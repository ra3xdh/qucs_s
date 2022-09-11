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

#include "cline_filter.h"

#include <QString>
#include <QMessageBox>

CoupledLine_Filter::CoupledLine_Filter()
{
}

// -------------------------------------------------------------------
QString* CoupledLine_Filter::createSchematic(tFilter *Filter, tSubstrate *Substrate, bool isMicrostrip)
{
  int i, x, y, dx;
  double Value, gap, len, dl, width, er_eff, Z0e, Z0o;
  double freq = (Filter->Frequency2 + Filter->Frequency) / 2.0;
  double Bandwidth = fabs(Filter->Frequency2 - Filter->Frequency) / freq;

  width = gap = er_eff = 1.0;

  // create the Qucs schematic
  QString *s = new QString("<Qucs Schematic " PACKAGE_VERSION ">\n");

  x = 60;
  *s += "<Components>\n";
  *s += QString("<Pac P1 1 %1 380 18 -26 0 1 \"1\" 1 \"%2 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(x).arg(Filter->Impedance);
  *s += QString("<GND * 1 %1 410 0 0 0 0>\n").arg(x);

  x -= 30;
  y  = 170;
  len = dl = 0.0;
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

    Z0e = (1.0 + Value + Value*Value) * Filter->Impedance;
    Z0o = (1.0 - Value + Value*Value) * Filter->Impedance;

    if(isMicrostrip) {
      sythesizeCoupledMicrostrip(Z0e, Z0o, freq, Substrate, width, gap, er_eff);
      if((width < 1e-7) || (gap < 1e-7)) {
        QMessageBox::critical(0, "Error", "Filter can't be created.");
        delete s;
        return NULL;
      }
      dl = getMicrostripOpen(width / Substrate->height, Substrate->er, er_eff);
    }

    len = LIGHTSPEED / freq / 4.0 / sqrt(er_eff);

    if(isMicrostrip) {
      y += 40;
      len -= dl * Substrate->height;
      *s += QString("<MCOUPLED MS1 1 %1 %2 -26 37 0 0 \"Sub1\" 1 \"%3\" 1 \"%4\" 1 \"%5\" 1 \"Kirschning\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(x).arg(y).arg(num2str(width)).arg(num2str(len)).arg(num2str(gap));
      *s += QString("<MOPEN MS1 1 %1 %2 -12 15 1 2 \"Sub1\" 1 \"%3\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"Kirschning\" 0>\n").arg(x-60).arg(y+30).arg(num2str(width));
      *s += QString("<MOPEN MS1 1 %1 %2 -26 -81 1 0 \"Sub1\" 1 \"%3\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"Kirschning\" 0>\n").arg(x+60).arg(y-30).arg(num2str(width));
      y += 20;
      x += 60;
    }
    else {
      y += 20;
      *s += QString("<CTLIN Line1 1 %1 %2 -26 16 0 0 \"%3\" 1 \"%4\" 1 \"%5\" 1 \"1\" 0 \"1\" 0 \"0 dB\" 0 \"0 dB\" 0 \"26.85\" 0>\n").arg(x).arg(y).arg(Z0e).arg(Z0o).arg(num2str(len));
    }
  }


  if(isMicrostrip)
    x += 20;
  else
    x += 80;
  *s += QString("<Pac P2 1 %1 %2 18 -26 0 1 \"2\" 1 \"%3 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(x).arg(y+70).arg(Filter->Impedance);
  *s += QString("<GND * 1 %1 %2 0 0 0 0>\n").arg(x).arg(y+100);

  *s += QString("<.SP SP1 1 70 460 0 67 0 0 \"lin\" 1 \"%2Hz\" 1 \"%3Hz\" 1 \"300\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n").arg(num2str(2.0 * Filter->Frequency  - Filter->Frequency2)).arg(num2str(2.0 * Filter->Frequency2 - Filter->Frequency));
  if(isMicrostrip)
    *s += QString("<SUBST Sub1 1 310 500 -30 24 0 0 \"%1\" 1 \"%2m\" 1 \"%3m\" 1 \"%4\" 1 \"%5\" 1 \"%6\" 1>\n").arg(Substrate->er).arg(num2str(Substrate->height)).arg(num2str(Substrate->thickness)).arg(Substrate->tand).arg(Substrate->resistivity).arg(Substrate->roughness);
  *s += QString("<Eqn Eqn1 1 450 560 -28 15 0 0 \"S21_dB=dB(S[2,1])\" 1 \"S11_dB=dB(S[1,1])\" 1 \"yes\" 0>\n");
  *s += "</Components>\n";

  *s += "<Wires>\n";

  // connect left source
  *s += QString("<60 180 60 350 \"\" 0 0 0>\n");
  *s += QString("<60 180 90 180 \"\" 0 0 0>\n");

  // connect right source
  y += 10;
  *s += QString("<%1 %2 %3 %4 \"\" 0 0 0>\n").arg(x).arg(y).arg(x).arg(y+30);
  *s += QString("<%1 %2 %3 %4 \"\" 0 0 0>\n").arg(x-50).arg(y).arg(x).arg(y);

  // wires between components
  x = 150;
  y = 200;
  dx = 30;
  if(isMicrostrip) {
    dx = 90;
    y += 40;
  }
  for(i = 0; i < Filter->Order; i++) {
    *s += QString("<%1 %2 %3 %4 \"\" 0 0 0>\n").arg(x).arg(y).arg(x+dx).arg(y);
    if(isMicrostrip) {
      x += 150;
      y += 60;
    }
    else {
      x += 90;
      y += 20;
    }
  }

  *s += "</Wires>\n";

  *s += "<Diagrams>\n";
  *s += "</Diagrams>\n";

  *s += "<Paintings>\n";

  *s += QString("<Text 70 650 12 #000000 0 \"Coupled-line bandpass filter \\n ");
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
