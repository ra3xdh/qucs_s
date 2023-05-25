/*
 * csv_producer.cpp - the CSV data file producer
 *
 * Copyright (C) 2006, 2007 Stefan Jahn <stefan@lkcc.org>
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

#include <complex>
#include "qucs_typedefs.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "dataset.h"

#include "csv_producer.h"

using namespace qucs;

/* Global variables. */
qucs::dataset * qucs_data = NULL;
/* FILE * csv_out = NULL; -- already defined in CSV lexer */

struct csv_data {
  char type;  // type of variable
  vector * v; // appropriate data vector
  int idx;    // index into vector
  int skip;   // skip length
  int len;    // length of vector
};

/* Definition of line separator. */
#ifdef __MINGW32__
#define csv_crlf "\n"
#else
#define csv_crlf "\r\n"
#endif

/* The CSV data printer. */
void csv_print (struct csv_data * data, int vectors, const char * sep) {

  int len = 0;

  // print header
  for (int i = 0; i < vectors; i++) {
    if (data[i].type == 'c')
      fprintf (csv_out, "\"r %s\"%s\"i %s\"", data[i].v->getName (),
	       sep, data[i].v->getName ());
    else
      fprintf (csv_out, "\"%s\"", data[i].v->getName ());
    fprintf (csv_out, "%s", i != vectors - 1 ? sep : csv_crlf);
    // find longest vector
    if (len < data[i].len) len = data[i].len;
  }

  // print data
  for (int k = 0; k < len; k++) {
    for (int i = 0; i < vectors; i++) {
      if (data[i].type == 'c')
	fprintf (csv_out, "%+." "20" "e%s%+." "20" "e",
		 (double) real (data[i].v->get (data[i].idx)), sep,
		 (double) imag (data[i].v->get (data[i].idx)));
      else
	fprintf (csv_out, "%+." "20" "e",
		 (double) real (data[i].v->get (data[i].idx)));
      fprintf (csv_out, "%s", i != vectors - 1 ? sep : csv_crlf);
      data[i].idx = ((k + 1) / data[i].skip) % data[i].len;
    }
  }
}

/* This is the overall CSV producer. */
void csv_producer (char * variable, const char * sep) {
  vector * v;
  // save variable including its dependencies
  if (variable && (v = qucs_data->findVariable (variable)) != NULL) {

    // prepare variable + dependency structures
    strlist * deps = v->getDependencies ();
    int vectors = 1 + (deps ? deps->length () : 0);
    struct csv_data * data = new struct csv_data[vectors];
    int i = vectors - 1;
    data[i].type = real (sum (norm (imag (*v)))) > 0.0 ? 'c' : 'r';
    data[i].v = v;
    data[i].idx = 0;
    data[i].skip = 1;
    data[i].len = v->getSize ();

    int a = v->getSize ();
    for (i = vectors - 2; i >= 0; i--) {
      vector * d = qucs_data->findDependency (deps->get (i));
      data[i].type = real (sum (norm (imag (*d)))) > 0.0 ? 'c' : 'r';
      data[i].v = d;
      data[i].idx = 0;
      a /= d->getSize ();
      data[i].skip = a;
      data[i].len = d->getSize ();
    }

    csv_print (data, vectors, sep);
    delete[] data;
  }
  // save dependency + all variable depending on it
  else if (variable && (v = qucs_data->findDependency (variable)) != NULL) {

    // prepare dependency + variables structures
    vector * vars;
    int vectors = 1;
    for (vars = qucs_data->getVariables (); vars != NULL;
	 vars = (vector *) vars->getNext ()) {
      strlist * deps = vars->getDependencies ();
      if (deps->contains (v->getName ()))
	vectors++;
    }
    struct csv_data * data = new struct csv_data[vectors];

    data[0].type = real (sum (norm (imag (*v)))) > 0.0 ? 'c' : 'r';
    data[0].v = v;
    data[0].idx = 0;
    data[0].skip = 1;
    data[0].len = v->getSize ();
    int i = 1;
    for (vars = qucs_data->getVariables (); vars != NULL;
	 vars = (vector *) vars->getNext ()) {
      strlist * deps = vars->getDependencies ();
      if (deps->contains (v->getName ())) {
	vector * d = vars;
	data[i].type = real (sum (norm (imag (*d)))) > 0.0 ? 'c' : 'r';
	data[i].v = d;
	data[i].idx = 0;
	data[i].skip = 1;
	data[i].len = d->getSize ();
	i++;
      }
    }

    csv_print (data, vectors, sep);
    delete[] data;
  }
  // no such data found
  else {
    fprintf (stderr, "no such data variable `%s' found\n", variable);
  }
}
