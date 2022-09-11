/*
 * environment.cpp - variable environment class implementation
 *
 * Copyright (C) 2004, 2005, 2006, 2007, 2009 Stefan Jahn <stefan@lkcc.org>
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

#include <list>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "complex.h"
#include "variable.h"
#include "equation.h"
#include "logging.h"
#include "environment.h"

using namespace qucs::eqn;

namespace qucs {

// Constructor creates an unnamed instance of the environment class.
environment::environment () :
  name(),
  children()
{
  root = NULL;
  solvee = NULL;
  checkee = NULL;
  defs = NULL;
  iscopy = false;
}


// Constructor creates a named instance of the environment class.
environment::environment (const std::string & p_name) :
  name(p_name),
  children()
{
  root = NULL;
  solvee = NULL;
  checkee = NULL;
  defs = NULL;
  iscopy = false;
}

/* The copy constructor creates a new instance of the environment
   class based on the given environment object. */
environment::environment (const environment & e)  {
  this->name = e.name;
  copyVariables (e.root);
  solvee = e.solvee;
  checkee = e.checkee;
  defs = e.defs;
  iscopy = true;
  children = std::list<environment *>();
}

/* Very alike the copy constructor the function copies the content of
   the given environment into the calling environment. */
void environment::copy (const environment & e) {
  this->name = e.name;
  deleteVariables ();
  copyVariables (e.root);
  solvee = e.solvee;
  checkee = e.checkee;
  defs = e.defs;
  iscopy = true;
  children = std::list<environment *>();
}

// Destructor deletes the environment object.
environment::~environment () {
  deleteVariables ();
  // delete solver and checker if this is not just a reference
  if (!iscopy) {
    if (solvee)
      delete solvee;
    if (checkee) {
      checkee->setEquations (NULL);
      delete checkee;
    }
  }
  // delete children
  for (auto it = children.begin(); it != children.end(); ++it) {
    environment * etmp = *it;
    delete etmp;
  }
}


/* This function copies all variables in the given variable list into
   an environment. */
void environment::copyVariables (variable * org) {
  variable * var;
  root = NULL;
  while (org != NULL) {
    // copy variable (references only)
    var = new variable (*org);
    constant * c; reference * r;
    // depending on variable type copy values too
    switch (var->getType ()) {
    case VAR_CONSTANT:
      c = new constant (*(var->getConstant ()));
      var->setConstant (c);
      break;
    case VAR_VALUE:
      c = new constant (*(org->getValue ()));
      var->setValue (c);
      break;
    case VAR_REFERENCE:
      r = new reference ();
      r->n = strdup (var->getReference()->n);
      var->setReference (r);
      break;
    }
    var->setNext (root);
    root = var;
    org = org->getNext ();
  }
}

// Deletes all variable in the environment.
void environment::deleteVariables (void) {
  variable * n;
  for (variable * var = root; var != NULL; var = n) {
    n = var->getNext ();
    if (var->getType () == VAR_CONSTANT)
      delete var->getConstant ();
    else if (var->getType () == VAR_VALUE)
      delete var->getValue ();
    else if (var->getType () == VAR_SUBSTRATE)
      delete var->getSubstrate ();
    else if (var->getType () == VAR_REFERENCE) {
      constant * c = var->getReference()->getResult ();
      delete c;
      delete var->getReference ();
    }
    delete var;
  }
  root = NULL;
}

/* This function adds a variable to the environment. */
void environment::addVariable (variable * const var, const bool pass) {
  var->setNext (root);
  var->setPassing (pass);
  this->root = var;
}

/* This function looks for the variable name in the environment and
   returns it if possible.  Otherwise the function returns NULL. */
variable * environment::getVariable (const char * const n) const {
  for (variable * var = root; var != NULL; var = var->getNext ()) {
    if (var->getType () != VAR_VALUE)
      if (!strcmp (var->getName (), n))
	return var;
  }
  return NULL;
}

// The function runs the equation checker for this environment.
int environment::equationChecker (const int noundefined) const {
  checkee->setDefinitions (defs);
  return checkee->check (noundefined);
}

// The function runs the equation solver for this environment.
int environment::equationSolver (dataset * const data) {
  checkee->setDefinitions (defs);
  solvee->setEquations (checkee->getEquations ());
  int err = solvee->solve (data);
  checkee->setEquations (solvee->getEquations ());
  return err;
}

// The function runs the equation solver for this environment without
// checking it previously and without considering an additional
// dataset.
void environment::equationSolver (void) {
  checkee->setDefinitions (defs);
  solvee->setEquations (checkee->getEquations ());
  solvee->evaluate ();
  checkee->setEquations (solvee->getEquations ());
}


/* The function solves the equations of the current environment object
   as well as these of its children, updates the variables and passes
   the arguments to each children. */
int environment::runSolver (void) {
  int ret = 0;

  // solve equations in current environment
  ret |= equationSolver (NULL);
  fetchConstants ();

  // cycle through children
  for(auto it = children.begin(); it != children.end(); ++it) {
    // pass constants to solver
    (*it)->passConstants ();
    // pass references
    (*it)->updateReferences (this);
    // actually run the solver
    ret |= (*it)->runSolver ();
#if 0
    // save local results
    (*it)->saveResults ();
#endif
  }

  return ret;
}

/* Passes the constants of the environment to the equation solver.
   This is necessary since equally typed environments use the same
   equation checker and solver. */
void environment::passConstants (void) {
  for (variable * var = root; var != NULL; var = var->getNext ()) {
    if (var->getPassing () && var->getType () == VAR_CONSTANT) {
      constant * c = var->getConstant ();
      setDouble (var->getName (), c->d);
    }
  }
}

/* Fetches the values of variables from the equation solver. */
void environment::fetchConstants (void) {
  for (variable * var = root; var != NULL; var = var->getNext ()) {
    if (var->getType () == VAR_CONSTANT) {
      constant * c = var->getConstant ();
      switch (c->getType ()) {
      case TAG_DOUBLE:
	c->d = getDouble (var->getName ());
	break;
      case TAG_VECTOR:
	*c->v = getVector (var->getName ());
	break;
      }
    }
  }
}

/* Looks through the environment variables for a given variable name
   being a saved value and returns the variable pointer or NULL if
   there is no such variable. */
variable * environment::findValue (char * n) {
  for (variable * var = root; var != NULL; var = var->getNext ()) {
    if (var->getType () == VAR_VALUE)
      if (!strcmp (var->getName (), n))
	return var;
  }
  return NULL;
}

/* Puts the given variable name and its computed result into the list
   of environment variables. */
void environment::setValue (char * n, constant * value) {
  variable * var = findValue (n);
  if (var != NULL) {
    // replace variable
    delete var->getValue ();
    var->setValue (new constant (*value));
  } else {
    // create new variable
    var = new variable (n);
    var->setValue (new constant (*value));
    addVariable (var);
  }
}

// Local macro definition to go through the list of equations.
#define foreach_equation(eqn)                        \
  for (assignment * (eqn) = A (equations);           \
       (eqn) != NULL; (eqn) = A ((eqn)->getNext ()))

// Short helper macro.
#define A(a) ((assignment *) (a))

/* The function puts local variables (parameters and equation results)
   into the set of environment variables. */
void environment::saveResults (void) {
  node * equations = checkee->getEquations ();
  // go through equations
  foreach_equation (eqn) {
    char * inst = eqn->getInstance ();
    if (inst != NULL && eqn->evaluated) {
      char * result = A(eqn)->result;
      if ((inst[0] != '#' && !strchr (result, '.')) ||
	  !strcmp (inst, "#subcircuit")) {
	setValue (result, eqn->getResult ());
      }
    }
  }
}

/* This function looks through all variables which are references.  If
   found the variable gets resolved in the upper (parent) environment
   and the value put into the result of the reference as well as into
   the equation checker of the current environment. */
void environment::updateReferences (environment * up) {
  for (variable * var = root; var != NULL; var = var->getNext ()) {
    if (var->getType () == VAR_REFERENCE) {
      reference * r = var->getReference ();
      // possible because no self-referring subcircuit types possible
      nr_double_t d = up->getDouble (r->n);
      constant * c = r->getResult ();
      c->d = d;
      setDouble (var->getName (), d);
    }
  }
}

// Returns vector of an assignment in the equation checker.
qucs::vector environment::getVector (const char * const ident) const {
  return checkee->getVector (ident);
}

// Returns double value of an assignment in the equation checker.
nr_double_t environment::getDouble (const char * const ident) const {
  return checkee->getDouble (ident);
}

// Sets the double value of an assignment in the equation checker.
void environment::setDouble (const char * const ident, const nr_double_t val) {
  checkee->setDouble (ident, val);
}

// Return double value of a variable in the environment.
nr_double_t environment::getDoubleConstant (const char * const ident) const {
  variable * var = getVariable (ident);
  if (var != NULL && var->getType () == VAR_CONSTANT) {
    constant * c = var->getConstant ();
    return c->d;
  }
  return 0.0;
}

// Sets the double value of a variable in the environment.
void environment::setDoubleConstant (const char * const ident, nr_double_t val) {
  variable * var = getVariable (ident);
  if (var != NULL && var->getType () == VAR_CONSTANT) {
    constant * c = var->getConstant ();
    c->d = val;
  }
}

// Returns the referenced value of a variable in the environment.
char * environment::getDoubleReference (const char * const ident) const {
  variable * var = getVariable (ident);
  if (var != NULL &&  var->getType () == VAR_REFERENCE) {
    reference * r = var->getReference ();
    return r->n;
  }
  return NULL;
}

// Sets the referenced value of a variable in the environment.
void environment::setDoubleReference (const char * const ident, char * val) {
  variable * var = getVariable (ident);
  if (var != NULL) {
    if (var->getType () == VAR_CONSTANT) {
      // its a constant, so make it a reference
      delete var->getConstant ();
      reference * r = new reference ();
      r->n = strdup (val);
      constant * c = new constant (TAG_DOUBLE);
      r->setResult (c);
      var->setReference (r);
    }
    else if (var->getType () == VAR_REFERENCE) {
      // just apply the reference
      reference * r = var->getReference ();
      free (r->n);
      r->n = strdup (val);
    }
  }
}

//! Prints the environment.
void environment::print (const bool all) const {
  logprint (LOG_STATUS, "environment %s\n",this->name.c_str());
  for (variable * var = root; var != NULL; var = var->getNext ()) {
    logprint (LOG_STATUS, "  %s [%s]\n", var->getName (), var->toString ());
  }
  for (auto it = children.begin(); it != children.end() ; ++it) {
    logprint (LOG_STATUS, "  %s\n", (*it)->name.c_str ());
  }
  if (all) {
    for (auto it = children.begin(); it != children.end() ; ++it)
      (*it)->print ();
  }
}

} // namespace qucs
