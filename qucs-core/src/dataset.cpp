/*
 * dataset.cpp - dataset class implementation
 *
 * Copyright (C) 2003, 2004, 2005, 2006, 2007 Stefan Jahn <stefan@lkcc.org>
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
#include <errno.h>
#include <assert.h>
#include <cmath>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "strlist.h"
#include "vector.h"
#include "dataset.h"
#include "check_dataset.h"
#include "check_touchstone.h"
#include "check_csv.h"
#include "check_citi.h"
#include "check_zvr.h"
#include "check_mdl.h"

namespace qucs {

// Constructor creates an unnamed instance of the dataset class.
dataset::dataset () : object () {
  variables = dependencies = NULL;
  file = NULL;
}

// Constructor creates an named instance of the dataset class.
dataset::dataset (char * n) : object (n) {
  variables = dependencies = NULL;
  file = NULL;
}

/* The copy constructor creates a new instance based on the given
   dataset object. */
dataset::dataset (const dataset & d) : object (d) {
  file = d.file ? strdup (d.file) : NULL;
  vector * v;
  // copy dependency vectors
  for (v = d.dependencies; v != NULL; v = (vector *) v->getNext ()) {
    addDependency (new vector (*v));
  }
  // copy variable vectors
  for (v = variables; v != NULL; v = (vector *) v->getNext ()) {
    addVariable (new vector (*v));
  }
}

// Destructor deletes a dataset object.
dataset::~dataset () {
  vector * n, * v;
  // delete dependency vectors
  for (v = dependencies; v != NULL; v = n) {
    n = (vector *) v->getNext ();
    delete v;
  }
  // delete variable vectors
  for (v = variables; v != NULL; v = n) {
    n = (vector *) v->getNext ();
    delete v;
  }
  free (file);
}

// This function adds a dependency vector to the current dataset.
void dataset::addDependency (vector * v) {
  if (dependencies) dependencies->setPrev (v);
  v->setNext (dependencies);
  v->setPrev (NULL);
  dependencies = v;
}

// This function removes a dependency vector from the current dataset.
void dataset::delDependency (vector * v) {
  if (dependencies == v) {
    dependencies = (vector *) v->getNext ();
    if (dependencies) dependencies->setPrev (NULL);
  }
  else {
    vector * next = (vector *) v->getNext ();
    vector * prev = (vector *) v->getPrev ();
    prev->setNext (next);
    if (next) next->setPrev (prev);
  }
  delete v;
}

/* The function adds the given list of vectors to the dependency set
   of the current dataset. */
void dataset::addDependencies (vector * v) {
  vector * next;
  for (vector * t = v; t != NULL; t = next) {
    next = (vector *) t->getNext ();
    addDependency (t);
  }
}

// This function appends a dependency vector to the current dataset.
void dataset::appendDependency (vector * v) {
  vector * e;
  if (dependencies) {
    for (e = dependencies; e->getNext (); e = (vector *) e->getNext ()) ;
    v->setPrev (e);
    e->setNext (v);
  }
  else {
    v->setPrev (NULL);
    dependencies= v;
  }
  v->setNext (NULL);
}

/* The function appends the given list of vectors to the dependency
   set of the current dataset. */
void dataset::appendDependencies (vector * v) {
  vector * next;
  for (vector * t = v; t != NULL; t = next) {
    next = (vector *) t->getNext ();
    appendDependency (t);
  }
}

// This function adds a variable vector to the current dataset.
void dataset::addVariable (vector * v) {
  if (variables) variables->setPrev (v);
  v->setNext (variables);
  v->setPrev (NULL);
  variables = v;
}

// This function removes a variable vector from the current dataset.
void dataset::delVariable (vector * v) {
  if (variables == v) {
    variables = (vector *) v->getNext ();
    if (variables) variables->setPrev (NULL);
  }
  else {
    vector * next = (vector *) v->getNext ();
    vector * prev = (vector *) v->getPrev ();
    prev->setNext (next);
    if (next) next->setPrev (prev);
  }
  delete v;
}

/* The function adds the given list of vectors to the variable set of
   the current dataset. */
void dataset::addVariables (vector * v) {
  vector * next;
  for (vector * t = v; t != NULL; t = next) {
    next = (vector *) t->getNext ();
    addVariable (t);
  }
}

// This function appends a variable vector to the current dataset.
void dataset::appendVariable (vector * v) {
  vector * e;
  if (variables) {
    for (e = variables; e->getNext (); e = (vector *) e->getNext ()) ;
    v->setPrev (e);
    e->setNext (v);
  }
  else {
    v->setPrev (NULL);
    variables = v;
  }
  v->setNext (NULL);
}

/* The function appends the given list of vectors to the variable set
   of the current dataset. */
void dataset::appendVariables (vector * v) {
  vector * next;
  for (vector * t = v; t != NULL; t = next) {
    next = (vector *) t->getNext ();
    appendVariable (t);
  }
}

/* This function applies the dependency string list of the given
   vector to the list of vectors appended to this vector. */
void dataset::applyDependencies (vector * v) {
  strlist * deps = v->getDependencies ();
  if (deps != NULL) {
    vector * next;
    for (vector * t = (vector *) v->getNext (); t != NULL; t = next) {
      next = (vector *) t->getNext ();
      if (t->getDependencies () == NULL) {
	t->setDependencies (new strlist (*deps));
      }
    }
  }
}

/* This function returns the dataset vector (both independent and
   dependent) with the given origin.  It returns NULL if there is no
   such vector. */
vector * dataset::findOrigin (char * n) {
  vector * v;
  for (v = variables; v != NULL; v = (vector *) v->getNext ()) {
    char * origin = v->getOrigin ();
    if (origin != NULL && n != NULL && !strcmp (n, origin))
      return v;
  }
  for (v = dependencies; v != NULL; v = (vector *) v->getNext ()) {
    char * origin = v->getOrigin ();
    if (origin != NULL && n != NULL && !strcmp (n, origin))
      return v;
  }
  return NULL;
}

/* This function assigns dependency entries to variable vectors which
   do have the specified origin. */
void dataset::assignDependency (const char * const origin, const char * const depvar) {
  for (vector * v = variables; v != NULL; v = (vector *) v->getNext ()) {
    char * n = v->getOrigin ();
    if (n != NULL && origin != NULL && !strcmp (origin, n)) {
      strlist * deplist = v->getDependencies ();
      if (deplist != NULL) {
	if (!deplist->contains (depvar)) {
	  deplist->append (depvar);
	}
      }
      else {
	deplist = new strlist ();
	deplist->add (depvar);
	v->setDependencies (deplist);
      }
    }
  }
}

// Return non-zero if the given vector is an independent variable vector.
int dataset::isDependency (vector * dep) {
  for (vector * v = dependencies; v != NULL; v = (vector *) v->getNext ())
    if (v == dep) return 1;
  return 0;
}

// Return non-zero if the given vector is a dependent variable vector.
int dataset::isVariable (vector * var) {
  for (vector * v = variables; v != NULL; v = (vector *) v->getNext ())
    if (v == var) return 1;
  return 0;
}

/* The function goes through the list of dependencies in the dataset
   and returns the vector specified by the given name.  Otherwise the
   function returns NULL. */
vector * dataset::findDependency (const char * n) {
  for (vector * v = dependencies; v != NULL; v = (vector *) v->getNext ()) {
    if (!strcmp (v->getName (), n))
      return v;
  }
  return NULL;
}

/* The function goes through the list of variables in the dataset and
   returns the vector specified by the given name.  If there is no
   such variable registered the function returns NULL. */
vector * dataset::findVariable (const std::string &name) {
  for (vector * v = variables; v != NULL; v = (vector *) v->getNext ()) {
    if (!strcmp (v->getName (), name.c_str()))
      return v;
  }
  return NULL;
}

// Returns the number of variable vectors.
int dataset::countVariables (void) {
  int count = 0;
  for (vector * v = variables; v != NULL; v = (vector *) v->getNext ())
    count++;
  return count;
}

// Returns the number of dependency vectors.
int dataset::countDependencies (void) {
  int count = 0;
  for (vector * v = dependencies; v != NULL; v = (vector *) v->getNext ())
    count++;
  return count;
}

// Returns the current output file name.
char * dataset::getFile (void) {
  return file;
}

/* Sets the current output file name.  The file name is used during
   the print functionality of the dataset class. */
void dataset::setFile (const char * f) {
  free (file);
  file = f ? strdup (f) : NULL;
}

/* This function prints the current dataset representation either to
   the specified file name (given by the function setFile()) or to
   stdout if there is no such file name given. */
void dataset::print (void) {

  FILE * f = stdout;

  // open file for writing
  if (file) {
    if ((f = fopen (file, "w")) == NULL) {
      logprint (LOG_ERROR, "cannot create file `%s': %s\n",
		file, strerror (errno));
      return;
    }
  }

  // print header
  fprintf (f, "<Qucs Dataset " PACKAGE_VERSION ">\n");

  // print dependencies
  for (vector * d = dependencies; d != NULL; d = (vector *) d->getNext ()) {
    printDependency (d, f);
  }

  // print variables
  for (vector * v = variables; v != NULL; v = (vector *) v->getNext ()) {
    if (v->getDependencies () != NULL)
      printVariable (v, f);
    else
      printDependency (v, f);
  }

  // close file if necessary
  if (file) fclose (f);
}

/* Prints the given vector as independent dataset vector into the
   given file descriptor. */
void dataset::printDependency (vector * v, FILE * f) {
  // print data header
  fprintf (f, "<indep %s %d>\n", v->getName (), v->getSize ());
  // print data itself
  printData (v, f);
  // print data footer
  fprintf (f, "</indep>\n");
}

/* Prints the given vector as dependent dataset vector into the given
   file descriptor. */
void dataset::printVariable (vector * v, FILE * f) {
  // print data header
  fprintf (f, "<dep %s", v->getName ());
  if (v->getDependencies () != NULL) {
    for (strlistiterator it (v->getDependencies ()); *it; ++it)
      fprintf (f, " %s", *it);
  }
  fprintf (f, ">\n");

  // print data itself
  printData (v, f);

  // print data footer
  fprintf (f, "</dep>\n");
}

/* This function is a helper routine for the print() functionality of
   the dataset class.  It prints the data items of the given vector
   object to the given output stream. */
void dataset::printData (vector * v, FILE * f) {
  for (int i = 0; i < v->getSize (); i++) {
    nr_complex_t c = v->get (i);
    if (imag (c) == 0.0) {
      fprintf (f, "  %+." "20" "e\n", (double) real (c));
    }
    else {
      fprintf (f, "  %+." "20" "e%cj%." "20" "e\n", (double) real (c), 
	       imag (c) >= 0.0 ? '+' : '-', (double) fabs (imag (c)));
    }
  }
}

/* This static function read a full dataset from the given file and
   returns it.  On failure the function emits appropriate error
   messages and returns NULL. */
dataset * dataset::load (const char * file) {
  FILE * f;
  if ((f = fopen (file, "r")) == NULL) {
    logprint (LOG_ERROR, "error loading `%s': %s\n", file, strerror (errno));
    return NULL;
  }
  dataset_in = f;
  dataset_restart (dataset_in);
  if (dataset_parse () != 0) {
    fclose (f);
    return NULL;
  }
  if (dataset_result != NULL) {
    if (dataset_check (dataset_result) != 0) {
      fclose (f);
      delete dataset_result;
      return NULL;
    }
  }
  fclose (f);
  dataset_lex_destroy ();
  dataset_result->setFile (file);
  return dataset_result;
}

/* This static function read a full dataset from the given touchstone
   file and returns it.  On failure the function emits appropriate
   error messages and returns NULL. */
dataset * dataset::load_touchstone (const char * file) {
  FILE * f;
  if ((f = fopen (file, "r")) == NULL) {
    logprint (LOG_ERROR, "error loading `%s': %s\n", file, strerror (errno));
    return NULL;
  }
  touchstone_in = f;
  touchstone_restart (touchstone_in);
  if (touchstone_parse () != 0) {
    fclose (f);
    return NULL;
  }
  if (touchstone_check () != 0) {
    fclose (f);
    return NULL;
  }
  fclose (f);
  touchstone_lex_destroy ();
  touchstone_result->setFile (file);
  return touchstone_result;
}

/* This static function read a full dataset from the given CSV file
   and returns it.  On failure the function emits appropriate error
   messages and returns NULL. */
dataset * dataset::load_csv (const char * file) {
  FILE * f;
  if ((f = fopen (file, "r")) == NULL) {
    logprint (LOG_ERROR, "error loading `%s': %s\n", file, strerror (errno));
    return NULL;
  }
  csv_in = f;
  csv_restart (csv_in);
  if (csv_parse () != 0) {
    fclose (f);
    return NULL;
  }
  if (csv_check () != 0) {
    fclose (f);
    return NULL;
  }
  fclose (f);
  csv_lex_destroy ();
  csv_result->setFile (file);
  return csv_result;
}

/* The function read a full dataset from the given CITIfile and
   returns it.  On failure the function emits appropriate error
   messages and returns NULL. */
dataset * dataset::load_citi (const char * file) {
  FILE * f;
  if ((f = fopen (file, "r")) == NULL) {
    logprint (LOG_ERROR, "error loading `%s': %s\n", file, strerror (errno));
    return NULL;
  }
  citi_in = f;
  citi_restart (citi_in);
  if (citi_parse () != 0) {
    fclose (f);
    return NULL;
  }
  if (citi_check () != 0) {
    fclose (f);
    return NULL;
  }
  fclose (f);
  citi_lex_destroy ();
  citi_result->setFile (file);
  return citi_result;
}

/* The function read a full dataset from the given ZVR file and
   returns it.  On failure the function emits appropriate error
   messages and returns NULL. */
dataset * dataset::load_zvr (const char * file) {
  FILE * f;
  if ((f = fopen (file, "r")) == NULL) {
    logprint (LOG_ERROR, "error loading `%s': %s\n", file, strerror (errno));
    return NULL;
  }
  zvr_in = f;
  zvr_restart (zvr_in);
  if (zvr_parse () != 0) {
    fclose (f);
    return NULL;
  }
  if (zvr_check () != 0) {
    fclose (f);
    return NULL;
  }
  fclose (f);
  zvr_lex_destroy ();
  if (zvr_result) zvr_result->setFile (file);
  return zvr_result;
}

/* The function read a full dataset from the given MDL file and
   returns it.  On failure the function emits appropriate error
   messages and returns NULL. */
dataset * dataset::load_mdl (const char * file) {
  FILE * f;
  if ((f = fopen (file, "r")) == NULL) {
    logprint (LOG_ERROR, "error loading `%s': %s\n", file, strerror (errno));
    return NULL;
  }
  mdl_in = f;
  mdl_restart (mdl_in);
  if (mdl_parse () != 0) {
    fclose (f);
    return NULL;
  }
  if (mdl_check () != 0) {
    fclose (f);
    return NULL;
  }
  fclose (f);
  mdl_lex_destroy ();
  if (mdl_result) mdl_result->setFile (file);
  return mdl_result;
}

} // namespace qucs
