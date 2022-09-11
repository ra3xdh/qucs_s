/*
 * input.cpp - input netlist class implementation
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

#include "logging.h"
#include "component.h"
#include "components.h"
#include "net.h"
#include "variable.h"
#include "property.h"
#include "environment.h"
#include "nodeset.h"
#include "input.h"
#include "check_netlist.h"
#include "equation.h"
#include "module.h"

namespace qucs {

// Global variables.
int netlist_check = 0;

// Constructor creates an unnamed instance of the input class.
input::input () : object () {
  fd = stdin;
  subnet = NULL;
  env = NULL;
}

// Constructor creates an named instance of the input class.
input::input (char * file) : object (file) {
  if ((fd = fopen (file, "r")) == NULL) {
    logprint (LOG_ERROR, "cannot open file `%s': %s, using stdin instead\n",
	      file, strerror (errno));
    fd = stdin;
  }
  subnet = NULL;
  env = NULL;
}

// Destructor deletes an input object.
input::~input () {
  if (fd != stdin) fclose (fd);
}

/* This function scans, parses and checks a netlist from the input
   file (specified by the constructor call) or stdin if there is no
   such file.  Afterwards the function builds the netlist
   representation and stores it into the given netlist object.  The
   function returns zero on success and non-zero otherwise. */
int input::netlist (net * netlist) {

  // tell the scanner to use the specified file
  netlist_in = getFile ();

  // save the netlist object
  subnet = netlist;

  logprint (LOG_STATUS, "parsing netlist...\n");

  if (netlist_parse () != 0)
    return -1;

  logprint (LOG_STATUS, "checking netlist...\n");
  if (netlist_checker (env) != 0)
    return -1;

  if (netlist_checker_variables (env) != 0)
    return -1;

#if DEBUG
  netlist_list ();
#endif /* DEBUG */
  netlist_status ();

  logprint (LOG_STATUS, "creating netlist...\n");
  factory ();

  netlist_destroy ();
  return 0;
}

/* The little helper function creates a vector given by the list of
   values. */
qucs::vector * input::createVector (struct value_t * values) {
  qucs::vector * v = new qucs::vector ();
  for (; values != NULL; values = values->next) v->add (values->value);
  return v;
}

/* This function builds up the netlist representation from the checked
   netlist input.  It creates circuit components as necessary. */
void input::factory (void) {

  struct definition_t * def, * next;
  struct node_t * nodes;
  struct pair_t * pairs;
  circuit * c;
  object * o;
  analysis * a;
  substrate * s;
  nodeset * n;
  int i;

  // go through the list of input definitions
  for (def = definition_root; def != NULL; def = next) {
    next = def->next;
    // handle actions
    if (def->action) {
      if ((a = createAnalysis (def->type)) != NULL) {
	a->setName (def->instance);

	// add the properties to analysis
	for (pairs = def->pairs; pairs != NULL; pairs = pairs->next)
	  if (pairs->value->ident) {
	    if (pairs->value->var && strcmp (pairs->key, "Param")) {
	      variable * v;
	      if ((v = def->env->getVariable (pairs->value->ident)) != NULL) {
		// equation variable reference in analysis property
		a->addProperty (pairs->key, v);
	      }
	      else {
		// should not be reached!
		a->addProperty (pairs->key, pairs->value->ident);
	      }
	    }
	    else {
	      // usual string property
	      a->addProperty (pairs->key, pairs->value->ident);
	    }
	  } else {
	    if (pairs->value->var) {
	      // add list sweeps and constants to the properties
	      variable * v = new variable (pairs->key);
	      eqn::constant * c = new eqn::constant (eqn::TAG_VECTOR);
	      c->v = createVector (pairs->value);
	      v->setConstant (c);
	      a->addProperty (pairs->key, v);
	    }
	    else {
	      a->addProperty (pairs->key, pairs->value->value);
	    }
	  }
	// additionally add missing optional properties
	assignDefaultProperties (a, def->define);
	a->setEnv (def->env);
	subnet->insertAnalysis (a);
      }
      // remove this definition from the list
      definition_root = netlist_unchain_definition (definition_root, def);
    }
  }

  // go through the list of input definitions
  for (def = definition_root; def != NULL; def = next) {
    next = def->next;
    // handle substrate definitions
    if (!def->action && def->substrate) {
      if ((s = createSubstrate (def->type)) != NULL) {
	s->setName (def->instance);

	// add the properties to substrate
	for (pairs = def->pairs; pairs != NULL; pairs = pairs->next)
	  if (pairs->value->ident) {
	    // a variable
	    if (pairs->value->var) {
	      // at this stage it should be ensured that the variable is
	      // already in the root environment
	      variable * v = def->env->getVariable (pairs->value->ident);
	      s->addProperty (pairs->key, v);
	    }
	    // a usual string property
	    else {
	      s->addProperty (pairs->key, pairs->value->ident);
	    }
	  } else {
	    s->addProperty (pairs->key, pairs->value->value);
	  }
	// additionally add missing optional properties
	assignDefaultProperties (s, def->define);

	// put new substrate definition into environment
	char * n = strrchr (def->instance, '.');
	variable * v = new variable (n ? n + 1 : def->instance);
	v->setSubstrate (s);
	def->env->addVariable (v);
      }
      // remove this definition from the list
      definition_root = netlist_unchain_definition (definition_root, def);
    }
    // handle nodeset definitions
    else if (!def->action && def->nodeset) {
      n = new nodeset ();
      n->setName (def->nodes->node);
      n->setValue (def->pairs->value->value);
      subnet->addNodeset (n);
      // remove this definition from the list
      definition_root = netlist_unchain_definition (definition_root, def);
    }
  }

  // go through the list of input definitions
  for (def = definition_root; def != NULL; def = next) {
    next = def->next;
    // handle component definitions
    if (!def->action && !def->substrate && !def->nodeset) {
      c = createCircuit (def->type);
      assert (c != NULL);
      o = (object *) c;
      c->setName (def->instance);
      c->setNonLinear (def->nonlinear != 0);
      c->setSubcircuit (def->subcircuit == nullptr ? "" : def->subcircuit);

      // change size (number of ports) of variable sized components
      if (c->isVariableSized ()) {
	c->setSize (def->ncount);
      }
      // add appropriate nodes to circuit
      for (i = 0, nodes = def->nodes; nodes; nodes = nodes->next, i++)
	if (i < c->getSize ())
	  c->setNode (i, nodes->node);

      // add the properties to circuit
      for (pairs = def->pairs; pairs != NULL; pairs = pairs->next) {
	if (pairs->value == NULL) {
	  // zero-length value lists
	  variable * v = new variable (pairs->key);
	  eqn::constant * c = new eqn::constant (eqn::TAG_VECTOR);
	  c->v = new qucs::vector ();
	  v->setConstant (c);
	  o->addProperty (pairs->key, v);
	}
	else if (pairs->value->ident) {
	  if (pairs->value->var) {
	    // at this stage it should be ensured that the variable is
	    // already in the root environment
	    variable * v = def->env->getVariable (pairs->value->ident);
	    o->addProperty (pairs->key, v);
	  } else {
	    if (pairs->value->subst) {
	      variable * v = def->env->getVariable (pairs->value->ident);
	      c->setSubstrate (v->getSubstrate ());
	    }
	    o->addProperty (pairs->key, pairs->value->ident);
	  }
	} else {
	  if (pairs->value->var) {
	    // add value lists to the properties
	    variable * v = new variable (pairs->key);
	    eqn::constant * c = new eqn::constant (eqn::TAG_VECTOR);
	    c->v = createVector (pairs->value);
	    v->setConstant (c);
	    o->addProperty (pairs->key, v);
	  } else {
	    o->addProperty (pairs->key, pairs->value->value);
	  }
	}
      }
      // set local circuit environment
      c->setEnv (def->env);

      // additionally add missing optional properties
      assignDefaultProperties (c, def->define);

      // insert the circuit into the netlist object
      subnet->insertCircuit (c);

      // remove this definition from the list
      definition_root = netlist_unchain_definition (definition_root, def);
    }
  }
}

/* This static function applies the optional missing properties's
   default values to the given object. */
void input::assignDefaultProperties (object * obj, struct define_t * def) {
  // go through optional properties
  for (int i = 0; PROP_IS_PROP (def->optional[i]); i++) {
    // is the property already assigned ?
    if (!obj->hasProperty (def->optional[i].key)) {
      if (PROP_IS_VAL (def->optional[i])) {
	// add double property
	obj->addProperty (def->optional[i].key,
			  def->optional[i].defaultval.d,true);
      }
      else {
	// add string property
	obj->addProperty (def->optional[i].key,
	    	          def->optional[i].defaultval.s,true);
      }
    }
  }
}

// The function creates components specified by the type of component.
circuit * input::createCircuit (char * type) {
  module * m;
  if ((m = module::modules.get (type)) != NULL)
    return m->circreate ();

  logprint (LOG_ERROR, "no such circuit type `%s'\n", type);
  return NULL;
}

// The function creates an analysis specified by the type of analysis.
analysis * input::createAnalysis (char * type) {
  module * m;
  if ((m = module::modules.get (type)) != NULL)
    return m->anacreate ();

  logprint (LOG_ERROR, "no such analysis type `%s'\n", type);
  return NULL;
}

// The function creates a substrate specified by the type of substrate.
substrate * input::createSubstrate (char * type) {
  if (!strcmp (type, "SUBST"))
    return new substrate ();

  logprint (LOG_ERROR, "no such substrate type `%s'\n", type);
  return NULL;
}

} // namespace qucs
