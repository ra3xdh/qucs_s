/*
 * check_csv.cpp - checker for CSV files
 *
 * Copyright (C) 2007, 2009 Stefan Jahn <stefan@lkcc.org>
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
#include "check_csv.h"

using namespace qucs;

strlist * csv_header = NULL;
qucs::vector  * csv_vector = NULL;
dataset * csv_result = NULL;

/* Removes temporary data items from memory if necessary. */
static void csv_finalize (void) {
  qucs::vector * root, * next;
  for (root = csv_vector; root != NULL; root = next) {
    next = (qucs::vector *) root->getNext ();
    delete root;
  }
  csv_vector = NULL;
  if (csv_header != NULL) {
    delete csv_header;
    csv_header = NULL;
  }
  csv_lex_destroy ();
}

/* Validates a data vector identifier. */
static void csv_validate_str (char * n) {
  char * p = n;
  if (!isalpha (*p)) *p = '_';
  p++;
  while (*p) {
    if (!isalnum (*p) && *p != '.' && *p != ',' && *p != '[' && *p != ']')
      *p = '_';
    p++;
  }
}

/* Creates dataset from CSV vectors. */
static void csv_create_dataset (int len) {
  qucs::vector * dep, * indep, * v;
  char * n, depn[256];
  strlist * s;

  // create dataset
  csv_result = new dataset ();

  // add dependency vector
  indep = new qucs::vector ();
  csv_result->appendDependency (indep);
  s = new strlist ();
  n = csv_header ? csv_header->get (0) : (char *) "x";
  csv_validate_str (n);
  s->add (n);
  indep->setName (n);

  // create variable vector(s)
  for (int i = 1; i < len; i++) {
    v = new qucs::vector ();
    n = csv_header ? csv_header->get (i) : NULL;
    if (n == NULL) {
      sprintf (depn, "y%d", i);
      n = depn;
    }
    csv_validate_str (n);
    v->setName (n);
    v->setDependencies (new strlist (*s));
    csv_result->addVariable (v);
  }

  // fill all vectors in dataset
  for (v = csv_vector; v != NULL; v = (qucs::vector *) v->getNext ()) {
    dep = csv_result->getVariables ();
    int l;
    for (l = 0; l < v->getSize () - 1; l++) {
      dep->add (v->get (l));
      dep = (qucs::vector *) dep->getNext ();
    }
    indep->add (v->get (l));
  }

  // cleanup
  delete s;
}

/* This function is the checker routine for a parsed CSV.  It returns
   zero on success or non-zero if the parsed csv contained errors. */
int csv_check (void) {

  int len = -1, errors = 0;

  // no data
  if (csv_vector == NULL) {
    logprint (LOG_ERROR, "checker error, no data in csv file\n");
    errors++;
  }
  // data lines available
  else {
    // check number of columns in each data line
    for (qucs::vector * v = csv_vector; v != NULL; v = (qucs::vector *) v->getNext ()) {
      if (len == -1) len = v->getSize ();
      else {
	if (v->getSize () != len) {
	  logprint (LOG_ERROR, "checker error, different cols (%d != %d) in "
		    "csv data line\n", v->getSize (), len);
	  errors++;
	}
      }
    }
    // check number of columns in data and header
    if (csv_header && csv_header->length () != len) {
      logprint (LOG_ERROR, "checker error, different cols (%d != %d) in "
		"data and header lines\n", csv_header->length (), len);
      errors++;
    }
    // create dataset if possible
    if (!errors) {
      csv_create_dataset (len);
    }
  }

  /* free temporary memory */
  csv_finalize ();

  return errors ? -1 : 0;
}


// Destroys data used by the CSV file lexer, parser and checker.
void csv_destroy (void) {
  if (csv_result != NULL) {
    // delete associated dataset
    delete csv_result;
    csv_result = NULL;
  }
  if (csv_vector != NULL) {
    csv_finalize ();
    csv_vector = NULL;
  }
}

// Initializes the CSV file checker.
void csv_init (void) {
  csv_result = NULL;
  csv_vector = NULL;
  csv_header = NULL;
}

