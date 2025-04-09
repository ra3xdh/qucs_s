/***************************************************************************
                                 node.h
                                --------
    begin                : Sat Sep 20 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NODE_H
#define NODE_H

#include "conductor.h"
#include <list>

class Node : public Conductor {
public:
  Node(int x, int y);
 ~Node() override;

  void  paint(QPainter* painter) const;
  bool  getSelected(int, int);
  void  setName(const QString&, const QString&, int x=0, int y=0);

  // Add an element to the node's connections.
  // No-op if element is already connected.
  void connect(Element* connectable);

  // Remove element from the node's connections.
  void disconnect(Element* connectable);

  // Tells if an element is among node's connections.
  bool is_connected(Element* connectable) const;

  std::size_t conn_count() const;

  // Returns one of node's connections or nullptr when
  // there is no connections
  Element* any() const;

  // Returns an element from node's connections which is
  // not equal to e; nullptr if there is no such element
  Element* other_than(Element* e) const;

  using const_iterator = std::list<Element*>::const_iterator;
  const_iterator begin() const;
  const_iterator end() const;

  bool moveCenter(int dx, int dy) noexcept override;

  QString Name;  // node name used by creation of netlist
  QString DType; // type of node (used by digital files)
  int State;	 // remember some things during some operations

  int x() const { return cx; }
  int y() const { return cy; }

private:
  // Nodes usually have quite a few connections. In ideal case, when all wire
  // placement optimizations work properly, there can be at most four connections
  // to a single node because Qucs-S allows only orthogonal element placement.
  //
  // Additions/deletions are frequent and made in random order. Considering
  // all that I think the doubly-linked list is a good choice here.
  //
  // A node doesn't claim ownership of any connected object, storing raw pointers is OK.
  //
  // Long-term TODO: refactor so that node will keep only pointers to *connectable*
  //                 objects, i.e. components and wires. Paintings, graphs, wirelabels,
  //                 etc., are Elements too, it's just wrong to use so generic type.
  std::list<Element*> connections;
};

// Calling this while iterating over the node's connections via Node::begin() and
// Node::end() will cause segfault because of iterator invalidation
inline void Node::connect(Element* connectable)
{
  if (is_connected(connectable)) {
    return;
  }
  connections.push_front(connectable);
}

// Calling this while iterating over the node's connections via Node::begin() and
// Node::end() will cause segfault because of iterator invalidation
inline void Node::disconnect(Element* connectable)
{
  connections.remove(connectable);
}

inline bool Node::is_connected(Element *connectable) const
{
  return std::find(connections.begin(), connections.end(), connectable) != connections.end();
}

inline std::size_t Node::conn_count() const
{
  return connections.size();
}

inline Node::const_iterator Node::begin() const
{
  return connections.begin();
}

inline Node::const_iterator Node::end() const
{
  return connections.end();
}

inline Element* Node::any() const
{
  return connections.empty() ? nullptr : connections.front();
}

#endif
