/*
 * strlist.cpp - string list class implementation
 *
 * Copyright (C) 2003, 2004, 2005, 2006 Stefan Jahn <stefan@lkcc.org>
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

#include "strlist.h"

namespace qucs {

// Constructor creates an instance of the strlist class.
strlist::strlist () {
  root = NULL;
  txt = NULL;
}

/* This copy constructor creates a instance of the strlist class based
   on the given strlist. */
strlist::strlist (const strlist & o) {
  struct strlist_t * s;
  root = NULL;
  txt = NULL;
  for (s = o.root; s != NULL; s = s->next) append (s->str);
}

// Destructor deletes an instance of the strlist class.
strlist::~strlist () {
  struct strlist_t * next;
  while (root) {
    next = root->next;
    free (root->str);
    free (root);
    root = next;
  }
  free (txt);
}

// This function adds a string to the list.
void strlist::add (const char * const str) {
  struct strlist_t * s;
  s = (struct strlist_t *) calloc (sizeof (struct strlist_t), 1);
  s->next = root;
  s->str = str ? strdup (str) : NULL;
  root = s;
}

// The function adds the given string list to the list.
void strlist::add (const strlist * const lst) {
  if (lst)
    for (int i = lst->length () - 1; i >= 0; i--)
      add (lst->get (i));
}

// The function apends the given string list to the list.
void strlist::append (const strlist * const lst) {
  if (lst) for (int i = 0; i < lst->length (); i++)
	     append (lst->get (i));
}

// This function append a string to the list.
void strlist::append (const char * const str) {
  struct strlist_t * s;
  s = (struct strlist_t *) calloc (sizeof (struct strlist_t), 1);
  s->next = NULL;
  s->str = str ? strdup (str) : NULL;
  if (root) {
    struct strlist_t * e;
    for (e = root; e->next != NULL; e = e->next) ;
    e->next = s;
  }
  else {
    root = s;
  }
}

// This function counts the string in the list.
int strlist::length (void) const {
  int res = 0;
  for (struct strlist_t * s = root; s != NULL; s = s->next) res++;
  return res;
}

// This function finds the specified string in the list.
int strlist::contains (const char * const str) const {
  int res = 0;
  for (struct strlist_t * s = root; s != NULL; s = s->next) {
    if (s->str != NULL && str != NULL && !strcmp (s->str, str))
      res++;
  }
  return res;
}

/* The returns the position of the first occurrence of the specified
   string in the list or -1 if it does not contains the string. */
int strlist::index (char * str) {
  int res = 0;
  for (struct strlist_t * s = root; s != NULL; s = s->next, res++) {
    if (s->str != NULL && str != NULL && !strcmp (s->str, str))
      return res;
  }
  return -1;
}

/* This function returns the string positioned at the specified
   location in the string list. */
char * strlist::get (int pos) const {
  struct strlist_t * s = root;
  for (int i = 0; i < pos && s != NULL; s = s->next, i++) ;
  return s ? s->str : NULL;
}

/* This function returns the string positioned at the end of the
   string list. */
char * strlist::last (void) const {
  struct strlist_t * s;
  for (s = root; s != NULL && s->next != NULL; s = s->next) ;
  return s ? s->str : NULL;
}

/* This function returns the string positioned at the beginning of the
   string list. */
char * strlist::first (void) const {
  struct strlist_t * s = root;
  return s ? s->str : NULL;
}

/* The function removes each occurrence of the given string list entry
   from the string list object. */
void strlist::del (strlist * cand) {
  if (cand == NULL) return;
  struct strlist_t * next;
  strlist * res = new strlist ();
  while (root) {
    next = root->next;
    if (cand->contains (root->str) == 0) res->append (root->str);
    free (root->str);
    free (root);
    root = next;
  }
  *this = *res;
}

/* The function joins the given string lists to each other and returns
   the resulting list. */
strlist * strlist::join (strlist * pre, strlist * post) {
  strlist * res = pre ? new strlist (*pre) : new strlist ();
  for (int i = 0; post != NULL && i < post->length (); i++)
    res->append (post->get (i));
  return res;
}

/* The function returns a space seperated string representation of the
   string list instance. */
char * strlist::toString (const char * concat) {
  if (txt) { free (txt); txt = NULL; }
  int size = 0;
  for (struct strlist_t * s = root; s != NULL; s = s->next) {
    char * t = s->str ? s->str : (char *) "(null)";
    int len = strlen (t);
    size += len + strlen (concat) + 1;
    txt = (char *) (txt ? realloc (txt, size) : malloc (size));
    txt = (s == root) ? strcpy (txt, t) : strcat (txt, t);
    txt = strcat (txt, concat);
  }
  if (txt) txt[strlen (txt) - 1] = '\0';
  return txt ? txt : (char *) "";
}

// Constructor for string list iterator.
strlistiterator::strlistiterator (strlist & s) {
  _strlist = &s;
  toLast ();
  toFirst ();
}

// Another constructor for string list iterator.
strlistiterator::strlistiterator (strlist * s) {
  _strlist = s;
  toLast ();
  toFirst ();
}

// Default constructor for string list iterator.
strlistiterator::strlistiterator () {
  _strlist = NULL;
  _first = _last = _current = NULL;
}

// Destructor for string list iterator.
strlistiterator::~strlistiterator () {
}

// Returns number of items this iterator operates on.
int strlistiterator::count (void) {
  return _strlist->length ();
}

// Sets the current to the first item in the iterator list.
char * strlistiterator::toFirst (void) {
  _current = _first = _strlist->root;
  return _current ? _current->str : NULL;
}

// Sets the current to the last item in the iterator list.
char * strlistiterator::toLast (void) {
  for (_last = _strlist->root; _last && _last->next; _last = _last->next) ;
  _current = _last;
  return _current ? _current->str : NULL;
}

// Makes the succeeding item current and returns the new current item.
char * strlistiterator::operator++ (void) {
  _current = _current->next;
  return _current ? _current->str : NULL;
}

// Returns the current iterator item.
char * strlistiterator::current (void) {
  return _current ? _current->str : NULL;
}

// Returns the first iterator item.
char * strlistiterator::first (void) {
  return _first ? _first->str : NULL;
}

// Returns the last iterator item.
char * strlistiterator::last (void) {
  return _last ? _last->str : NULL;
}

} // namespace qucs
