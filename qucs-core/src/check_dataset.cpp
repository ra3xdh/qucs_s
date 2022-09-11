/*
 * check_dataset.cpp - checker for the Qucs dataset
 *
 * Copyright (C) 2003, 2006 Stefan Jahn <stefan@lkcc.org>
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

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "vector.h"
#include "dataset.h"
#include "strlist.h"
#include "check_dataset.h"

using namespace qucs;

strlist * dataset_idents = NULL;
dataset * dataset_result = NULL;
vector  * dataset_vector = NULL;

/* This function is the checker routine for a parsed dataset.  It
   returns zero on success or non-zero if the parsed dataset contained
   errors. */
int dataset_check (dataset * data) {

  int errors = 0;
  vector * v, * d;

  /* check actual size and requested size of independent vectors */
  for (v = data->getDependencies (); v != NULL; v = (vector *) v->getNext ()) {
    if (v->getSize () != v->getRequested ()) {
      logprint (LOG_ERROR, "checker error, vector `%s' contains %d values, "
		"%d have been stated\n", v->getName (), v->getSize (),
		v->getRequested ());
      errors++;
    }
  }

  /* check dependencies of dependent vectors */
  for (v = data->getVariables (); v != NULL; v = (vector *) v->getNext ()) {
    strlist * s = v->getDependencies ();
    /* any dependencies at all ? */
    if (s == NULL || s->length () == 0) {
      logprint (LOG_ERROR, "checker error, vector `%s' contains no "
		"dependencies\n", v->getName ());
      errors++;
    }
    /* yes */
    else {
      int n = 1;
      /* go through each dependency and check it */
      for (strlistiterator it (s); *it; ++it) {
	if ((d = data->findDependency (*it)) == NULL) {
	  logprint (LOG_ERROR, "checker error, no such dependency `%s' as "
		    "stated in `%s'\n", *it, v->getName ());
	  errors++;
	}
	else {
	  n *= d->getSize ();
	}
      }
      if (n != 0) {
	if (v->getSize () % n != 0) {
	  logprint (LOG_ERROR, "checker error, size of vector `%s' %d should "
		    "be dividable by %d\n", v->getName (), v->getSize (), n);
	  errors++;
	}
      }
    }
  }

  return errors ? -1 : 0;
}
