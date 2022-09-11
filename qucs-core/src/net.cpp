/*
 * net.cpp - net class implementation
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
#include <assert.h>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "strlist.h"
#include "vector.h"
#include "dataset.h"
#include "net.h"
#include "tee.h"
#include "open.h"
#include "itrafo.h"
#include "ptrlist.h"
#include "analysis.h"
#include "nodelist.h"
#include "nodeset.h"
#include "equation.h"
#include "environment.h"
#include "component_id.h"

namespace qucs {

// Constructor creates an unnamed instance of the net class.
net::net () : object () {
  root = drop = NULL;
  nPorts = nCircuits = nSources = 0;
  insertedNodes = inserted = reduced = 0;
  actions = new ptrlist<analysis> ();
  orgacts = new ptrlist<analysis> ();
  env = NULL;
  nset = NULL;
  srcFactor = 1;
}

// Constructor creates a named instance of the net class.
net::net (const std::string &n) : object (n) {
  root = drop = NULL;
  nPorts = nCircuits = nSources = 0;
  insertedNodes = inserted = reduced = 0;
  actions = new ptrlist<analysis> ();
  orgacts = new ptrlist<analysis> ();
  env = NULL;
  nset = NULL;
  srcFactor = 1;
}

// Destructor deletes the net class object.
net::~net () {
  circuit * n;
  // delete each and every circuit
  for (circuit * c = root; c != NULL; c = n) {
    n = (circuit *) c->getNext ();
    delete c;
  }
  // delete original actions 
  for(auto * element : *orgacts) {
    delete element;
    element = nullptr;
  }

  delete orgacts;
  // delete nodeset
  delNodeset ();
  delete actions;
}

/* The copy constructor creates a new instance of the net class based
   on the given net object. */
net::net (net & n) : object (n) {
  root = drop = NULL;
  nPorts = nCircuits = nSources = 0;
  insertedNodes = inserted = reduced = 0;
  actions = n.actions ? new ptrlist<analysis> (*n.actions) : NULL;
  orgacts = new ptrlist<analysis> ();
  env = n.env;
  nset = NULL;
  srcFactor = 1;
}

/* This function prepends the given circuit to the list of registered
   circuits. */
void net::insertCircuit (circuit * c) {
#if 0
  assert (!containsCircuit (c));
#endif

  // chain circuit appropriately
  if (root) root->setPrev (c);
  c->setNext (root);
  c->setPrev (NULL);
  root = c;
  nCircuits++;
  c->setEnabled (1);
  c->setNet (this);

  /* handle AC power sources as s-parameter ports if it is not part of
     a subcircuit */
  if (c->getType () == CIR_PAC && c->getSubcircuit ().empty()) {
    nPorts++;
    if (!c->getPort ()) c->setPort (c->getPropertyInteger ("Num"));
  }
  // handle DC voltage sources
  if (c->getVoltageSources () > 0) {
    if (c->getVoltageSource () < 0) c->setVoltageSource (nSources);
    nSources += c->getVoltageSources ();
  }
}

/* The function removes the given circuit from the list of registered
   circuits. */
void net::removeCircuit (circuit * c, int dropping) {
#if 0
  assert (containsCircuit (c));
#endif

  // adjust the circuit chain appropriately
  if (c == root) {
    root = (circuit *) c->getNext ();
    if (root) root->setPrev (NULL);
  }
  else {
    if (c->getNext ()) c->getNext()->setPrev (c->getPrev ());
    c->getPrev()->setNext (c->getNext ());
  }
  nCircuits--;
  c->setEnabled (0);
  c->setNet (NULL);
  if (c->getPort ()) nPorts--;
  if (c->getVoltageSource () >= 0) nSources -= c->getVoltageSources ();

  // shift the circuit object to the drop list
  if (c->isOriginal ()) {
    if (dropping) {
      if (drop) drop->setPrev (c);
      c->setNext (drop);
      c->setPrev (NULL);
      drop = c;
    }
  }
  // really destroy the circuit object
  else delete c;
}

/* The function returns non-zero if the given circuit is already part
   of the netlist. It returns zero if not. */
int net::containsCircuit (circuit * cand) {
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ())
    if (c == cand) return 1;
  return 0;
}

/* This function prepends the given analysis to the list of registered
   analyses. */
void net::insertAnalysis (analysis * a) {
  orgacts->push_front (a);
  actions->push_front (a);
}

/* The function removes the given analysis from the list of registered
   analyses. */
void net::removeAnalysis (analysis * a) {
  actions->remove(a);
}

/* The function returns the analysis associated with the netlist
   object specified by the given instance name and returns NULL if
   there is no such analysis. */
analysis * net::findAnalysis (const std::string &n) const {
  for (auto *a : *actions) {
    if (a->getName ()== n)
      return a;
  }
  return NULL;
}

/* The function returns the analysis associated with the netlist
   object specified by the given type of analysis and returns NULL if
   there is no such analysis. */
analysis * net::findAnalysis (int type) {
  for (auto *a: *actions) {
    if (a->getType () == type)
      return a;
  }
  return NULL;
}

/* Looks recursively for a type of analysis. */
int net::containsAnalysis (analysis * child, int type) {
  ptrlist<analysis> * alist = child->getAnalysis ();
  if(alist != nullptr) {
    for (auto *a : *alist) {
      if (a->getType () == type)
	return 1;
      else if (a->getType () == ANALYSIS_SWEEP)
	return containsAnalysis (a, type);
    }
  }
  return 0;
}

/* This function runs all registered analyses applied to the current
   netlist, except for external analysis types. */
dataset * net::runAnalysis (int &err) {
  dataset * out = new dataset ();

  // apply some data to all analyses
  for (auto *a : *actions) {
    if (!a->isExternal ())
    {
      a->setNet (this);
      a->setData (out);
    }
  }

  // re-order analyses
  orderAnalysis ();

  // initialize analyses
  for (auto *a: * actions) {
    if (!a->isExternal ())
    {
      err |= a->initialize ();
    }
  }

  // solve the analyses
  for (auto *a: * actions) {
    if (!a->isExternal ())
    {
      a->getEnv()->runSolver ();
      err |= a->solve ();
    }
  }

  // cleanup analyses
  for (auto *a: *actions) {
    if (!a->isExternal ())
    {
        err |= a->cleanup ();
    }
  }

  return out;
}

/* The function returns the analysis with the second lowest order.  If
   there is no recursive sweep it returns NULL. */
analysis * net::findSecondOrder (void) {
  analysis * parent = NULL;
  for (auto *a : *actions) {
    // parameter sweeps are potential parent sweeps
    if (a->getType () == ANALYSIS_SWEEP) {
      // find the appropriate sub analysis
      analysis * child = getChildAnalysis (a);
      if (child != NULL) {
	// check if child is not another variable sweep
	if (child->getType () != ANALYSIS_SWEEP) {
	  parent = a;
	  break;
	}
	// check if the child's child is still in the analysis list
	else if (getChildAnalysis (child) == NULL) {
	  parent = a;
	  break;
	}
      }
    }
  }
  return parent;
}

/* The function reorders (prioritizes) the registered analysis to the
   netlist object.  In fact it chains the analyses to be executed in
   a certain order. */
void net::orderAnalysis (void) {
  analysis * parent, * child;
  analysis * dc = findAnalysis (ANALYSIS_DC);
  int dcApplied = 0;
  do {
    // get second order sweep
    if ((parent = findSecondOrder ()) != NULL) {
      child = getChildAnalysis (parent);
      removeAnalysis (child);
      // apply sub-analysis to each parent analysis if any
      if (actions != nullptr) {
	for (auto *a: *actions) {
	  const char * cn = getChild (a);
	  if (cn != NULL && !strcmp (cn, child->getName ())) {
	    a->addAnalysis (child);
	    // apply DC analysis if necessary
	    if (child->getType () != ANALYSIS_DC &&
		child->getType () != ANALYSIS_SWEEP && dc != NULL) {
	      if (!dcApplied)
		removeAnalysis (dc);
	      a->addAnalysis (dc);
	      dcApplied++;
	    }
	  }
	}
      }
      // sort the sub-analysis of each parent
      for (auto *a: *actions) {
	sortChildAnalyses (a);
      }
    }
  } while (parent != NULL);

  // sort the parent analyses
  parent = new analysis ();
  parent->setAnalysis (actions);
  sortChildAnalyses (parent);
  actions = new ptrlist<analysis> (*(parent->getAnalysis ()));
  delete parent;
}

// This function sorts the analyses of the given parent analysis.
void net::sortChildAnalyses (analysis * parent) {
  ptrlist<analysis> * alist = parent->getAnalysis ();
  if (alist != nullptr) {

    for (auto it = alist->begin(); it != alist->end(); /* empty */) {
      // Copy the value of the element (a pointer), and advance the
      // iterator prior to manipulating the list.
      analysis *a = *it;
      ++it;

      if (a->getType () == ANALYSIS_DC
	  || containsAnalysis (a, ANALYSIS_DC)) {
	parent->delAnalysis (a);
	parent->addAnalysis (a);
      }
    }
  }
}

// Returns the instance name of the given parents child analysis.
const char * net::getChild (analysis * parent) const {
  const char * child = NULL;
  if (parent != NULL && parent->getType () == ANALYSIS_SWEEP)
    child = parent->getPropertyString ("Sim");
  return child;
}

// Returns the child analysis of the given parent if possible.
analysis * net::getChildAnalysis (analysis * parent) {
  return findAnalysis (getChild (parent));
}

// Returns the last order sweep being not an parameter sweep.
analysis * net::findLastOrder (analysis * a) {
  ptrlist<analysis> * alist = a->getAnalysis ();
  analysis * child = alist ? alist->front() : NULL;
  if (child != NULL && child->getType () == ANALYSIS_SWEEP) {
    return findLastOrder (child);
  }
  return child ? child : a;
}

// Returns the last order sweep being not an parameter sweep.
ptrlist<analysis> * net::findLastOrderChildren (analysis * a) {
  ptrlist<analysis> * alist = a->getAnalysis ();
  analysis * child = alist ? alist->front() : NULL;
  if (child != NULL && child->getType () == ANALYSIS_SWEEP) {
    return findLastOrderChildren (child);
  }
  return alist;
}

/* The function re-shifts all circuits in the drop list to the actual
   list of circuit objects. */
void net::getDroppedCircuits (nodelist * nodes) {
  circuit * n;
  for (circuit * c = drop; c != NULL; c = n) {
    n = (circuit *) c->getNext ();
    if (nodes) nodes->insert (c);
    insertCircuit (c);
  }
  drop = NULL;
}

/* This function deletes all unnecessary circuits in the list of
   registered circuit objects. */
void net::deleteUnusedCircuits (nodelist * nodes) {
  circuit * n;
  for (circuit * c = root; c != NULL; c = n) {
    n = (circuit *) c->getNext ();
    if (!c->isOriginal ()) {
      if (nodes) nodes->remove (c);
      removeCircuit (c);
    }
  }
}

/* Returns the first node in the list of real circuit objects
   connected to the given node.  If there is no such node (unconnected
   node) the function returns NULL. */
node * net::findConnectedCircuitNode (node * n) {

  const char * _name = n->getName ();
  node * _node;

  // through the list of circuit objects
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    // skip signal circuits
    if (c->getPort ()) continue;
    // through the list of nodes in a circuit
    for (int i = 0; i < c->getSize (); i++) {
      _node = c->getNode (i);
      if (!strcmp (_node->getName (), _name)) {
	if (_node != n) {
	  return _node;
	}
      }
    }
  }
  return NULL;
}

/* Returns the first node in the list of circuit objects (including
   signals) connected to the given node.  If there is no such node
   (unconnected node) the function returns NULL. */
node * net::findConnectedNode (node * n) {

  const char * _name = n->getName ();
  node * _node;

  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    for (int i = 0; i < c->getSize (); i++) {
      _node = c->getNode (i);
      if (!strcmp (_node->getName (), _name)) {
	if (_node != n) {
	  return _node;
	}
      }
    }
  }
  return NULL;
}

// Rename the given circuit and mark it as being a reduced one.
void net::reducedCircuit (circuit * c) {
  char n[32];
  sprintf (n, "reduced%d", reduced++);
  c->setName (n);
}

/* Rename the given circuit and mark it as being a inserted one and
   remember when it was inserted. */
void net::insertedCircuit (circuit * c) {
  char n[32];
  sprintf (n, "inserted%d", inserted);
  c->setName (n);
  c->setInserted (inserted);
  inserted++;
}

// Rename the given node and mark it as being a inserted one.
void net::insertedNode (node * c) {
  char n[32];
  sprintf (n, "inode%d", insertedNodes++);
  c->setName (n);
}

/* This helper function checks whether the circuit chain of the
   netlist is properly working.  It returns the number of errors or
   zero if there are no errors. */
int net::checkCircuitChain (void) {
  int error = 0;
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    if (c->getPrev ())
      if (c->getPrev()->getNext () != c) {
	error++;
	logprint (LOG_ERROR, "ERROR: prev->next != circuit '%s'\n",
		  c->getName ());
      }
    if (c->getNext ())
      if (c->getNext()->getPrev () != c) {
	error++;
	logprint (LOG_ERROR, "ERROR: next->prev != circuit '%s'\n",
		  c->getName ());
      }
  }
  return error;
}

/* This function counts the number of signals (ports) within the list
   of registered circuits. */
int net::countPorts (void) {
  int count = 0;
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    if (c->getPort ()) count++;
  }
  return count;
}

/* This function counts the number of circuits within the list of
   registered circuits. */
int net::countNodes (void) {
  int count = 0;
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    if (!c->getPort ()) count += c->getSize ();
  }
  return count;
}

/* The function returns the number of non-linear circuits within the
   list of registered circuits. */
int net::isNonLinear (void) {
  int count = 0;
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    if (c->isNonLinear ()) count++;
  }
  return count;
}

/* The function adds the given nodeset object to the netlist's nodeset
   list. */
void net::addNodeset (nodeset * n) {
  n->setNext (nset);
  nset = n;
}

/* The following function deletes all the nodeset list of the netlist
   object.  Called from the destructor. */
void net::delNodeset (void) {
  nodeset * next;
  for (nodeset * n = nset; n != NULL; n = next) {
    next = n->getNext ();
    delete n;
  }
  nset = NULL;
}


void net::setActionNetAll(net  * subnet)
{
  for (auto *a : *(this->actions))
    a->setNet(subnet);
}


#if DEBUG
// DEBUG function: Lists the netlist.
void net::list (void) {
  logprint (LOG_STATUS, "DEBUG: netlist `%s' (%d circuits, "
	    "%d ports, %d nodes)\n", getName (), countPorts (),
	    countPorts (), countNodes ());
  // go through circuit list
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    // list each circuit
    logprint (LOG_STATUS, "       %s[", c->getName ());
    for (int i = 0; i < c->getSize (); i++) {
      logprint (LOG_STATUS, "%s-%d",
		c->getNode(i)->getName (), c->getNode(i)->getNode ());
      if (i < c->getSize () - 1)
	logprint (LOG_STATUS, ",");
    }
    logprint (LOG_STATUS, "] { %s }\n", c->propertyList ());
  }
}
#endif /* DEBUG */

} // namespace qucs
