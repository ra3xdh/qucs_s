/*
 * rectwaveguide.cpp - rectangular waveguide class implementation
 *
 * Copyright (C) 2001 Gopal Narayanan <gopal@astro.umass.edu>
 * Copyright (C) 2005, 2006 Stefan Jahn <stefan@lkcc.org>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.  
 *
 */

#include <stdio.h>
#include <string.h>
#include <cmath>

#include "units.h"
#include "transline.h"
#include "rectwaveguide.h"

rectwaveguide::rectwaveguide() : transline()
{
}

rectwaveguide::~rectwaveguide()
{
}

/*
 * returns k
 */
double rectwaveguide::kval ()
{
  double kval;
  kval = 2.0 * pi * f * sqrt (mur * er) / C0;
  return kval;
}
 
/*
 * given mode numbers m and n 
 * returns cutoff kc value
 */
double rectwaveguide::kc (int m, int n)
{
  double kcval;
  kcval = sqrt (pow ((m * pi / a), 2.0) + pow ((n * pi / b), 2.0));
  return kcval;
}

/*
 * given mode numbers m and n 
 * returns cutoff fc value
 */
double rectwaveguide::fc (int m, int n)
{
  double fcval;
  fcval =  kc (m, n) * C0 / (2.0 * pi * sqrt (mur * er));
  return fcval;
}

/*
 * alphac - returns attenuation due to conductor losses for all propagating
 * modes in the waveguide
 */
double rectwaveguide::alphac ()
{
  double Rs, k, f_c;
  double ac;
  short m, n, mmax, nmax;

  Rs = sqrt ((pi * f * mur * MU0) / sigma);
  k = kval ();
  ac = 0.0;
  mmax = (int) floor (f / fc (1,0));
  nmax = mmax;

  /* below from Ramo, Whinnery & Van Duzer */

  /* TE(m,n) modes */
  for (n = 0; n<= nmax; n++){
    for (m = 1; m <= mmax; m++){
      f_c = fc(m, n);
      if (f > f_c) {
	switch (n) {
	case 0:
	  ac += (Rs/(b * ZF0 * sqrt(1.0 - pow((f_c/f),2.0)))) *
	    (1.0 + ((2 * b/a)*pow((f_c/f),2.0)));
	  break;
	default:
	  ac += ((2. * Rs)/(b * ZF0 * sqrt(1.0 - pow((f_c/f),2.0)))) *
	    (((1. + (b/a))*pow((f_c/f),2.0)) + 
	     ((1. - pow((f_c/f),2.0)) * (((b/a)*(((b/a)*pow(m,2.)) + pow(n,2.)))/
					(pow((b*m/a),2.0) + pow(n,2.0)))));
	  break;
	}
      }
    }
  }

  /* TM(m,n) modes */
  for (n = 1; n<= nmax; n++) {
    for (m = 1; m<= mmax; m++) {
      f_c = fc(m, n);
      if (f > f_c) {
	ac += ((2. * Rs)/(b * ZF0 * sqrt(1.0 - pow((f_c/f),2.0)))) *
	  (((pow(m,2.0)*pow((b/a),3.0)) + pow(n,2.))/
	   ((pow((m*b/a),2.)) + pow(n,2.0)));
      }
    }
  }
  
  ac = ac * 20.0 * log10 (exp (1.0)); /* convert from Np/m to db/m */
  return ac;
}

/*
 * alphac_cutoff - returns attenuation for a cutoff wg
 */
double rectwaveguide::alphac_cutoff ()
{
  double acc;
  acc = sqrt (pow (kc(1,0), 2.0) - pow (kval (), 2.0));
  acc = 20 * log10 (exp (1.0)) * acc;
  return acc;
}

/*
 * returns attenuation due to dielectric losses
 */
double rectwaveguide::alphad()
{
  double k, beta;
  double ad;

  k = kval ();
  beta = sqrt (pow (k, 2.0) - pow (kc (1,0), 2.0));  
  
  ad = (pow (k, 2.0) * tand) / (2.0 * beta);
  ad = ad * 20.0 * log10 (exp (1.0)); /* convert from Np/m to db/m */
  return ad;
}
  
/*
 * get_rectwaveguide_sub
 * get and assign rectwaveguide substrate parameters
 * into rectwaveguide structure
 */
void rectwaveguide::get_rectwaveguide_sub ()
{
  er = getProperty ("Er");
  mur = getProperty ("Mur");
  sigma = getProperty ("Cond");
  tand = getProperty ("Tand");
  tanm = getProperty ("TanM");
}

/*
 * get_rectwaveguide_comp
 * get and assign rectwaveguide component parameters
 * into rectwaveguide structure
 */
void rectwaveguide::get_rectwaveguide_comp ()
{
  f = getProperty ("Freq", UNIT_FREQ, FREQ_HZ);
}

/*
 * get_rectwaveguide_elec
 * get and assign rectwaveguide electrical parameters
 * into rectwaveguide structure
 */
void rectwaveguide::get_rectwaveguide_elec ()
{
  Z0 = getProperty ("Z0", UNIT_RES, RES_OHM);
  ang_l = getProperty ("Ang_l", UNIT_ANG, ANG_RAD);
}

/*
 * get_rectwaveguide_phys
 * get and assign rectwaveguide physical parameters
 * into rectwaveguide structure
 */
void rectwaveguide::get_rectwaveguide_phys ()
{
  a = getProperty ("a", UNIT_LENGTH, LENGTH_M);
  b = getProperty ("b", UNIT_LENGTH, LENGTH_M);
  l = getProperty ("L", UNIT_LENGTH, LENGTH_M);
}

/*
 * analyze - analysis function
 */
void rectwaveguide::analyze ()
{
  double lambda_g;
  double k;
  double beta;

  /* Get and assign substrate parameters */
  get_rectwaveguide_sub();

  /* Get and assign component parameters */
  get_rectwaveguide_comp();
      
  /* Get and assign physical parameters */
  get_rectwaveguide_phys();

  k = kval ();
      
  if (kc (1,0) <= k) {
    /* propagating modes */
    beta = sqrt (pow (k, 2.0) - pow (kc (1,0), 2.0));
    lambda_g = 2.0 * pi / beta;
    /* Z0 = (k * ZF0) / beta; */
    Z0 = k * ZF0 * sqrt(mur/er) / beta;

    /* calculate electrical angle */
    lambda_g = 2.0 * pi / beta;
    ang_l = 2.0 * pi * l / lambda_g;    /* in radians */
    atten_cond = alphac () * l;
    atten_dielectric = alphad () * l;
    er_eff = (1.0 - pow ((fc (1,0) / f), 2.0));
  } else { 
    /* evanascent modes */	
    Z0 = 0;
    ang_l = 0;
    er_eff = 0;
    atten_dielectric = 0.0;
    atten_cond = alphac_cutoff () * l;
  }

  setProperty ("Z0", Z0, UNIT_RES, RES_OHM);
  setProperty ("Ang_l", ang_l, UNIT_ANG, ANG_RAD);

  show_results ();
}

/*
 * synthesize - synthesis function
 */
int rectwaveguide::synthesize ()
{
  double lambda_g, k, beta;

  /* Get and assign substrate parameters */
  get_rectwaveguide_sub();

  /* Get and assign component parameters */
  get_rectwaveguide_comp();
      
  /* Get and assign electrical parameters */
  get_rectwaveguide_elec();

  /* Get and assign physical parameters */
  get_rectwaveguide_phys();


  if (isSelected ("b")) {
    /* solve for b */
    b = Z0 * a * sqrt(1.0 - pow((fc(1,0)/f),2.0))/
      (2. * ZF0);
    setProperty ("b", b, UNIT_LENGTH, LENGTH_M);
  } else if (isSelected ("a")) {
    /* solve for a */
    a = sqrt(pow((2.0 * ZF0 * b/Z0), 2.0) + 
		 pow((C0/(2.0 * f)),2.0));
    setProperty ("a", a, UNIT_LENGTH, LENGTH_M);
  }

  k = kval ();
  beta = sqrt(pow(k,2.) - pow(kc(1,0),2.0));
  lambda_g = (2. * pi)/beta;
  l = (ang_l * lambda_g)/(2.0 * pi);    /* in m */

  setProperty ("L", l, UNIT_LENGTH, LENGTH_M);

  if (kc(1,0) <= k) {
    /*propagating modes */
    beta = sqrt(pow(k,2.) - pow(kc(1,0),2.0));
    lambda_g = (2. * pi)/beta;
    atten_cond = alphac () * l;
    atten_dielectric = alphad () * l;
    er_eff = (1.0 - pow((fc(1,0)/f),2.0));
  } else { 
    /*evanascent modes */	
    Z0 = 0;
    ang_l = 0;
    er_eff = 0;
    atten_dielectric = 0.0;
    atten_cond = alphac_cutoff () * l;
  }

  show_results ();

  return 0;
}

void rectwaveguide::show_results ()
{
  short m, n, mmax, nmax;
  
  setResult (0, er_eff, "");
  setResult (1, atten_cond, "dB");
  setResult (2, atten_dielectric, "dB");

  setResult (3, "none");
  if (f >= (2.*fc(1,0))) {
    char text[256], txt[256];
    strcpy (text, "");
    /* multiple modes possible in waveguide */
    /*	  mmax = floor(f/fc(1,0));*/
    mmax = 5;
    nmax = mmax;
    for (m = 2; m<= mmax; m++) {
      for (n=0; n<= nmax; n++) {
	if (f >= (fc(m,n))){
	  sprintf(txt,"TE(%u,%u) ",m, n);
	  strcat(text,txt);
	}
      }
    }
    setResult (3, text);
  }

  setResult (4, "none");
  if (f >= fc(1,1)){ /*TM(1,1) mode possible*/
    char text[256], txt[256];
    strcpy (text, "");
    /*	  mmax = floor(f/fc(1,1));*/
    mmax = 5;
    nmax = mmax;
    for (m = 1; m<= mmax; m++) {
      for (n=1; n<= nmax; n++) {
	if (f >= (fc(m,n))){
	  sprintf(txt,"TM(%u,%u) ",m, n);
	  strcat(text,txt);
	}
      }
    }
    setResult (3, text);
  }
}
