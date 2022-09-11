/*
 * check_citi.cpp - checker for CITIfiles
 *
 * Copyright (C) 2006 Stefan Jahn <stefan@lkcc.org>
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
#include "check_citi.h"

using namespace qucs;

qucs::dataset * citi_result = NULL;
struct citi_package_t * citi_root = NULL;

/* Returns the number of vectors in a package. */
static int citi_count_vectors (struct citi_package_t * p) {
  int i = 0;
  for (qucs::vector * v = p->data; v != NULL; v = (qucs::vector *) v->getNext ()) i++;
  return i;
}

/* Returns the number of variables in a package. */
static int citi_count_variables (struct citi_package_t * p) {
  int i = 0;
  for (struct citi_header_t * h = p->head; h != NULL; h = h->next) {
    if (h->var != NULL) i++;
  }
  return i;
}

/* Returns the n-th vector in the package. */
static qucs::vector * citi_get_vector (struct citi_package_t * p, int n) {
  qucs::vector * v = p->data;
  for (int i = 0; v != NULL; v = (qucs::vector *) v->getNext (), i++) {
    if (i == n) return v;
  }
  return NULL;
}

/* Returns the number of packages. */
static int citi_count_packages (struct citi_package_t * p) {
  int i = 0;
  for (; p != NULL; p = p->next) i++;
  return i;
}

/* Find name of the package in its headers. */
static char * citi_get_package (struct citi_package_t * p) {
  for (struct citi_header_t * h = p->head; h != NULL; h = h->next) {
    if (h->package != NULL) return h->package;
  }
  return NULL;
}

/* Create a valid vector for the dataset. */
static qucs::vector * citi_create_vector (struct citi_package_t * p, int i,
				    char * n, char * type) {
  qucs::vector * vec;
  vec = citi_get_vector (p, i); // fetch vector
  vec = new qucs::vector (*vec);      // copy vector
  vec->reverse ();              // reverse vector

  // convert data if necessary
  if (!strcmp (type, "MAGANGLE")) {
    for (int i = 0; i < vec->getSize (); i++) {
      nr_complex_t val = vec->get (i);
      val = std::polar (real (val), deg2rad (imag (val)));
      vec->set (val, i);
    }
  }
  else if (!strcmp (type, "DBANGLE")) {
    for (int i = 0; i < vec->getSize (); i++) {
      nr_complex_t val = vec->get (i);
      val = std::polar (std::pow (10.0, real (val) / 20.0), deg2rad (imag (val)));
      vec->set (val, i);
    }
  }

  // return named vector
  vec->setName (n);
  return vec;
}

/* Returns dependent variable length for the given dependencies. */
static int citi_vector_length (strlist deps) {
  int n = 1;
  // no dependencies
  if (deps.length () <= 0)
    return 0;
  // calculate length of resulting dependent variable
  for (int i = 0; i < deps.length(); i++) {
    qucs::vector * v = citi_result->findDependency (deps.get (i));
    if (v != NULL) n *= v->getSize ();
  }
  return n;
}

/* Checks length of variable vectors. */
static int citi_check_dep_length (qucs::vector * v, strlist deps, char * package) {
  int rlength = v->getSize ();
  int dlength = citi_vector_length (deps);
  if (rlength != dlength) {
    logprint (LOG_ERROR, "checker error, invalid vector `%s' length "
	      "(%d != %d) in package `%s'\n", v->getName (), rlength, dlength,
	      package);
    return 1;
  }
  return 0;
}

// Destroys data used by the CITIfile parser.
void citi_finalize (void) {
  struct citi_package_t * p, * pn;
  /* go through all packages */
  for (p = citi_root; p != NULL; p = pn) {
    struct citi_header_t * h, * hn;
    /* go through each header */
    for (h = p->head; h != NULL; h = hn) {
      free (h->package);
      free (h->var);
      free (h->type);
      hn = h->next;
      free (h);
    }
    qucs::vector * v, * vn;
    /* go through each vector */
    for (v = p->data; v != NULL; v = vn) {
      vn = (qucs::vector *) v->getNext ();
      delete v;
    }
    pn = p->next;
    free (p);
  }
  citi_lex_destroy ();
}

/* This function is the checker routine for a parsed CITIfile.  It
   returns zero on success or non-zero if it contained errors. */
int citi_check (void) {

  int errors = 0;

  /* create dataset */
  citi_result = new dataset ();
  /* get root package */
  struct citi_package_t * p = citi_root;
  /* count packages */
  int packages = citi_count_packages (p);

  /* go through all packages */
  for (p = citi_root; p != NULL; p = p->next) {
    struct citi_header_t * h;
    char * package = citi_get_package (p);
    int n = 0;
    strlist deps;

    /* check number of defined variables and vectors */
    int cvec = citi_count_vectors (p);
    int cvar = citi_count_variables (p);
    if (cvec != cvar) {
      logprint (LOG_ERROR, "checker error, no. of vectors (%d) does not equal "
		"no. of variables (%d) in package `%s'\n", cvec, cvar,
		package);
      errors++;
      break;
    }

    /* no package info if there is just one */
    char opack[256];
    if (packages < 2) {
      opack[0] = '\0';
    } else {
      sprintf (opack, "%s.", package);
    }

    /* go through each header */
    for (h = p->head; h != NULL; h = h->next) {
      qucs::vector * v;
      if (h->var != NULL) {
	char txt[256];
	if (h->i1 >= 0) {
	  /* dependent variables */
	  if (h->i2 >= 0) {
	    sprintf (txt, "%s%s[%d,%d]", opack, h->var, h->i1, h->i2);
	    v = citi_create_vector (p, n, txt, h->type);
	    v->setDependencies (new strlist (deps));
	    errors += citi_check_dep_length (v, deps, package);
	    citi_result->addVariable (v);
	    n++;
	  } else {
	    sprintf (txt, "%s%s[%d]", opack, h->var, h->i1);
	    v = citi_create_vector (p, n, txt, h->type);
	    v->setDependencies (new strlist (deps));
	    errors += citi_check_dep_length (v, deps, package);
	    citi_result->addVariable (v);
	    n++;
	  }
	} else if (h->n >= 0) {
	  /* independent variable */
	  sprintf (txt, "%s%s", opack, h->var);
	  v = citi_create_vector (p, n, txt, h->type);
	  deps.add (txt);
	  if (!citi_result->findDependency (txt)) {
	    /* add independent vectors only once */
	    citi_result->addDependency (v);
	  }
	  n++;
	  // check length of independent vector
	  if (v->getSize () != h->n) {
	    logprint (LOG_ERROR, "checker error, vector `%s' length (%d) "
		      "does not equal defined length (%d) in package `%s'\n",
		      h->var, v->getSize (), h->n, package);
	    errors++;
	  }
	} else {
	  /* dependent variables, no indices */
	  sprintf (txt, "%s%s", opack, h->var);
	  v = citi_create_vector (p, n, txt, h->type);
	  v->setDependencies (new strlist (deps));
	  errors += citi_check_dep_length (v, deps, package);
	  citi_result->addVariable (v);
	  n++;
	}
      }
    }
  }

  citi_finalize ();
  citi_root = NULL;
  return errors ? -1 : 0;
}

// Destroys data used by the CITIfile parser and checker.
void citi_destroy (void) {
  if (citi_result != NULL) {
    // delete associated dataset
    delete citi_result;
    citi_result = NULL;
  }
  if (citi_root != NULL) {
    citi_finalize ();
    citi_root = NULL;
  }
}

// Initializes the CITIfile checker.
void citi_init (void) {
  citi_result = NULL;
  citi_root = NULL;
}

