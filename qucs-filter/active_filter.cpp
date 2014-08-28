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

#include "active_filter.h"

#include <qstring.h>
#include <qmessagebox.h>


Active_Filter::Active_Filter()
{
}

// --------------------------------------------------------------------
QString* Active_Filter::createSchematic(tFilter *Filter, double gain)
{
  int i, x;
  double a, b, C1, C2, R1, R2, R3, Omega;
//  double bandwidth = M_PI * fabs(Filter->Frequency2 - Filter->Frequency);

  Omega = Filter->Frequency;
  if((Filter->Class == CLASS_BANDPASS) || (Filter->Class == CLASS_BANDSTOP))
    Omega = (Filter->Frequency2 + Filter->Frequency) / 2.0;
  Omega *= 2.0 * M_PI;

  if(Filter->Type == TYPE_CHEBYSHEV) {
    // for Chebyshev filters: set cut-off frequency to 3dB-frequency
    a = sqrt(pow(10.0, Filter->Ripple/10.0) - 1.0);  // = epsilon
    a = cosh(acosh(1.0 / a) / double(Filter->Order));
    switch(Filter->Class) {
      case CLASS_LOWPASS:
        Omega /= a;  break;
      case CLASS_HIGHPASS:
        Omega *= a;  break;
    }
  }

  C1 = getE6value(5e-5 / Filter->Frequency);

  // create the Qucs schematic
  QString *s = new QString("<Qucs Schematic " PACKAGE_VERSION ">\n");

  x = 60;
  *s += "<Components>\n";
  *s += QString("<Vac V1 1 %1 270 18 -26 0 1 \"1 V\" 1 \"1 GHz\" 0 \"0\" 0 \"0\" 0>\n").arg(x);
  *s += QString("<GND * 1 %1 300 0 0 0 0>\n").arg(x);

  x += 60;
  if(Filter->Order & 1) {
    a = getQuadraticNormValues(0, Filter, b);

    *s += QString("<OpAmp OP1 1 %1 200 -23 -60 0 0 \"1e6\" 0 \"15 V\" 0>\n").arg(x+120);
    *s += QString("<GND * 1 %1 300 0 0 0 0>\n").arg(x+30);

    switch(Filter->Class) {
      case CLASS_LOWPASS:
        R1 = a / Omega / C1;
        *s += QString("<R R1 1 %1 180 -26 -59 1 0 \"%2\" 1 \"26.85\" 0 \"0\" 0 \"0\" 0 \"26.85\" 0 \"european\" 0 \"SMD0603\" 0>\n").arg(x).arg(R1);
        *s += QString("<C C1 1 %1 270 17 -26 0 1 \"%2F\" 1>\n").arg(x+30).arg(num2str(C1));
        break;
      case CLASS_HIGHPASS:
        R1 = 1.0 / a / Omega / C1;
        *s += QString("<R R1 1 %1 270 17 -26 0 1 \"%2\" 1 \"26.85\" 0 \"0\" 0 \"0\" 0 \"26.85\" 0 \"european\" 0 \"SMD0603\" 0>\n").arg(x+30).arg(R1);
        *s += QString("<C C1 1 %1 180 -26 -59 1 0 \"%2F\" 1>\n").arg(x).arg(num2str(C1));
        break;
      default:
        QMessageBox::critical(0, "Error", "Filter class not supported.");
        delete s;
        return NULL;
    }
    x += 190;
  }

  for(i = 1; i <= Filter->Order/2; i++) {
    a = getQuadraticNormValues(i, Filter, b);

    *s += QString("<OpAmp OP1 1 %1 200 -18 40 1 0 \"1e6\" 0 \"15 V\" 0>\n").arg(x+240);
    *s += QString("<GND * 1 %1 220 0 0 0 0>\n").arg(x+210);
    *s += QString("<GND * 1 %1 300 0 0 0 0>\n").arg(x+70);

    switch(Filter->Class) {
      case CLASS_LOWPASS:
        gain = pow(10.0 , gain/20.0);
        C2 = getE6value(4.1 * b * (1.0+gain) * C1 / a / a);
        R2 = (a*C2 - sqrt(a*a*C2*C2 - 4.0*C1*C2*b*(1.0+gain))) / 2.0/C1/C2/Omega;
        R1 = R2 / gain;
        R3 = b / C1/C2/R2/Omega/Omega;

        *s += QString("<R R1 1 %1 180 -26 -59 1 0 \"%2\" 1 \"26.85\" 0 \"0\" 0 \"0\" 0 \"26.85\" 0 \"european\" 0 \"SMD0603\" 0>\n").arg(x).arg(R1);
        *s += QString("<R R2 1 %1 130 15 -26 0 1 \"%2\" 1 \"26.85\" 0 \"0\" 0 \"0\" 0 \"26.85\" 0 \"european\" 0 \"SMD0603\" 0>\n").arg(x+70).arg(R2);
        *s += QString("<R R3 1 %1 180 -26 15 0 0 \"%2\" 1 \"26.85\" 0 \"0\" 0 \"0\" 0 \"26.85\" 0 \"european\" 0 \"SMD0603\" 0>\n").arg(x+130).arg(R3);
        *s += QString("<C C1 1 %1 130 17 -26 0 1 \"%2F\" 1>\n").arg(x+190).arg(num2str(C1));
        *s += QString("<C C2 1 %1 270 17 -26 0 1 \"%2F\" 1>\n").arg(x+70).arg(num2str(C2));
        break;
      case CLASS_HIGHPASS:
        C2 = C1 / gain;
        R1 = (2.0 + gain) / a / Omega / C2;
        R2 = a / b / Omega / C2 / (2.0 + gain);
        *s += QString("<R R1 1 %1 130 17 -26 0 1 \"%2\" 1 \"26.85\" 0 \"0\" 0 \"0\" 0 \"26.85\" 0 \"european\" 0 \"SMD0603\" 0>\n").arg(x+190).arg(R1);
        *s += QString("<R R2 1 %1 270 17 -26 0 1 \"%2\" 1 \"26.85\" 0 \"0\" 0 \"0\" 0 \"26.85\" 0 \"european\" 0 \"SMD0603\" 0>\n").arg(x+70).arg(R2);
        *s += QString("<C C1 1 %1 180 -26 -59 1 0 \"%2F\" 1>\n").arg(x).arg(num2str(C1));
        *s += QString("<C C2 1 %1 130 15 -26 0 1 \"%2F\" 1>\n").arg(x+70).arg(num2str(C2));
        *s += QString("<C C3 1 %1 180 -26 15 0 0 \"%2F\" 1>\n").arg(x+130).arg(num2str(C2));
        break;
/*      case CLASS_BANDPASS:
        R2 = 1.0 / C1 / bandwidth;
        R1 = 0.5 * R2 / gain;
        R3 = bandwidth * M_PI / C1
             / (Omega*Omega - 2.0*gain*bandwidth*bandwidth);

        *s += QString("<R R1 1 %1 180 -26 -59 1 0 \"%2\" 1 \"26.85\" 0 \"0\" 0 \"0\" 0 \"26.85\" 0 \"european\" 0 \"SMD0603\" 0>\n").arg(x).arg(R1);
        *s += QString("<R R2 1 %1 130 17 -26 0 1 \"%2\" 1 \"26.85\" 0 \"0\" 0 \"0\" 0 \"26.85\" 0 \"european\" 0 \"SMD0603\" 0>\n").arg(x+190).arg(R2);
        *s += QString("<R R3 1 %1 270 17 -26 0 1 \"%2\" 1 \"26.85\" 0 \"0\" 0 \"0\" 0 \"26.85\" 0 \"european\" 0 \"SMD0603\" 0>\n").arg(x+70).arg(R3);
        *s += QString("<C C1 1 %1 130 15 -26 0 1 \"%2F\" 1>\n").arg(x+70).arg(num2str(C1));
        *s += QString("<C C2 1 %1 180 -26 15 0 0 \"%2F\" 1>\n").arg(x+130).arg(num2str(C1));
        break;
      case CLASS_BANDSTOP:
        break;
*/
      default:
        QMessageBox::critical(0, "Error", "Filter class not supported.");
        delete s;
        return NULL;
    }
    x += 310;
  }


  x -= 10;
  *s += QString("<R Rload 1 %1 270 15 -26 0 1 \"1k\" 1 \"26.85\" 0 \"0\" 0 \"0\" 0 \"26.85\" 0 \"european\" 0 \"SMD0603\" 0>\n").arg(x);
  *s += QString("<GND * 1 %1 300 0 0 0 0>\n").arg(x);
  *s += QString("<.AC AC1 1 70 350 0 41 0 0 \"lin\" 1 \"%1Hz\" 1 \"%2Hz\" 1 \"300\" 1 \"no\" 0>\n").arg(num2str(0.1*Filter->Frequency)).arg(num2str(10.0*Filter->Frequency));
  *s += QString("<.DC DC1 1 70 550 0 71 0 0 \"26.85\" 0 \"0.001\" 0 \"1 pA\" 0 \"1 uV\" 0 \"no\" 0 \"150\" 0 \"no\" 0 \"none\" 0 \"CroutLU\" 0>\n");
  *s += QString("<Eqn Eqn1 1 350 350 -28 15 0 0 \"Ku=dB(output.v)\" 1 \"Phase=phase(output.v)\" 1 \"yes\" 0>\n");
  *s += "</Components>\n";

  *s += "<Wires>\n";

  // connect left source
  *s += QString("<60 180 60 240 \"\" 0 0 0>\n");
  *s += QString("<60 180 90 180 \"\" 0 0 0>\n");

  // connect right load resistor
  *s += QString("<%1 200 %2 240 \"output\" %3 180 30 \"\">\n").arg(x).arg(x).arg(x+30);
  *s += QString("<%1 200 %2 200 \"\" 0 0 0>\n").arg(x-20).arg(x);

  // wires between components
  x = 150;
  if(Filter->Order & 1) {
    *s += QString("<%1 180 %2 180 \"\" 0 0 0>\n").arg(x).arg(x+60);
    *s += QString("<%1 180 %2 240 \"\" 0 0 0>\n").arg(x).arg(x);
    *s += QString("<%1 220 %2 250 \"\" 0 0 0>\n").arg(x+60).arg(x+60);
    *s += QString("<%1 250 %2 250 \"\" 0 0 0>\n").arg(x+60).arg(x+130);
    *s += QString("<%1 180 %2 250 \"\" 0 0 0>\n").arg(x+130).arg(x+130);
    x += 190;
  }

  for(i = 1; i < Filter->Order; i+=2) {
    *s += QString("<%1 180 %2 180 \"\" 0 0 0>\n").arg(x).arg(x+70);
    *s += QString("<%1 180 %2 180 \"\" 0 0 0>\n").arg(x+130).arg(x+180);
    *s += QString("<%1 160 %2 180 \"\" 0 0 0>\n").arg(x+40).arg(x+40);
    *s += QString("<%1 180 %2 240 \"\" 0 0 0>\n").arg(x+40).arg(x+40);
    *s += QString("<%1 160 %2 180 \"\" 0 0 0>\n").arg(x+160).arg(x+160);
    *s += QString("<%1 100 %2 100 \"\" 0 0 0>\n").arg(x+40).arg(x+250);
    *s += QString("<%1 100 %2 200 \"\" 0 0 0>\n").arg(x+250).arg(x+250);
    x += 310;
  }

  *s += "</Wires>\n";

  *s += "<Diagrams>\n";
  *s += "</Diagrams>\n";

  *s += "<Paintings>\n";

  *s += QString("<Text 500 350 12 #000000 0 \"Active ");
  switch(Filter->Class) {
    case CLASS_LOWPASS:
      *s += QString("low-pass filter \\n %1Hz cutoff ").arg(num2str(Filter->Frequency)); break;
    case CLASS_HIGHPASS:
      *s += QString("high-pass filter \\n %1Hz cutoff ").arg(num2str(Filter->Frequency)); break;
    case CLASS_BANDPASS:
      *s += QString("band-pass filter \\n %1Hz...%2Hz ").arg(num2str(Filter->Frequency)).arg(num2str(Filter->Frequency2)); break;
    case CLASS_BANDSTOP:
      *s += QString("band-reject filter \\n %1Hz...%2Hz ").arg(num2str(Filter->Frequency)).arg(num2str(Filter->Frequency2)); break;
  }

  switch(Filter->Type) {
    case TYPE_BESSEL:      *s += QString("Bessel"); break;
    case TYPE_BUTTERWORTH: *s += QString("Butterworth"); break;
    case TYPE_CHEBYSHEV:   *s += QString("Chebyshev"); break;
  }
  *s += QString(", order %1\">\n").arg(Filter->Order);
  *s += "</Paintings>\n";

  return s;
}
