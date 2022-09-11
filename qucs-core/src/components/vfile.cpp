/*
 * vfile.cpp - file based voltage source class implementation
 *
 * Copyright (C) 2007 Gunther Kraut <gn.kraut@t-online.de>
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
#include "vfile.h"

using namespace qucs;

// Constructor creates vfile object in memory.
vfile::vfile () : circuit (2) {
  type = CIR_VFILE;
  setVSource (true);
  setVoltageSources (1);
  interpolType = dataType = 0;
  data = NULL;
  inter = NULL;
}

// Destructor deletes vfile object from memory.
vfile::~vfile () {
  delete data;
  delete inter;
}

void vfile::prepare (void) {

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
		  "independent and the voltage source samples as dependents\n",
		  file);
	return;
      }
      qucs::vector * vs = data->getVariables();    // voltage
      qucs::vector * ts = data->getDependencies(); // time
      inter = new interpolator ();
      inter->rvectors (vs, ts);
      inter->prepare (interpolType, dataType);
    }
  }
}

void vfile::initSP (void) {
  allocMatrixS ();
  setS (NODE_1, NODE_1, 0.0);
  setS (NODE_1, NODE_2, 1.0);
  setS (NODE_2, NODE_1, 1.0);
  setS (NODE_2, NODE_2, 0.0);
}

void vfile::initDC (void) {
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
  prepare ();
  setE (VSRC_1, 0);
}

void vfile::initAC (void) {
  initDC ();
  setE (VSRC_1, 0);
}

void vfile::initTR (void) {
  initDC ();
}

void vfile::calcTR (nr_double_t t) {
  nr_double_t G = getPropertyDouble ("G");
  nr_double_t T = getPropertyDouble ("T");
  nr_double_t u = inter->rinterpolate (t - T);
  setE (VSRC_1, G * u);
}

// properties
PROP_REQ [] = {
  { "File", PROP_STR, { PROP_NO_VAL, "vfile.dat" }, PROP_NO_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Interpolator", PROP_STR, { PROP_NO_VAL, "linear" },
    PROP_RNG_STR3 ("hold", "linear", "cubic") },
  { "Repeat", PROP_STR, { PROP_NO_VAL, "no" }, PROP_RNG_YESNO },
  { "G", PROP_REAL, { 1, PROP_NO_STR }, PROP_NO_RANGE },
  { "T", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGE },
  PROP_NO_PROP };
struct define_t vfile::cirdef =
  { "Vfile", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
