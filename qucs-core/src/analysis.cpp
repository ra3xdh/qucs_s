/*
 * analysis.cpp - analysis class implementation
 *
 *
 * Copyright (C) 2003-2008 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: analysis.cpp 1869 2013-03-06 12:50:21Z crobarcro $
 *
 */

 /*! \file analysis.cpp
  * \brief Implementation of the analysis class.
  *
  * Contains the implementation of the analysis class.
  *
  */

#if HAVE_CONFIG_H
# include <config.h>
#endif

//#include <stdio.h>
//#include <stdlib.h>
#include <string.h>

#include "object.h"
#include "complex.h"
#include "sweep.h"
#include "vector.h"
#include "strlist.h"
#include "dataset.h"
#include "ptrlist.h"
#include "analysis.h"

namespace qucs {

//Constructor. Creates an unnamed instance of the analysis class.
analysis::analysis () : object () {
  data = NULL;
  subnet = NULL;
  env = NULL;
  actions = NULL;
  type = ANALYSIS_UNKNOWN;
  runs = 0;
  progress = true;
}

// Constructor creates a named instance of the analysis class.
analysis::analysis (const std::string &n) : object (n) {
  data = NULL;
  subnet = NULL;
  env = NULL;
  actions = NULL;
  type = ANALYSIS_UNKNOWN;
  runs = 0;
  progress = true;
}

// Destructor deletes the analysis class object.
analysis::~analysis () {
  delete actions;
}

/* The copy constructor creates a new instance of the analysis class
   based on the given analysis object. */
analysis::analysis (analysis & a) : object (a) {
  data = a.data;
  subnet = a.subnet;
  env = a.env;
  actions = a.actions ? new ptrlist<analysis> (*a.actions) : NULL;
  type = a.type;
  runs = a.runs;
  progress = a.progress;
}

/* This function adds the given analysis to the actions being
   associated with the current analysis object. */
void analysis::addAnalysis (analysis * a) {
  if (!actions) actions = new ptrlist<analysis> ();
  actions->push_front (a);
}

/* This function deletes the given analysis from the actions being
   associated with the current analysis object. */
void analysis::delAnalysis (analysis * a) {
  if (actions != nullptr) {
    actions->remove (a);
  }
}

/* The following function creates a sweep object depending on the
   analysis's properties.  Supported sweep types are: linear,
   logarithmic, lists and constants. */
sweep * analysis::createSweep (const std::string& n) {
  sweep * swp = NULL;
  // get type of sweep
  const char * const type = getPropertyString ("Type");

  // linearly or logarithmically stepped sweeps
  if (!strcmp (type, "lin") || !strcmp (type, "log")) {
    nr_double_t start = getPropertyDouble ("Start");
    nr_double_t stop = getPropertyDouble ("Stop");
    int points = getPropertyInteger ("Points");
    if (!strcmp (type, "lin")) {
      swp = new linsweep (n);
      ((linsweep *) swp)->create (start, stop, points);
    }
    else if (!strcmp (type, "log")) {
      swp = new logsweep (n);
      ((logsweep *) swp)->create (start, stop, points);
    }
  }

  // lists of values
  else if (!strcmp (type, "list")) {
    vector * values = getPropertyVector ("Values");
    int points = values->getSize ();
    swp = new lstsweep (n);
    ((lstsweep *) swp)->create (points);
    for (int i = 0; i < values->getSize (); i++)
      swp->set (i, real (values->get (i)));
  }

  // constant value
  else if (!strcmp (type, "const")) {
    nr_double_t val = getPropertyDouble ("Values");
    swp = new consweep (n);
    ((consweep *) swp)->create (1);
    swp->set (0, val);
  }

  swp->setParent (this);
  return swp;
}

/* Saves the given variable into the dataset.  Creates the dataset
   vector if necessary. */
  void analysis::saveVariable (const std::string &n, nr_complex_t z, vector * f) {
  vector * d;
  if ((d = data->findVariable (n)) == NULL) {
    d = new vector (n);
    if (f != NULL) {
      d->setDependencies (new strlist ());
      d->getDependencies()->add (f->getName ());
    }
    d->setOrigin (getName ());
    data->addVariable (d);
  }
  d->add (z);
}

} // namespace qucs
