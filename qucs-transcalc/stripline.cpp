/*
 * stripline.cpp - stripline class implementation
 * 
 * Copyright (C) 2016 Andres Martinez-Mera <andresmartinezmera@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.  
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cmath>

#include "units.h"
#include "transline.h"
#include "stripline.h"

stripline::stripline() : transline()
{
}

stripline::~stripline()
{
}

/*
 * get_stripline_sub() - get and assign stripline substrate parameters into stripline
 * structure
 */
void stripline::get_stripline_sub ()
{
  er = getProperty ("Er");
  mur = getProperty ("Mur");
  tand = getProperty ("Tand");
  sigma = getProperty ("Sigma");
  t = getProperty ("T", UNIT_LENGTH, LENGTH_M);
  h = getProperty ("h", UNIT_LENGTH, LENGTH_M);
}

/*
 * get_stripline_comp() - get and assign stripline component parameters into
 * stripline structure
 */
void stripline::get_stripline_comp ()
{
  f = getProperty ("Freq", UNIT_FREQ, FREQ_HZ);
  lambda_0 = C0/f;
}

/*
 * get_stripline_elec() - get and assign stripline electrical parameters into
 * stripline structure
 */
void stripline::get_stripline_elec ()
{
  Z0 = getProperty ("Z0", UNIT_RES, RES_OHM);
  ang_l = getProperty ("Ang_l", UNIT_ANG, ANG_RAD);
}

/*
 * get_stripline_phys() - get and assign stripline physical parameters into stripline
 * structure
 */
void stripline::get_stripline_phys ()
{
  W = getProperty ("W", UNIT_LENGTH, LENGTH_M);
  l = getProperty ("L", UNIT_LENGTH, LENGTH_M);
}

/* This function calculates the dielectric attenuation coefficient*/
double stripline::alphad_stripline ()
{
  return 27.3*sqrt(er)*tand/lambda_0;
}

/* This function calculates the conductor attenuation coefficient*/
double stripline::alphac_stripline ()
{
  double w = 2*pi*f, alpha_c;
  double mu = mur*pi*4e-7;
  double Rs = sqrt(w*mu/(2*sigma));//Skin effect resistance
  skindepth = sqrt(2/(w*mu*sigma));
  double A = 1 + 2*W/(2*h-t) + (1/pi)*((2*h+t)/(2*h-t))*log((4*h-t)/t);
  double B = 1 + (2*h/(0.5*W+0.7*t))*(0.5 + 0.414*t/W + (1/(2*pi))*log(4*pi*W/t));

  (Z0 < 120/sqrt(er)) ?
    alpha_c = (23.4e-3*Rs*er*Z0*A)/(30*pi*(2*h-t)) :
    alpha_c = 1.4*Rs*B/(Z0*2*h);

  return alpha_c;
}

/*
 * analyze() - Reads both the substrate and the physical properties of the line and calculates Z0 and the electrical length
 */
void stripline::analyze ()
{
  /* Get and assign substrate parameters */
  get_stripline_sub();

  /* Get and assign component parameters */
  get_stripline_comp();
      
  /* Get and assign physical parameters */
  get_stripline_phys();

  calculateZ0(); // Z0
  getStriplineLength();// Line length
  alphac_stripline();// alpha_c and skin depth
  alphad_stripline();// alpha_d
  setProperty ("Z0", Z0, UNIT_RES, RES_OHM);
  setProperty ("Ang_l", ang_l, UNIT_ANG, ANG_RAD);
  show_results();
}


// This function calculates the characteristic impedance of a symmetric stripline according to [1], eq. 4.80-4.84
double stripline::getZ0fromWidth(double W_)
{
  double x = t/(2.*h);
  double m= 2/(1 + 2*x/3*(1-x));
  double B = (x/(pi*(1-x)))*(1 - 0.5*log(pow(x/(2.-x),2) + pow((0.0796*x)/((W_/(2.*h)) + 1.1*x),m)));
  double A = 1/((W_/(2*h - t)) + B);
  // Line impedance
  return (30/sqrt(er))*log(1 + (4/pi)*A*((8/pi)*A + sqrt(pow((8/pi)*A,2) + 6.27)));
}

/* This function calculates the static line impedance*/
void stripline::calculateZ0()
{
  Z0 = getZ0fromWidth(W);
}

void stripline::getStriplineLength()
{
  double lambda_g = (C0/(f))/sqrt(er * mur);
  /* calculate electrical angle */
  ang_l = (2.0 * pi * l)/lambda_g;    /* in radians */
}

/*
 * 	
 */
int stripline::synthesize ()
{
  get_stripline_sub();//Substrate
  get_stripline_elec();//Z0 and electrical length
  get_stripline_phys();//Line size
  alphac_stripline();// alpha_c and skin depth
  alphad_stripline();// alpha_d


  // Zero-thickness approximation
  double B = exp(Z0*sqrt(er)/30)-1;
  double C = sqrt(4*B+6.27);
  double A = 2*B/C;
  double A1 = 8/(pi*A);

  double x = t/(2*h);
  double m= 2/(1 + 2*x/3*(1-x));
  double We = (2*h - t)*A1;
  double A2 = (x/(pi*(1-x))) * (1 - 0.5*log( (x*x/(4-2*x+x*x)) + pow((0.0796*x/( (We/(2*h)) + 1.1*x)),m) ));

  double Wi = (A1 - A2)*(2*h-t);//Width given by the zero-thickness approximation (Initial guess for the refinement)
 
  double Zi;
  double dW = Wi*1e-4;//Differential width
  unsigned int MAX_ITER = 100, iter = 0;
  double MAX_ERR = 1e-5;
  double Zi_1 = 0, Zi_diff, step;


  // The Newton-Raphson method is employed to refine the width given by the zero-thickness approximation with the analysis formulae.
  while ((std::abs(Z0 - Zi_1) > MAX_ERR)&&(iter < MAX_ITER))//Stop condition: |Z0 - Z_i| < MAX_ERR or max. number iterations exceeded
  {
    Zi = getZ0fromWidth(Wi+dW);
    Zi_diff = (Zi - Zi_1) / dW;
    step = (Z0 - Zi_1) / Zi_diff - dW;
    Wi += step+dW;
    if (Wi <= 0.0) Wi = dW;
    Zi_1 = getZ0fromWidth(Wi);
    iter++;
  }

  setProperty ("W", Wi, UNIT_LENGTH, LENGTH_M);
  double lambda_g = lambda_0/sqrt(er * mur);
  /* calculate physical length */
  l = (lambda_g * ang_l)/(2.0 * pi);    /* in m */
  setProperty ("L", l, UNIT_LENGTH, LENGTH_M);

  show_results();
  return 0;
}

/*
 * show_results() - show results
 */
void stripline::show_results()
{
  atten_dielectric = alphad_stripline () * l;
  atten_cond = alphac_stripline () * l;

  setResult (0, atten_cond, "dB");
  setResult (1, atten_dielectric, "dB");
  double val = convertProperty ("T", skindepth, UNIT_LENGTH, LENGTH_M);
  setResult (2, val, getUnit ("T"));//Skin depth
}
