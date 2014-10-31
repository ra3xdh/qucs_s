/*
 * transline.h - base for a transmission line class definition
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

#ifndef __TRANSLINE_H
#define __TRANSLINE_H

class QucsTranscalc;

class transline {
 public:
  transline ();
  virtual ~transline ();

  void   setApplication (QucsTranscalc *);
  void   setProperty (const char *, double);
  void   setProperty (const char *, double, int, int);
  double getProperty (const char *);
  double getProperty (const char *, int, int);
  double convertProperty (const char *, double, int, int);
  void   setResult (int, double, const char *);
  void   setResult (int, const char *);
  int    translateUnit (char *);
  char * getUnit (const char *);
  bool   isSelected (const char *);

  virtual int synthesize () { return -1; };
  virtual void analyze () { };

 protected:
  double f;			/* Frequency of operation */
  double sigma;			/* Conductivity of the metal */
  double mur;			/* mag. permeability */
  double skindepth;		/* Skin depth */
  double skin_depth();

 private:
  QucsTranscalc * app;
};

#endif /* __TRANSLINE_H */
