/*
 * transline.cpp - base for a transmission line implementation
 *
 * Copyright (C) 2005 Stefan Jahn <stefan@lkcc.org>
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

#include "qucstrans.h"
#include "transline.h"
#include "units.h"

// Unit conversion array for length.
static double conv_length[7][7] = {
  { 1.0,       2.54e-3,   2.54e-2,   2.54e-5,   25.4,     1.e-3,   1./12000},
  {1./2.54e-3,    1.0,     10.0,      1.e-2,    1.e4,   1./2.54,   1./30.48},
  {1./2.54e-2,  1./10.,     1.0,       1.e-3,   1.e3,   1./25.4,   1./304.8},
  {1./2.54e-5,    1.e2,    1.e3,         1.0,   1.e6,  1./2.54e-2, 1./0.3048},
  {1./25.4,      1.e-4,   1.e-3,       1.e-6,    1.0,  1./2.54e4,  1./3.048e5},
  {1.e3,          2.54,    25.4,     2.54e-2, 2.54e4,        1.0,    1./12.},
  {1.2e4,        30.48,   304.8,      0.3048, 3.048e5,      12.0,       1.0}
};

// Unit conversion array for frequencies.
static double conv_freq[4][4] = {
  { 1.0,     1.e9,     1.e6,      1.e3},
  { 1.e-9,   1.0,      1.e-3,     1.e-6},
  { 1.e-6,   1.e3,     1.0,       1.e-3},
  { 1.e-3,   1.e6,     1.e3,      1.0}
};

// Unit conversion array for resistances.
static double conv_res[2][2] = {
  {1.0,    1.e-3},
  {1.e3,   1.0}
};

// Unit conversion array for angles.
static double conv_ang[2][2] = {
  {1.0,         pi/180.0},
  {180.0/pi,         1.0}
};

/* Constructor creates a transmission line instance. */
transline::transline () {
  app = 0;
  mur = 1.0;
}

/* Destructor destroys a transmission line instance. */
transline::~transline () {
}

/* Sets the application instance. */
void transline::setApplication (QucsTranscalc * a) {
  app = a;
}

/* Sets a named property to the given value, access through the
   application. */
void transline::setProperty (const char * prop, double value) {
  app->setProperty (prop, value);
}

/* Sets a named property to a given value.  Depending on the source
   and destination unit the value gets previously converted. */
void transline::setProperty (const char * prop, double value, int type,
			     int srcunit) {
  int dstunit = translateUnit (getUnit (prop));
  if (type == UNIT_LENGTH)
    value *= conv_length[srcunit][dstunit];
  else if (type == UNIT_RES)
    value *= conv_res[srcunit][dstunit];
  else if (type == UNIT_ANG)
    value *= conv_ang[srcunit][dstunit];
  else if (type == UNIT_FREQ)
    value *= conv_freq[srcunit][dstunit];
  setProperty (prop, value);
}

/* Converts the given value/unit pair into a text representation and
   puts this into the given result line. */
void transline::setResult (int line, double value, const char * unit) {
  char text[256];
  sprintf (text, "%g %s", value, unit);
  app->setResult (line, text);
}

/* Puts the text into the given result line. */
void transline::setResult (int line, const char * text) {
  app->setResult (line, text);
}

/* Returns a named property value. */
double transline::getProperty (const char * prop) {
  return app->getProperty (prop);
}

/* Returns a named property selection. */
bool transline::isSelected (const char * prop) {
  return app->isSelected (prop);
}

/* Returns a named property value.  Depending on the source and
   destination unit the actual value is converted. */
double transline::getProperty (const char * prop, int type, int dstunit) {
  int srcunit = translateUnit (getUnit (prop));
  double value = getProperty (prop);
  if (type == UNIT_LENGTH)
    return value * conv_length[srcunit][dstunit];
  else if (type == UNIT_RES)
    return value * conv_res[srcunit][dstunit];
  else if (type == UNIT_ANG)
    return value * conv_ang[srcunit][dstunit];
  else if (type == UNIT_FREQ)
    return value * conv_freq[srcunit][dstunit];
  return value;
}

/* The function converts the given value depending on the requested
   unit and its source unit. */
double transline::convertProperty (const char * prop, double value, int type,
				   int srcunit) {
  int dstunit = translateUnit (getUnit (prop));
  if (type == UNIT_LENGTH)
    value *= conv_length[srcunit][dstunit];
  else if (type == UNIT_RES)
    value *= conv_res[srcunit][dstunit];
  else if (type == UNIT_ANG)
    value *= conv_ang[srcunit][dstunit];
  else if (type == UNIT_FREQ)
    value *= conv_freq[srcunit][dstunit];
  return value;
}

/* Returns the unit of the given property. */
char * transline::getUnit (const char * prop) {
  return app->getUnit (prop);
}

/* The function translates the given textual unit into an
   identifier. */
int transline::translateUnit (char * text) {
  if (!strcmp (text, "mil"))       return LENGTH_MIL;
  else if (!strcmp (text, "cm"))   return LENGTH_CM;
  else if (!strcmp (text, "mm"))   return LENGTH_MM;
  else if (!strcmp (text, "m"))    return LENGTH_M;
  else if (!strcmp (text, "um"))   return LENGTH_UM;
  else if (!strcmp (text, "in"))   return LENGTH_IN;
  else if (!strcmp (text, "ft"))   return LENGTH_FT;

  else if (!strcmp (text, "GHz"))  return FREQ_GHZ;
  else if (!strcmp (text, "Hz"))   return FREQ_HZ;
  else if (!strcmp (text, "kHz"))  return FREQ_KHZ;
  else if (!strcmp (text, "MHz"))  return FREQ_MHZ;

  else if (!strcmp (text, "Ohm"))  return RES_OHM;
  else if (!strcmp (text, "kOhm")) return RES_KOHM;

  else if (!strcmp (text, "Deg"))  return ANG_DEG;
  else if (!strcmp (text, "Rad"))  return ANG_RAD;
  return -1;
}

/*
 * skin_depth - calculate skin depth
 */
double transline::skin_depth()
{
  double depth;
  depth = 1.0 / (sqrt(pi * f * mur * MU0 * sigma));
  return depth;
}
