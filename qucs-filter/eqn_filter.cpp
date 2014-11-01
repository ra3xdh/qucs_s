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

#include "eqn_filter.h"

#include <QString>
#include <QMessageBox>


Equation_Filter::Equation_Filter()
{
}


// ====================================================================
// This is the main function. It creates the filter schematic.
// Input parameters:
//       Class      - CLASS_LOWPASS
//                    CLASS_HIGHPASS
//                    CLASS_BANDPASS
//                    CLASS_BANDSTOP
//       Impedance  - input and output reference impedance in ohms
//       Order      - order of the filter
//       Frequency  - corner frequency (lowpass and highpass) or
//                    band start frequency (bandpass and bandstop)
//       Frequency2 - band stop frequency (only for bandpass and bandstop)
QString* Equation_Filter::createSchematic(tFilter *Filter)
{
  int i;
  double a, b, Omega;
  if((Filter->Class == CLASS_BANDPASS) || (Filter->Class == CLASS_BANDSTOP))
    Omega = (Filter->Frequency2 + Filter->Frequency) / 2.0;
  else {
    Filter->Frequency2 = 0.0;
    Omega = Filter->Frequency;
  }

  // create the Qucs schematic
  QString *s = new QString("<Qucs Schematic " PACKAGE_VERSION ">\n");

  *s += "<Components>\n";
  *s += QString("<Pac P1 1 100 30 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(Filter->Impedance);
  *s += QString("<GND * 1 100 60 0 0 0 0>\n");

  QString eqn = "2";  // because of 50ohms input and output

  if(Filter->Type == TYPE_CHEBYSHEV) {
    // for Chebyshev filters: set cut-off frequency to 3dB-frequency
    a = sqrt(pow(10.0, Filter->Ripple/10.0) - 1.0);  // = epsilon
    Omega /= cosh(acosh(1.0 / a) / double(Filter->Order));

    // Even order Chebyshev filters have ripple above 0dB, let's compensate it.
    if((Filter->Order & 1) == 0)
      eqn = QString::number(2.0 / pow(10.0, Filter->Ripple/20.0));
  }

  Omega *= 2.0*M_PI;   // angular frequency

  QString varS = "*S";
  switch(Filter->Class) {
    case CLASS_HIGHPASS:  // transform to highpass
      varS = "/S";
      Omega = 1.0 / Omega;
      break;
    case CLASS_BANDPASS:  // transform to bandpass
      varS = QString("*(S+%1/S)").arg(Omega*Omega);
      Omega = 0.5 / M_PI / fabs(Filter->Frequency2 - Filter->Frequency);
      break;
    case CLASS_BANDSTOP:  // transform to bandstop
      varS = QString("/(S+%1/S)").arg(Omega*Omega);
      Omega = 2.0 * M_PI * fabs(Filter->Frequency2 - Filter->Frequency);
      break;
  }


  if(Filter->Order & 1) {
    a = getQuadraticNormValues(0, Filter, b);
    eqn += QString(" / (1 + %1%2)").arg(a / Omega).arg(varS);
  }
  for(i = 1; i <= Filter->Order/2; i++) {
    a = getQuadraticNormValues(i, Filter, b);
    eqn += QString(" / (1 + %1%2 + %3%4%5)")
           .arg(a/Omega).arg(varS).arg(b/Omega/Omega).arg(varS).arg(varS);
  }


  *s += QString("<RFEDD X1 1 260 0 -26 21 0 0 \"G\" 0 \"2\" 0 \"open\" 0 \"%1\" 1 \"0\" 1 \"%2\" 1 \"%3\" 1>\n").arg(1.0/Filter->Impedance).arg(eqn).arg(Filter->Impedance);

  *s += QString("<Pac P2 1 400 30 18 -26 0 1 \"2\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(Filter->Impedance);
  *s += QString("<GND * 1 400 60 0 0 0 0>\n");

  if((Filter->Class == CLASS_BANDPASS) || (Filter->Class == CLASS_BANDSTOP))
    a = 10.0 * Filter->Frequency2;
  else
    a = 10.0 * Filter->Frequency;
  *s += QString("<.SP SP1 1 110 160 0 67 0 0 \"log\" 1 \"%1Hz\" 1 \"%2Hz\" 1 \"200\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n").arg(num2str(0.1*Filter->Frequency)).arg(num2str(a));
  *s += QString("<Eqn Eqn1 1 350 170 -28 15 0 0 \"S21_dB=dB(S[2,1])\" 1 \"S11_dB=dB(S[1,1])\" 1 \"yes\" 0>\n");
  *s += "</Components>\n";

  *s += "<Wires>\n";

  // connect left source
  *s += QString("<100 0 230 0 \"\" 0 0 0>\n");

  // connect right source
  *s += QString("<290 0 400 0 \"\" 0 0 0>\n");

  *s += "</Wires>\n";

  *s += "<Diagrams>\n";
  *s += "</Diagrams>\n";

  *s += "<Paintings>\n";

  *s += QString("<Text 450 150 12 #000000 0 \"");
  switch(Filter->Type) {
    case TYPE_BESSEL:      *s += QString("Bessel "); break;
    case TYPE_BUTTERWORTH: *s += QString("Butterworth "); break;
    case TYPE_CHEBYSHEV:   *s += QString("Chebyshev "); break;
  }

  switch(Filter->Class) {
    case CLASS_LOWPASS:
      *s += QString("low-pass filter, %1Hz cutoff").arg(num2str(Filter->Frequency)); break;
    case CLASS_HIGHPASS:
      *s += QString("high-pass filter, %1Hz cutoff").arg(num2str(Filter->Frequency)); break;
    case CLASS_BANDPASS:
      *s += QString("band-pass filter, %1Hz...%2Hz").arg(num2str(Filter->Frequency)).arg(num2str(Filter->Frequency2)); break;
    case CLASS_BANDSTOP:
      *s += QString("band-reject filter, %1Hz...%2Hz").arg(num2str(Filter->Frequency)).arg(num2str(Filter->Frequency2)); break;
  }
  *s += QString(" \\n PI-type, impedance matching %1 Ohm\">\n").arg(Filter->Impedance);
  *s += "</Paintings>\n";

  return s;
}
