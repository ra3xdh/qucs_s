/*
 * node.h - node class definitions
 *
 * Copyright (C) 2003, 2004 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __NODE_H__
#define __NODE_H__

namespace qucs {

class circuit;

class node : public object
{
 public:
  //! Constructor creates an unnamed instance of the node class.
  node () : object (), nNode(0), port(0), internal(0), _circuit(nullptr) {};
  //! Constructor creates a named instance of the node class.
  node (char * const n) : object (n), nNode(0), port(0), internal(0), _circuit(nullptr) {};
  //! Sets the unique number of this node
  void setNode (const int n) { this->nNode = n ; };
  //! Returns the unique number of this node.
  int  getNode (void) const { return this->nNode; };
  //! Sets the port number of this node.
  void setPort (const int p) { this->port = p; };
  //! Returns the port number of this node.
  int  getPort (void) const { return this->port; };
  //! Sets this node's circuit.
  void setCircuit (circuit *const c) { this->_circuit = c; };
  circuit * getCircuit (void) const { return this->_circuit; };
  void setInternal (int i) { internal = i; }
  int  getInternal (void) { return internal; }

 private:
  int nNode;
  int port;
  int internal;
  circuit * _circuit;
};

} // namespace qucs

#endif /* __NODE_H__ */
