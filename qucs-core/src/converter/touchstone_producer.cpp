/*
 * touchstone_producer.cpp - the Touchstone data file producer
 *
 * Copyright (C) 2007 Stefan Jahn <stefan@lkcc.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * $Id$
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

#include "touchstone_producer.h"
#include "matrix.h"
#include "matvec.h"
#include "constants.h"

using namespace qucs;

/* Global variables. */
/* dataset * qucs_data = NULL;   -- already defined in CSV producer */
/* FILE * touchstone_out = NULL; -- already defined in Touchstone lexer */

struct touchstone_data_t {
  char parameter;      // type of variable
  int ports;           // number of S-parameter ports
  double resistance;   // reference impedance
  const char * format; // data format
  qucs::vector * vd;         // appropriate dependency vector
  matvec * mv;         // appropriate data matrix vector
  qucs::vector * fmin;       // minimum noise figure
  qucs::vector * sopt;       // optimum input refelction for minimum noise figure
  qucs::vector * rn;         // effective noise resistance
  qucs::vector * vf;         // dependency vector for noise
}
touchstone_data;

/* Definition of line separator. */
#ifdef __MINGW32__
#define touchstone_crlf "\n"
#else
#define touchstone_crlf "\r\n"
#endif

/* The Touchstone noise data printer. */
void touchstone_print_noise (void) {
  if (touchstone_data.vf != NULL && touchstone_data.sopt != NULL &&
      touchstone_data.rn != NULL && touchstone_data.fmin != NULL) {
    // blank line separator
    fprintf (touchstone_out, touchstone_crlf);
    // noise data
    for (int i = 0; i < touchstone_data.vf->getSize (); i++) {
      nr_double_t f = real (touchstone_data.vf->get (i));
      fprintf (touchstone_out, "%." "20" "e"
	       " %+." "20" "e"
	       " %+." "20" "e" " %+." "20" "e"
	       " %+." "20" "e"
	       touchstone_crlf, f,
	       10.0 * std::log10 (real (touchstone_data.fmin->get (i))),
	       abs (touchstone_data.sopt->get (i)),
	       rad2deg (arg (touchstone_data.sopt->get (i))),
	       real (touchstone_data.rn->get (i)) /
	       touchstone_data.resistance);
    }
  }
}

/* The Touchstone data printer. */
void touchstone_print (void) {
  // header line
  fprintf (touchstone_out, "# %s %c %s R %g" touchstone_crlf,
	   "HZ", touchstone_data.parameter, touchstone_data.format,
	   touchstone_data.resistance);
  // one-port file
  if (touchstone_data.ports == 1) {
    for (int i = 0; i < touchstone_data.vd->getSize (); i++) {
      matrix S = touchstone_data.mv->get (i);
      nr_double_t f = real (touchstone_data.vd->get (i));
      fprintf (touchstone_out, "%." "20" "e"
	       " %+." "20" "e" " %+." "20" "e"
	       touchstone_crlf, f, real (S(0,0)), imag (S(0,0)));
    }
  }
  // two-port file
  else if (touchstone_data.ports == 2) {
    for (int i = 0; i < touchstone_data.vd->getSize (); i++) {
      matrix S = touchstone_data.mv->get (i);
      nr_double_t f = real (touchstone_data.vd->get (i));
      fprintf (touchstone_out, "%." "20" "e"
	       " %+." "20" "e" " %+." "20" "e"
	       " %+." "20" "e" " %+." "20" "e"
	       " %+." "20" "e" " %+." "20" "e"
	       " %+." "20" "e" " %+." "20" "e"
	       touchstone_crlf, f,
	       real (S(0,0)), imag (S(0,0)),
	       real (S(1,0)), imag (S(1,0)),
	       real (S(0,1)), imag (S(0,1)),
	       real (S(1,1)), imag (S(1,1)));
    }
  }
  // three-port file
  else if (touchstone_data.ports == 3) {
    for (int i = 0; i < touchstone_data.vd->getSize (); i++) {
      matrix S = touchstone_data.mv->get (i);
      nr_double_t f = real (touchstone_data.vd->get (i));
      fprintf (touchstone_out, "%." "20" "e"
	       " %+." "20" "e" " %+." "20" "e"
	       " %+." "20" "e" " %+." "20" "e"
	       " %+." "20" "e" " %+." "20" "e"
	       touchstone_crlf "      %"  "20" "s"
	       " %+." "20" "e" " %+." "20" "e"
	       " %+." "20" "e" " %+." "20" "e"
	       " %+." "20" "e" " %+." "20" "e"
	       touchstone_crlf "      %"  "20" "s"
	       " %+." "20" "e" " %+." "20" "e"
	       " %+." "20" "e" " %+." "20" "e"
	       " %+." "20" "e" " %+." "20" "e"
	       touchstone_crlf,
	       f,
	       real (S(0,0)), imag (S(0,0)),
	       real (S(0,1)), imag (S(0,1)),
	       real (S(0,2)), imag (S(0,2)), " ",
	       real (S(1,0)), imag (S(1,0)),
	       real (S(1,1)), imag (S(1,1)),
	       real (S(1,2)), imag (S(1,2)), " ",
	       real (S(2,0)), imag (S(2,0)),
	       real (S(2,1)), imag (S(2,1)),
	       real (S(2,2)), imag (S(2,2)));
    }
  }
  // four-port and above files
  else if (touchstone_data.ports >= 4) {
    for (int i = 0; i < touchstone_data.vd->getSize (); i++) {
      matrix S = touchstone_data.mv->get (i);
      nr_double_t f = real (touchstone_data.vd->get (i));
      int cs = S.getCols ();
      int rs = S.getRows ();
      fprintf (touchstone_out, "%." "20" "e", f);
      for (int r = 0; r < rs; r++) {
	if (r >= 1)
	  fprintf (touchstone_out, "      %"  "20" "s", " ");
	for (int c = 0; c < cs; c++) {
	  if (c > 1 && (c & 3) == 0)
	    fprintf (touchstone_out, "      %"  "20" "s", " ");
	  fprintf (touchstone_out, " %+." "20" "e" " %+." "20" "e",
		   real (S(r,c)), imag (S(r,c)));
	  if ((c > 1 && (c & 3) == 3) || (c == cs - 1))
	    fprintf (touchstone_out, touchstone_crlf);
	}
      }
    }
  }
}

/* The function finds an appropriate S-parameters or other parameter
   type (G, H, Y, Z, etc.) matrix from the given dataset and stores it
   into the global Touchstone structure. */
void touchstone_find_data (dataset * data, const char * name) {
  qucs::vector * v;
  char * n;
  const char * vn;
  char * vd = NULL, * vf = NULL;
  strlist * deps;
  int r, c, rs = -1, cs  = -1, s = 0;

  // find parameter matrix data and its dimensions
  for (v = data->getVariables (); v != NULL; v = (::vector *) v->getNext ()) {
    vn = v->getName ();
    // requested matrix vector name found?
    if (strstr (vn, name) == vn) {
      if ((n = matvec::isMatrixVector (vn, r, c)) != NULL) {
	if (rs < r) rs = r;
	if (cs < c) cs = c;
	s = v->getSize ();
	free (n);
	if ((deps = v->getDependencies ()) != NULL) {
	  vd = deps->get (0);
	}
      }
    }
    // minimum noise figure?
    if (!strcmp (vn, "Fmin")) {
      touchstone_data.fmin = v;
      if ((deps = v->getDependencies ()) != NULL) vf = deps->get (0);
    }
    // optimal input reflection for minimum noise figure?
    else if (!strcmp (vn, "Sopt")) {
      touchstone_data.sopt = v;
      if ((deps = v->getDependencies ()) != NULL) vf = deps->get (0);
    }
    // effective noise resitance?
    else if (!strcmp (vn, "Rn")) {
      touchstone_data.rn = v;
      if ((deps = v->getDependencies ()) != NULL) vf = deps->get (0);
    }
  }

  // matrix entries found
  if (rs >= 0 && cs >= 0 && vd != NULL) {
    int ss = rs > cs ? rs : cs;
    // fill in number of ports
    touchstone_data.ports = ss + 1;
    // create quadratic matrix vector
    matvec * mv = new matvec (s, ss + 1, ss + 1);
    mv->setName (name);
    // fill in matrix vectors
    for (v = data->getVariables (); v; v = (::vector *) v->getNext ()) {
      vn = v->getName ();
      if (strstr (vn, name) == vn) {
	if ((n = matvec::isMatrixVector (vn, r, c)) != NULL) {
	  mv->set (*v, r, c);
	  free (n);
	}
      }
    }
    // store in global structure
    touchstone_data.mv = mv;
    touchstone_data.parameter = toupper (mv->getName ()[0]);
    // look for dependency (frequency) vector
    for (v = data->getDependencies (); v; v = (::vector *) v->getNext ()) {
      if (vd && !strcmp (v->getName (), vd)) {
	touchstone_data.vd = v;
      }
      if (vf && !strcmp (v->getName (), vf)) {
	touchstone_data.vf = v;
      }
    }
  }
}

/* This is the overall Touchstone producer. */
void touchstone_producer (const char * variable) {

  // apply data variable name
  if (variable == NULL) {
    variable = "S";
  }

  // initialize global Touchstone structure
  touchstone_data.mv = NULL;
  touchstone_data.vd = NULL;
  touchstone_data.format = "RI";
  touchstone_data.resistance = 50.0;
  touchstone_data.fmin = NULL;
  touchstone_data.sopt = NULL;
  touchstone_data.rn = NULL;
  touchstone_data.vf = NULL;

  // look for appropriate matrix data
  touchstone_find_data (qucs_data, variable);

  // print matrix data if available
  if (touchstone_data.mv != NULL) {
    touchstone_print ();
    delete touchstone_data.mv;
    touchstone_print_noise ();
  }
  else {
    fprintf (stderr, "no such data variable `%s' found\n", variable);
  }
}
