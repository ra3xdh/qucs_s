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

#include "stepz_filter.h"

#include <qstring.h>
#include <qmessagebox.h>

StepImpedance_Filter::StepImpedance_Filter()
{
}

// -----------------------------------------------------------------------
QString* StepImpedance_Filter::createSchematic(tFilter *Filter, tSubstrate *Substrate, bool isMicrostrip)
{
  int i, x;
  double len, width, er_eff_min, er_eff_max, Z0;
  double Omega = 2.0 * M_PI * Filter->Frequency;
  double Zlow  = 20.0;
  double Zhigh = 400.0;

  width = er_eff_min = er_eff_max = 1.0;

  if(isMicrostrip) {
    TL_Filter::calcMicrostrip(Substrate, Substrate->minWidth,
                              Filter->Frequency, er_eff_min, Zhigh);
    TL_Filter::calcMicrostrip(Substrate, Substrate->maxWidth,
                              Filter->Frequency, er_eff_max, Zlow);

    if((Substrate->er > 4.0) || (Substrate->height > 0.6))
      QMessageBox::warning(0, QObject::tr("Warning"),
          QObject::tr("High-impedance is %1 ohms, low-impedance is %2 ohms.\n"
                      "To get acceptable results it is recommended to use\n"
                      "a substrate with lower permittivity and larger height.\n").arg(Zhigh).arg(Zlow));
  }

  // create the Qucs schematic
  QString *s = new QString("<Qucs Schematic " PACKAGE_VERSION ">\n");

  x = 60;
  *s += "<Components>\n";
  *s += QString("<Pac P1 1 %1 330 18 -26 0 1 \"1\" 1 \"%2 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(x).arg(Filter->Impedance);
  *s += QString("<GND * 1 %1 360 0 0 0 0>\n").arg(x);

  x -= 30;
  for(i = 0; i < Filter->Order; i++) {
    x += 90;

    // de-normalize values and calculate transmission line length
    if(i & 1) {
      // replace a capacitor
      Z0 = Zlow;
      len = getNormValue(i, Filter) / Filter->Impedance * Zlow;
      if(isMicrostrip)
        len -= 0.1 * Zlow / Zhigh;
      len *= LIGHTSPEED / sqrt(er_eff_max) / Omega;
      width = Substrate->maxWidth;
    }
    else {
      // replace an inductor
      Z0 = Zhigh;
      len = getNormValue(i, Filter) * Filter->Impedance;
      if(i > 0)
        len -= 0.5 * Zlow * Zlow * getNormValue(i-1, Filter) / Filter->Impedance;
      if(isMicrostrip)
        if((i+1) < Filter->Order)
          len -= 0.5 * Zlow * Zlow * getNormValue(i+1, Filter) / Filter->Impedance;
      len /= Zhigh;
      len *= LIGHTSPEED / sqrt(er_eff_min) / Omega;
      width = Substrate->minWidth;
    }

    if(isMicrostrip)
      *s += QString("<MLIN MS1 1 %1 180 -26 15 0 0 \"Sub1\" 1 \"%2\" 1 \"%3\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").arg(x).arg(num2str(width)).arg(num2str(len));
    else
      *s += QString("<TLIN Line1 1 %1 180 -26 20 0 0 \"%2\" 1 \"%3\" 1 \"0 dB\" 0 \"26.85\" 0>\n").arg(x).arg(Z0).arg(num2str(len));
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
    x += 90;
  }

  *s += "</Wires>\n";

  *s += "<Diagrams>\n";
  *s += "</Diagrams>\n";

  *s += "<Paintings>\n";

  *s += QString("<Text 420 460 12 #000000 0 \"stepped-impedance lowpass filter \\n ");
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
