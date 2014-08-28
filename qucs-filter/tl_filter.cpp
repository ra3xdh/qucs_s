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

#include "tl_filter.h"

#define  MAX_ERROR  1e-7

TL_Filter::TL_Filter()
{
}

// --------------------------------------------------------------
// Calculates the impedance 'zl' and the relative effective permittivity 'er_eff'
// of a microstrip line.
void TL_Filter::calcMicrostrip(tSubstrate *substrate,
                               double width, double freq, double& er_eff, double& zl)
{
  double a, b;
  double h  = substrate->height;
  double t  = substrate->thickness;
  double er = substrate->er;
  double Wh = width / h;
  t /= h;

  // quasi-static models by Hammerstad

  double w1 = Wh;
  if(t > 1e-100) { // width correction due to metal thickness?
    a = coth(sqrt(6.517*Wh));
    b = t / M_PI * log(1.0 + 10.873127 / t / a / a);
    w1 += b;
    Wh += 0.5 * b * (1.0 + sech(sqrt(er - 1.0)));
  }

  // relative effective permittivity
  a = Wh * Wh;
  b = a * a;
  er_eff  = -0.564 * pow((er-0.9) / (er+3.0), 0.053);
  er_eff *= 1.0 + log((b + a/2704.0) / (b + 0.432)) / 49.0 + log(1.0 + a*Wh/5929.741) / 18.7;
  er_eff  = (er+1.0) / 2.0 + (er-1.0) / 2.0 * pow(1.0 + 10.0/Wh, er_eff);

  // characteristic impedance
  zl = 6.0 + 0.2831853 * exp(-pow(30.666/Wh, 0.7528));
  zl = Z_FIELD / 2.0/M_PI * log(zl/Wh + sqrt(1.0 + 4.0/Wh/Wh));

  // characteristic impedance (same again for "w1")
  a = 6.0 + 0.2831853 * exp(-pow(30.666/w1, 0.7528));
  a = Z_FIELD / 2.0/M_PI * log(a/w1 + sqrt(1.0 + 4.0/w1/w1));

  a /= zl;
  zl /= sqrt(er_eff);
  er_eff *= a * a;


  // dispersion models by Kirschning

  freq *= h / 1e6;  // normalize frequency into GHz*mm
    
  // relative effective permittivity
  a = 0.0363 * exp(-4.6*Wh) * (1.0 - exp(-pow(freq/38.7, 4.97)));
  a *= 1.0 + 2.751 * (1.0 - exp(-pow(er/15.916, 8.0)));
  a = pow((0.1844 + a) * freq, 1.5763);
  a *= 0.27488 + Wh*(0.6315 + 0.525 / pow(1.0+0.0157*freq, 20.0)) - 0.065683*exp(-8.7513*Wh);
  a *= 0.33622 * (1.0 - exp(-0.03442*er));
  double er_freq = er - (er - er_eff) / (1.0 + a);

  // characteristic impedance
  a = -0.03891 * pow(er, 1.4);
  b = -0.267 * pow (Wh, 7.0);
  double R7 = 1.206 - 0.3144*exp(a) * (1.0 - exp(b));

  a = 0.016 + pow(0.0514*er, 4.524);
  b = pow(freq/28.843, 12.0);
  a = 5.086 * a * b / (0.3838 + 0.386*a) / (1.0 + 1.2992*b);
  b = -22.2 * pow(Wh, 1.92);
  a *= exp(b);
  b = pow(er - 1.0, 6.0);
  double R9 = a*b / (1.0 + 10.0*b);

  a = 4.766 * exp(-3.228 * pow(Wh, 0.641));  // = R3
  a = 1.0 + 1.275
    * (1.0 - exp(-0.004625*a*pow(er, 1.674) * pow(freq/18.365, 2.745))); // = R8

  b = 0.9408 * pow(er_freq, a) - 0.9603;  // = R13
  b /= (0.9408 - R9) * pow(er_eff, a) - 0.9603;
  R9 = b; // = R13 / R14

  a = 0.00044 * pow(er, 2.136) + 0.0184;  // = R10
  a *= 0.707 * pow(freq/12.3, 1.097); // = R15
  a = exp(-0.026*pow(freq, 1.15656) - a);
  b = pow(freq/19.47, 6.0);
  b /= 1.0 + 0.0962 * b;  // = R11
  b = 1.0 + 0.0503 *er*er* b * (1.0 - exp(-pow(Wh/15, 6.0))); // = R16
  R7 *= (1.0 - 1.1241 * a / b / (1.0 + 0.00245*Wh*Wh));  // = R17

  zl *= pow(R9, R7);
  er_eff = er_freq;
}

// -------------------------------------------------------------------
// Calculates the width 'width' and the relative effective permittivity 'er_eff'
// of a microstrip line. It uses an iterative search algorithm because
// synthesis equations doesn't exist.
void TL_Filter::getMicrostrip(double Z0, double freq, tSubstrate *substrate,
                              double &width, double &er_eff)
{
  int iteration = 0;  // iteration counter
  double Z0_current, Z0_result, increment;

  width = 1e-3;  // start with 1mm

  do {
    // compute line parameters
    calcMicrostrip(substrate, width, freq, er_eff, Z0_current);

    if(fabs(Z0 - Z0_current) < MAX_ERROR)
      break;  // wanted value was found

    increment = width / 100.0;
    width += increment;

    // compute line parameters
    calcMicrostrip(substrate, width, freq, er_eff, Z0_result);

    // Newton iteration: w(n+1) = w(n) - f(w(n))/f'(w(n))
	//   with f(w(n))  = Z0_current - Z0
    //   and  f'(w(n)) = (Z0_result - Z0_current) / increment
    width -= (Z0_current - Z0) / (Z0_result - Z0_current) * increment;
    if(width <= 0.0)
      width = increment;

    iteration++;
  } while(iteration < 150);
}

// ---------------------------------------------------------------------
// Calculates the additional line length (per substrate height) of a microstrip open.
double TL_Filter::getMicrostripOpen(double Wh, double er, double er_eff)
{
  // model by Kirschning
  double Q1, Q3, Q4, Q5;
  Q4 = pow(er_eff, 0.81);
  Q5 = pow(Wh, 0.8544);
  Q1 = 0.434907 * (Q4+0.26) / (Q4-0.189) * (Q5+0.236) / (Q5+0.87);
  Q3 = 1.0 + pow(Wh, 0.371) / (2.358*er + 1.0);
  Q3 = 1.0 + 0.5274 / pow(er_eff, 0.9236) * atan(0.084 * pow(Wh, 1.9413/Q3));
  Q4 = 1.0 + 0.0377 * (6.0 - 5.0*exp(0.036*(1.0-er))) * atan(0.067*pow(Wh, 1.456));
  Q5 = 1.0 - 0.218 * exp(-7.5*Wh);
  return Q1 * Q3 * Q5 / Q4;
}

// ---------------------------------------------------------------------
// For coupled microstrip lines: Get physical dimensions and effective
// relative permittivity from the characteristic impedance of
// even and odd mode. It uses an iterative search algorithm because
// synthesis equations doesn't exist.
void TL_Filter::sythesizeCoupledMicrostrip
        (double zl_e, double zl_o, double freq, tSubstrate *substrate,
         double& width, double& gap, double& er_eff)
{
  double a, er, Wh_e, Wh_o, ce, co, j11, j12, j21, j22, dw, dg;
  double zl_e_current, zl_e_result1, zl_e_result2;
  double zl_o_current, zl_o_result1, zl_o_result2;

  er = substrate->er;

  // Wheeler formula for single microstrip synthesis
  a = exp(zl_e * sqrt(er + 1.0) / 84.8) - 1.0;
  Wh_e = 8.0 * sqrt(a * ((7.0 + 4.0 / er) / 11.0) + ((1.0 + 1.0 / er) / 0.81)) / a;

  // Wheeler formula for single microstrip synthesis
  a = exp(zl_o * sqrt(er + 1.0) / 84.8) - 1.0;
  Wh_o = 8.0 * sqrt(a * ((7.0 + 4.0 / er) / 11.0) + ((1.0 + 1.0 / er) / 0.81)) / a;

  // first rough estimation
  ce = cosh(0.5 * M_PI * Wh_e);
  co = cosh(0.5 * M_PI * Wh_o);
  gap = (2.0 / M_PI) * acosh((ce + co - 2.0) / (co - ce)) * substrate->height;
  width = acosh((ce * co - 1.0) / (co - ce)) / M_PI - gap / 2.0;
  width *= substrate->height;

  int iteration = 0;  // iteration counter
  do {
    // compute line parameters
    getCoupledMicrostrip
	   (width, gap, freq, substrate, zl_e_current, zl_o_current, ce, co);

    if(fabs(zl_e - zl_e_current) < MAX_ERROR)
      if(fabs(zl_o - zl_o_current) < MAX_ERROR)
        break;  // wanted value was found

    // compute new line parameters
    dw = width / 100.0;
    getCoupledMicrostrip
	   (width+dw, gap, freq, substrate, zl_e_result1, zl_o_result1, ce, co);

    // compute line parameters
    dg = gap / 100.0;
    getCoupledMicrostrip
	   (width, gap+dg, freq, substrate, zl_e_result2, zl_o_result2, ce, co);

    // Newton iteration: w(n+1) = w(n) - f(w(n))/f'(w(n))
	//   with f(w(n))  = Z0_current - Z0
    //   and  f'(w(n)) = (Z0_result - Z0_current) / increment

    // Jacobian matrix
	j11 = (zl_e_result1 - zl_e_current) / dw;
	j12 = (zl_e_result2 - zl_e_current) / dg;
	j21 = (zl_o_result1 - zl_o_current) / dw;
	j22 = (zl_o_result2 - zl_o_current) / dg;

    a = j11 * j22 - j12 * j21;   // determinante

    // invert Jacobian
    j22 = j22 * (zl_e_current - zl_e) - j12 * (zl_o_current - zl_o);
    j11 = j11 * (zl_o_current - zl_o) - j21 * (zl_e_current - zl_e);
    width -= j22 / a;
	gap -= j11 / a;

    if(width <= 0.0)
      width = dw;
    if(gap <= 0.0)
      gap = dg;

    iteration++;
  } while(iteration < 200);

  er_eff = 0.5 * (ce + co);

  if(iteration >= 200)
    width = gap = er_eff = -1.0;
}

// --------------------------------------------------------------
// dispersive relative permittivity and characteristic impedance of single line
// by Kirschning and Jansen (freq = freq * h / 1e6)
double TL_Filter::dispersionKirschning
       (double er, double Wh, double freq, double& er_eff, double& zl)
{
  double a, b;

  // relative effective permittivity
  a = 0.0363 * exp(-4.6*Wh) * (1.0 - exp(-pow(freq/38.7, 4.97)));  // = P3
  a *= 1.0 + 2.751 * (1.0 - exp(-pow(er/15.916, 8.0)));            // = P4
  a = pow((0.1844 + a) * freq, 1.5763);
  a *= 0.27488 + Wh*(0.6315 + 0.525 / pow(1.0+0.0157*freq, 20.0))
       - 0.065683*exp(-8.7513*Wh);                                 // = P1
  a *= 0.33622 * (1.0 - exp(-0.03442*er));                         // = P2
  double er_freq = er - (er - er_eff) / (1.0 + a);

  // characteristic impedance
  a = -0.03891 * pow(er, 1.4);
  b = -0.267 * pow (Wh, 7.0);
  double R7 = 1.206 - 0.3144*exp(a) * (1.0 - exp(b));

  a = 0.016 + pow(0.0514*er, 4.524);
  b = pow(freq/28.843, 12.0);
  a = 5.086 * a * b / (0.3838 + 0.386*a) / (1.0 + 1.2992*b);
  b = -22.2 * pow(Wh, 1.92);
  a *= exp(b);
  b = pow(er - 1.0, 6.0);
  double R9 = a*b / (1.0 + 10.0*b);

  a = 4.766 * exp(-3.228 * pow(Wh, 0.641));  // = R3
  a = 1.0 + 1.275
      * (1.0 - exp(-0.004625*a*pow(er, 1.674) * pow(freq/18.365, 2.745))); // = R8

  b = 0.9408 * pow(er_freq, a) - 0.9603;  // = R13
  b /= (0.9408 - R9) * pow(er_eff, a) - 0.9603;
  R9 = b; // = R13 / R14

  a = 0.00044 * pow(er, 2.136) + 0.0184;  // = R10
  a *= 0.707 * pow(freq/12.3, 1.097); // = R15
  a = exp(-0.026*pow(freq, 1.15656) - a);
  b = pow(freq/19.47, 6.0);
  b /= 1.0 + 0.0962 * b;  // = R11
  b = 1.0 + 0.0503 *er*er* b * (1.0 - exp(-pow(Wh/15, 6.0))); // = R16
  R7 *= (1.0 - 1.1241 * a / b / (1.0 + 0.00245*Wh*Wh));  // = R17

  zl *= pow(R9, R7);
  er_eff = er_freq;
  return R7;
}

// --------------------------------------------------------------
// For coupled microstrip lines: Get characteristic impedance and effective
// relative permittivity of even and odd mode from the physical dimensions.
void TL_Filter::getCoupledMicrostrip
        (double ms_width, double g, double freq, tSubstrate *substrate,
         double& zl_e, double& zl_o, double& er_eff_e, double& er_eff_o)
{
  double Wh_o, a, b, c, d;
  double Wh_e = ms_width;
  double h  = substrate->height;
  double t  = substrate->thickness;
  double er = substrate->er;

  // width correction due to metal thickness
  a = 1.0;
  if(t > 1e-20) {
    if(Wh_e > (h/2.0/M_PI)) {
      if(h > (t*4.0*M_PI))
        a = t / M_PI * (1.0 + log(2.0*h/t));
    }
    else {
      if(Wh_e > (t*2.0))
        a = t / M_PI * (1.0 + log(4.0*M_PI*Wh_e/t));
    }
  }
  if(g > 20.0*t) {
    b = 2.0 * t / g / er;
    a *= 1.0 - 0.5*exp(-0.69*a/b);
  }
  else
    a = b = 0.0;
  Wh_e = (Wh_e + a) / h;
  Wh_o = Wh_e + b;

  g /= h;
  double fn = freq / 1e6 * h;


  // static zero-thickness relative permittivity by Hammerstad and Jensen
  double er_e, er_o;
  a = Wh_e * Wh_e;
  b = a * a;
  er_e  = -0.564 * pow((er-0.9) / (er+3.0), 0.053);
  er_e *= 1.0 + log((b + a/2704.0) / (b + 0.432)) / 49.0 + log(1.0 + a*Wh_e/5929.741) / 18.7;
  er_e  = (er+1.0) / 2.0 + (er-1.0) / 2.0 * pow(1.0 + 10.0/Wh_e, er_e);
  a = Wh_o * Wh_o;
  b = a * a;
  er_o  = -0.564 * pow((er-0.9) / (er+3.0), 0.053);
  er_o *= 1.0 + log((b + a/2704.0) / (b + 0.432)) / 49.0 + log(1.0 + a*Wh_o/5929.741) / 18.7;
  er_o  = (er+1.0) / 2.0 + (er-1.0) / 2.0 * pow(1.0 + 10.0/Wh_o, er_o);


  // static effective permittivity of even mode
  er_eff_e = 0.5 * (er + 1.0);
  b  = Wh_e * (20.0 + g*g) / (10.0 + g*g) + g*exp(-g);
  a  = -0.564 * pow((er-0.9) / (er+3.0), 0.053);
  a *= 1.0 + log(b*b * (b*b + 3.698e-4) / (b*b*b*b + 0.432)) / 49.0
       + log(1.0 + pow(b/18.1, 3.0)) / 18.7;
  er_eff_e += 0.5 * (er - 1.0) * pow(1.0 + 10.0/b, a);


  // static effective permittivity of odd mode
  b  = 0.747 * er / (0.15 + er);
  b -= (b - 0.207) * exp(-0.414 * Wh_o);
  b *= pow(g, 0.593 + 0.694 * exp(-0.562 * Wh_o));
  a = 0.7287 * (er_o - 0.5*(er+1.0)) * (1.0 - exp(-0.179*Wh_o));
  er_eff_o = er_o + (0.5 * (er + 1.0) + a - er_o) * exp(-b);


  // static zero-thickness characteristic impedance by Hammerstad and Jensen
  zl_e = 6.0 + 0.2831853 * exp(-pow(30.666/Wh_e, 0.7528));
  zl_e = Z_FIELD/sqrt(er_e) / 2.0/M_PI * log(zl_e/Wh_e + sqrt(1.0 + 4.0/Wh_e/Wh_e));

  // static characteristic impedance of even mode
  b  = 1.0 + 0.7519 * g + 0.189 * pow(g, 2.31);                  // = Q2
  c  = 0.1975 + pow(16.6 + pow(8.4 / g, 6.0), -0.387)
       + log(pow(g, 10.0) / (1.0 + pow(g / 3.4, 10.0))) / 241.0; // = Q3
  a  = b * (exp(-g) * pow(Wh_e, c)
            + (2.0 - exp(-g)) * pow(Wh_e, -c));                 // = 1/Q4
  a  = 1.739 * pow(Wh_e, 0.194) / a;                            // = Q1
  zl_e *= sqrt(er_e / er_eff_e) / (1.0 - zl_e / Z_FIELD * sqrt(er_e) * a);


  // static zero-thickness characteristic impedance by Hammerstad and Jensen
  zl_o = 6.0 + 0.2831853 * exp(-pow(30.666/Wh_o, 0.7528));
  zl_o = Z_FIELD/sqrt(er_o) / 2.0/M_PI * log(zl_o/Wh_o + sqrt(1.0 + 4.0/Wh_o/Wh_o));

  // static characteristic impedance of odd mode
  a  = exp(-6.5 - 0.95 * log(g) - pow(g / 0.15, 5.0));                // = Q8
  a  = a + 1.0 / 16.5;                                                // = Q9
  a *= log((10.0 + 190.0 * g * g) / (1.0 + 82.3 * g * g * g));        // log(Q7)
  a  = pow(Wh_o, -a);                                                 // = Q10_1
  a *= 0.2305 + log(pow(g, 10.0) / (1.0 + pow(g / 5.8, 10.0))) / 281.3
       + log(1.0 + 0.598 * pow(g, 1.154)) / 5.1;                      // = Q6
  a  = exp(log(Wh_o) * a);                                            // = Q10_2
  a *= 1.794 + 1.14 * log(1.0 + 0.638 / (g + 0.517 * pow(g, 2.43)));  // = Q5
  a /= b;                                                             // = Q10_3
  a  = 1.739 * pow(Wh_o, 0.194) / b
       / (exp(-g) * pow(Wh_o, c) + (2.0 - exp(-g)) * pow(Wh_o, -c)) - a;// = Q4/Q10
  zl_o *= sqrt(er_o / er_eff_o) / (1.0 - zl_o / Z_FIELD * sqrt(er_o) * a);



  er_e = er_eff_e;
  er_o = er_eff_o;

  // dispersive relative permittivity of even mode
  a  = 0.0363 * exp(-4.6 * Wh_e) * (1.0 - exp(-pow(fn / 38.7, 4.97))); // = P3
  d  = 1.0 + 2.751 * (1.0 - exp(-pow(er / 15.916, 8.0)));              // = P4
  a *= d;
  b  = 0.334 * exp(-3.3 * pow(er / 15.0, 3.0)) + 0.746;                // = P5
  b *= exp(-pow(fn / 18.0, 0.368));                                    // = P6
  b  = 1.0 + 4.069 * b * pow(g, 0.479)
              * exp(-1.347 * pow(g, 0.595) - 0.17 * pow(g, 2.5));      // = P7
  a  = pow((a + 0.1844 * b) * fn, 1.5763);                             // = Fe
  a *= 0.27488 + (0.6315 + 0.525 / pow(1.0 + 0.0157 * fn, 20.0)) * Wh_e
      - 0.065683 * exp(-8.7513 * Wh_e);                                // = P1
  c  = 0.33622 * (1.0 - exp(-0.03442 * er));                           // = P2
  er_eff_e = er - (er - er_eff_e) / (1.0 + a*c);


  // dispersive relative permittivity of odd mode
  a  = 0.7168 * (1.0 + 1.076 / (1.0 + 0.0576 * (er - 1.0)));           // = P8
  a -= 0.7913 * (1.0 - exp(-pow(fn / 20.0, 1.424)))
       * atan(2.481 * pow(er / 8.0, 0.946));                           // = P9
  a += (1.0 - a) / (1.0 + 1.183 * pow(Wh_o, 1.376));                   // = P12
  b = 0.242 * pow(er - 1.0, 0.55);                                     // = P10
  b = -1.695 * b / (0.414 + 1.605 * b);                                 // = P13
  a *= exp(b * pow(g, 1.092));                                        // = P15_1
  a /= 0.8928 + 0.1072 * (1.0 - exp(-0.42 * pow(fn / 20.0, 3.215)));   // = P14
  b = 0.6366 * (exp(-0.3401 * fn) - 1.0)
      * atan(1.263 * pow(Wh_o / 3.0, 1.629));                          // P11
  b = fabs(1.0 - 0.8928 * (1.0 + b) * a);                              // = P15_2
  a  = 0.0363 * exp(-4.6 * Wh_o) * (1.0 - exp(-pow(fn / 38.7, 4.97))); // = P3
  a  = pow((a*d + 0.1844) * fn * b, 1.5763);                           // = Fo
  a *= 0.27488 + (0.6315 + 0.525 / pow(1.0 + 0.0157 * fn, 20.0)) * Wh_o
      - 0.065683 * exp(-8.7513 * Wh_o);                                // = P1
  er_eff_o = er - (er - er_eff_o) / (1.0 + a*c);


  // dispersive characteristic impedance of even mode
  a  = 0.893 * (1.0 - 0.3 / (1.0 + 0.7 * (er - 1.0)));                   // = Q11
  a  = -2.121 * (pow(fn / 20.0, 4.91) / (1.0 + a * pow(fn / 20.0, 4.91)))
       * exp(-2.87 * g) * pow(g, 0.902);                                 // = Q12
  b  = 1.0 + 0.038 * pow(er / 8.0, 5.1);                                 // = Q13
  c  = 1.0 + 1.203 * pow(er / 15.0, 4.0) / (1.0 + pow(er / 15.0, 4.0));  // = Q14
  b  = 1.887 * exp(-1.5 * pow(g, 0.84)) * pow(g, c)
       / (1.0 + 0.41 * pow(fn / 15.0, 3.0)
          * pow(Wh_e, 2.0 / b) / (0.125 + pow(Wh_e, 1.626 / b)));        // = Q15
  a += b * (1.0 + 9.0 / (1.0 + 0.403 * pow(er - 1.0, 2)));               // = Q16
  a -= 0.394 * (1.0 - exp(-1.47 * pow(Wh_e / 7.0, 0.672)))
       * (1.0 - exp(-4.25 * pow(fn / 20.0, 1.87)));                      // = Q17
  a += 0.61 * (1.0 - exp(-2.13 * pow(Wh_e / 8.0, 1.593)))
       / (1.0 + 6.544 * pow(g, 4.17));                                   // = Q18
  b  = 0.21 * g * g * g * g / ((1.0 + 0.18 * pow(g, 4.9))
       * (1.0 + 0.1*Wh_e*Wh_e) * (1.0 + pow(fn / 24.0, 3.0)));           // = Q19
  a += b * (0.09 + 1.0 / (1.0 + 0.1 * pow(er - 1, 2.7)));                // = Q20

  b  = 4.766 * exp(-3.228 * pow(Wh_e, 0.641));                           // = pe
  a += 1.275 * (1.0 - exp(-0.004625 * b * pow(er, 1.674) * pow(fn / 18.365, 2.745)))
       + 1.0;                                                            // = Ce

  b  = fabs(1.0 - 42.54 * pow(g, 0.133) * exp(-0.812 * g)
                  * pow(Wh_e, 2.5) / (1.0 + 0.033 * pow(Wh_e, 2.5)));    // = Q21
  b  = 0.016 + pow(0.0514 * er * b, 4.524);                              // = qe
  c  = pow(fn / 28.843, 12);                                             // = re
  b  = 5.086 * b * c / (0.3838 + 0.386 * b)
       * (exp(-22.2 * pow(Wh_e, 1.92)) / (1.0 + 1.2992 * c))
	   * (pow(er - 1.0, 6.0) / (1.0 + 10.0 * pow(er - 1.0, 6.0)));       // = de

  d = 1;
  c = er_e;
  d = dispersionKirschning(er, Wh_e, fn, c, d);
  a = (0.9408 * pow(c, a) - 0.9603) / ((0.9408 - b) * pow(er_e, a) - 0.9603);
  zl_e *= pow(a, d);


  // dispersive characteristic impedance of odd mode
  d = zl_o;
  c = er_o;
  dispersionKirschning(er, Wh_o, fn, c, d);

  a  = pow((er - 1.0) / 13.0, 12.0);
  a  = 30.0 - 22.2 * a / (1.0 + 3.0 * a);                                // = Q26
  a -= 15.16 / (1.0 + 0.196 * pow(er - 1.0, 2.0));                       // = Q29
  a  = 0.925 * pow(fn / a, 1.536) / (1.0 + 0.3 * pow(fn / 30.0, 1.536)); // = Q22
  zl_o *= pow(er_eff_o / er_o, a);

  a = pow(er - 1.0, 1.5);
  a = 0.4 * pow(g, 0.84) * (1.0 + 2.5 * a / (5.0 + a));                      // = Q27
  a = 1.0 + 0.005 * fn * a
            / ((1.0 + 0.812 * pow(fn/15.0, 1.9)) * (1.0 + 0.025*Wh_o*Wh_o)); // = Q23
  zl_o -= d * a;

  a  = (er - 1.0) * (er - 1.0);
  a  = (0.3 * fn * fn / (10.0 + fn * fn)) * (1.0 + 2.333 * a / (5.0 + a));   // = Q25
  b  = pow(er - 1.0, 3.0);
  b  = 0.149 * b / (94.5 + 0.038 * b);                                       // = Q28
  c  = pow(Wh_o, 0.894);
  b *= 2.506 * c * pow((1.0 + 1.3 * Wh_o) * fn / 99.25, 4.29) / (3.575 + c); // = Q24
  zl_o = d + zl_o / (1.0 + b + pow(0.46 * g, 2.2) * a);
}
