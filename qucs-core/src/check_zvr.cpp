/*
 * check_zvr.cpp - iterate a zvr file
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <assert.h>
#include <float.h>
#include <ctype.h>

#include "strlist.h"
#include "object.h"
#include "complex.h"
#include "vector.h"
#include "dataset.h"
#include "constants.h"
#include "check_zvr.h"

using namespace qucs;

// Global variables.
dataset * zvr_result = NULL;
struct zvr_data_t * zvr_root = NULL;

// Creates a valid data vector description.
static char * zvr_vector_txt (struct zvr_vector_t * vec) {
  int i, i1 = -1, i2 = -1, off = 0, len = strlen (vec->n1);
  static char txt[64];
  // strip off leading 're' or 'im'
  if (strstr (vec->n1, "re") == vec->n1 ||
      strstr (vec->n1, "im") == vec->n1 ||
      strstr (vec->n1, "db") == vec->n1) {
    off = 2;
  }
  // strip off leading 'mag' or 'ang'
  else if (strstr (vec->n1, "mag") == vec->n1 ||
	   strstr (vec->n1, "ang") == vec->n1) {
    off = 3;
  }
  for (i = off; i < len; i++) if (!isalpha (vec->n1[i])) break;
  // get index 1
  if (i < len) {
    if (isdigit (vec->n1[i])) { i1 = vec->n1[i] - '0'; vec->n1[i] = '\0'; }
  }
  // get index 2
  if (++i < len) {
    if (isdigit (vec->n1[i])) { i2 = vec->n1[i] - '0'; vec->n1[i] = '\0'; }
  }
  // create vector description
  if (i1 >= 0 && i2 >= 0) {
    sprintf (txt, "%s[%d,%d]", &vec->n1[off], i1, i2);
  }
  else if (i1 >= 0) {
    sprintf (txt, "%s[%d]", &vec->n1[off], i1);
  }
  else {
    sprintf (txt, "%s", &vec->n1[off]);
  }
  return txt;
}

// The function free's the memory used by the ZVR checker.
static void zvr_finalize (void) {
  struct zvr_data_t * root, * next;
  // go through each dataset
  for (root = zvr_root; root; root = next) {
    struct zvr_vector_t * vec = root->v;
    struct zvr_header_t * hdr = root->h;
    next = root->next;
    // free each data line
    if (root->d) {
      struct zvr_line_t * rl, * nl;
      for (rl = root->d; rl; rl = nl) {
	nl = rl->next;
	free (rl);
      }
    }
    // free header
    if (hdr) {
      free (hdr->funit);
      free (hdr->d_UNT);
      free (hdr->d_FMT);
      free (hdr->d_TYP);
      free (hdr);
    }
    // free data vector
    if (vec) {
      free (vec->nf);
      free (vec->n1);
      free (vec->n2);
      free (vec);
    }
  }
  zvr_root = NULL;
}

// Create a dependency string list.
static strlist * zvr_create_dep (char * n) {
  strlist * dep = new strlist ();
  dep->add (n);
  return dep;
}

// Handles the dependency vectors.
static void zvr_check_dependencies (void) {
  vector * dep1 = zvr_result->getDependencies ();
  vector * depn = (vector *) dep1->getNext ();
  bool equal;

  // check for differing dependency vectors
  for (equal = true; depn != NULL; depn = (vector *) depn->getNext ()) {
    if (depn->getSize () != dep1->getSize ()) {
      equal = false; // differs in size
      break;
    } else {
      for (int i = 0; i < depn->getSize (); i++) {
	if (depn->get (i) != dep1->get (i)) {
	  equal = false; // differs in content
	  break;
	}
      }
    }
  }

  // all dependency vectors equal
  if (equal) {
    vector * ndep;
    // delete unnecessary vectors
    for (depn = (vector *) dep1->getNext (); depn != NULL; depn = ndep) {
      ndep = (vector *) depn->getNext ();
      zvr_result->delDependency (depn);
    }
  }
  // at least one dependency vectors not equal
  else {
    vector * depn = zvr_result->getDependencies ();
    vector * varn = zvr_result->getVariables ();
    char txt[64]; int i = 1;
    // change name of dependency vectors as well as the dependency
    // reference in the appropriate variable vectors
    while (depn != NULL && varn != NULL) {
      sprintf (txt, "%s.%d", depn->getName (), i);
      depn->setName (txt);
      varn->setDependencies (zvr_create_dep (txt));
      depn = (vector *) depn->getNext ();
      varn = (vector *) varn->getNext ();
      i++;
    }
  }
}

// The function performs the data conversion if necessary.
static void zvr_conversion (struct zvr_data_t * root) {
  for (; root != NULL; root = root->next) {
    struct zvr_vector_t * vec = root->v;
    struct zvr_header_t * hdr = root->h;
    vector * var = vec->vd; int n;
    // magnitude in [dB] and angle in [degree]
    if (!strcmp (hdr->d_FMT, "COMPLEX") && !strcmp (hdr->d_UNT, "dB")) {
      for (n = 0; n < var->getSize (); n++) {
	nr_double_t r = real (var->get (n));
	nr_double_t i = imag (var->get (n));
	var->set (std::polar (std::pow (10.0, r / 20.0), deg2rad (i)), n);
      }
    }
    // magnitude in [dB]
    else if (!strcmp (hdr->d_FMT, "MAGNITUDE") && !strcmp (hdr->d_UNT, "dB")) {
      for (n = 0; n < var->getSize (); n++) {
	nr_double_t r = real (var->get (n));
	var->set (std::pow (10.0, r / 20.0), n);
      }
    }
    // linear magnitude and angle in [degree]
    else if (!strcmp (hdr->d_FMT, "MA")) {
      for (n = 0; n < var->getSize (); n++) {
	nr_double_t r = real (var->get (n));
	nr_double_t i = imag (var->get (n));
	var->set (std::polar (r, deg2rad (i)), n);
      }
    }
    // magnitude in [dB] and angle in [degree]
    else if (!strcmp (hdr->d_FMT, "DB")) {
      for (n = 0; n < var->getSize (); n++) {
	nr_double_t r = real (var->get (n));
	nr_double_t i = imag (var->get (n));
	var->set (std::polar (std::pow (10.0, r / 20.0), deg2rad (i)), n);
      }
    }
  }
}

/* This function is the overall ZVR data checker.  It returns zero on
   success, non-zero otherwise. */
int zvr_check (void) {
  int errors = 0;
  struct zvr_data_t * root;

  // create a dataset
  zvr_result = new dataset ();

  // transfer ZVR tree data into the dataset
  for (root = zvr_root; root; root = root->next) {
    struct zvr_vector_t * vec = root->v;
    vec->vi->setName (vec->nf);
    vec->vd->setName (zvr_vector_txt (vec));
    for (struct zvr_line_t * line = root->d; line; line = line->next) {
      vec->vi->add (line->d);
      vec->vd->add (nr_complex_t (line->r, line->i));
    }
    vec->vd->setDependencies (zvr_create_dep (vec->nf));
    zvr_result->appendDependency (vec->vi);
    zvr_result->appendVariable (vec->vd);
  }

  // handle dependency vectors
  zvr_check_dependencies ();

  // perform data conversions
  zvr_conversion (zvr_root);

  // free temporary memory
  zvr_finalize ();
  zvr_root = NULL;

  return errors ? -1 : 0;
}

// Destroys data used by the ZVR checker.
void zvr_destroy (void) {
  if (zvr_result != NULL) {
    // delete associated dataset
    delete zvr_result;
    zvr_result = NULL;
  }
  if (zvr_root != NULL) {
    zvr_finalize ();
    zvr_root = NULL;
  }
}

// Initializes the ZVR checker.
void zvr_init (void) {
  zvr_result = NULL;
  zvr_root = NULL;
}
