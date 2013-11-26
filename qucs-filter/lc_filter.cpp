/***************************************************************************
                               lc_filter.cpp
                              ---------------
    begin                : Wed Mar 02 2005
    copyright            : (C) 2005 by Toyoyuki Ishikawa
                           (C) 2005 by Vincent Habchi, F5RCS
                           (C) 2005 by Michael Margraf
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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <QString>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
 
LC_Filter::LC_Filter()
{
}

LC_Filter::~LC_Filter()
{
}

// #########################################################################
QString LC_Filter::num2str(double Num)
{
  char c = 0;
  double cal = fabs(Num);
  if(cal > 1e-20) {
    cal = log10(cal) / 3.0;
    if(cal < -0.2)  cal -= 0.98;
    int Expo = int(cal);

    if(Expo >= -5) if(Expo <= 4)
      switch(Expo) {
        case -5: c = 'f'; break;
        case -4: c = 'p'; break;
        case -3: c = 'n'; break;
        case -2: c = 'u'; break;
        case -1: c = 'm'; break;
        case  1: c = 'k'; break;
        case  2: c = 'M'; break;
        case  3: c = 'G'; break;
        case  4: c = 'T'; break;
      }

    if(c)  Num /= pow(10.0, double(3*Expo));
  }

  QString Str = QString::number(Num, 'g', 4);
  if(c)  Str += c;
  
  return Str;
}


double BesselCoef[18][23] = {

/*  2 */ {0.57550275, 2.1478055,    0.0,        0.0,        0.0,
          0.0,         0.0,         0.0,        0.0,        0.0,
          0.0,         0.0,         0.0,        0.0,        0.0,
          0.0,         0.0,         0.0,        0.0,},

/*  3 */ {0.33742149, 0.97051182,   2.2034114,  0.0,        0.0,
          0.0,         0.0,         0.0,        0.0,        0.0,
          0.0,         0.0,         0.0,        0.0,        0.0,
          0.0,         0.0,         0.0,        0.0,},

/*  4 */ {0.2334158,  0.67252481,   1.0815161,  2.2403786,  0.0,
          0.0,         0.0,         0.0,        0.0,        0.0,
          0.0,         0.0,         0.0,        0.0,        0.0,
          0.0,         0.0,         0.0,        0.0,},

/*  5 */ {0.17431938, 0.50724063,   0.80401117, 1.1110332,  2.2582171,
          0.0,         0.0,         0.0,        0.0,        0.0,
          0.0,         0.0,         0.0,        0.0,        0.0,
          0.0,         0.0,         0.0,        0.0,},

/*  6 */ {0.13649238, 0.40018984,   0.6391554,  0.85378587, 1.112643,
          2.2645236,  0.0,          0.0,        0.0,        0.0,
          0.0,         0.0,         0.0,        0.0,        0.0,
          0.0,         0.0,         0.0,        0.0,},

/*  7 */ {0.11056245, 0.32588813,   0.52489273, 0.70200915, 0.86902684,
          1.1051644,  2.2659006,    0.0,        0.0,        0.0,
          0.0,         0.0,         0.0,        0.0,        0.0,
          0.0,         0.0,         0.0,        0.0,},

/*  8 */ {0.091905558, 0.27191069,  0.44092213, 0.59357268, 0.73025665,
          0.86950037,  1.0955593,   2.2656071,  0.0,        0.0,
          0.0,         0.0,         0.0,        0.0,        0.0,
          0.0,         0.0,         0.0,        0.0,},

/*  9 */ {0.077965506, 0.23129119,  0.37698651, 0.5107787,  0.63059516,
          0.74073299,  0.86387345,  1.0862838,  2.2648789,  0.0,
          0.0,         0.0,         0.0,        0.0,        0.0,
          0.0,         0.0,         0.0,        0.0,},

/* 10 */ {0.067229245, 0.19984023,  0.32699699, 0.44543381, 0.55281473,
          0.64933545,  0.74201735,  0.85607238, 1.0780948,  2.2641262,
          0.0,         0.0,         0.0,        0.0,        0.0,
          0.0,         0.0,         0.0,        0.0,},

/* 11 */ {0.058753264, 0.1749102,   0.28706331, 0.39272513, 0.48983467,
          0.57742576,  0.6573719,   0.73864992, 0.84789472, 1.0711184,
          2.2634675,   0.0,         0.0,        0.0,        0.0,
          0.0,         0.0,         0.0,        0.0,},

/* 12 */ {0.051923,    0.15475798,  0.25458412, 0.34949045, 0.43777572,
          0.51824604,  0.59097358,  0.65913453, 0.73309605, 0.84013853,
          1.0652583,   2.2629233,   0.0,        0.0,        0.0,
          0.0,         0.0,         0.0,        0.0,},

/* 13 */ {0.046323098, 0.13819538,  0.22775963, 0.31352943, 0.39412779,
          0.46842754,  0.53589822,  0.59744624, 0.65727966, 0.72670805,
          0.83311934,  1.0603538,   2.2624829,  0.0,        0.0,
          0.0,         0.0,         0.0,        0.0,},

/* 14 */ {0.041663913, 0.12438811,  0.20530976, 0.28325704, 0.35711858,
          0.42591257,  0.48895292,  0.54624778, 0.59939971, 0.6534283,
          0.72022065,  0.82691896,  1.0562406,  2.262128,   0.0,
          0.0,         0.0,         0.0,        0.0,},

/* 15 */ {0.037738195, 0.11273464,  0.186303,   0.25750279, 0.32543754,
          0.38927393,  0.44833655,  0.5022966,  0.55162099, 0.59850238,
          0.64857809,  0.71401822,  0.8215094,  1.0527729,  2.2618408,
          0.0,         0.0,         0.0,        0.0},

/* 16 */ {0.03439122,  0.102799,    0.1700389,  0.23539257, 0.29808411,
          0.35745153,  0.41285998,  0.4638916,  0.51051757, 0.55360981,
          0.59585789,  0.64334149,  0.70828423, 0.81681581, 1.0498286,
          2.2616066,   0.0,         0.0,        0.0},

/* 17 */ {0.031523496, 0.094214373, 0.1560301,  0.21622781, 0.27429733,
          0.32962199,  0.38165374,  0.42998671, 0.47437364, 0.51507678,
          0.55331904,  0.59220181,  0.63808843, 0.70308744, 0.81274832,
          1.0473087,   2.2614134,   0.0,        0.0},

/* 18 */ {0.028450351, 0.088289337, 0.14199897, 0.20112875, 0.25237658,
          0.30571616,  0.35380272,  0.39992872, 0.44218287, 0.48110224,
          0.5170226,   0.55152405,  0.58802463, 0.63303427, 0.69843256,
          0.80921764,  1.0451338,   2.2612522,  0.0},

/* 19 */ {0.027944243, 0.077389235, 0.13716781, 0.18087554, 0.23831545,
          0.28142035,  0.33055916,  0.37249664, 0.41348365, 0.45066049,
          0.48503552,  0.51711998,  0.54877223, 0.58365096, 0.62829555,
          0.69429034,  0.8061422,   1.043241,   2.2611162}
};

// Calculate normalized component value for Bessel filter
double LC_Filter::BesselValue(int No, int Order)
{
  return BesselCoef [Order - 2] [No];
}


// Calculate normalized component value for Butterworth filter
double LC_Filter::ButterworthValue(int No, int Order)
{
  return 2.0 * sin(double(2*No + 1) / double(2*Order) * M_PI);
}


// Calculate normalized component value for Chebyshev filter
// "Ripple" is in dB !
double LC_Filter::ChebyshevValue(int No, int Order, double Ripple)
{
  // We must define static variables since g(k+1) is tied to g(k)
  static double ak, gk ;
  double        a, b;

  Ripple = sqrt(pow(10.0,  (Ripple / 10.0)) - 1.0);
  Ripple = sinh(asinh(1.0 / Ripple) / double(Order));

  a = sin(double(2 * No + 1) / double(2 * Order) * M_PI);
  if (No == 0)
    gk = a / Ripple;
  else {
    b   = sin(double(No) * M_PI / double(Order));
    gk *= Ripple * Ripple + b * b;
    gk  = ak * a / gk;
  }
  ak = a;

  return 2.0 * gk;
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
QString* LC_Filter::createSchematic(tFilter *Filter)
{
  double Value, Value2, Omega, Bandwidth;
  if((Filter->Class == CLASS_BANDPASS) || (Filter->Class == CLASS_BANDSTOP))
    Omega = (Filter->Frequency2 + Filter->Frequency) / 2.0;
  else {
    Filter->Frequency2 = 0.0;
    Omega = Filter->Frequency;
  }

  Bandwidth = fabs(Filter->Frequency2 - Filter->Frequency) / Omega;
  Omega *= 2.0*M_PI;   // angular frequency

  // create the Qucs schematic
  QString *s = new QString("<Qucs Schematic ");
  *s += PACKAGE_VERSION;
  *s += ">\n";

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

    switch(Filter->Type) {
      case TYPE_BESSEL:
        Value = BesselValue(i, Filter->Order);
        break;
      case TYPE_BUTTERWORTH:
        Value = ButterworthValue(i, Filter->Order);
        break;
      case TYPE_CHEBYSHEV:
        Value = ChebyshevValue(i, Filter->Order, Filter->Ripple);
        break;
      default:
        Value = 0.0;
        *s = QString("");
        return s;
    }

    // de-normalize
    if(i & 1)  Value *= Filter->Impedance / Omega;
    else  Value /= Filter->Impedance * Omega;


    switch(Filter->Class) {

      case CLASS_LOWPASS:
        if(i & 1)
          *s += QString("<L L%1 1 %2 %3 -26 10 0 0 \"%4H\" 1>\n").arg(i+1).arg(x).arg(yl).arg(num2str(Value));
        else
          *s += QString("<C C%1 1 %2 %3 17 -26 0 1 \"%4F\" 1>\n").arg(i+1).arg(x).arg(yc).arg(num2str(Value));
        break;


      case CLASS_HIGHPASS:
        Value = 1.0 / Omega / Omega / Value;  // transform to highpass
        if(i & 1)
          *s += QString("<C C%1 1 %2 %3 -27 10 0 0 \"%4F\" 1>\n").arg(i+1).arg(x).arg(yl).arg(num2str(Value));
        else
          *s += QString("<L L%1 1 %2 %3 17 -26 0 1 \"%4H\" 1>\n").arg(i+1).arg(x).arg(yc).arg(num2str(Value));
        break;


      case CLASS_BANDPASS:
        Value /= Bandwidth;    // transform to bandpass
        Value2 = 0.25 / Filter->Frequency / Filter->Frequency2 / M_PI / M_PI / Value;
        if(i & 1) {
          *s += QString("<L L%1 1 %2 %3 -26 -44 0 0 \"%4H\" 1>\n").arg(i+1).arg(x+40).arg(yl).arg(num2str(Value));
          *s += QString("<C C%1 1 %2 %3 -26 10 0 0 \"%4F\" 1>\n").arg(i+1).arg(x-20).arg(yl).arg(num2str(Value2));
        }
        else {
          *s += QString("<L L%1 1 %2 %3 8 -26 0 1 \"%4H\" 1>\n").arg(i+1).arg(x).arg(yc).arg(num2str(Value2));
          *s += QString("<C C%1 1 %2 %3 -8 46 0 1 \"%4F\" 1>\n").arg(i+1).arg(x-30).arg(yc).arg(num2str(Value));
        }
        break;


      case CLASS_BANDSTOP:
        Value2 = 1.0 / Omega / Omega / Bandwidth / Value; // transform to bandstop
        Value *= 0.5 * fabs(Filter->Frequency2/Filter->Frequency - Filter->Frequency/Filter->Frequency2);
        if(i & 1) {
          *s += QString("<L L%1 1 %2 %3 -26 -44 0 0 \"%4H\" 1>\n").arg(i+1).arg(x).arg(yl-35).arg(num2str(Value));
          *s += QString("<C C%1 1 %2 %3 -26 10 0 0 \"%4F\" 1>\n").arg(i+1).arg(x).arg(yl).arg(num2str(Value2));
        }
        else {
          *s += QString("<L L%1 1 %2 %3 17 -26 0 1 \"%4H\" 1>\n").arg(i+1).arg(x).arg(yc).arg(num2str(Value2));
          *s += QString("<C C%1 1 %2 %3 17 -26 0 1 \"%4F\" 1>\n").arg(i+1).arg(x).arg(yc+60).arg(num2str(Value));
        }
        yc += 60;
        break;

    }

    if((i & 1) == 0)
      *s += QString("<GND * 1 %1 %2 0 0 0 0>\n").arg(x).arg(yc + 30);

  }


  if(Filter->Order & 1)  x += 110;
  else  x += 70;
  *s += QString("<Pac P2 1 %1 320 18 -26 0 1 \"2\" 1 \"%2 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(x).arg(Filter->Impedance);
  *s += QString("<GND * 1 %1 350 0 0 0 0>\n").arg(x);

  yc += 100;
  Value = Filter->Frequency / 10.0;
  if((Filter->Class == CLASS_BANDPASS) || (Filter->Class == CLASS_BANDSTOP))
    Value2 = 10.0 * Filter->Frequency2;
  else
    Value2 = 10.0 * Filter->Frequency;
  *s += QString("<.SP SP1 1 70 %1 0 50 0 0 \"log\" 1 \"%2Hz\" 1 \"%3Hz\" 1 \"200\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n").arg(yc).arg(num2str(Value)).arg(num2str(Value2));
  *s += QString("<Eqn Eqn1 1 260 %1 -28 15 0 0 \"dBS21=dB(S[2,1])\" 1 \"dBS11=dB(S[1,1])\" 1 \"yes\" 0>\n").arg(yc+10);
  *s += "</Components>\n";

  *s += "<Wires>\n";

  // connect left source
  x = 20;
  if(Filter->Class != CLASS_BANDPASS)  x += 40;
  *s += QString("<%1 240 %2 290 \"\" 0 0 0>\n").arg(x).arg(x);
  *s += QString("<%1 240 170 240 \"\" 0 0 0>\n").arg(x);

  // wires down to shunt components
  for(i = 0; i < (Filter->Order / 2) + 1; i++) {
      x = 170 + (i * 140);
      *s += QString("<%1 240 %2 290 \"\" 0 0 0>\n").arg(x).arg(x);
  }

  // horizontal wires for series components
  if(Filter->Class == CLASS_BANDPASS) {
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
      x = 170 + (i * 140);
      *s += QString("<%1 240 %2 240 \"\" 0 0 0>\n").arg(x).arg(x+40);
      *s += QString("<%1 240 %2 240 \"\" 0 0 0>\n").arg(x+100).arg(x+140);
      if(Filter->Class == CLASS_BANDSTOP) {
        *s += QString("<%1 240 %2 205 \"\" 0 0 0>\n").arg(x+40).arg(x+40);
        *s += QString("<%1 240 %2 205 \"\" 0 0 0>\n").arg(x+100).arg(x+100);
      }
    }

  // connect right source
  if(Filter->Order & 1) {
    x  += 140 + 110;
    *s += QString("<%1 240 %2 290 \"\" 0 0 0>\n").arg(x).arg(x);
    *s += QString("<%1 240 %2 240 \"\" 0 0 0>\n").arg(x-110).arg(x);
  }
  *s += "</Wires>\n";

  *s += "<Diagrams>\n";
  *s += "</Diagrams>\n";

  *s += "<Paintings>\n";

  *s += QString("<Text 400 %1 12 #000000 0 \"").arg(yc+10);
  switch(Filter->Type) {
    case TYPE_BESSEL:      *s += QString("Bessel "); break;
    case TYPE_BUTTERWORTH: *s += QString("Butterworth "); break;
    case TYPE_CHEBYSHEV:   *s += QString("Chebyshev "); break;
  }

  switch(Filter->Class) {
    case CLASS_LOWPASS:
      *s += QString("low-pass filter\\n%1Hz cutoff").arg(num2str(Filter->Frequency)); break;
    case CLASS_HIGHPASS:
      *s += QString("high-pass filter\\n%1Hz cutoff").arg(num2str(Filter->Frequency)); break;
    case CLASS_BANDPASS:
      *s += QString("band-pass filter\\n%1Hz...%2Hz").arg(num2str(Filter->Frequency)).arg(num2str(Filter->Frequency2)); break;
    case CLASS_BANDSTOP:
      *s += QString("band-reject filter\\n%1Hz...%2Hz").arg(num2str(Filter->Frequency)).arg(num2str(Filter->Frequency2)); break;
  }
  *s += QString(", PI-type,\\nimpedance matching %3 Ohm\">\n").arg(Filter->Impedance);
  *s += "</Paintings>\n";

  return s;
}
