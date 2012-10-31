/***************************************************************************
                               qf_comp.cpp
                              -------------
    begin                : Fri May 1 2009
    copyright            : (C) 2009 by Vincent Habchi, F5RCS
    email                : 10.50@free.fr
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
#include <config.h>
#endif

#include "qf_common.h"
#include "qf_comp.h"
//Added by qt3to4:
#include <Q3TextStream>

// DUMP functions and utilities
static QString valstr (qf_double_t num) {

  char	      c = 0;

  num = fabs (num);

  if ((num > 1e-18) && (num < 1e12)) {
    qf_double_t cal = floor (log10 (num) / 3);
    int		expo = static_cast <int> (cal);

    switch (expo) {
      case -6: c = 'a'; break;
      case -5: c = 'f'; break;
      case -4: c = 'p'; break;
      case -3: c = 'n'; break;
      case -2: c = 'u'; break;
      case -1: c = 'm'; break;
      case  0: break;
      case  1: c = 'k'; break;
      case  2: c = 'M'; break;
      case  3: c = 'G'; break;
      case  4: c = 'T'; break;
      }

    if (c != 0) num /= pow (10.0, 3 * cal);
  }

  QString buffer;
  Q3TextStream str (& buffer, QIODevice::WriteOnly);

  str. precision (3);

  str << num;
  if (c) 
    str << c;

  return buffer;
}

void qf_cmp1::dump (unsigned n1, unsigned n2,
		    Q3TextStream& com, Q3TextStream& wir) {

  // type of component
  if (name == "CAP")
    com << "<C C1";
  else if (name == "IND")
    com << "<L L1";
  else if (name == "RES")
    com << "<R R1";

  // single component to ground
  if (n2 == 0) {
    // wire
    wir << "<" << x << " " << y-30 << " " << x << " " << y-80
	<< " \"\" 0 0 0>\n";
    // position
    com << " 1 " << x << " " << y;
    com << " 17 -26 0 1";
    // value of component
    com << " \"" << valstr (val) << unit;
    com << "\" 1>\n";
    // place ground symbol here
    com << "<GND * 1 " << x << " " << y+30 << " 0 0 0 0>\n";
  }
  // single component in series
  else {
    // wires
    wir << "<" << x << " " << y-80 << " " << x+40 << " " << y-80
	<< " \"\" 0 0 0>\n";
    wir << "<" << x+100 << " " << y-80 << " " << x+140 << " " << y-80
	<< " \"\" 0 0 0>\n";
    // position
    com << " 1 " << x+70 << " " << y-80;
    com << " -26 10 0 0";
    // value of commponent
    com << " \"" << valstr (val) << unit;
    com << "\" 1>\n";
    x += 140;
  }
}

void qf_cmplc::dump (unsigned n1, unsigned n2,
		     Q3TextStream& com, Q3TextStream& wir) {

  if (name == "PLC") {
    // parallel L and C to ground
    if (n2 == 0) {
      // wire
      wir << "<" << x << " " << y-30 << " " << x << " " << y-80
	  << " \"\" 0 0 0>\n";
      // inductor
      com << "<L L1";
      // position
      com << " 1 " << x << " " << y;
      com << " 17 -26 0 1";
      // value of component
      com << " \"" << valstr (vL) << "H";
      com << "\" 1>\n";
      // place ground symbol here
      com << "<GND * 1 " << x << " " << y+30 << " 0 0 0 0>\n";

      // connecting above wire
      wir << "<" << x << " " << y-80 << " " << x+110 << " " << y-80
	  << " \"\" 0 0 0>\n";

      x += 110;

      // wire
      wir << "<" << x << " " << y-30 << " " << x << " " << y-80
	  << " \"\" 0 0 0>\n";
      // capacitor
      com << "<C C1";
      // position
      com << " 1 " << x << " " << y;
      com << " 17 -26 0 1";
      // value of component
      com << " \"" << valstr (vC) << "F";
      com << "\" 1>\n";
      // place ground symbol here
      com << "<GND * 1 " << x << " " << y+30 << " 0 0 0 0>\n";
    }
    // parallel L and C in series
    else {
      // wires
      wir << "<" << x << " " << y-80 << " " << x+40 << " " << y-80
	  << " \"\" 0 0 0>\n";
      wir << "<" << x+100 << " " << y-80 << " " << x+140 << " " << y-80
	  << " \"\" 0 0 0>\n";
      // inductor
      com << "<L L1";
      // position
      com << " 1 " << x+70 << " " << y-80;
      com << " -26 10 0 0";
      // value of commponent
      com << " \"" << valstr (vL) << "H";
      com << "\" 1>\n";

      // wires
      wir << "<" << x+40 << " " << y-80 << " " << x+40 << " " << y-120
	  << " \"\" 0 0 0>\n";
      wir << "<" << x+100 << " " << y-80 << " " << x+100 << " " << y-120
	  << " \"\" 0 0 0>\n";
      // capacitor
      com << "<C C1";
      // position
      com << " 1 " << x+70 << " " << y-120;
      com << " -26 -48 0 0";
      // value of commponent
      com << " \"" << valstr (vC) << "F";
      com << "\" 1>\n";

      x += 140;
    }
  }
  else if (name == "SLC") {
    // series L and C to ground
    if (n2 == 0) {
      // wire
      wir << "<" << x << " " << y-30 << " " << x << " " << y-80
	  << " \"\" 0 0 0>\n";
      // inductor
      com << "<L L1";
      // position
      com << " 1 " << x << " " << y;
      com << " 17 -26 0 1";
      // value of component
      com << " \"" << valstr (vL) << "H";
      com << "\" 1>\n";

      // capacitor
      com << "<C C1";
      // position
      com << " 1 " << x << " " << y+60;
      com << " 17 -26 0 1";
      // value of component
      com << " \"" << valstr (vC) << "F";
      com << "\" 1>\n";
      // place ground symbol here
      com << "<GND * 1 " << x << " " << y+90 << " 0 0 0 0>\n";
    }
    // series L and C in series
    else {
      // wires
      wir << "<" << x << " " << y-80 << " " << x+40 << " " << y-80
	  << " \"\" 0 0 0>\n";
      wir << "<" << x+100 << " " << y-80 << " " << x+120 << " " << y-80
	  << " \"\" 0 0 0>\n";
      wir << "<" << x+180 << " " << y-80 << " " << x+220 << " " << y-80
	  << " \"\" 0 0 0>\n";
      // inductor
      com << "<L L1";
      // position
      com << " 1 " << x+70 << " " << y-80;
      com << " -26 10 0 0";
      // value of commponent
      com << " \"" << valstr (vL) << "H";
      com << "\" 1>\n";

      // capacitor
      com << "<C C1";
      // position
      com << " 1 " << x+150 << " " << y-80;
      com << " -26 10 0 0";
      // value of commponent
      com << " \"" << valstr (vC) << "F";
      com << "\" 1>\n";

      x += 220;
    }
  }
}

void  qf_pslc::dump (unsigned n1, unsigned n2,
		     Q3TextStream& s, Q3TextStream& wir) {
  qf_slc (vC, vL, shnt, gnd). dump (n1, n2, s, wir);
  qf_slc (vC2, vL2, shnt, gnd). dump (n1, n2, s, wir);
}

// Dumps the whole filter structure
int qf_lcmp::dump_node (int& p, list <qf_cmp*>::iterator& i,
			  Q3TextStream& com, Q3TextStream& wir) {

  int n = p + 1;
  int g = 0;

  if ((*i) -> name == "END") return -n;		  // End of structure
  while (i != lst. end ()) {

    if ((*i) == NULL) return 0;
    if ((*i) -> name == "END") return -n;	  // End of structure

    if ((*i) -> gnd) {
      if (g) {
	// connecting wire for consecutive branches to ground
	wir << "<" << x << " " << y-80 << " " << x+110 << " " << y-80
	    << " \"\" 0 0 0>\n";
	x += 110;
      }
      (*i) -> x = x;
      (*i) -> y = y;
      (*i) -> dump (p, 0, com, wir);
      x = (*i) -> x;
      y = (*i) -> y;
      g = 1;
      i ++;
      continue;
    }

    g = 0;
    (*i) -> x = x;
    (*i) -> y = y;
    (*i) -> dump (p, n, com, wir);
    x = (*i) -> x;
    y = (*i) -> y;
    if ((*i) -> shnt) {
      i ++;
      n = dump_node (n, i, com, wir);
    }
    else {
      i ++;
      return n;
    }
  } 
  return -n;
}

void qf_lcmp::dump_all (Q3TextStream& out) {

  if (lst.size () == 0) {
    return;
  }

  int node = 1;
  QByteArray  buf1, buf2;
  Q3TextStream com (buf1, QIODevice::ReadWrite);
  Q3TextStream wir (buf2, QIODevice::ReadWrite);

  x = 0;
  y = 0;

  // schematic start
  out << "<Qucs Schematic " << PACKAGE_VERSION << ">\n";

  // input port and according ground symbol
  com << "<Pac P1 1 " << x << " " << y << " 18 -26 0 1 \"1\" 1 \""
      << r1 << " Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n";
  com << "<GND * 1 " << x << " " << y+30 << " 0 0 0 0>\n";
  wir << "<" << x << " " << y-30 << " " << x << " " << y-80
      << " \"\" 0 0 0>\n";
  wir << "<" << x << " " << y-80 << " " << x+110 << " " << y-80
      << " \"\" 0 0 0>\n";

  x += 110;
  for (list <qf_cmp*>::iterator i = lst. begin (); i != lst. end ();) 
    if ((node = dump_node (node, i, com, wir)) < 0) break;

  // output port and according ground symbol
  x += 110;
  wir << "<" << x << " " << y-30 << " " << x << " " << y-80
      << " \"\" 0 0 0>\n";
  wir << "<" << x-110 << " " << y-80 << " " << x << " " << y-80
      << " \"\" 0 0 0>\n";
  com << "<Pac P2 1 "<< x << " " << y << " 18 -26 0 1 \"2\" 1 \""
      << r2 << " Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n"
      << "<GND * 1 " << x << " " << y+30 << " 0 0 0 0>\n";

  x = 0;

  // S-parameter simulation box and equation
  com << "<.SP SP1 1 " << x << " " << y-280 << " 0 50 0 0 \"log\" 1 \""
      << valstr(fc/2/M_PI/10) << "Hz\" 1 \"" << valstr(fc/2/M_PI*10)
      << "Hz\" 1 \"200\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n";
  com << "<Eqn Eqn1 1 " << x+200 << " " << y-270
      << " -28 15 0 0 \"dBS21=dB(S[2,1])\" 1 "
      << "\"dBS11=dB(S[1,1])\" 1 \"yes\" 0>\n";

  // components
  out << "<Components>\n";
  out << QString(buf1);
  out << "</Components>\n";
  // wires
  out << "<Wires>\n";
  out << QString(buf2);
  out << "</Wires>\n";
  // paintings
  out << "<Paintings>\n";
  out << "<Text " << x+310 << " " << y-280 << " 12 #000000 0 \""
      << tx << "\\n"
      << valstr(fc/2/M_PI) << "Hz cutoff/center frequency\\n"
      << "impedance matching Z_{IN}=" << valstr(r1) << "\\Omega, Z_{OUT}="
      << valstr(r2) << "\\Omega\">\n";
  out << "</Paintings>\n";
}

// Access to the list of components

void	qf_lcmp::init (void) {

  iter = lst. begin ();
}

qf_cmp*	qf_lcmp::next (void) {

  qf_cmp* cmp = * iter;

  if ((cmp == NULL) || (typeid (* cmp) == typeid (qf_end))) return NULL;
  iter ++;
  return cmp;
}
