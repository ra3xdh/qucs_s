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

#include "lc_filter.h"

#include <QString>


LC_Filter::LC_Filter()
{
}


// ====================================================================
// This is the main function. It creates an LC filter.
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
QString* LC_Filter::createSchematic(tFilter *Filter, bool piType)
{
  double Value, Value2, Omega, Bandwidth;
  if((Filter->Class == CLASS_BANDPASS) || (Filter->Class == CLASS_BANDSTOP))
    Omega = (Filter->Frequency2 + Filter->Frequency) / 2.0;
  else {
    Filter->Frequency2 = 0.0;
    Omega = Filter->Frequency;
  }

  if(Filter->Type == TYPE_CHEBYSHEV) {
    // for Chebyshev filters: set cut-off frequency to 3dB-frequency
    Value = sqrt(pow(10.0, Filter->Ripple/10.0) - 1.0);  // = epsilon
    Omega /= cosh(acosh(1.0 / Value) / double(Filter->Order));
  }

  Bandwidth = fabs(Filter->Frequency2 - Filter->Frequency) / Omega;
  Omega *= 2.0*pi;   // angular frequency

  // create the Qucs schematic
  QString *s = new QString("<Qucs Schematic " PACKAGE_VERSION ">\n");

  int i, x, yc=320, yl;
  x = 20;
  if(Filter->Class != CLASS_BANDPASS)  x += 40;
  *s += "<Components>\n";
  *s += QString("<Pac P1 1 %1 320 18 -26 0 1 \"1\" 1 \"%2 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(x).arg(Filter->Impedance);
  *s += QString("<GND * 1 %1 350 0 0 0 0>\n").arg(x);

  for(i = 0; i < Filter->Order; i++) {
    x = 100 +((i+1) * 70);
    yc = 320;
    yl = 240;

    Value = getNormValue(i, Filter);
    if(Value > 1e30) {
      delete s;
      return NULL;
    }

    if(!piType)
      i++;

    // de-normalize
    if(i & 1)
      Value *= Filter->Impedance / Omega;
    else
      Value /= Filter->Impedance * Omega;

    switch(Filter->Class) {

      case CLASS_LOWPASS:
        if(i & 1)
          *s += QString("<L L1 1 %1 %2 -26 10 0 0 \"%3H\" 1>\n").arg(x).arg(yl).arg(num2str(Value));
        else
          *s += QString("<C C1 1 %1 %2 17 -26 0 1 \"%3F\" 1>\n").arg(x).arg(yc).arg(num2str(Value));
        break;


      case CLASS_HIGHPASS:
        Value = 1.0 / Omega / Omega / Value;  // transform to highpass
        if(i & 1)
          *s += QString("<C C1 1 %1 %2 -27 10 0 0 \"%3F\" 1>\n").arg(x).arg(yl).arg(num2str(Value));
        else
          *s += QString("<L L1 1 %1 %2 17 -26 0 1 \"%3H\" 1>\n").arg(x).arg(yc).arg(num2str(Value));
        break;


      case CLASS_BANDPASS:
        Value /= Bandwidth;    // transform to bandpass
        Value2 = 0.25 / Filter->Frequency / Filter->Frequency2 / pi / pi / Value;
        if(i & 1) {
          *s += QString("<L L1 1 %1 %2 -26 -44 0 0 \"%3H\" 1>\n").arg(x+40).arg(yl).arg(num2str(Value));
          *s += QString("<C C1 1 %1 %2 -26 10 0 0 \"%3F\" 1>\n").arg(x-20).arg(yl).arg(num2str(Value2));
        }
        else {
          *s += QString("<L L1 1 %1 %2 8 -26 0 1 \"%3H\" 1>\n").arg(x).arg(yc).arg(num2str(Value2));
          *s += QString("<C C1 1 %1 %2 -8 46 0 1 \"%3F\" 1>\n").arg(x-30).arg(yc).arg(num2str(Value));
        }
        break;


      case CLASS_BANDSTOP:
        Value2 = 1.0 / Omega / Omega / Bandwidth / Value; // transform to bandstop
        Value *= 0.5 * fabs(Filter->Frequency2/Filter->Frequency - Filter->Frequency/Filter->Frequency2);
        if(i & 1) {
          *s += QString("<L L1 1 %1 %2 -26 -44 0 0 \"%3H\" 1>\n").arg(x).arg(yl-35).arg(num2str(Value));
          *s += QString("<C C1 1 %1 %2 -26 10 0 0 \"%3F\" 1>\n").arg(x).arg(yl).arg(num2str(Value2));
        }
        else {
          *s += QString("<L L1 1 %1 %2 17 -26 0 1 \"%3H\" 1>\n").arg(x).arg(yc).arg(num2str(Value2));
          *s += QString("<C C1 1 %1 %2 17 -26 0 1 \"%3F\" 1>\n").arg(x).arg(yc+60).arg(num2str(Value));
        }
        yc += 60;
        break;

    }

    if((i & 1) == 0)
      *s += QString("<GND * 1 %1 %2 0 0 0 0>\n").arg(x).arg(yc + 30);

    if(!piType)
      i--;
  }


  x += 70;
  if(piType) {
    if(Filter->Order & 1)
      x += 40;
  }
  else {
    if((Filter->Order & 1) == 0)
      x += 70;
  }
  *s += QString("<Pac P2 1 %1 320 18 -26 0 1 \"2\" 1 \"%2 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(x).arg(Filter->Impedance);
  *s += QString("<GND * 1 %1 350 0 0 0 0>\n").arg(x);

  yc += 100;
  Value = Filter->Frequency / 10.0;
  if((Filter->Class == CLASS_BANDPASS) || (Filter->Class == CLASS_BANDSTOP))
    Value2 = 10.0 * Filter->Frequency2;
  else
    Value2 = 10.0 * Filter->Frequency;
  *s += QString("<.SP SP1 1 70 %1 0 67 0 0 \"log\" 1 \"%2Hz\" 1 \"%3Hz\" 1 \"201\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n").arg(yc).arg(num2str(Value)).arg(num2str(Value2));
  *s += QString("<Eqn Eqn1 1 290 %1 -28 15 0 0 \"dBS21=dB(S[2,1])\" 1 \"dBS11=dB(S[1,1])\" 1 \"yes\" 0>\n").arg(yc+10);
  *s += "</Components>\n";

  *s += "<Wires>\n";

  // connect left source
  x = 20;
  if(Filter->Class != CLASS_BANDPASS)
    x += 40;
  *s += QString("<%1 240 %2 290 \"\" 0 0 0>\n").arg(x).arg(x);
  if(piType)
    *s += QString("<%1 240 170 240 \"\" 0 0 0>\n").arg(x);
  else if(Filter->Class == CLASS_BANDPASS)
    *s += QString("<%1 240 120 240 \"\" 0 0 0>\n").arg(x);
  else
    *s += QString("<%1 240 140 240 \"\" 0 0 0>\n").arg(x);

  // wires down to shunt components
  x = 30;
  if(!piType)
    x += 70;
  for(i = 0; i < (Filter->Order / 2) + 1; i++) {
    x += 140;
    *s += QString("<%1 240 %2 290 \"\" 0 0 0>\n").arg(x).arg(x);
  }

  // horizontal wires for series components
  if(Filter->Class == CLASS_BANDPASS) {
    if(piType) {
      for(i = 0; i < (Filter->Order / 2); i++) {
        x = 170 + (i * 140);
        *s += QString("<%1 240 %2 240 \"\" 0 0 0>\n").arg(x).arg(x+20);
        *s += QString("<%1 290 %2 290 \"\" 0 0 0>\n").arg(x-30).arg(x);
        *s += QString("<%1 350 %2 350 \"\" 0 0 0>\n").arg(x-30).arg(x);
      }
      if(Filter->Order & 1) {
        *s += QString("<%1 290 %2 290 \"\" 0 0 0>\n").arg(x+110).arg(x+140);
        *s += QString("<%1 350 %2 350 \"\" 0 0 0>\n").arg(x+110).arg(x+140);
      }
    }
    else
      for(i = 0; i < (Filter->Order / 2); i++) {
        x = 240 + (i * 140);
        *s += QString("<%1 240 %2 240 \"\" 0 0 0>\n").arg(x).arg(x+20);
        *s += QString("<%1 290 %2 290 \"\" 0 0 0>\n").arg(x-30).arg(x);
        *s += QString("<%1 350 %2 350 \"\" 0 0 0>\n").arg(x-30).arg(x);
      }
  }
  else
    for(i = 0; i < (Filter->Order / 2); i++) {
      x = 170 + (i * 140);
      if(piType) {
        *s += QString("<%1 240 %2 240 \"\" 0 0 0>\n").arg(x).arg(x+40);
        *s += QString("<%1 240 %2 240 \"\" 0 0 0>\n").arg(x+100).arg(x+140);
      }
      else
        *s += QString("<%1 240 %2 240 \"\" 0 0 0>\n").arg(x+30).arg(x+110);
    }

  // vertical wires for connecting the second series component
  if(Filter->Class == CLASS_BANDSTOP) {
    x = 0;
    if(piType) {
      i = Filter->Order / 2 - 1;
      x += 70;
    }
    else
      i = (Filter->Order+1) / 2 - 1;
    for(; i>=0; i--) {
      x += 140;
      *s += QString("<%1 240 %2 205 \"\" 0 0 0>\n").arg(x).arg(x);
      *s += QString("<%1 240 %2 205 \"\" 0 0 0>\n").arg(x+60).arg(x+60);
    }
    if(piType)
      x -= 40;
    else
      x += 30;
  }

  // connect right source
  if(piType) {
    if(Filter->Order & 1) {
      x  += 140 + 110;
      *s += QString("<%1 240 %2 290 \"\" 0 0 0>\n").arg(x).arg(x);
      *s += QString("<%1 240 %2 240 \"\" 0 0 0>\n").arg(x-110).arg(x);
    }
  }
  else {
    if(Filter->Class == CLASS_BANDPASS) {
      if((Filter->Order & 1) == 0)
        *s += QString("<%1 240 %2 240 \"\" 0 0 0>\n").arg(x+20).arg(x+140);
    }
    else {
      if(Filter->Order & 1)
        *s += QString("<%1 240 %2 240 \"\" 0 0 0>\n").arg(x+170).arg(x+210);
      else
        *s += QString("<%1 240 %2 240 \"\" 0 0 0>\n").arg(x+110).arg(x+210);
    }
  }
  *s += "</Wires>\n";

  *s += "<Diagrams>\n";
  *s += "</Diagrams>\n";

  *s += "<Paintings>\n";

  *s += QString("<Text 400 %1 12 #000000 0 \"").arg(yc);
  switch(Filter->Type) {
    case TYPE_BESSEL:      *s += QString("Bessel "); break;
    case TYPE_BUTTERWORTH: *s += QString("Butterworth "); break;
    case TYPE_CHEBYSHEV:   *s += QString("Chebyshev "); break;
  }

  switch(Filter->Class) {
    case CLASS_LOWPASS:
      *s += QString("low-pass filter \\n %1Hz cutoff").arg(num2str(Filter->Frequency)); break;
    case CLASS_HIGHPASS:
      *s += QString("high-pass filter \\n %1Hz cutoff").arg(num2str(Filter->Frequency)); break;
    case CLASS_BANDPASS:
      *s += QString("band-pass filter \\n %1Hz...%2Hz").arg(num2str(Filter->Frequency)).arg(num2str(Filter->Frequency2)); break;
    case CLASS_BANDSTOP:
      *s += QString("band-stop filter \\n %1Hz...%2Hz").arg(num2str(Filter->Frequency)).arg(num2str(Filter->Frequency2)); break;
  }
  if (piType == true)
    *s += QString(", pi-type, \\n impedance matching %1 Ohm\">\n").arg(Filter->Impedance);
  else
    *s += QString(", tee-type, \\n impedance matching %1 Ohm\">\n").arg(Filter->Impedance);
  *s += "</Paintings>\n";

  return s;
}
