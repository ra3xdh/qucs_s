/*
 * check_touchstone.cpp - checker for Touchstone files
 *
 * Copyright (C) 2003, 2004, 2005, 2006 Stefan Jahn <stefan@lkcc.org>
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
#include <string.h>
#include <ctype.h>
#include <cmath>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "vector.h"
#include "matrix.h"
#include "matvec.h"
#include "dataset.h"
#include "strlist.h"
#include "constants.h"
#include "check_touchstone.h"

#define ZREF 50.0 /* reference impedance */

using namespace qucs;

strlist * touchstone_idents = NULL;
dataset * touchstone_result = NULL;
qucs::vector  * touchstone_line = NULL;
qucs::vector  * touchstone_vector = NULL;

/* default touchstone options */
struct touchstone_t touchstone_options = {
  "GHz", 'S', "MA", 50.0, 1e9, 0, 0, 0 };

/* available touchstone options */
static const char * touchstone_valid_options[] = {
  "hz", "khz", "mhz", "ghz", "s", "y", "z", "g", "h", "ma", "db", "ri", NULL };

/* This subroutine is going to join vectors on multiple lines.  The
   input and output list of vectors of this function is the
   touchstone_vector variable. */
static void touchstone_join (void) {
  qucs::vector * yroot, * xroot, * next = NULL;
  /* go through each vector */
  for (yroot = touchstone_vector; yroot != NULL; yroot = next) {
    /* go through each trailing vector */
    next = (qucs::vector *) yroot->getNext ();
    for (xroot = next; xroot != NULL; xroot = next) {
      next = (qucs::vector *) xroot->getNext ();
      /* append xroot vector to yroot vector (even no. of values) ? */
      if ((xroot->getSize () & 1) == 0) {
	/* yes, delete the xroot vector and adjust list */
	yroot->add (xroot);
	yroot->setNext (next);
	delete xroot;
      }
      else {
	/* no, handle next vectors */
	next = xroot;
	break;
      }
    }
  }
}

/* This subroutine checks the size and overall conformance of each
   touchstone matrix at a given frequency derived from the first
   matrix.  The function return zero on success and non-zero
   otherwise. */
static int touchstone_vector_check (void) {
  qucs::vector * root = touchstone_vector, * next;
  int even = 0, errors = 0, size = root->getSize (), noise = 0, lines = 1;
  nr_double_t f = real (root->get (0));

  /* check size of first line */
  if ((size & 1) == 0) {
    logprint (LOG_ERROR, "checker error, first data line has %d (even) "
	      "values\n", size);
    errors++;
    even = 1;
  }
  /* first line determines the number of expected ports */
  touchstone_options.ports = (int) std::sqrt ((size - 1) / 2.0);

  /* check first frequency value */
  if (f < 0.0) {
    logprint (LOG_ERROR, "checker error, negative data frequency "
	      "value %g\n", f);
    errors++;
  }

  /* go through each vector */
  for (root = (qucs::vector *) root->getNext (); root != NULL; root = next) {
    next = (qucs::vector *) root->getNext ();
    nr_double_t freq = real (root->get (0));

    /* check increasing frequency value */
    if (f >= freq) {
      if (!noise) {
	/* determined start of noise parameters */
	noise++;
	size = 5;
	if (freq < 0.0) {
	  logprint (LOG_ERROR, "checker error, negative noise frequency "
		    "value %g\n", freq);
	  errors++;
	}
      }
      else {
	logprint (LOG_ERROR, "checker error, %s line (f = %g) has "
		  "decreasing frequency value\n", noise ? "noise" : "data",
		  freq);
	errors++;
      }
    }
    f = freq;

    /* check size of vector */
    if (!even && root->getSize () != size) {
      logprint (LOG_ERROR, "checker error, %s line (f = %g) has %d values, "
		"%d required\n", noise ? "noise" : "data",
		real (root->get (0)), root->getSize (), size);
      errors++;
    }

    /* count number of data lines without noise entries */
    if (!noise) lines++;
  }
  touchstone_options.noise = noise;
  touchstone_options.lines = lines;
  return errors;
}

/* The function evaluates the identifiers in the option line and fills
   the touchstone_options structure with appropriate values. */
static void touchstone_options_eval (void) {
  /* go through all identifiers */
  for (int i = 0; i < touchstone_idents->length (); i++) {
    char * str = touchstone_idents->get (i);
    /* frequency unit */
    if (!strcmp (str, "hz")) {
      touchstone_options.factor = 1.0;
      touchstone_options.unit = "Hz";
    }
    else if (!strcmp (str, "khz")) {
      touchstone_options.factor = 1e3;
      touchstone_options.unit = "kHz";
    }
    else if (!strcmp (str, "mhz")) {
      touchstone_options.factor = 1e6;
      touchstone_options.unit = "MHz";
    }
    else if (!strcmp (str, "ghz")) {
      touchstone_options.factor = 1e9;
      touchstone_options.unit = "GHz";
    }
    /* parameter type */
    else if (!strcmp (str, "s")) {
      touchstone_options.parameter = 'S';
    }
    else if (!strcmp (str, "y")) {
      touchstone_options.parameter = 'Y';
    }
    else if (!strcmp (str, "z")) {
      touchstone_options.parameter = 'Z';
    }
    else if (!strcmp (str, "g")) {
      touchstone_options.parameter = 'G';
    }
    else if (!strcmp (str, "h")) {
      touchstone_options.parameter = 'H';
    }
    /* value formats */
    else if (!strcmp (str, "ma")) {
      touchstone_options.format = "MA";
    }
    else if (!strcmp (str, "db")) {
      touchstone_options.format = "dB";
    }
    else if (!strcmp (str, "ri")) {
      touchstone_options.format = "RI";
    }
  }
}

/* This little function returns a static string containing an
   appropriate variable name. */
static char * touchstone_create_set (int r, int c) {
  char * text;
  text = matvec::createMatrixString (touchstone_options.parameter, r, c);
  return text;
}

/* The function actually creates the resulting dataset. */
static void touchstone_create (void) {
  qucs::vector * f, * v, * root, * next, * nf = NULL;
  int ports = touchstone_options.ports, n;
  nr_complex_t val;
  strlist * s;

  /* create dataset and frequency vector */
  touchstone_result = new dataset ();
  f = new qucs::vector ("frequency");
  touchstone_result->appendDependency (f);
  s = new strlist ();
  s->add (f->getName ());
  /* create variable vectors for the resulting dataset */
  for (int r = 0; r < ports; r++) {
    for (int c = 0; c < ports; c++) {
      v = new qucs::vector ();
      v->setName (touchstone_create_set (r, c));
      v->setDependencies (new strlist (*s));
      touchstone_result->appendVariable (v);
    }
  }
  delete s;

  /* create noise vectors if necessary */
  if (touchstone_options.noise) {
    nf = new qucs::vector ("nfreq");
    touchstone_result->appendDependency (nf);
    s = new strlist ();
    s->add (nf->getName ());
    /* append noise parameters to dataset */
    v = new qucs::vector ("Fmin");
    v->setDependencies (new strlist (*s));
    touchstone_result->appendVariable (v);
    v = new qucs::vector ("Sopt");
    v->setDependencies (new strlist (*s));
    touchstone_result->appendVariable (v);
    v = new qucs::vector ("Rn");
    v->setDependencies (new strlist (*s));
    touchstone_result->appendVariable (v);
    delete s;
  }

  /* go through each vector */
  for (n = 0, root = touchstone_vector; root != NULL; root = next, n++) {
    next = (qucs::vector *) root->getNext ();
    // handle data lines
    if (n < touchstone_options.lines) {
      /* fill frequency vector */
      f->add (real (root->get (0)) * touchstone_options.factor);
      /* go through each variable vector */
      v = touchstone_result->getVariables ();
      for (int i = 0; i < ports; i++) {
	for (int j = 0; j < ports; j++) {
	  int pos = 1 + j * 2 + i * 2 * ports;
	  /* handle special case for 2-port touchstone data, '21' data
	     precedes the '12' data */
	  if (ports == 2 && i != j) {
	    pos = 1 + i * 2 + j * 2 * ports;
	  }
	  /* depending on the touchstone data format */
	  if (!strcmp (touchstone_options.format, "RI")) {
	    val = nr_complex_t (real (root->get (pos + 0)),
			real (root->get (pos + 1)));
	  }
	  else if (!strcmp (touchstone_options.format, "MA")) {
	    val = qucs::polar (real (root->get (pos + 0)),
			 deg2rad (real (root->get (pos + 1))));
	  }
	  else if (!strcmp (touchstone_options.format, "dB")) {
	    val = qucs::polar (std::pow (10.0, real (root->get (pos + 0)) / 20.0),
			 deg2rad (real (root->get (pos + 1))));
	  }
	  v->add (val);
	  v = (qucs::vector *) v->getNext ();
	}
      }
    }
    // handle noise lines
    else if (touchstone_options.noise) {
      /* fill frequency vector */
      nf->add (real (root->get (0)) * touchstone_options.factor);
      /* fill minimum noise figure vector */
      v = touchstone_result->findVariable ("Fmin");
      val = std::pow (10.0, real (root->get (1)) / 10.0);
      v->add (val);
      /* fill optimal noise reflexion coefficient vector */
      v = touchstone_result->findVariable ("Sopt");
      val = qucs::polar (real (root->get (2)), deg2rad (real (root->get (3))));
      if (ZREF != touchstone_options.resistance) {
	// re-normalize reflexion coefficient if necessary
	nr_double_t r = (ZREF - touchstone_options.resistance) /
	  (ZREF + touchstone_options.resistance);
	val = (val - r) / (1.0 - r * val);
      }
      v->add (val);
      /* fill equivalent noise resistance vector */
      v = touchstone_result->findVariable ("Rn");
      val = real (root->get (4)) * touchstone_options.resistance;
      v->add (val);
    }
  }
}

/* The function re-normalizes S-parameters to the internal reference
   impedance 50 Ohms. */
static void touchstone_normalize_sp (void) {
  int ports = touchstone_options.ports;
  qucs::vector * v = touchstone_result->getVariables ();
  int i, j, n, len = v->getSize ();
  matrix s = matrix (ports);

  // go through each matrix entry
  for (n = 0; n < len; n++) {
    v = touchstone_result->getVariables ();
    // save entries in a temporary matrix
    for (i = 0; i < ports; i++) {
      for (j = 0; j < ports; j++) {
	s.set (i, j, v->get (n));
	v = (qucs::vector *) v->getNext ();
      }
    }
    // convert the temporary matrix
    s = stos (s, touchstone_options.resistance, ZREF);
    v = touchstone_result->getVariables ();
    // restore the results in the entries
    for (i = 0; i < ports; i++) {
      for (j = 0; j < ports; j++) {
	v->set (s.get (i, j), n);
	v = (qucs::vector *) v->getNext ();
      }
    }
  }
}

/* The function transforms the reference impedance given in the
   touchstone file to the internal reference impedance 50 Ohms. */
static void touchstone_normalize (void) {
  qucs::vector * v = touchstone_result->getVariables ();
  int ports = touchstone_options.ports;

  // transform S-parameters if necessary
  if (touchstone_options.parameter == 'S') {
    if (touchstone_options.resistance != ZREF)
      touchstone_normalize_sp ();
    return;
  }
  // transform any other X-parameters
  for (int i = 1; i <= ports; i++) {
    for (int j = 1; j <= ports; j++) {
      switch (touchstone_options.parameter) {
      case 'Y': // Y-parameters
	*v /= touchstone_options.resistance;
	break;
      case 'Z': // Z-parameters
	*v *= touchstone_options.resistance;
	break;
      case 'G': // hybrid G-parameters
	if (i == 1 && j == 1)
	  *v /= touchstone_options.resistance;
	else if (i == 2 && j == 2)
	  *v *= touchstone_options.resistance;
	break;
      case 'H': // hybrid H-parameters
	if (i == 1 && j == 1)
	  *v *= touchstone_options.resistance;
	else if (i == 2 && j == 2)
	  *v /= touchstone_options.resistance;
	break;
      }
      v = (qucs::vector *) v->getNext ();
    }
  }
}

/* Removes temporary data items from memory if necessary. */
static void touchstone_finalize (void) {
  qucs::vector * root, * next;
  for (root = touchstone_vector; root != NULL; root = next) {
    next = (qucs::vector *) root->getNext ();
    delete root;
  }
  touchstone_vector = NULL;
  if (touchstone_idents != NULL) {
    delete touchstone_idents;
    touchstone_idents = NULL;
  }
  touchstone_lex_destroy ();
  /* apply default values again */
  touchstone_options.unit = "GHz";
  touchstone_options.parameter = 'S';
  touchstone_options.format = "MA";
  touchstone_options.resistance = 50.0;
  touchstone_options.factor = 1e9;
  touchstone_options.ports = 0;
  touchstone_options.noise = 0;
  touchstone_options.lines = 0;
}


/* This function is the checker routine for a parsed touchstone.  It
   returns zero on success or non-zero if the parsed touchstone
   contained errors. */
int touchstone_check (void) {

  int i, n, errors = 0;

  /* first checking the options */
  if (touchstone_idents->length () > 3) {
    logprint (LOG_ERROR, "checker error, found %d options\n",
	      touchstone_idents->length ());
    errors++;
  }
  /* touchstone is case insensitive */
  for (i = 0; i < touchstone_idents->length (); i++) {
    for (char * p = touchstone_idents->get (i); *p != '\0'; p++)
      *p = tolower (*p);
  }
  /* check duplicate options */
  for (i = 0; i < touchstone_idents->length (); i++) {
    char * str = touchstone_idents->get (i);
    if ((n = touchstone_idents->contains (str)) != 1) {
      logprint (LOG_ERROR, "checker error, option `%s' occurred %dx\n",
		str, n);
      errors++;
    }
  }
  /* check valid options */
  for (i = 0; i < touchstone_idents->length (); i++) {
    char * str = touchstone_idents->get (i);
    int valid = 0;
    for (int v = 0; touchstone_valid_options[v] != NULL; v++) {
      if (!strcmp (touchstone_valid_options[v], str))
	valid = 1;
    }
    if (!valid) {
      logprint (LOG_ERROR, "checker error, invalid option `%s'\n", str);
      errors++;
    }
  }

  /* evaluate the option line and put values into touchstone_options
     structure */
  touchstone_options_eval ();

  if (touchstone_vector == NULL) {
    logprint (LOG_ERROR, "checker error, no data in touchstone file\n");
    errors++;
  }
  else {
    /* join vectors on multiple lines */
    touchstone_join ();

    /* check each vector */
    errors += touchstone_vector_check ();

    /* check validity of ports and parameters */
    if ((touchstone_options.parameter == 'G' ||
	 touchstone_options.parameter == 'H') &&
	touchstone_options.ports != 2) {
      logprint (LOG_ERROR, "checker error, %c-parameters for %d-ports not "
		"defined\n", touchstone_options.parameter,
		touchstone_options.ports);
      errors++;
    }

    /* check noise parameter compatibility */
    if (touchstone_options.noise && touchstone_options.ports != 2) {
      logprint (LOG_ERROR, "checker error, noise parameters for %d-ports not "
		"defined\n", touchstone_options.ports);
      errors++;
    }
  }

  /* finally create a dataset */
  if (!errors) {
    touchstone_create ();
    touchstone_normalize ();
  }

#if DEBUG
  /* emit little notify message on successful loading */
  if (!errors) {
    logprint (LOG_STATUS, "NOTIFY: touchstone %d-port %c-data%s loaded\n",
	      touchstone_options.ports, touchstone_options.parameter,
	      touchstone_options.noise ? " including noise" : "");
  }
#endif

  /* free temporary memory */
  touchstone_finalize ();

  return errors ? -1 : 0;
}

// Destroys data used by the Touchstone file lexer, parser and checker.
void touchstone_destroy (void) {
  if (touchstone_result != NULL) {
    // delete associated dataset
    delete touchstone_result;
    touchstone_result = NULL;
  }
  if (touchstone_vector != NULL) {
    touchstone_finalize ();
    touchstone_vector = NULL;
  }
}

// Initializes the Touchstone file checker.
void touchstone_init (void) {
  touchstone_result = NULL;
  touchstone_vector = NULL;
  touchstone_idents = NULL;
}
