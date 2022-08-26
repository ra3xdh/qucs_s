/*
 * stripline.h - stripline class definition
 *
 * Copyright (C) 2016 Andres Martinez-Mera <andresmartinezmera@gmail.com>
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

#ifndef __STRIPLINE_H
#define __STRIPLINE_H

/*
Reference:
[1] Analysis Methods for RF, Microwave and Milimeter-Wave Planar Transmission Line Structures,
Cam Nguyen. John Wiley and Sons Inc., 2001, Pages 76 - 78
*/

class stripline : public transline {
 public:
  stripline();
  ~stripline();

 private:
  double er;               /* dielectric constant */
  double tand;             /* Dielectric Loss Tangent */
  double t;                /* Thickness */
  double h;                /* Substrate height */
  double W;                /* Width of the conductor strip */
  double l;                /* Length of cable */
  double Z0;               /* characteristic impedance */
  double ang_l;            /* Electrical length in angle */
  double er_eff;           /* Effective dielectric constant */
  double atten_dielectric; /* Loss in dielectric (dB) */
  double atten_cond;       /* Loss in conductors (dB) */
  double lambda_0;         /* Wavelength */

 public:
  void analyze ();
  int synthesize ();

 private:
  void get_stripline_sub();
  void get_stripline_comp();
  void get_stripline_phys();
  void get_stripline_elec();
  void fixdin();
  void fixdout();
  double alphad_stripline();
  double alphac_stripline();
  void show_results();
  void getStriplineLength();
  void calculateZ0();
  double getZ0fromWidth(double);
};

#endif /* __stripline_H */
