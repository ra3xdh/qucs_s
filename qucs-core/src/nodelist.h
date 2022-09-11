/*
 * nodelist.h - node list class definitions
 *
 * Copyright (C) 2003, 2004, 2008 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __NODELIST_H__
#define __NODELIST_H__

#include <vector>
#include <list>
#include <memory>
#include <algorithm>

namespace qucs {

class node;
class net;

namespace detail {
  typedef std::vector<node *> nodevector;
}

struct nodelist_t {
public:
  nodelist_t(const std::string &n="", bool intern = false) :
    n(0), name(n), internal(intern), nodes() {}

  nodelist_t(nodelist_t &c) = default;

  typedef detail::nodevector::value_type value_type;
  typedef detail::nodevector::iterator iterator;
  typedef detail::nodevector::const_iterator const_iterator;
  typedef detail::nodevector::size_type size_type;
  typedef detail::nodevector::reference reference;
  typedef detail::nodevector::const_reference const_reference;
#ifdef HAVE_ERASE_CONSTANT_ITERATOR
  typedef detail::nodevector::const_iterator erase_iterator;
#else
  typedef detail::nodevector::iterator erase_iterator;
#endif

  /*! alias node number */
  std::size_t n;
  /*! name of node */
  std::string name;
  bool internal;

  reference operator[] (size_type n) {
    return (this->nodes[n]);
  }
  const_reference operator[] (size_type n) const {
    return (this->nodes[n]);
  }

  size_type size() const noexcept {
    return nodes.size();
  }

  void push_back(const value_type &val) {
    this->nodes.push_back(val);
  }

  iterator begin() noexcept {
    return nodes.begin();
  }
  const_iterator begin() const noexcept {
    return nodes.begin();
  }
  iterator end() noexcept {
    return nodes.end();
  }
  const_iterator end() const noexcept {
    return nodes.end();
  }
  iterator erase (erase_iterator position)
  { return nodes.erase(position); };
  iterator erase (erase_iterator first, erase_iterator last)
  { return nodes.erase(first,last); };

  bool empty() const noexcept {
    return nodes.empty();
  }
private:
  std::vector<value_type> nodes;
};

class nodelist
{
 public:
  // Constructor creates an instance of the nodelist class.
  nodelist () :  narray(), sorting(0) {
  }
  nodelist (net *);
  ~nodelist ();
  int length (void) const ;
  int getNodeNr (const std::string &) const ;
  std::string get (int) const ;
  bool isInternal (int) const ;
  void assignNodes (void);
  void print (void) const;
  std::string getNodeString (int) const;
  void sort (void);
  void remove (circuit *);
  void insert (circuit *);
  void sortedNodes (node **, node **);
  struct nodelist_t * getNode (const std::string &) const;
  struct nodelist_t * getNode (int nr) const {
    return narray[nr + 1];
  }
  nodelist_t &operator[](int nr) const {
    return *narray[nr + 1];
  }

 private:
  std::vector<nodelist_t *> narray;
  std::list<nodelist_t *> root;
  int sorting;
  bool contains (const std::string &) const;
  void insert (struct nodelist_t *);
  void addCircuitNode (struct nodelist_t *, node *);
};

} // namespace qucs

#endif /* __NODELIST_H__ */
