/*
 * nodeset.cpp - node set class implementation
 *
 * Copyright (C) 2004, 2008 Stefan Jahn <stefan@lkcc.org>
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

#include "object.h"
#include "netdefs.h"
#include "nodeset.h"

namespace qucs {

// Constructor creates an unnamed instance of the node set class.
nodeset::nodeset () {
  name = NULL;
  value = 0.0;
  next = NULL;
}

// Constructor creates a named instance of the node set class.
nodeset::nodeset (char * n) {
  name = n ? strdup (n) : NULL;
  value = 0.0;
  next = NULL;
}

/* This full qualified constructor creates an instance of the node set
   class containing both the key and the value of the node set. */
nodeset::nodeset (char * n, nr_double_t val) {
  name = n ? strdup (n) : NULL;
  value = val;
  next = NULL;
}

/* The copy constructor creates a new instance of the node set class
   based on the given node set object. */
nodeset::nodeset (const nodeset & p) {
  name = NULL;
  if (p.name) name = strdup (p.name);
  value = p.value;
  next = p.next;
}

// Destructor deletes the node set object.
nodeset::~nodeset () {
  free (name);
}

// Sets the name of the node set.
void nodeset::setName (char * n) {
  free (name);
  name = n ? strdup (n) : NULL;
}

// Returns the name of the node set.
char * nodeset::getName (void) {
  return name;
}

/* Goes through the chained list of the node sets and looks for a node
   set matching the given key and returns its value if possible.  If
   there is no such node set the function returns NULL. */
nodeset * nodeset::findNodeset (char * n) {
  for (nodeset * p = this; p != NULL; p = p->getNext ()) {
    if (!strcmp (p->getName (), n)) return p;
  }
  return NULL;
}

// properties
PROP_REQ [] = {
  { "U", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE }, PROP_NO_PROP };
PROP_OPT [] = {
  PROP_NO_PROP };
struct define_t nodeset::miscdef =
  { "NodeSet", 1, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };

} // namespace qucs
