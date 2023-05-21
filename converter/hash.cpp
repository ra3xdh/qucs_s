/*
 * hash.cpp - hash table functions
 *
 * Copyright (C) 2005, 2007 Stefan Jahn <stefan@lkcc.org>
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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"

using namespace qucs;

/* Calculate the hash code for a given string. This is the standard
   callback for any newly created hash table.  */
static inline int hash_code (char * key) {
  int code = 0;
  char * p = key;
  while (*p) { code = (code << 1) ^ *p; p++; }
  return code;
}

/* This is the default callback for any newly created hash for
   determining two keys being equal.  Return zero if both strings are
   equal, otherwise non-zero.  */
static inline int hash_key_equals (char * key1, char * key2) {
  char * p1, * p2;
  if (key1 == key2) return 0;
  p1 = key1;
  p2 = key2;
  while (*p1 && *p2) {
    if (*p1 != *p2) return -1;
    p1++; p2++;
  }
  if (!*p1 && !*p2) return 0;
  return -1;
}

/* This is the default routine for determining the actual hash table
   key length of the given key.  */
static inline unsigned hash_key_length (char * key) {
  unsigned len = 0;
  while (*key++) len++;
  len++;
  return len;
}

// Constructor for the hash table.
template <class type_t>
qucs::hash<type_t>::hash (int size) {
  // set initial hash table size to a binary value
  int n;
  for (n = size, buckets = 1; n != 1; n >>= 1) 
    buckets <<= 1;
  if (buckets < HASH_MIN_SIZE)
    buckets = HASH_MIN_SIZE;

  // initialize default values
  fill = 0;
  keys = 0;
  code = hash_code;
  equals = hash_key_equals;
  keylen = hash_key_length;

  // allocate space for the hash table buckets
  table = (hashbucket<type_t> **)
    calloc (buckets, sizeof (hashbucket<type_t> *));
}

// Destructor for the hash table.
template <class type_t>
qucs::hash<type_t>::~hash () {
  for (int n = 0; n < buckets; n++) {
    delete table[n];
  }
  free (table);
}

/* Clears the hash table.  Afterwards it does not contain any key.
   The hash table is also shrunken to a minimal size.  */
template <class type_t>
void qucs::hash<type_t>::clear (void) {
  for (int n = 0; n < buckets; n++) {
    delete table[n];
  }
  free (table);

  // reinitialize the hash table
  buckets = HASH_MIN_SIZE;
  fill = 0;
  keys = 0;
  table = (hashbucket<type_t> **)
    calloc (buckets, sizeof (hashbucket<type_t> *));
}

// Returns number of items in the hash.
template <class type_t>
int qucs::hash<type_t>::count (void) {
  return keys;
}

/* Rebuilds a hash table.  Double (type is HASH_EXPAND) its size and
   expand the hash codes or half (type is HASH_SHRINK) its size and
   shrink the hash codes if these would be placed somewhere else.  */
template <class type_t>
void qucs::hash<type_t>::rehash (int type) {
  int n, e;
  hashbucket<type_t> * bucket, * next;

  // Expand!
  if (type == HASH_EXPAND) {
    // Reallocate and initialize the hash table itself.
    buckets *= 2;
    table = (hashbucket<type_t> **)
      realloc (table, sizeof (hashbucket<type_t> *) * buckets);
    for (n = buckets / 2; n < buckets; n++) { table[n] = NULL; }

    /* Go through all hash table entries and check if it is necessary
       to relocate them.  */
    for (n = 0; n < buckets / 2; n++){
      bucket = table[n];
      for (e = 0; bucket && e < bucket->size; e++) {
	int loc = HASH_LOCATION (bucket->entry[e]->code);
	if (n != loc) {
	  /* copy this entry to the far entry */
	  if ((next = table[loc]) == NULL) {
	    next = new hashbucket<type_t> ();
	    table[loc] = next;
	  }
	  next->add (bucket->entry[e]);
	  if (next->size == 1) fill++;
	  /* delete this entry */
	  bucket->del (e);
	  if (bucket->size == 0) fill--;
	  e--;
	}
      }
    }
  }
  // Shrink!
  else if (type == HASH_SHRINK && buckets > HASH_MIN_SIZE) {
    buckets /= 2;
    for (n = buckets; n < buckets * 2; n++) {
      bucket = table[n];
      if (bucket && bucket->size) {
	for (e = 0; e < bucket->size; e++) {
	  int loc = HASH_LOCATION (bucket->entry[e]->code);
	  if ((next = table[loc]) == NULL) {
	    next = new hashbucket<type_t> ();
	  }
	  next->add (bucket->entry[e]);
	  if (next->size == 1) fill++;
	}
	delete bucket;
      }
      fill--;
    }
    table = (hashbucket<type_t> **)
      realloc (table, sizeof (hashbucket<type_t> *) * buckets);
  }
}

/* This function adds a new element consisting of key and value to an
   existing hash.  If the hash is 75% filled it gets rehashed (size
   will be doubled).  When the key already exists then the value just
   gets replaced dropping and returning the old value.  */
template <class type_t>
type_t * qucs::hash<type_t>::put (char * key, type_t * value) {
  int code = this->code (key);
  hashbucket<type_t> * bucket = table[HASH_LOCATION (code)];

  /* Check if the key is already stored. If so replace the value. */
  if (bucket) {
    for (int e = 0; e < bucket->size; e++) {
      if (bucket->entry[e]->code == code) {
	if (equals (bucket->entry[e]->key, key) == 0) {
	  type_t * old = bucket->entry[e]->value;
	  bucket->entry[e]->value = value;
	  return old;
	}
      }
    }
  }
  else {
    bucket = new hashbucket<type_t> ();
    table[HASH_LOCATION (code)] = bucket;
  }

  /* Fill this entry. */
  hashentry<type_t> * entry = new hashentry<type_t> ();
  entry->key = (char *) malloc (keylen (key));
  memcpy (entry->key, key, keylen (key));
  entry->value = value;
  entry->code = code;

  bucket->add (entry);
  keys++;

  /* 75% filled ? */
  if (bucket->size == 1) {
    fill++;
    if (fill > HASH_EXPAND_LIMIT) {
      rehash (HASH_EXPAND);
    }
  }
  return NULL;
}

/* Delete an existing hash entry accessed via a given key form the
   hash table.  Return NULL if the key has not been found within the
   hash, otherwise the previous value.  */
template <class type_t>
type_t * qucs::hash<type_t>::del (char * key) {
  type_t * value;
  int code = this->code (key);
  hashbucket<type_t> * bucket = table[HASH_LOCATION (code)];
  if (bucket) {
    for (int n = 0; n < bucket->size; n++) {
      if (bucket->entry[n]->code == code) {
	if (equals (bucket->entry[n]->key, key) == 0) {
	  value = bucket->entry[n]->value;
	  bucket->del (n);
	  if (bucket->size == 0) {
	    fill--;
	    if (fill < HASH_SHRINK_LIMIT) {
	      rehash (HASH_SHRINK);
	    }
	  }
	  keys--;
	  return value;
	}
      }
    }
  }
  return NULL;
}

/* Hash table lookup.  Find a value for a given key in the hash table.
   Return NULL if the key has not been found within the hash table.  */
template <class type_t>
type_t * qucs::hash<type_t>::get (char * key) {
  int code = this->code (key);
  hashbucket<type_t> * bucket = table[HASH_LOCATION (code)];
  if (bucket) {
    for (int n = 0; n < bucket->size; n++) {
      if (bucket->entry[n]->code == code)
	if (equals (bucket->entry[n]->key, key) == 0)
	  return bucket->entry[n]->value;
    }
  }
  return NULL;
}

// Constructor for hash table iterator.
template <class type_t>
hashiterator<type_t>::hashiterator (hash<type_t> & h) {
  _hash = &h;
  _bucket = 0;
  _entry = 0;
  toLast ();
  toFirst ();
}

// Default constructor for hash table iterator.
template <class type_t>
hashiterator<type_t>::hashiterator () {
  _hash = NULL;
  _bucket = 0;
  _entry = 0;
  _first = _last = _current = NULL;
}

// Destructor for hash table iterator.
template <class type_t>
hashiterator<type_t>::~hashiterator () {
}

// Returns number of items this iterator operates on.
template <class type_t>
int hashiterator<type_t>::count (void) {
  return _hash->keys;
}

// Sets the current to the first item in the iterator list.
template <class type_t>
char * hashiterator<type_t>::toFirst (void) {
  for (int n = 0; n < _hash->buckets; n++) {
    hashbucket<type_t> * bucket = _hash->table[n];
    if (bucket && bucket->size) {
      _bucket = n;
      _entry = 0;
      _current = _first = bucket->entry[_entry];
      return _current->key;
    }
  }
  _current = _first = NULL;
  return NULL;
}

// Sets the current to the last item in the iterator list.
template <class type_t>
char * hashiterator<type_t>::toLast (void) {
  for (int n = _hash->buckets - 1; n >= 0; n--) {
    hashbucket<type_t> * bucket = _hash->table[n];
    if (bucket && bucket->size) {
      _bucket = n;
      _entry = bucket->size - 1;
      _current = _last = bucket->entry[_entry];
      return _current->key;
    }
  }
  _current = _last = NULL;
  return NULL;
}

// Makes the succeeding item current and returns the new current item.
template <class type_t>
char * hashiterator<type_t>::operator++ (void) {
  hashbucket<type_t> * bucket = _hash->table[_bucket];
  if (bucket && _entry < bucket->size - 1) {
    _entry++;
    _current = bucket->entry[_entry];
    return _current->key;
  }
  for (int n = _bucket + 1; n < _hash->buckets; n++) {
    bucket = _hash->table[n];
    if (bucket && bucket->size) {
      _bucket = n;
      _entry = 0;
      _current = bucket->entry[_entry];
      return _current->key;
    }
  }
  _current = NULL;
  return NULL;
}

// Makes the preceding item current and returns the new current item.
template <class type_t>
char * hashiterator<type_t>::operator-- (void) {
  hashbucket<type_t> * bucket = _hash->table[_bucket];
  if (bucket && _entry > 0) {
    _entry--;
    _current = bucket->entry[_entry];
    return _current->key;
  }
  for (int n = _bucket - 1; n >= 0 ; n--) {
    bucket = _hash->table[n];
    if (bucket && bucket->size) {
      _bucket = n;
      _entry = bucket->size - 1;
      _current = bucket->entry[_entry];
      return _current->key;
    }
  }
  _current = NULL;
  return NULL;
}

// Returns the current iterator item.
template <class type_t>
char * hashiterator<type_t>::current (void) {
  return _current ? _current->key : NULL;
}

// Returns the current iterator items key.
template <class type_t>
char * hashiterator<type_t>::currentKey (void) {
  return current ();
}

// Returns the current iterator items value.
template <class type_t>
type_t * hashiterator<type_t>::currentVal (void) {
  return _current ? _current->value : NULL;
}

// Returns the first iterator item.
template <class type_t>
char * hashiterator<type_t>::first (void) {
  return _first ? _first->key : NULL;
}

// Returns the last iterator item.
template <class type_t>
char * hashiterator<type_t>::last (void) {
  return _last ? _last->key : NULL;
}
