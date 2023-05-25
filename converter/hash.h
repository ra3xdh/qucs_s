/*
 * hash.h - hash function interface
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

#ifndef __HASH_H__
#define __HASH_H__

namespace qucs {

/* Useful defines. */
#define HASH_SHRINK         4
#define HASH_EXPAND         8
#define HASH_MIN_SIZE       4
#define HASH_SHRINK_LIMIT   (buckets >> 2)
#define HASH_EXPAND_LIMIT   ((buckets >> 1) + (buckets >> 2))
#define HASH_LOCATION(code) ((code) & (buckets - 1))

template <class type_t> class hashentry;
template <class type_t> class hashbucket;
template <class type_t> class hash;
template <class type_t> class hashiterator;

/* This is the basic structure of a hash entry consisting of its key,
   the actual value stored in the hash table and the hash code of the
   key. */
template <class type_t>
class hashentry
{
  friend class hashiterator<type_t>;
  friend class hashbucket<type_t>;
  friend class hash<type_t>;

 public:
  hashentry () {  // Constructor.
    code = 0; key = NULL; value = NULL;
  }
  ~hashentry () { // Destructor.
    free (key);
  }

 private:
  int code;       // Hash code.
  char * key;     // Hash key.
  type_t * value; // Hash value.
};

/* The hash table consists of different hash buckets.  This contains
   the bucket's size and the entry array.  */
template <class type_t>
class hashbucket
{
  friend class hashiterator<type_t>;
  friend class hash<type_t>;

 public:
  hashbucket () {  // Constructor.
    capacity = size = 0;
    entry = NULL;
  }
  ~hashbucket () { // Destructor.
    if (entry) {
      for (int n = 0; n < size; n++) delete entry[n];
      free (entry);
    }
  }

 public:
  // Adds an entry to the bucket.
  void add (hashentry<type_t> * e) {
    if (capacity == 0) {
      capacity = HASH_MIN_SIZE;
      entry = (hashentry<type_t> **)
	malloc (sizeof (hashentry<type_t> *) * capacity);
    }
    else if (size >= capacity) {
      capacity *= 2;
      entry = (hashentry<type_t> **)
	realloc (entry, sizeof (hashentry<type_t> *) * capacity);
    }
    entry[size++] = e;
  }
  // Removes an entry from the bucket.
  void del (int idx) {
    size--;
    if (idx != size) entry[idx] = entry[size];
  }

 private:
  int capacity;               // The capacity of the bucket.
  int size;                   // The current size.
  hashentry<type_t> ** entry; // Hash entry table.
};


/* This structure keeps information of a specific hash table.  */
template <class type_t>
class hash
{
  friend class hashiterator<type_t>;

 public:
  hash (int size = HASH_MIN_SIZE);
  ~hash ();

  int  count (void);
  void clear (void);
  void rehash (int);
  type_t * put (char *, type_t *);
  type_t * get (char *);
  type_t * del (char *);

 private:
  int buckets;
  int fill;
  int keys;
  int (* equals) (char *, char *);
  int (* code) (char *);
  unsigned (* keylen) (char *);
  hashbucket<type_t> ** table;
};

/* Definition of the hash table iterator.  */
template <class type_t>
class hashiterator
{
 public:
  hashiterator ();
  hashiterator (hash<type_t> &);
  ~hashiterator ();

  int count (void);
  char * toFirst (void);
  char * toLast (void);
  char * operator++ (void);
  char * operator-- (void);
  char * operator * (void) { return current (); }
  char * current (void);
  char * currentKey (void);
  type_t * currentVal (void);
  char * first (void);
  char * last (void);

 private:
  hash<type_t> * _hash;
  hashentry<type_t> * _first;
  hashentry<type_t> * _last;
  hashentry<type_t> * _current;
  int _bucket;
  int _entry;
};

} /* namespace qucs */

#include "hash.cpp"

#endif /* __HASH_H__ */
