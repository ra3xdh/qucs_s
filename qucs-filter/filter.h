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

#ifndef FILTER_H
#define FILTER_H

#define CLASS_LOWPASS   0
#define CLASS_HIGHPASS  1
#define CLASS_BANDPASS  2
#define CLASS_BANDSTOP  3

#define TYPE_BESSEL      0
#define TYPE_BUTTERWORTH 1
#define TYPE_CHEBYSHEV   2
#define TYPE_CAUER       3

#include <cmath>

// guard in case cmath also includes math.h
#ifndef M_PI
const double M_PI   = 3.1415926535897932384626433832795029;  /* pi */
const double M_1_PI = 0.3183098861837906715377675267450287;  /* 1/pi */
const double M_LN2  = 0.6931471805599453094172321214581766;
#endif

const double LIGHTSPEED = 299792458.0;
const double Z_FIELD = 376.73031346958504364963;

#define coth(x)  (1.0 + 2.0 / (exp(2.0*(x)) - 1.0))
#define sech(x)  (2.0 / (exp(x) + exp(-(x))))


struct tFilter {
  int Type;
  int Class;
  int Order;
  double Ripple;  // in dB
  double Impedance;
  double Frequency;
  double Frequency2;
  double Frequency3;
  double Attenuation;
};


class QString;

class Filter {
public:
  Filter();

  static double getNormValue(int, tFilter*);
  static double getQuadraticNormValues(int, tFilter*, double&);

protected:
  static QString num2str(double);
  static double getE6value(double);

private:
  static double BesselValue(int, int);
  static double ButterworthValue(int, int);
  static double ChebyshevValue(int, int, double);

  static double quadraticBesselValues(int, int, double&);
  static double quadraticButterworthValues(int, int, double&);
  static double quadraticChebyshevValues(int, int, double, double&);
};

#endif
