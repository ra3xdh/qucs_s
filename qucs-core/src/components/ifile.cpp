/*
 * ifile.cpp - file based current source class implementation
 *
 * Copyright (C) 2007, 2008, 2009 Stefan Jahn <stefan@lkcc.org>
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

#include "component.h"
#include "dataset.h"
#include "poly.h"
#include "spline.h"
#include "interpolator.h"
#include "ifile.h"

using namespace qucs;

// Constructor creates ifile object in memory.
ifile::ifile () : circuit (2) {
  type = CIR_IFILE;
  setISource (true);
  interpolType = dataType = 0;
  data = NULL;
  inter = NULL;
}

// Destructor deletes ifile object from memory.
ifile::~ifile () {
  delete data;
  delete inter;
}

void ifile::prepare (void) {

  // check type of interpolator
  const char * itype = getPropertyString ("Interpolator");
  if (!strcmp (itype, "linear")) {
    interpolType = INTERPOL_LINEAR;
  } else if (!strcmp (itype, "cubic")) {
    interpolType = INTERPOL_CUBIC;
  } else if (!strcmp (itype, "hold")) {
    interpolType = INTERPOL_HOLD;
  }

  // check type of repetition
  const char * rtype = getPropertyString ("Repeat");
  if (!strcmp (rtype, "no")) {
    // rectangular data
    dataType = REPEAT_NO;
  } else if (!strcmp (rtype, "yes")) {
    // polar data
    dataType = REPEAT_YES;
  }

  // load file with samples
  const char * file = getPropertyString ("File");
  if (data == NULL) {
    if (strlen (file) > 4 && !strcasecmp (&file[strlen (file) - 4], ".dat"))
      data = dataset::load (file);
    else
      data = dataset::load_csv (file);
    if (data != NULL) {
      // check number of variables / dependencies defined by that file
      if (data->countVariables () != 1 || data->countDependencies () != 1) {
	logprint (LOG_ERROR, "ERROR: file `%s' must have time as an "
		  "independent and the current source samples as dependents\n",
		  file);
	return;
      }
      qucs::vector * is = data->getVariables();    // current
      qucs::vector * ts = data->getDependencies(); // time
      inter = new interpolator ();
      inter->rvectors (is, ts);
      inter->prepare (interpolType, dataType);
    }
  }
}

void ifile::initSP (void) {
  allocMatrixS ();
  setS (NODE_1, NODE_1, 1.0);
  setS (NODE_1, NODE_2, 0.0);
  setS (NODE_2, NODE_1, 0.0);
  setS (NODE_2, NODE_2, 1.0);
}

void ifile::initDC (void) {
  allocMatrixMNA ();
  prepare ();
}

void ifile::initAC (void) {
  initDC ();
}

void ifile::initTR (void) {
  initDC ();
}

void ifile::calcTR (nr_double_t t) {
  nr_double_t G = getPropertyDouble ("G");
  nr_double_t T = getPropertyDouble ("T");
  nr_double_t i = inter->rinterpolate (t - T);
  setI (NODE_1, +G * i); setI (NODE_2, -G * i);
}

// properties
PROP_REQ [] = {
  { "File", PROP_STR, { PROP_NO_VAL, "ifile.dat" }, PROP_NO_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Interpolator", PROP_STR, { PROP_NO_VAL, "linear" },
    PROP_RNG_STR3 ("hold", "linear", "cubic") },
  { "Repeat", PROP_STR, { PROP_NO_VAL, "no" }, PROP_RNG_YESNO },
  { "G", PROP_REAL, { 1, PROP_NO_STR }, PROP_NO_RANGE },
  { "T", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGE },
  PROP_NO_PROP };
struct define_t ifile::cirdef =
  { "Ifile", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
