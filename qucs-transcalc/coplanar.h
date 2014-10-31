/*
 * coplanar.h - microstrip class definition
 *
 * Copyright (C) 2008 Michael Margraf <michael.margraf@alumni.tu-berlin.de>
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

#ifndef __COPLANAR_H
#define __COPLANAR_H

class coplanar : public transline {
 public:
  coplanar();

 private:
  double er;			/* dielectric constant */
  double h;			/* height of substrate */
  double t;			/* thickness of top metal */
  double tand;			/* Dielectric Loss Tangent */
  double w;			/* width of line */
  double s;			/* width of gap between line and ground */
  double len;			/* length of line */
  double Z0;			/* characteristic impedance */
  double er_eff;		/* effective dielectric constant */
  double ang_l;			/* Electrical length in angle */
  double atten_dielectric;	/* Loss in dielectric (dB) */
  double atten_cond;		/* Loss in conductors (dB) */

 public:
  void analyze();
  int synthesize();

 protected:
  bool backMetal;

 private:
  void calc();
  void show_results();
  void getProperties();

  void ellipke(double, double&, double&);
  double ellipk(double);
};


class groundedCoplanar : public coplanar {
 public:
  groundedCoplanar();
};

#endif /* __COPLANAR_H */
