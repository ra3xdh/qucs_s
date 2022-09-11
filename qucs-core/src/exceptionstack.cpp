/*
 * exceptionstack.cpp - exception stack class implementation
 *
 * Copyright (C) 2004 Stefan Jahn <stefan@lkcc.org>
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

#include "logging.h"
#include "exception.h"
#include "exceptionstack.h"

using namespace qucs;

// Global exception stack.
exceptionstack qucs::estack;

// Constructor creates an instance of the exception stack class.
exceptionstack::exceptionstack () {
  root = NULL;
}

/* This copy constructor creates a instance of the exception stack
   class based on the given exception stack. */
exceptionstack::exceptionstack (const exceptionstack & e) {
  exception * last, * prev = NULL;
  for (exception * next = e.root; next != NULL; next = next->getNext ()) {
    last = new exception (*next);
    if (prev != NULL)
      prev->setNext (last);
    else
      root = last;
    prev = last;
  }
}

// Destructor deletes an instance of the exception stack class.
exceptionstack::~exceptionstack () {
  exception * next;
  while (root) {
    next = root->getNext ();
    delete root;
    root = next;
  }
}

// The function pushes a new exception onto the exception stack.
void exceptionstack::push (exception * e) {
  e->setNext (root);
  root = e;
}

/* This function removes the top exception from the exception stack
   and returns the new top exception. */
exception * exceptionstack::pop (void) {
  if (root != NULL) {
    exception * next = root->getNext ();
    delete root;
    root = next;
  }
  return root;
}

// The function returns the top exception.
exception * exceptionstack::top (void) {
  return root;
}

/* This function prints the complete exception stack and removes each
   exception from the stack. */
void exceptionstack::print (const char * prefix) {
  exception * next;
  if (root)
    logprint (LOG_ERROR, "%s%sexception stack\n",
	      prefix ? prefix : "", prefix ? " " : "");
  while ((next = top ()) != NULL) {
    logprint (LOG_ERROR, "  %03d: %s\n", next->getCode (), next->getText ());
    pop ();
  }
}
