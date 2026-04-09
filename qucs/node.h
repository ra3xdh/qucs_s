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

class Component;
class Wire;


class Node : public Conductor {
public:
  Node(int x, int y);

  void  paint(QPainter* painter) const;
  bool  getSelected(int, int);
  void  setName(const QString&, const QString&, int x=0, int y=0);

  // Add an element to the node's connections.
  // No-op if element is already connected.
  void connect(Wire* wire) { if (!is_connected(wire)) m_wires.emplace_front(wire); }
  void connect(Component* comp) { if (!is_connected(comp)) m_components.emplace_front(comp); }

  // Remove element from the node's connections.
  void disconnect(Wire* wire) { m_wires.remove(wire); }
  void disconnect(Component* comp) { m_components.remove(comp); }

  // Tells if an element is among node's connections.
  bool is_connected(Wire* wire) const { return std::ranges::find(m_wires, wire) != m_wires.end(); }
  bool is_connected(Component* comp) const { return std::ranges::find(m_components, comp) != m_components.end(); }

  bool isOverlapping(int, int) const;
  bool isOverlapping(const Node*) const;

  std::size_t conn_count() const { return m_wires.size() + m_components.size(); }

  Wire* anyWire() const { return m_wires.empty() ? nullptr : m_wires.front(); }
  Component* anyComp() const { return m_components.empty() ? nullptr : m_components.front(); }

  // Returns an element from node's connections which is
  // not equal to e; nullptr if there is no such element
  Wire* other_than(Wire* wire) const
  {
    auto other = std::ranges::find_if(m_wires, [wire](auto other){ return other != wire; });
    return other == m_wires.end() ? nullptr : *other;
  }


  bool moveCenter(int dx, int dy) noexcept override;

  QString Name;  // node name used by creation of netlist
  QString DType; // type of node (used by digital files)
  int State;	 // remember some things during some operations

  int x() const { return cx; }
  int y() const { return cy; }

  const std::list<Wire*>& wires() const { return m_wires; }
  const std::list<Component*>& components() const { return m_components; }

  Node* merge(Node* other);

private:
  // Nodes usually have quite a few connections. In ideal case, when all wire
  // placement optimizations work properly, there can be at most four connections
  // to a single node because Qucs-S allows only orthogonal element placement.
  //
  // Additions/deletions are frequent and made in random order. Considering
  // all that I think the doubly-linked list is a good choice here.
  //
  // A node doesn't claim ownership of any connected object, storing raw pointers is OK.
  std::list<Wire*> m_wires;
  std::list<Component*> m_components;
};

#endif
