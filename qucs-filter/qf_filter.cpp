/***************************************************************************
                               qf_filter.cpp
                             -----------------
    begin                : Mon Jan 02 2006
    copyright            : (C) 2006 by Vincent Habchi, F5RCS
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
# include <config.h>
#endif

#include <math.h>
#include <iostream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <string.h>

#include "qf_poly.h"
#include "qf_filter.h"

// Constructor of a filter of nth order

// Just initialize a few things
qf_filter::qf_filter (int n) :
  type (LOWPASS), kind (UNDEF), ord (n), fc (0), bw (0), imp (1),
  ncomp (0), Comp (NULL) {
}

// Default constructor
qf_filter::qf_filter () :
  type (LOWPASS), kind (UNDEF), ord (0), fc (0), bw (0), imp (1),
  ncomp (0), Comp (NULL) {
}

qf_filter::qf_filter (int n, qfk k, qft t) :
  type (t), kind (k), ord (n), fc (0), bw (0), imp (1),
  ncomp (0), Comp (NULL) {
}

qf_filter::qf_filter (qfk k, qft t, qf_double_t r,
		      qf_double_t f = 1, qf_double_t b = 0) :
  type (t), kind (k), fc (f), bw (b), imp (r), ncomp (0), Comp (NULL) {
  fstart = fc - bw / 2;
  fstop  = fc + bw / 2;
}

qf_filter::qf_filter (qfk k, qft t) :
  type (t), kind (k), ord (0), fc (0), bw (0), imp (1),
  ncomp (0), Comp (NULL) {
}

// Destructor of a filter
qf_filter::~qf_filter (void) {
  if (Comp != NULL)
    free (Comp);
}

// Extraction routines

// Extract finite attenuation pole
// Result is a parallel cap, and a serial resonator (L // C)
void qf_filter::extract_pole_pCsLC (qf_double_t p, qfc * pComp,
				    qf_double_t Ws) {
  BN.disp ("BN");
  BD.disp ("BD");

  qf_double_t pl = -p * p;
  qf_double_t bdpl = BD.evalX2 (pl);

  // Partial removal of infinite pole (first // cap)
  // c = [B(s)/s] (s^2 = - O^2)
  qf_double_t c = ((BN << 1).evalX2 (pl)) / bdpl;
  pComp->val = c * Ws;
  qf_poly cS (c, 0, 0, 1);
  BN = BN - (BD * cS);		// B = B - cs
  BN.disp ("BN");
  BN.div (0, p);

  // Full removal of finite pole
  // c1 = (s B(s) / (s^2 + O^2)) @ s^2 = - O^2
  BN.disp ("BN");
  qf_double_t c1 = (BN >> 1).evalX2 (pl) / bdpl;
  (pComp + 1)->val = c1;
  (pComp + 2)->val = -Ws / (c1 * pl);
  (pComp + 1)->val *= Ws;

  // 1/B = 1/B - (s/c1) / (s^2 + O^2)
  BD = BD - (BN >> 1) * (1 / c1);
  BD.div (0, p);

  BN.disp ("BN");
  BD.disp ("BD");
}

// User readable value string.
std::string qf_filter::num2str (qf_double_t num) {
  char c = 0;
  qf_double_t cal = fabs(num);
  if(cal > 1e-20) {
    cal = log10(cal) / 3.0;
    if(cal < -0.2)  cal -= 0.98;
    int expo = int(cal);

    if(expo >= -5) if(expo <= 4)
      switch(expo) {
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

    if(c) num /= pow(10.0, (qf_double_t)(3*expo));
  }

  std::stringstream str;
  str << num;
  if(c) str << c;
  return str.str();
}

// Dumps a filter to std::cout
void qf_filter::dump_cout (void) {
  std::string unit;

  for (unsigned i = 0; i < ncomp; i++) {
    switch (Comp[i].comp) {
    case CAP:
      std::cout << "Cap. from node ";
      unit = "F";
      break;
    case IND:
      std::cout << "Ind. from node ";
      unit = "H";
      break;
    case RES:
      std::cout << "Res. from node ";
      unit = "Ohm";
      break;
    }

    std::cout << Comp[i].node1 << " to ";

    if (Comp[i].node2 == 0)
      std::cout << "ground, value = ";
    else
      std::cout << "node " << Comp[i].node2 << ", value = ";

    if (Comp[i].val > 1)
      std::cout << Comp[i].val << " " << unit << '\n';
    else if (Comp[i].val >= 1e-3)
      std::cout << Comp[i].val * 1e3 << " m" << unit << '\n';
    else if (Comp[i].val >= 1e-6)
      std::cout << Comp[i].val * 1e6 << " u" << unit << '\n';
    else if (Comp[i].val >= 1e-9)
      std::cout << Comp[i].val * 1e9 << " n" << unit << '\n';
    else if (Comp[i].val >= 1e-12)
      std::cout << Comp[i].val * 1e12 << " p" << unit << '\n';
    else if (Comp[i].val >= 1e-15)
      std::cout << Comp[i].val * 1e15 << " f" << unit << '\n';
    else
      std::cout << "0 " << unit << '\n';
  }
}

std::string qf_filter::to_spice (void) {
  std::ostringstream res;
  unsigned i = 0;

  res << "* SPICE netlist\n";
  switch(type) {
    case LOWPASS:
      res << "* low-pass filter " << fc << " Hz cutoff\n";
      break;
    case HIGHPASS:
      res << "* high-pass filter " << fc << " Hz cutoff\n";
      break;
    case BANDPASS:
      res << "* band-pass filter "
	  << fstart << " Hz ... " << fstop << " Hz\n";
      break;
    case BANDSTOP:
      res << "* band-reject filter "
	  << fstart << " Hz ... " << fstop << " Hz\n";
      break;
  }
  res << "* PI-type, impedance matching " << imp << " Ohm\n";

  for (unsigned ic = 1, il = 1, ir = 1; i < ncomp; i++) {
    switch (Comp[i].comp) {
    case CAP:
      res << "C" << ic << "\t";
      res << Comp[i].node1 << "\t" << Comp[i].node2;
      res << "\t" << Comp[i].val << '\n';
      // Insert parallel resistor
      res << "R" << ir << "\t";
      res << Comp[i].node1 << "\t" << Comp[i].node2;
      res << "\t10G\n";
      ic++;
      ir++;
      break;
    case IND:
      res << "L" << il << "\t";
      res << Comp[i].node1 << "\t"
	<< Comp[i].node1 * 100 + Comp[i].node2 << "\t";
      res << Comp[i].val << "\n";
      il++;
      // Insert serial resistor
      res << "R" << ir << "\t";
      res << Comp[i].node1 * 100 + Comp[i].node2
	<< "\t" << Comp[i].node2 << "\t";
      res << "1u\n";
      ir++;
      break;
    case RES:
      res << "R" << ir << "\t";
      ir++;
      break;
    }
  }

  res << "RI\t10000\t1\t" << imp << "\n";
  res << "RL\t";

  if (Comp[i - 1].node2 == 0)
    res << Comp[i - 1].node1;
  else
    res << Comp[i - 1].node2;

  res << "\t0\t" << imp << "\n";
  res << "VCC\t10000\t0\tDC\t1\tAC\t1\n";
  res << ".END\n";
  return res.str();
}

void qf_filter::dump_spice (void) {
  std::cout << to_spice().c_str();
}

std::string qf_filter::to_qucs (void) {
  std::ostringstream res;
  std::ostringstream wir;
  qf_double_t Value, Value2;

  // create the Qucs schematic
  res << "<Qucs Schematic " << PACKAGE_VERSION << ">\n";

  int x2, x = 20 + 40;
  res << "<Components>\n";
  res << "<Pac P1 1 " << x << " 320 18 -26 0 1 \"1\" 1 \""
      << imp << " Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n";
  res << "<GND * 1 " << x << " 350 0 0 0 0>\n";

  int yc = 320, yl = 240;
  unsigned int i, j = 0, k = ord, repser = 0, gndser = 1;

  // connect left source
  x = 20 + 40;
  x2 = x + 110;
  if(type == HIGHPASS) x2 -= 30;
  wir << "<" << x << " 240 " << x << " 290 \"\" 0 0 0>\n"
      << "<" << x << " 240 " << x2 << " 240 \"\" 0 0 0>\n";

  x = 170;
  if (type == BANDPASS || type == BANDSTOP) k = ord * 2;
  for(i = 0; i < k && ncomp > 0; i++) {
    // find series components to ground
    unsigned int a = j;
    while (Comp[a].node2 != 0) {
      if (a+1 < ncomp && Comp[a].node2 == Comp[a+1].node1 &&
	  Comp[a].comp != Comp[a+1].comp)
	a++;
      else
	break;
    }
    // handle series components to ground
    if (Comp[a].node2 == 0) {
      // wire down to shunt components
      wir << "<" << x << " 240 " << x << " 290 \"\" 0 0 0>\n";
      wir << "<" << x-35 << " 240 " << x+35 << " 240 \"\" 0 0 0>\n";
      unsigned int b, c;
      for (c = 0, b = j; b <= a; b++, c++) {
	// type of component
	switch (Comp[b].comp) {
	case CAP: res << "<C C"; break;
	case IND: res << "<L L"; break;
	case RES: res << "<R R"; break;
	}
	res << j+1 << " 1 " << x;
	res << " " << yc+c*60 << " 17 -26 0 1";

	// value of component
	res << " \"" << num2str(Comp[b].val).c_str();
	switch (Comp[b].comp) {
	case CAP: res << "F"; break;
	case IND: res << "H"; break;
	case RES: res << "Ohm"; break;
	}
	res << "\" 1>\n";
      }
      // place ground symbol here
      res << "<GND * 1 " << x << " " << yc+c*60-30 << " 0 0 0 0>\n";
      j = a + 1;
      if (gndser < c) gndser = c;
      repser = 0;
    }
    // handle parallel components in series
    else {
      if (repser) x += 35;
      // connect in series
      if (i != 0)
	wir << "<" << x-70 << " 240 " << x-30 << " 240 \"\" 0 0 0>\n";
      if (i != k - 1)
	wir << "<" << x+70 << " 240 " << x+30 << " 240 \"\" 0 0 0>\n";
      unsigned int b = j, c = 0;
      do {
	// type of component
	switch (Comp[b].comp) {
	case CAP: res << "<C C"; break;
	case IND: res << "<L L"; break;
	case RES: res << "<R R"; break;
	}
	res << j+1 << " 1 " << x;
	res << " " << yl-c*35 << " -26 ";
	if (c) {
	  res << "-44";
	  wir << "<" << x-30 << " " << yl 
	      << " " << x-30 << " " << yl-c*35 << " \"\" 0 0 0>\n"
	      << "<" << x+30 << " " << yl
	      << " " << x+30 << " " << yl-c*35 << " \"\" 0 0 0>\n";
	}
	else
	  res << "10";
	res << " 0 0";

	// value of commponent
	res << " \"" << num2str(Comp[b].val).c_str();
	switch (Comp[b].comp) {
	case CAP: res << "F"; break;
	case IND: res << "H"; break;
	case RES: res << "Ohm"; break;
	}
	res << "\" 1>\n";
	b++;
	c++;
      }
      while (b < ncomp && Comp[b-1].node1 == Comp[b].node1 &&
	     Comp[b-1].node2 == Comp[b].node2);
      j = b;
      repser++;
    }
    if (j >= ncomp) break;
    x += 70;
  }

  if (ord & 1)
    x += 110;
  else
    x += 70;
  res << "<Pac P2 1 "<< x << " 320 18 -26 0 1 \"2\" 1 \""
      << imp << " Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n"
      << "<GND * 1 " << x << " 350 0 0 0 0>\n";

  yc += 20 + gndser * 60;
  Value = fc / 10.0;
  Value2 = 10.0 * fc;
  res << "<.SP SP1 1 70 " << yc << " 0 50 0 0 \"log\" 1 \""
      << num2str(Value).c_str() << "Hz\" 1 \"" << num2str(Value2).c_str()
      << "Hz\" 1 \"200\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n"
      << "<Eqn Eqn1 1 260 " << yc+10
      << " -28 15 0 0 \"dBS21=dB(S[2,1])\" 1 "
      << "\"dBS11=dB(S[1,1])\" 1 \"yes\" 0>\n"
      << "</Components>\n";

  res << "<Wires>\n";

  // internal wires
  res << wir.str().c_str();

  // connect right source
  x2 = x - 110;
  if(type == HIGHPASS) x2 += 30;
  res << "<" << x << " 240 " << x << " 290 \"\" 0 0 0>\n"
      << "<" << x2 << " 240 " << x << " 240 \"\" 0 0 0>\n";

  res << "</Wires>\n"
      << "<Diagrams>\n"
      << "</Diagrams>\n"
      << "<Paintings>\n";

  res << "<Text 400 " << (yc+10) << " 12 #000000 0 \"";

  switch (kind) {
  case CAUER: res << "Cauer "; break;
  case BUTT: res << "Butterworth "; break;
  case CHEB: res << "Chebichev "; break;
  case ICHEB: res << "Inverse Chebichev "; break;
  case BESS: res << "Bessel "; break;
  case UNDEF: res << "Undefined "; break;
  }

  switch(type) {
    case LOWPASS:
      res << "low-pass filter\\n"
	  << num2str(fc).c_str() << "Hz cutoff";
      break;
    case HIGHPASS:
      res << "high-pass filter\\n"
	  << num2str(fc).c_str() << "Hz cutoff";
      break;
    case BANDPASS:
      res << "band-pass filter\\n"
	  << num2str(fstart).c_str() << "Hz ... "
	  << num2str(fstop).c_str() << "Hz";
      break;
    case BANDSTOP:
      res << "band-reject filter\\n"
	  << num2str(fstart).c_str() << "Hz ... "
	  << num2str(fstop).c_str() << "Hz";
      break;
  }
  res << ", PI-type,\\nimpedance matching " << imp << " Ohm\">\n";

  res << "</Paintings>\n";
  return res.str();
}

void qf_filter::dump_qucs (void) {
  std::cout << to_qucs().c_str();
}
