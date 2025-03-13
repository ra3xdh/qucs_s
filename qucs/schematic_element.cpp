/***************************************************************************
                            schematic_element.cpp
                           -----------------------
    begin                : Sat Mar 3 2006
    copyright            : (C) 2006 by Michael Margraf
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
#include <optional>
#include <stdlib.h>
#include <limits.h>

#include "geometry/multi_point.h"
#include "healer.h"
#include "portsymbol.h"
#include "schematic.h"

#include <set>
#include <unordered_set>

/**
    Given an element bounding rectangle and selection rectangle tells
    whether the element should be marked as "selected".

    If \c entirely param is \c false then an element is "selected"
    if its bounding rectangle intersect with selection rectangle.
    Otherwise the element must lie entirely within the selection
    rectangle.
*/
static bool shouldBeSelected(const QRect& elementBoundingRect, const QRect& selectionRect, bool entirely)
{
    // QRect.contains() and QRect.intersects() always return false
    // for empty rectangles. Thus an element with zero size can't be
    // selected. To overcome this, lets treat it like a point, not
    // as a rectangle (empty rectangle has all its corners in one spot)
    if (elementBoundingRect.isNull()) {
        return selectionRect.contains(elementBoundingRect.topLeft());
    }

    if (entirely) {
        return selectionRect.contains(elementBoundingRect);
    } else {
        return selectionRect.intersects(elementBoundingRect);
    }
}

// This namespace contains a bunch of functions to check schematic invariants.
//
// Mutating a schematic is quite complicated task and to make it a little bit
// easier we can check invariants after changing something. In correct schematic
// all of them must hold.
//
// Code below is not very efficient or beatiful and it's not intended to be used
// "in production". Its main purpose is to help debugging and changing other
// schematic logic.
namespace invariants {

bool noOrphanNodes(const Q3PtrList<Node>* nodes)
{
    for (auto* n : *nodes) {
        assert(n->conn_count() > 0);
    }
    return true;
}

bool noSamePlaceNodes(const Q3PtrList<Node>* nodes)
{
    bool is_ok = true;
    for (auto* n1 : *nodes) {
        for (auto* n2 : *nodes) {
            if (n1 == n2) continue;

            if (n1->center() == n2->center()) {
                qCritical() << "Two different nodes at the same location!"
                            << "node 1:"
                            << n1
                            << "at" << n1->center()
                            << "with" << n1->conn_count() << "connections"
                            << "node 2:"
                            << n2
                            << "at" << n2->center()
                            << "with" << n2->conn_count() << "connections";
                is_ok = false;
            }
        }
    }
    return is_ok;
}

// It's an error to have a node lying in the middle of a wire and not splitting
// the wire. If coordinates of a node lie on a wire, the wire MUST be splitted
// into two wires.
bool noNodesOnWires(const Q3PtrList<Node>* nodes, const Q3PtrList<Wire>* wires)
{
    bool is_ok = true;
    for (auto* w : *wires) {
        for (auto* n : *nodes) {
            if (qucs_s::geom::is_between(n, w->Port1, w->Port2)) {
                qCritical() << "A node lies on a wire withou splitting it!"
                            << "node:" << n << "at" << n->center()
                            << "wire:" << w
                            << "from" << w->Port1->center()
                            << "to" << w->Port2->center();

                is_ok = false;
            }
        }
    }
    return is_ok;
}

// Two nodes may be connected with at most one wire
bool noDuplicateWires(const Q3PtrList<Wire>* wires)
{
    bool is_ok = true;
    for (auto* wire : *wires) {
        // In correct schematic nodes have at least one connection.
        // If a node of the wire has exactly one connection, then
        // there is no chance some other wire connects them too.
        // This "one" connection is the current wire.
        if (wire->Port1->conn_count() == 1 || wire->Port2->conn_count() == 1) {
            continue;
        }

        for (auto* conn : *wire->Port1) {
            if (conn == wire) continue;

            auto* other_wire = dynamic_cast<Wire*>(conn);
            if (other_wire == nullptr) continue; // it was not a wire

            // Some other wire connected too Port1 of current wire was found,
            // let's check if it is not connected to Port2
            if (wire->Port2->is_connected(other_wire)) {
                qCritical() << "Duplicate wires found!"
                            << "Nodes" 
                            << wire->Port1 << "(" << wire->Port1->center() << ")"
                            << "and"
                            << wire->Port2 << "(" << wire->Port2->center() << ")"
                            << "connected with" << wire << "and" << other_wire;
                is_ok = false;
            }
        }

        for (auto* conn : *wire->Port2) {
            if (conn == wire) continue;

            auto* other_wire = dynamic_cast<Wire*>(conn);
            if (other_wire == nullptr) continue; // it was not a wire

            // Some other wire connected too Port2 of current wire was found,
            // let's check if it is not connected to Port1
            if (wire->Port1->is_connected(other_wire)) {
                qCritical() << "Duplicate wires found!"
                            << "Nodes" 
                            << wire->Port1 << "(" << wire->Port1->center() << ")"
                            << "and"
                            << wire->Port2 << "(" << wire->Port2->center() << ")"
                            << "connected with" << wire << "and" << other_wire;
                is_ok = false;
            }
        }
    }
    return is_ok;
}

bool noZeroLenWires(const Q3PtrList<Wire>* wires)
{
    bool is_ok = true;
    for (auto* w : *wires) {
        if (w->Port1->center() == w->Port2->center()) {
            qCritical() << "A wire connecting two nodes at the same location is found!"
                        << "wire: " << w
                        << "node 1:" << w->Port1
                        << "at" << w->Port1->center()
                        << "with" << w->Port1->conn_count() << "connections"
                        << "node 2:" << w->Port2
                        << "at" << w->Port2->center()
                        << "with" << w->Port2->conn_count() << "connections";
            is_ok = false;
        }
    }
    return is_ok;
}

bool allWiresAreConsistent(const Q3PtrList<Wire>* wires)
{
    bool is_ok = true;
    for (auto* w : *wires) {
        if (w->Port1 == nullptr) {
            qCritical() << "Incostistent wire is found!"
                        << "Port1 of" << w << "is nullptr";
            is_ok = false;
        }

        if (w->Port2 == nullptr) {
            qCritical() << "Incostistent wire is found!"
                        << "Port2 of" << w << "is nullptr";
            is_ok = false;
        }

        if (w->Port1 != nullptr && w->Port2 != nullptr && w->Port1 == w->Port2) {
            qCritical() << "Incostistent wire is found!"
                        << w << "has the same node" << w->Port1 << "on both ports";
            is_ok = false;
        }

        if (!w->Port1->is_connected(w)) {
            qCritical() << "Incostistent wire is found!"
                        << "w->Port1->is_connected(w) == false"
                        << "for" << w;
            is_ok = false;
        }

        if (!w->Port2->is_connected(w)) {
            qCritical() << "Incostistent wire is found!"
                        << "w->Port2->is_connected(w) == false"
                        << "for" << w;
            is_ok = false;
        }
    }
    return is_ok;
}

bool allComponentsAreConsistent(const Q3PtrList<Component>* components)
{
    bool is_ok = true;
    for (auto* c : *components) {
        for (auto* p : c->Ports) {
            if (p->Connection == nullptr) {
                qCritical() << "Incostistent component is found!"
                            << "port->Connection == nullptr"
                            << "for" << c;
                is_ok = false;
            }

            if (!p->Connection->is_connected(c)) {
                qCritical() << "Incostistent component is found!"
                            << "Port of the component" << c
                            << "located at" << (QPoint{p->x, p->y} + c->center())
                            << "is connected to a node" << p->Connection << "located at" << p->Connection->center()
                            << "but the node doesn't have a reference to the component.";
                is_ok = false;
            }
        }
    }
    return is_ok;
}
}

namespace internal {

// Helper function to ease transition from Q3PtrList to another
// container type. Use this when you need to remove item from
// Q3PtrList and don't want auto-delete side effect.
//
// Who knows if auto-deletion is on and will it be a dozen commits
// later? Use this function and get expected result.
template<typename Q3PtrListContainer, typename Object>
void removeFromPtrList(Object* obj, Q3PtrListContainer* cont) {
    auto autoDelete = cont->autoDelete();
    cont->setAutoDelete(false);
    while (cont->removeRef(obj)) {};
    cont->setAutoDelete(autoDelete);
}
}

/* *******************************************************************
   *****                                                         *****
   *****              Actions handling the nodes                 *****
   *****                                                         *****
   ******************************************************************* */

// Provides a node located at given coordinates, either new or existing one
Node* Schematic::provideNode(int x, int y)
{
    // Check if there is a node at given coordinates
    for (auto* node : *a_Nodes) {
      if (node->x() == x && node->y() == y) {
        return node;
      }
    }

    // Create new node, if no existing one at given coordinates
    Node *new_node = new Node(x, y);
    a_Nodes->append(new_node);

    // Check if the new node lies upon an existing wire
    for (auto* wire : *a_Wires)
    {
        if (qucs_s::geom::is_between(new_node, wire->Port1, wire->Port2)) {
            // split the wire into two wires
            splitWire(wire, new_node);
            return new_node;
        }
    }

    return new_node;
}

namespace internal {
void merge(Node* donor, Node* recipient) {
    // At first, replace old node with new one in every element connected to
    // old node
    for (auto* conn : *donor) {
        if (auto* w = dynamic_cast<Wire*>(conn)) {
            if (w->Port1 == donor) {
                w->Port1 = recipient;
            } else {
                w->Port2 = recipient;
            }
        } else if (auto* c = dynamic_cast<Component*>(conn)) {
            for (auto* p : c->Ports) {
                if (p->Connection == donor) {
                    p->Connection = recipient;
                }
            }
        } else {
            assert(false);
        }
    }

    // Transfer all connections from old node to new one
    while (donor->conn_count() > 0) {
        auto* conn = donor->any();
        recipient->connect(conn);
        donor->disconnect(conn);
    }
}

// A node is redundant if it connects only two wires which form a line i.e.
// For example, here B is redundant
//  A      B      C
//  o------o------o
bool is_redundant(const Node* node) {
    if (node->conn_count() != 2) {
        return false;
    }

    auto* wire_1 = dynamic_cast<Wire*>(node->any());
    if (wire_1 == nullptr) {
        return false;
    }

    auto* wire_2 = dynamic_cast<Wire*>(node->other_than(wire_1));
    if (wire_2 == nullptr) {
        return false;
    }

    // If control flow has reached this point, then node connects two wires.
    // Let's check if they form a line and can be replaced by a single wire.

    auto* node_a = wire_1->Port1 == node ? wire_1->Port2 : wire_1->Port1;
    auto* node_b = wire_2->Port1 == node ? wire_2->Port2 : wire_2->Port1;

    return qucs_s::geom::is_between(node, node_a, node_b);
}

template<typename NodeContainer>
Node* find_redundant_node(NodeContainer* nodes) {
    for (auto* n : *nodes) {
      if (is_redundant(n)) return n;
    }

    return nullptr;
}

Wire* merge_wires_at_node(Node* node) {
    auto* extended_wire = dynamic_cast<Wire*>(node->any());
    auto* dissapearing_wire = dynamic_cast<Wire*>(node->other_than(extended_wire));

    assert(node->is_connected(extended_wire));
    assert(node->is_connected(dissapearing_wire));
    assert(is_redundant(node));

    // First of all, let's deal with labels. Label of node, if present, has
    // priority over wire labels.
    auto* label = node->Label;
    if (label == nullptr) {
        // Node has no label, choose label of one of the wires
        label = extended_wire->Label == nullptr ? dissapearing_wire->Label : extended_wire->Label;
    }

    auto* extend_to = dissapearing_wire->Port1 == node
                    ? dissapearing_wire->Port2
                    : dissapearing_wire->Port1;

    dissapearing_wire->Port1->disconnect(dissapearing_wire);
    dissapearing_wire->Port1 = nullptr;
    dissapearing_wire->Port2->disconnect(dissapearing_wire);
    dissapearing_wire->Port2 = nullptr;

    node->disconnect(extended_wire);
    if (extended_wire->Port1 == node) {
        extended_wire->Port1 = extend_to;
        extended_wire->Port1->connect(extended_wire);
    } else {
        extended_wire->Port2 = extend_to;
        extended_wire->Port2->connect(extended_wire);
    }

    // Update wire dimensions
    extended_wire->x1 = extended_wire->Port1->x();
    extended_wire->y1 = extended_wire->Port1->y();
    extended_wire->x2 = extended_wire->Port2->x();
    extended_wire->y2 = extended_wire->Port2->y();

    if (label != extended_wire->Label) {
        delete extended_wire->Label;
        extended_wire->Label = label;
    }

    return dissapearing_wire;
}

}

void Schematic::optimizeWires() {
    while (auto* redundant_node = internal::find_redundant_node(a_Nodes)) {
        auto* obsolete_wire = internal::merge_wires_at_node(redundant_node);

        assert(obsolete_wire->Port1 == nullptr);
        assert(obsolete_wire->Port2 == nullptr);
        assert(redundant_node->conn_count() == 0);

        internal::removeFromPtrList(obsolete_wire, a_Wires);
        delete obsolete_wire;

        internal::removeFromPtrList(redundant_node, a_Nodes);
        delete redundant_node;
    }
}

// ---------------------------------------------------
Node* Schematic::selectedNode(int x, int y)
{
    for(Node *pn = a_Nodes->first(); pn != 0; pn = a_Nodes->next()) // test nodes
        if(pn->getSelected(x, y))
            return pn;

    return 0;
}


/* *******************************************************************
   *****                                                         *****
   *****              Actions handling the wires                 *****
   *****                                                         *****
   ******************************************************************* */


// ---------------------------------------------------
// Follows a wire line and selects it.
void Schematic::selectWireLine(Element *pe, Node *pn, bool ctrl)
{
    Node *pn_1st = pn;
    while(pn->conn_count() == 2)
    {
        pe = pn->other_than(pe);

        if(pe->Type != isWire) break;
        if(ctrl) pe->isSelected ^= ctrl;
        else pe->isSelected = true;

        if(((Wire*)pe)->Port1 == pn)  pn = ((Wire*)pe)->Port2;
        else  pn = ((Wire*)pe)->Port1;
        if(pn == pn_1st) break;  // avoid endless loop in wire loops
    }
}

// ---------------------------------------------------
Wire* Schematic::selectedWire(int x, int y)
{
    for(Wire *pw = a_Wires->first(); pw != 0; pw = a_Wires->next())
        if(pw->getSelected(x, y))
            return pw;

    return 0;
}

// ---------------------------------------------------
// Splits the wire "*pw" into two pieces by the node "*pn".
Wire* Schematic::splitWire(Wire *pw, Node *pn)
{
    Wire *newWire = new Wire(pn->cx, pn->cy, pw->x2, pw->y2, pn, pw->Port2);
    newWire->isSelected = pw->isSelected;

    pw->x2 = pn->cx;
    pw->y2 = pn->cy;
    pw->cx = (pw->x1 + pw->x2) / 2;
    pw->cy = (pw->y1 + pw->y2) / 2;
    pw->Port2 = pn;

    newWire->Port2->connect(newWire);
    pn->connect(pw);
    pn->connect(newWire);
    newWire->Port2->disconnect(pw);
    a_Wires->append(newWire);

    if(pw->Label)
        if((pw->Label->cx > pn->cx) || (pw->Label->cy > pn->cy))
        {
            newWire->Label = pw->Label;   // label goes to the new wire
            pw->Label = 0;
            newWire->Label->pOwner = newWire;
        }

    return newWire;
}

// Deletes the wire and the nodes it was connected to if they
// become orphan after removing the wires.
void Schematic::deleteWire(Wire *w, bool remove_orphans)
{
    w->Port1->disconnect(w);
    // Delete node if it has become an orphan
    if (remove_orphans && w->Port1->conn_count() == 0) {
        internal::removeFromPtrList(w->Port1, a_Nodes);
        delete w->Port1;
    }

    w->Port2->disconnect(w);
    // Delete node if it has become an orphan
    if (remove_orphans && w->Port2->conn_count() == 0) {
        internal::removeFromPtrList(w->Port2, a_Nodes);
        delete w->Port2;
    }

    internal::removeFromPtrList(w, a_Wires);
    delete w;
}

/* *******************************************************************
   *****                                                         *****
   *****                  Actions with markers                   *****
   *****                                                         *****
   ******************************************************************* */

Marker* Schematic::setMarker(int x, int y)
{
  // only diagrams ...
  for(Diagram *pd = a_Diagrams->last(); pd != 0; pd = a_Diagrams->prev()){
    if(Marker* m=pd->setMarker(x,y)){
      setChanged(true, true);
      return m;
    }
  }
  return NULL;
}

// Moves the marker pointer left/right on the graph.
void Schematic::markerLeftRight(bool left, const std::vector<Marker*>& markers)
{
    bool acted = false;
    for (auto* m : markers) {
        if (m->moveLeftRight(left)) acted = true;
    }

    if (acted) setChanged(true, true, 'm');
}

// ---------------------------------------------------
// Moves the marker pointer up/down on the more-dimensional graph.
void Schematic::markerUpDown(bool up, const std::vector<Marker*>& markers)
{
    bool acted = false;
    for (auto* m : markers) {
        if (m->moveUpDown(up)) acted = true;
    }

    if (acted) setChanged(true, true, 'm');
}


/* *******************************************************************
   *****                                                         *****
   *****               Actions with all elements                 *****
   *****                                                         *****
   ******************************************************************* */

/* Selects the element that contains the coordinates x/y.
   Returns the pointer to the element.

   If 'flag' is true, the element can be deselected. If
   'flag' is false the element cannot be deselected. The
   purpose of this is to prevent deselection in cases such
   as right-clicking on a selected element to get a context
   menu.
*/
Element* Schematic::selectElement(float fX, float fY, bool flag, int *index)
{
    int n, x = int(fX), y = int(fY);
    Element *pe_1st = 0;
    Element *pe_sel = 0;
    WireLabel *pl = 0;

    // test all nodes and their labels
    for(Node *pn = a_Nodes->last(); pn != 0; pn = a_Nodes->prev())
    {
        if(!flag)
        {
            // The element cannot be deselected
            if(index)
            {
                // 'index' is only true if called from MouseActions::MPressSelect()
                if(pn->getSelected(x, y))
                {
                    // Return the node pointer, as the selection cannot change
                    return pn;
                }
            }
        }

        pl = pn->Label; // Get any wire label associated with the Node
        if(pl)
        {
            if(pl->getSelected(x, y))
            {
                if(flag)
                {
                    // The element can be deselected, so toggle its isSelected member
                    // TODO: I don't see a need for the xor here, a simple ! on the current value
                    // would be clearer and have the same effect?
                    pl->isSelected ^= flag;
                    return pl;
                }
                if(pe_sel)
                {
                    // There is another currently
                    pe_sel->isSelected = false;
                    return pl;
                }
                if(pe_1st == 0)
                {
                    // give access to elements lying beneath by storing this label.
                    // If no label pointer (or other element) has previously been
                    // stored, the current label pointer is stored here.
                    // pe_1st is returned if no other selected element
                    pe_1st = pl;
                }
                if(pl->isSelected)
                {
                    // if current label is already selected, store a pointer to it.
                    // This can be used to cycle through
                    pe_sel = pl;
                }
            }
        }
    }

    // test all wires and wire labels
    for(Wire *pw = a_Wires->last(); pw != 0; pw = a_Wires->prev())
    {
        if(pw->getSelected(x, y))
        {
            if(flag)
            {
                // The element can be deselected
                pw->isSelected ^= flag;
                return pw;
            }
            if(pe_sel)
            {
                pe_sel->isSelected = false;
                return pw;
            }
            if(pe_1st == 0)
            {
                pe_1st = pw;   // give access to elements lying beneath
            }
            if(pw->isSelected)
            {
                pe_sel = pw;
            }
        }
        pl = pw->Label; // test any label associated with the wire
        if(pl)
        {
            if(pl->getSelected(x, y))
            {
                if(flag)
                {
                    // The element can be deselected
                    pl->isSelected ^= flag;
                    return pl;
                }
                if(pe_sel)
                {
                    pe_sel->isSelected = false;
                    return pl;
                }
                if(pe_1st == 0)
                {
                    // give access to elements lying beneath
                    pe_1st = pl;
                }
                if(pl->isSelected)
                {
                    pe_sel = pl;
                }
            }
        }
    }

    // test all components
    for(Component *pc = a_Components->last(); pc != 0; pc = a_Components->prev())
    {
        if(pc->getSelected(x, y))
        {
            if(flag)
            {
                // The element can be deselected
                pc->isSelected ^= flag;
                return pc;
            }
            if(pe_sel)
            {
                pe_sel->isSelected = false;
                return pc;
            }
            if(pe_1st == 0)
            {
                pe_1st = pc;
            }  // give access to elements lying beneath
            if(pc->isSelected)
            {
                pe_sel = pc;
            }
        }
        else
        {
            n = pc->getTextSelected(x, y);
            if(n >= 0)     // was property text clicked ?
            {
                pc->Type = isComponentText;
                if(index)  *index = n;
                return pc;
            }
        }
    }

    const double Corr = 5.0 / a_Scale;  // size of line select and area for resizing
    // test all diagrams
    for(Diagram *pd = a_Diagrams->last(); pd != 0; pd = a_Diagrams->prev())
    {

        for (Graph *pg : pd->Graphs)
        {
            // test markers of graphs
            for (Marker *pm : pg->Markers)
            {
                if(pm->getSelected(x-pd->cx, y-pd->cy))
                {
                    if(flag)
                    {
                        // The element can be deselected
                        pm->isSelected ^= flag;
                        return pm;
                    }
                    if(pe_sel)
                    {
                        pe_sel->isSelected = false;
                        return pm;
                    }
                    if(pe_1st == 0)
                    {
                        pe_1st = pm;   // give access to elements beneath
                    }
                    if(pm->isSelected)
                    {
                        pe_sel = pm;
                    }
                }
            }
        }

        // resize area clicked ?
        if(pd->isSelected)
        {
            if(pd->resizeTouched(fX, fY, Corr))
            {
                if(pe_1st == 0)
                {
                    pd->Type = isDiagramResize;
                    return pd;
                }
            }
        }

        if(pd->getSelected(x, y))
        {
            if(pd->Name[0] == 'T')     // tabular, timing diagram or truth table ?
            {
                if(pd->Name[1] == 'i')
                {
                    if(y > pd->cy)
                    {
                        if(x < pd->cx+pd->xAxis.numGraphs) continue;
                        pd->Type = isDiagramHScroll;
                        return pd;
                    }
                }
                else
                {
                    if(x < pd->cx)        // clicked on scroll bar ?
                    {
                        pd->Type = isDiagramVScroll;
                        return pd;
                    }
                }
            }

            // test graphs of diagram
            for (Graph *pg : pd->Graphs)
            {
                if(pg->getSelected(x-pd->cx, pd->cy-y) >= 0)
                {
                    if(flag)
                    {
                        // The element can be deselected
                        pg->isSelected ^= flag;
                        return pg;
                    }
                    if(pe_sel)
                    {
                        pe_sel->isSelected = false;
                        return pg;
                    }
                    if(pe_1st == 0)
                    {
                        pe_1st = pg;   // access to elements lying beneath
                    }
                    if(pg->isSelected)
                    {
                        pe_sel = pg;
                    }
                }
            }

            if(flag)
            {
                // The element can be deselected
                pd->isSelected ^= flag;
                return pd;
            }
            if(pe_sel)
            {
                pe_sel->isSelected = false;
                return pd;
            }
            if(pe_1st == 0)
            {
                pe_1st = pd;    // give access to elements lying beneath
            }
            if(pd->isSelected)
            {
                pe_sel = pd;
            }
        }
    }

    // test all paintings
    QPoint click(fX, fY);
    auto tolerance = static_cast<int>(Corr);
    for(Painting *pp = a_Paintings->last(); pp != 0; pp = a_Paintings->prev())
    {
        if(pp->isSelected)
        {
            if(pp->resizeTouched(click, tolerance))
            {
                if(pe_1st == 0)
                {
                    pp->Type = isPaintingResize;
                    return pp;
                }
            }
        }

        if(pp->getSelected(click, tolerance))
        {
            if(flag)
            {
                // The element can be deselected
                pp->isSelected ^= flag;
                return pp;
            }
            if(pe_sel)
            {
                pe_sel->isSelected = false;
                return pp;
            }
            if(pe_1st == 0)
            {
                pe_1st = pp;    // give access to elements lying beneath
            }
            if(pp->isSelected)
            {
                pe_sel = pp;
            }
        }
    }

    return pe_1st;
}

void Schematic::highlightWireLabels ()
{
    WireLabel *pltestinner = 0;
    WireLabel *pltestouter = 0;

    // First set highlighting for all wire and nodes labels to false
    for(Wire *pwouter = a_Wires->last(); pwouter != 0; pwouter = a_Wires->prev())
    {
        pltestouter = pwouter->Label; // test any label associated with the wire
        if (pltestouter)
        {
            pltestouter->setHighlighted (false);
        }
    }

    for(Node *pnouter = a_Nodes->last(); pnouter != 0; pnouter = a_Nodes->prev())
    {
        pltestouter = pnouter->Label; // test any label associated with the node
        if (pltestouter)
        {
            pltestouter->setHighlighted (false);
        }
    }

    // Then test every wire's label to see if we need to highlight it
    // and matching labels on wires and nodes
    Q3PtrListIterator<Wire> itwouter(*a_Wires);
    Wire *pwouter;
    while ((pwouter = itwouter.current()) != 0)
    {
        ++itwouter;
        // get any label associated with the wire
        pltestouter = pwouter->Label;
        if (pltestouter)
        {
            if (pltestouter->isSelected)
            {
                bool hiLightOuter = false;
                // Search for matching labels on wires
                Q3PtrListIterator<Wire> itwinner(*a_Wires);
                Wire *pwinner;
                while ((pwinner = itwinner.current()) != 0)
                {
                    ++itwinner;
                    pltestinner = pwinner->Label; // test any label associated with the wire
                    if (pltestinner)
                    {
                        // Highlight the label if it has the same name as the selected label
                        // if only one wire has this label, do not highlight it
                        if (pltestinner != pltestouter)
                        {
                            if (pltestouter->Name == pltestinner->Name)
                            {
                                pltestinner->setHighlighted (true);
                                hiLightOuter = true;
                            }
                        }
                    }
                }
                // Search for matching labels on nodes
                Q3PtrListIterator<Node> itninner(*a_Nodes);
                Node *pninner;
                while ((pninner = itninner.current()) != 0)
                {
                    ++itninner;
                    pltestinner = pninner->Label; // test any label associated with the node
                    if (pltestinner)
                    {
                        if (pltestouter->Name == pltestinner->Name)
                        {
                            // node label matches wire label
                            pltestinner->setHighlighted (true);
                            hiLightOuter = true;
                        }
                    }
                }
                // highlight if at least two different wires/nodes with the same label found
                pltestouter->setHighlighted (hiLightOuter);
            }
        }
    }
    // Same as above but for nodes labels:
    // test every node label to see if we need to highlight it
    // and matching labels on wires and nodes
    Q3PtrListIterator<Node> itnouter(*a_Nodes);
    Node *pnouter;
    while ((pnouter = itnouter.current()) != 0)
    {
        ++itnouter;
        // get any label associated with the node
        pltestouter = pnouter->Label;
        if (pltestouter)
        {
            if (pltestouter->isSelected)
            {
                bool hiLightOuter = false;
                // Search for matching labels on wires
                Q3PtrListIterator<Wire> itwinner(*a_Wires);
                Wire *pwinner;
                while ((pwinner = itwinner.current()) != 0)
                {
                    ++itwinner;
                    pltestinner = pwinner->Label; // test any label associated with the wire
                    if (pltestinner)
                    {
                        if (pltestouter->Name == pltestinner->Name)
                        {
                            // wire label matches node label
                            pltestinner->setHighlighted (true);
                            hiLightOuter = true;
                        }
                    }
                }
                // Search for matching labels on nodes
                Q3PtrListIterator<Node> itninner(*a_Nodes);
                Node *pninner;
                while ((pninner = itninner.current()) != 0)
                {
                    ++itninner;
                    pltestinner = pninner->Label; // test any label associated with the node
                    if (pltestinner)
                    {
                        // Highlight the label if it has the same name as the selected label
                        // if only one node has this label, do not highlight it
                        if (pltestinner != pltestouter)
                        {
                            if (pltestouter->Name == pltestinner->Name)
                            {
                                pltestinner->setHighlighted (true);
                                hiLightOuter = true;
                            }
                        }
                    }
                }
                // highlight if at least two different wires/nodes with the same label found
                pltestouter->setHighlighted (hiLightOuter);
            }
        }
    }
}

// ---------------------------------------------------
// Deselects all elements except 'e'.
void Schematic::deselectElements(Element *e) const
{
    Selection selection = currentSelection();
    auto deselector = [e](auto* elem) { elem->isSelected = elem == e; };

    std::for_each(selection.components.begin(), selection.components.end(), deselector);
    std::for_each(selection.wires.begin(), selection.wires.end(), deselector);
    std::for_each(selection.paintings.begin(), selection.paintings.end(), deselector);
    std::for_each(selection.diagrams.begin(), selection.diagrams.end(), deselector);
    std::for_each(selection.labels.begin(), selection.labels.end(), deselector);
    std::for_each(selection.markers.begin(), selection.markers.end(), deselector);

    for(auto* pd : *a_Diagrams) {
        std::for_each(pd->Graphs.begin(), pd->Graphs.end(), deselector);
    }
}

// Selects elements that lie within or intersect with the rectangle selectionRect
int Schematic::selectElements(const QRect& selection_rect, bool append, bool entirely) const {
    int selected_count = 0;
    int left, top, right, bottom;

    auto select_element = [=](Element* e, const QRect& ebr) {
        // If an element lies within selection rect, it must be selected regardless of any
        // conditions
        if (shouldBeSelected(ebr, selection_rect, entirely)) {
            e->isSelected = true;
        }
        // If an element is not within selection rectangle, but it is already selected and we're
        // not appending to a list of selected items, then the element must be deselected.
        else if (e->isSelected && !append) {
            e->isSelected = false;
        }

        return e->isSelected;
    };

    for (Component *component : *a_Components) {
        component->Bounding(left, top, right, bottom);

        if (select_element(component, QRect{left, top, right - left, bottom - top})) {
            selected_count++;
        }
    }


    for (Wire* wire : *a_Wires)
    {
        if (select_element(wire, QRect{wire->x1, wire->y1, wire->x2 - wire->x1, wire->y2 - wire->y1})) {
            selected_count++;
        }
    }

    WireLabel *label = nullptr;
    for (Wire* wire : *a_Wires) {
        if (wire->Label) {
            label = wire->Label;
            label->getLabelBounding(left,top,right,bottom);

            if (select_element(label, QRect{left, top, right - left, bottom - top})) {
                selected_count++;
            }
        }
    }

    for (Node *node : *a_Nodes) {
        label = node->Label;
        if (label) {
            label->getLabelBounding(left,top,right,bottom);

            if (select_element(label, QRect{left, top, right - left, bottom - top})) {
                selected_count++;
            }
        }
    }

    for (Diagram *diagram : *a_Diagrams) {
        for (Graph *graph: diagram->Graphs) {
            if (graph->isSelected &= append) {
                selected_count++;
            }

            for (Marker *marker: graph->Markers) {
                marker->Bounding(left, top, right, bottom);

                if (select_element(marker, QRect{left, top, right - left, bottom - top})) {
                    selected_count++;
                }
            }
        }

        diagram->Bounding(left, top, right, bottom);

        if (select_element(diagram, QRect{left, top, right - left, bottom - top})) {
            selected_count++;
        }
    }

    for (Painting *painting : *a_Paintings) {
        if (select_element(painting, painting->boundingRect())) {
            selected_count++;
        }
    }

    return selected_count;
}

// ---------------------------------------------------
// Selects all markers.
void Schematic::selectMarkers() const
{
    for(Diagram *pd = a_Diagrams->first(); pd != 0; pd = a_Diagrams->next())
        for (Graph *pg : pd->Graphs)
            for (Marker *pm : pg->Markers)
                pm->isSelected = true;
}

// ---------------------------------------------------
// Deletes all selected elements.
bool Schematic::deleteElements()
{
    bool sel = false;
    auto selection = currentSelection();

    for (auto* comp : selection.components) {     // all selected component
        deleteComp(comp);
        sel = true;
    }

    for (auto* l : selection.labels) {
        l->pOwner->Label = nullptr;
        delete l;
    }

    for (auto* wire : selection.wires) {
        deleteWire(wire);
        sel = true;
    }

    optimizeWires();
    assert(invariants::allComponentsAreConsistent(a_Components));
    assert(invariants::allWiresAreConsistent(a_Wires));
    assert(invariants::noOrphanNodes(a_Nodes));

    for (auto* diagram : selection.diagrams) {
        internal::removeFromPtrList(diagram, a_Diagrams);
        delete diagram;
        sel = true;
    }

    for (auto* pd : *a_Diagrams) {
        QMutableListIterator<Graph *> graph_iter(pd->Graphs);

        while (graph_iter.hasNext()) {
            Graph* graph = graph_iter.next();

            if (graph->isSelected) {
                graph_iter.remove();
                delete graph;
                pd->recalcGraphData();
                sel = true;
                continue;
            }

            QMutableListIterator<Marker*> marker_iter(graph->Markers);
            while (marker_iter.hasNext()) {
                Marker* marker = marker_iter.next();
                if (marker->isSelected) {
                    marker_iter.remove();
                    delete marker;
                    sel = true;
                }
            }
        }
    }

    for (auto* pp : selection.paintings)      // test all paintings
    {
            // Allow removing of manually inserted port symbols when in symbol
            // editing mode. If port symbol is inserted manually i.e. doesn't
            // have a corresponding port in schematic, its nameStr is empty.
            // If it's not empty, then invocation of Schematic::adjustPortNumbers
            // must have found a pairing port in schematic.
            if (pp->Name.trimmed() == ".PortSym" && a_isSymbolOnly) {
                sel = true;
                a_Paintings->remove(pp);
                continue;
            }

            if(pp->Name.at(0) != '.')    // do not delete "PortSym", "ID_text"
            {
                sel = true;
                a_Paintings->remove(pp);
                continue;
            }
    }

    if (sel) {
        updateAllBoundingRect();   // set new document size
        setChanged(sel, true);
    }
    return sel;
}

namespace internal {

std::size_t total_count(const Schematic::Selection& selection) {
    return selection.components.size()
       + selection.wires.size()
       + selection.paintings.size()
       + selection.diagrams.size()
       + selection.labels.size()
       + selection.markers.size();
}

std::optional<QRect> total_br(const std::vector<Component*> comps) {
    if (comps.empty()) return std::nullopt;

    return std::make_optional(std::transform_reduce(
        comps.begin() + 1,
        comps.end(),
        comps.at(0)->boundingRectNoProperties(),
        [](const QRect& a, const QRect& b) { return a.united(b);},
        [](const Component* e) { return e->boundingRectNoProperties(); }
    ));
}

template<typename T>
std::optional<QRect> total_br(const std::vector<T*> elems) {
    if (elems.empty()) return std::nullopt;

    return std::make_optional(std::transform_reduce(
        elems.begin() + 1,
        elems.end(),
        elems.at(0)->boundingRect(),
        [](const QRect& a, const QRect& b) { return a.united(b);},
        [](const T* e) { return e->boundingRect(); }
    ));
}

std::optional<QRect> total_br(const Schematic::Selection& selection) {
    std::optional<QRect> total = total_br(selection.components);

    {
        auto wires = total_br(selection.wires);
        if (total && wires) {
            total = std::make_optional(total->united(wires.value()));
        } else {
            total = wires ? wires : total;
        }
    }
    {
        auto paintings = total_br(selection.paintings);
        if (total && paintings) {
            total = std::make_optional(total->united(paintings.value()));
        } else {
            total = paintings ? paintings : total;
        }
    }
    {
        auto diagrams = total_br(selection.diagrams);
        if (total && diagrams) {
            total = std::make_optional(total->united(diagrams.value()));
        } else {
            total = diagrams ? diagrams : total;
        }
    }
    {
        auto labels = total_br(selection.labels);
        if (total && labels) {
            total = std::make_optional(total->united(labels.value()));
        } else {
            total = labels ? labels : total;
        }
    }
    {
        auto markers = total_br(selection.markers);
        if (total && markers) {
            total = std::make_optional(total->united(markers.value()));
        } else {
            total = markers ? markers : total;
        }
    }

    return total;
}

class Aligner {
    int m_mode;
    QRect m_bounds;
public:
    Aligner(int mode, QRect bounds) : m_mode{mode}, m_bounds{bounds} {};

    void operator() (Element* e) const {
        switch (m_mode) {
        case 0:  // align top
            e->moveCenter(0, m_bounds.top() - e->boundingRect().top());
            break;
        case 1:  // align bottom
            e->moveCenter(0, m_bounds.bottom() - e->boundingRect().bottom());
            break;
        case 2:  // align left
            e->moveCenter(m_bounds.left() - e->boundingRect().left(), 0);
            break;
        case 3:  // align right
            e->moveCenter(m_bounds.right() - e->boundingRect().right(), 0);
            break;
        case 4:  // center horizontally
            e->moveCenter(m_bounds.center().x() - e->boundingRect().center().x(), 0);
            break;
        case 5:  // center vertically
            e->moveCenter(0, m_bounds.center().y() - e->boundingRect().center().y());
            break;
    }
    }

    void operator() (Component* c) const {
        switch (m_mode) {
        case 0:  // align top
            c->moveCenter(0, m_bounds.top() - c->boundingRectNoProperties().top());
            break;
        case 1:  // align bottom
            c->moveCenter(0, m_bounds.bottom() - c->boundingRectNoProperties().bottom());
            break;
        case 2:  // align left
            c->moveCenter(m_bounds.left() - c->boundingRectNoProperties().left(), 0);
            break;
        case 3:  // align right
            c->moveCenter(m_bounds.right() - c->boundingRectNoProperties().right(), 0);
            break;
        case 4:  // center horizontally
            c->moveCenter(m_bounds.center().x() - c->boundingRectNoProperties().center().x(), 0);
            break;
        case 5:  // center vertically
            c->moveCenter(0, m_bounds.center().y() - c->boundingRectNoProperties().center().y());
            break;
    }
    }
};
}

/*!
 * \brief Schematic::aligning align selected elements.
 * \param Mode: top, bottom, left, right, center vertical, center horizontal
 * \return True if aligned
 */
bool Schematic::aligning(int mode)
{
    auto selection = currentSelection();

    if (internal::total_count(selection) <= 1) return false;

    const internal::Aligner aligner{mode, internal::total_br(selection).value()};
    std::ranges::for_each(selection.paintings, aligner);
    std::ranges::for_each(selection.diagrams, aligner);
    std::ranges::for_each(selection.labels, aligner);
    std::ranges::for_each(selection.components, aligner);
    std::ranges::for_each(selection.wires, aligner);

    heal(qucs_s::wire::Planner::PlanType::Straight);

    setChanged(true, true);
    return true;
}

namespace internal {
template <bool x_axis> struct CenterCoordinateSorter {
  bool operator()(Element *a, Element *b) {
    if constexpr (x_axis) {
      return a->center().x() < b->center().x();
    } else {
      return a->center().y() < b->center().y();
    }
  }
};

template <bool x_axis> class Distributor {
  std::size_t current_pos;
  std::size_t last_pos;
  int step;
  int current_coord;

public:
  Distributor(std::vector<Element *> &elements)
      : current_pos{0}, last_pos{elements.size()} {
    std::ranges::sort(elements, CenterCoordinateSorter<x_axis>());

    const auto *first = elements.front();
    const auto *last = elements.back();

    const auto distance =
        std::abs(x_axis ? last->center().x() - first->center().x()
                        : last->center().y() - first->center().y());

    current_coord = x_axis ? first->center().x() : first->center().y();
    step = distance / elements.size() - 1;
  }

  void next() {
    if (current_pos >= last_pos) {
      return;
    }
    current_coord += step;
    current_pos++;
  }

  int get() const { return current_coord; }
};
} // namespace

/*!
 * \brief Schematic::distributeHorizontal sort selection horizontally
 * \return True if sorted
 */
bool Schematic::distributeHorizontal()
{
    auto selection = currentSelection();

    if (internal::total_count(selection) <= 1 ) return false;

    std::vector<Element*> distributed;
    const auto appender = std::back_inserter(distributed);

    std::ranges::copy(selection.components, appender);
    std::ranges::copy(selection.wires, appender);
    std::ranges::copy(selection.paintings, appender);
    std::ranges::copy(selection.diagrams, appender);
    std::ranges::copy(selection.labels, appender);

    internal::Distributor<true> dist(distributed);

    for (auto* e : distributed) {
        e->moveCenterTo(setOnGrid(QPoint{dist.get(), e->center().y()}));
        dist.next();
    }

    heal(qucs_s::wire::Planner::PlanType::Straight);

    setChanged(true, true);
    return true;
}

/*!
 * \brief Schematic::distributeVertical sort selection vertically.
 * \return True if sorted
 */
bool Schematic::distributeVertical()
{
    auto selection = currentSelection();

    if (internal::total_count(selection) <= 1) return false;

    std::vector<Element*> distributed;
    const auto appender = std::back_inserter(distributed);

    std::ranges::copy(selection.components, appender);
    std::ranges::copy(selection.wires, appender);
    std::ranges::copy(selection.paintings, appender);
    std::ranges::copy(selection.diagrams, appender);
    std::ranges::copy(selection.labels, appender);

    internal::Distributor<true> dist(distributed);

    for (auto* e : distributed) {
        e->moveCenterTo(setOnGrid(QPoint{ e->center().x(), dist.get()}));
        dist.next();
    }

    heal(qucs_s::wire::Planner::PlanType::Straight);

    setChanged(true, true);
    return true;
}

// Sets selected elements on grid.
bool Schematic::elementsOnGrid()
{
    auto selection = currentSelection();

    if (internal::total_count(selection) == 0) return false;

    const auto onGridSetter = [this](Element* e) { e->moveCenterTo(setOnGrid(e->center())); };

    std::ranges::for_each(selection.components, onGridSetter);
    std::ranges::for_each(selection.paintings, onGridSetter);
    std::ranges::for_each(selection.diagrams, onGridSetter);
    std::ranges::for_each(selection.labels, onGridSetter);
    std::ranges::for_each(selection.nodes, onGridSetter);

    std::ranges::for_each(selection.wires, [this](Wire* w) {
        setOnGrid(w->x1, w->y1);
        setOnGrid(w->x2, w->y2);
        // Update center
        w->cx = (w->x1 + w->x2) / 2;
        w->cy = (w->y1 + w->y2) / 2;
    });

    heal(qucs_s::wire::Planner::PlanType::Straight);

    setChanged(true, true);
    return true;
}

// Rotates all selected components around their midpoint.
bool Schematic::rotateElements()
{
    auto selection = currentSelection();

    const auto count = internal::total_count(selection);

    if (count < 1) return false;

    const bool in_place = count == 1;
    const auto bounds = internal::total_br(selection);
    if (!bounds) assert(false);

    const auto rotc = bounds->center();

    const auto rotator = [rotc,in_place](Element* e) { in_place ? e->rotate() : e->rotate(rotc); };

    std::ranges::for_each(selection.components, rotator);
    std::ranges::for_each(selection.wires, rotator);
    std::ranges::for_each(selection.paintings, rotator);
    std::ranges::for_each(selection.diagrams, rotator);
    std::ranges::for_each(selection.labels, rotator);
    std::ranges::for_each(selection.nodes, rotator);

    elementsOnGrid();
    return true;
}

// Mirrors all selected components.
bool Schematic::mirrorXComponents()
{
    const auto selection = currentSelection();

    const auto count = internal::total_count(selection);

    if (count < 1) return false;

    const auto in_place = count == 1;
    const auto bounds = internal::total_br(selection);
    if (!bounds) assert(false);

    const auto axis = static_cast<int>(bounds->top() + std::round(bounds->height() / 2.0));

    const auto mirrorer = [axis,in_place](Element* e) { in_place ? e -> mirrorX() : e->mirrorX(axis); };

    std::ranges::for_each(selection.components, mirrorer);
    std::ranges::for_each(selection.wires, mirrorer);
    std::ranges::for_each(selection.paintings, mirrorer);
    std::ranges::for_each(selection.diagrams, mirrorer);
    std::ranges::for_each(selection.labels, mirrorer);
    std::ranges::for_each(selection.nodes, mirrorer);

    elementsOnGrid();
    return true;
}

// Mirrors all selected components.
bool Schematic::mirrorYComponents()
{
    const auto selection = currentSelection();

    const auto count = internal::total_count(selection);

    if (count < 1) return false;

    const auto in_place = count == 1;
    const auto bounds = internal::total_br(selection);
    if (!bounds) assert(false);

    const auto axis = static_cast<int>(bounds->left() + std::round(bounds->width() / 2.0));

    const auto mirrorer = [axis,in_place](Element* e) { in_place ? e->mirrorY() : e->mirrorY(axis); };

    std::ranges::for_each(selection.components, mirrorer);
    std::ranges::for_each(selection.wires, mirrorer);
    std::ranges::for_each(selection.paintings, mirrorer);
    std::ranges::for_each(selection.diagrams, mirrorer);
    std::ranges::for_each(selection.labels, mirrorer);
    std::ranges::for_each(selection.nodes, mirrorer);

    elementsOnGrid();
    return true;
}

/* *******************************************************************
   *****                                                         *****
   *****                Actions with components                  *****
   *****                                                         *****
   ******************************************************************* */

// Finds the correct number for power sources, subcircuit ports and
// digital sources and sets them accordingly.
void Schematic::setComponentNumber(Component* component)
{
    if (component->Props.isEmpty()) return;

    auto* component_number_property = component->Props.front();
    if (component_number_property == nullptr || component_number_property->Name != "Num") return;

    // Collect all numbers that are already in use
    std::set<int> used_numbers;
    for (auto* other_component : *a_Components){
        if (other_component->Model != component->Model) continue;
        used_numbers.insert(other_component->Props.front()->Value.toInt());
    }

    // If current component number cannot be found among used component numbers
    // then everything is ok, we're free to leave now
    if (used_numbers.find(component_number_property->Value.toInt()) == used_numbers.end()) return;

    // Otherwise look for the first free number
    for (int n = 1; ; n++) {
        if (used_numbers.find(n) != used_numbers.end()) continue;
        component_number_property->Value = QString::number(n);
        return;
    }
}

void Schematic::insertComponentNodes(Component *component, bool noOptimize)
{
    // simulation components do not have ports
    if (component->Ports.empty()) return;

    // connect every node of the component to corresponding schematic node
    for (Port *pp : component->Ports) {
        pp->Connection = provideNode(pp->x+component->cx, pp->y+component->cy);
        pp->Connection->connect(component);
    }

    if(noOptimize)  return;

    // if component over wire then delete this wire
    QListIterator<Port *> iport(component->Ports);
    // omit the first element
    Port *component_port = iport.next();
    std::vector<Wire*> dead_wires;
    while (iport.hasNext()) {
        component_port = iport.next();

        // At first iterate over all port's connections to find wires
        // connecting this port to another port of the same component.
        for (auto* connected : *component_port->Connection) {
            if (connected->Type != isWire) {
                continue;
            }

            auto wire = static_cast<Wire*>(connected);

            if (wire->Port1->is_connected(component) && wire->Port2->is_connected(component)) {
                dead_wires.push_back(wire);
            }
        }

        // Disconnect and remove the wires that have been found earlier.
        //
        // It must be done while not iterating over the node's connections
        // because invocation of disconnect() invalidates the iterator over
        // node's connections. In short: calling disconnect() while iterating
        // over connections leads to segmentation fault.
        for (auto w : dead_wires) {
            deleteWire(w);
        }

        dead_wires.clear();
    }
}

// ---------------------------------------------------
// Used for example in moving components.
void Schematic::insertRawComponent(Component *c, bool noOptimize)
{
    // connect every node of component to corresponding schematic node
    insertComponentNodes(c, noOptimize);
    a_Components->append(c);

    // a ground symbol erases an existing label on the wire line
    if(c->Model == "GND")
    {
        c->Model = "x";    // prevent that this ground is found as label
        Element *pe = getWireLabel(c->Ports.first()->Connection);
        if(pe) if((pe->Type & isComponent) == 0)
            {
                delete ((Conductor*)pe)->Label;
                ((Conductor*)pe)->Label = 0;
            }
        c->Model = "GND";    // rebuild component model
    }
}

// ---------------------------------------------------
void Schematic::recreateComponent(Component *Comp)
{

    WireLabel **plMem=0, **pl;
    int PortCount = Comp->Ports.count();

    if(PortCount > 0)
    {
        // Save the labels whose node is not connected to somewhere else.
        // Otherwise the label would be deleted.
        pl = plMem = (WireLabel**)malloc(PortCount * sizeof(WireLabel*));
        for (Port *pp : Comp->Ports)
            if(pp->Connection->conn_count() < 2)
            {
                *(pl++) = pp->Connection->Label;
                pp->Connection->Label = 0;
            }
            else  *(pl++) = 0;
    }


    int x = Comp->tx, y = Comp->ty;
    int x1 = Comp->x1, x2 = Comp->x2, y1 = Comp->y1, y2 = Comp->y2;
    QString tmp = Comp->Name;    // is sometimes changed by "recreate"
    Comp->recreate(this);   // to apply changes to the schematic symbol
    Comp->Name = tmp;
    if(x < x1)
        x += Comp->x1 - x1;
    else if(x > x2)
        x += Comp->x2 - x2;
    if(y < y1)
        y += Comp->y1 - y1;
    else if(y > y2)
        y += Comp->y2 - y2;
    Comp->tx = x;
    Comp->ty = y;


    if(PortCount > 0)
    {
        // restore node labels
        pl = plMem;
        for (Port *pp : Comp->Ports)
        {
            if(*pl != 0)
            {
                (*pl)->cx = pp->Connection->cx;
                (*pl)->cy = pp->Connection->cy;
                placeNodeLabel(*pl);
            }
            pl++;
            if((--PortCount) < 1)  break;
        }
        for( ; PortCount > 0; PortCount--)
        {
            delete (*pl);  // delete not needed labels
            pl++;
        }
        free(plMem);
    }
}

// ---------------------------------------------------
void Schematic::insertComponent(Component *c)
{
    // connect every node of component to corresponding schematic node
    insertComponentNodes(c, false);

    bool ok;
    QString s;
    int  max=1, len = c->Name.length(), z;
    if(c->Name.isEmpty())
    {
        // a ground symbol erases an existing label on the wire line
        if(c->Model == "GND")
        {
            c->Model = "x";    // prevent that this ground is found as label
            Element *pe = getWireLabel(c->Ports.first()->Connection);
            if(pe) if((pe->Type & isComponent) == 0)
                {
                    delete ((Conductor*)pe)->Label;
                    ((Conductor*)pe)->Label = 0;
                }
            c->Model = "GND";    // rebuild component model
        }
    }
    else
    {
        // determines the name by looking for names with the same
        // prefix and increment the number
        for(Component *pc = a_Components->first(); pc != 0; pc = a_Components->next())
            if(pc->Name.left(len) == c->Name)
            {
                s = pc->Name.right(pc->Name.length()-len);
                z = s.toInt(&ok);
                if(ok) if(z >= max) max = z + 1;
            }
        c->Name += QString::number(max);  // create name with new number
    }

    setComponentNumber(c); // important for power sources and subcircuit ports
    a_Components->append(c);
}

// ---------------------------------------------------
void Schematic::activateCompsWithinRect(int x1, int y1, int x2, int y2)
{
    bool changed = false;
    int  cx1, cy1, cx2, cy2, a;
    // exchange rectangle coordinates to obtain x1 < x2 and y1 < y2
    cx1 = (x1 < x2) ? x1 : x2;
    cx2 = (x1 > x2) ? x1 : x2;
    cy1 = (y1 < y2) ? y1 : y2;
    cy2 = (y1 > y2) ? y1 : y2;
    x1 = cx1;
    x2 = cx2;
    y1 = cy1;
    y2 = cy2;


    for(Component *pc = a_Components->first(); pc != 0; pc = a_Components->next())
    {
        pc->Bounding(cx1, cy1, cx2, cy2);
        if(cx1 >= x1) if(cx2 <= x2) if(cy1 >= y1) if(cy2 <= y2)
                    {
                        a = pc->isActive - 1;

                        if(pc->Ports.count() > 1)
                        {
                            if(a < 0)  a = 2;
                            pc->isActive = a;    // change "active status"
                        }
                        else
                        {
                            a &= 1;
                            pc->isActive = a;    // change "active status"
                            if(a == COMP_IS_ACTIVE)  // only for active (not shorten)
                                if(pc->Model == "GND")  // if existing, delete label on wire line
                                    oneLabel(pc->Ports.first()->Connection);
                        }
                        changed = true;
                    }
    }

    if(changed)  setChanged(true, true);
}

// ---------------------------------------------------
bool Schematic::activateSpecifiedComponent(int x, int y)
{
    int x1, y1, x2, y2, a;
    for(Component *pc = a_Components->first(); pc != 0; pc = a_Components->next())
    {
        pc->Bounding(x1, y1, x2, y2);
        if(x >= x1) if(x <= x2) if(y >= y1) if(y <= y2)
                    {
                        a = pc->isActive - 1;

                        if(pc->Ports.count() > 1)
                        {
                            if(a < 0)  a = 2;
                            pc->isActive = a;    // change "active status"
                        }
                        else
                        {
                            a &= 1;
                            pc->isActive = a;    // change "active status"
                            if(a == COMP_IS_ACTIVE)  // only for active (not shorten)
                                if(pc->Model == "GND")  // if existing, delete label on wire line
                                    oneLabel(pc->Ports.first()->Connection);
                        }
                        setChanged(true, true);
                        return true;
                    }
    }
    return false;
}

// ---------------------------------------------------
bool Schematic::activateSelectedComponents()
{
    int a;
    bool sel = false;

    for (auto* pc : currentSelection().components) {
            a = pc->isActive - 1;

            if(pc->Ports.count() > 1)
            {
                if(a < 0)  a = 2;
                pc->isActive = a;    // change "active status"
            }
            else
            {
                a &= 1;
                pc->isActive = a;    // change "active status"
                if(a == COMP_IS_ACTIVE)  // only for active (not shorten)
                    if(pc->Model == "GND")  // if existing, delete label on wire line
                        oneLabel(pc->Ports.first()->Connection);
            }
            sel = true;
        }

    if(sel) setChanged(true, true);
    return sel;
}

// ---------------------------------------------------
// Returns a pointer of the component on whose text x/y points.
Component* Schematic::selectCompText(int x_, int y_, int& w, int& h) const
{
    int a, b, dx, dy;
    for(Component *pc = a_Components->first(); pc != 0; pc = a_Components->next())
    {
        a = pc->cx + pc->tx;
        if(x_ < a)  continue;
        b = pc->cy + pc->ty;
        if(y_ < b)  continue;

        pc->textSize(dx, dy);
        if(x_ > a+dx)  continue;
        if(y_ > b+dy)  continue;

        w = dx;
        h = dy;
        return pc;
    }

    return 0;
}

Component* Schematic::searchSelSubcircuit()
{
    Component *sub=0;

    for(auto* pc : currentSelection().components)
    {
        if(pc->Model != "Sub")
            if(pc->Model != "VHDL")
                if(pc->Model != "Verilog") continue;

        if(sub != 0) return 0;    // more than one subcircuit selected
        sub = pc;
    }
    return sub;
}


// ---------------------------------------------------
Component* Schematic::selectedComponent(int x, int y)
{
    // test all components
    for(Component *pc = a_Components->first(); pc != 0; pc = a_Components->next())
        if(pc->getSelected(x, y))
            return pc;

    return 0;
}

// Disconnect component and remove it from the list of schematic components.
// Component is not deleted, pointer remains valid after call. It is responsibility
// of the caller to handle it by deleting, reinstalling, etc.
void Schematic::detachComp(Component *c)
{
    // delete all port connections
    for (auto* port : c->Ports) {
        port->Connection->disconnect(c);

        // Remove node if it has become orphan
        if (port->Connection->conn_count() == 0) {
            internal::removeFromPtrList(port->Connection, a_Nodes);
            delete port->Connection;
        }
    }
    emit signalComponentDeleted(c);
    internal::removeFromPtrList(c, a_Components);
}

// Deletes the component 'c'.
void Schematic::deleteComp(Component *c)
{
    detachComp(c);
    delete c;
}

Component *Schematic::getComponentByName(const QString& compname) const
{
    for(Component *pc = a_Components->first(); pc != nullptr; pc = a_Components->next()) {
        if (pc->Name.toLower() == compname.toLower()) {
            return pc;
        }
    }
    return nullptr;
}

/* *******************************************************************
   *****                                                         *****
   *****                  Actions with labels                    *****
   *****                                                         *****
   ******************************************************************* */

// Test, if wire connects wire line with more than one label and delete
// all further labels. Also delete all labels if wire line is grounded.
void Schematic::oneLabel(Node *n1)
{
    Wire *pw;
    Node *pn, *pNode;
    WireLabel *pl = 0;
    bool named=false;   // wire line already named ?
    Q3PtrList<Node> Cons;

    for(pn = a_Nodes->first(); pn!=0; pn = a_Nodes->next())
        pn->y1 = 0;   // mark all nodes as not checked

    Cons.append(n1);
    n1->y1 = 1;  // mark Node as already checked
    for(pn = Cons.first(); pn!=0; pn = Cons.next())
    {
        if(pn->Label)
        {
            if(named)
            {
                delete pn->Label;
                pn->Label = 0;    // erase double names
            }
            else
            {
                named = true;
                pl = pn->Label;
            }
        }

        for(auto* pe : *pn)
        {
            if(pe->Type != isWire)
            {
                if(((Component*)pe)->isActive == COMP_IS_ACTIVE)
                    if(((Component*)pe)->Model == "GND")
                    {
                        named = true;
                        if(pl)
                        {
                            pl->pOwner->Label = 0;
                            delete pl;
                        }
                        pl = 0;
                    }
                continue;
            }
            pw = (Wire*)pe;

            if(pn != pw->Port1) pNode = pw->Port1;
            else pNode = pw->Port2;

            if(pNode->y1) continue;
            pNode->y1 = 1;  // mark Node as already checked
            Cons.append(pNode);
            Cons.findRef(pn);

            if(pw->Label)
            {
                if(named)
                {
                    delete pw->Label;
                    pw->Label = 0;    // erase double names
                }
                else
                {
                    named = true;
                    pl = pw->Label;
                }
            }
        }
    }
}

// ---------------------------------------------------
int Schematic::placeNodeLabel(WireLabel *pl)
{
    Node *pn;
    int x = pl->cx;
    int y = pl->cy;

    // check if new node lies upon an existing node
    for(pn = a_Nodes->first(); pn != 0; pn = a_Nodes->next())
        if(pn->cx == x) if(pn->cy == y) break;

    if(!pn)  return -1;

    Element *pe = getWireLabel(pn);
    if(pe)      // name found ?
    {
        if(pe->Type & isComponent)
        {
            delete pl;
            return -2;  // ground potential
        }

        delete ((Conductor*)pe)->Label;
        ((Conductor*)pe)->Label = 0;
    }

    pn->Label = pl;   // insert node label
    pl->Type = isNodeLabel;
    pl->pOwner = pn;
    return 0;
}

// ---------------------------------------------------
// Test, if wire line is already labeled and returns a pointer to the
// labeled element.
Element* Schematic::getWireLabel(Node *pn_)
{
    Wire *pw;
    Node *pn, *pNode;
    Q3PtrList<Node> Cons;

    for(pn = a_Nodes->first(); pn!=0; pn = a_Nodes->next())
        pn->y1 = 0;   // mark all nodes as not checked

    Cons.append(pn_);
    pn_->y1 = 1;  // mark Node as already checked
    for(pn = Cons.first(); pn!=0; pn = Cons.next())
        if(pn->Label) return pn;
        else
            for(auto* pe : *pn)
            {
                if(pe->Type != isWire)
                {
                    if(((Component*)pe)->isActive == COMP_IS_ACTIVE)
                        if(((Component*)pe)->Model == "GND") return pe;
                    continue;
                }

                pw = (Wire*)pe;
                if(pw->Label) return pw;

                if(pn != pw->Port1) pNode = pw->Port1;
                else pNode = pw->Port2;

                if(pNode->y1) continue;
                pNode->y1 = 1;  // mark Node as already checked
                Cons.append(pNode);
                Cons.findRef(pn);
            }
    return 0;   // no wire label found
}


/* *******************************************************************
   *****                                                         *****
   *****                Actions with paintings                   *****
   *****                                                         *****
   ******************************************************************* */

Painting* Schematic::selectedPainting(float fX, float fY)
{
    QPoint click(fX, fY);
    int tolerance = static_cast<int>(5.0 / a_Scale);

    for(Painting *pp = a_Paintings->first(); pp != 0; pp = a_Paintings->next())
        if(pp->getSelected(click, tolerance))
            return pp;

    return 0;
}

std::pair<bool,Node*> Schematic::connectWithWire(const QPoint& a, const QPoint& b) noexcept {
    return connectWithWire(a, b, true, a_wirePlanner.planType());
}

std::pair<bool,Node*> Schematic::connectWithWire(const QPoint& a, const QPoint& b, bool optimize, qucs_s::wire::Planner::PlanType planType) noexcept {

    auto points = qucs_s::wire::Planner::plan(planType, a, b);

    bool hasChanges = false;
    // Take points by pairs
    for (std::size_t i = 1; i < points.size(); i++) {
        auto m = points[i-1];
        auto n = points[i];
        auto [ch, node] = installWire(new Wire(m.x(), m.y(), n.x(), n.y()));
        hasChanges = hasChanges || ch;
    }

    if (optimize) optimizeWires();

    const auto lastPoint = points.back();
    for (auto* node : *a_Nodes) {
        if (node->cx == lastPoint.x() && node->cy == lastPoint.y()) {
            return {hasChanges, node};
        }
    }

    // If we've got there, then the point at which the wire ended
    // was optimized away. For example if there was a wire AB…
    //
    //          A        B
    //          o--------o
    //
    // And a new wire was drawn from point M to point A:
    //
    //   M      A        B
    //   o------o--------o
    //
    // then two wires wouild be merged and point A would be removed:
    //
    //   M               B
    //   o---------------o
    return {hasChanges, nullptr};
}

void Schematic::showEphemeralWire(const QPoint& a, const QPoint& b) noexcept {
    auto points = a_wirePlanner.plan(a, b);
    // Take points by pairs
    for (std::size_t i = 1; i < points.size(); i++) {
        auto m = points[i-1];
        auto n = points[i];
        PostPaintEvent(_Line, m.x(), m.y(), n.x(), n.y(), 3);
    }

}

namespace internal {

template <typename T, std::ranges::forward_range C>
    requires std::same_as<T, std::iter_value_t<C>>
std::vector<std::pair<T, T>> by_pairs(const C& objects) {
    std::vector<std::pair<T, T>> pairs;
    for (std::size_t i = 1; i < objects.size(); i++) {
        pairs.push_back({objects[i - 1], objects[i]});
     }
    return pairs;
}

// Among all wires in a given container finds the one which connects
// nodes A and B. Returns nullptr if no such wire.
template <typename It>
Wire* find_wire(const Node* a, const Node* b, It begin, It end) {
    for (; begin != end; begin++) {
        Wire* w = *begin;
        if ((w->Port1 == a && w->Port2 == b) ||
            (w->Port1 == b && w->Port2 == a)) {
            return w;
        }
     }
    return nullptr;
}
}

std::pair<bool,Node*> Schematic::installWire(Wire* wire)
{
    assert(wire->Port1 == nullptr);
    assert(wire->Port2 == nullptr);

    auto* port1 = provideNode(wire->x1, wire->y1);
    auto* port2 = provideNode(wire->x2, wire->y2);

    auto crossed_nodes =
        qucs_s::geom::on_line(port1, port2, a_Nodes->begin(), a_Nodes->end());

    // Save for later
    auto* wire_label = wire->Label;
    wire->Label      = nullptr;

    // All the nodes the wire goes over taken by pairs, e.g. if the wires
    // goes over nodes A, B, C, D, then this is the sequence of [(A,B), (B,C),
    // (C,D)]
    const auto crossed_node_pairs = internal::by_pairs<Node*,std::vector<Node*>>(crossed_nodes);
    bool has_been_used    = false;
    bool has_changes = false;

    // We don't know how many nodes the wire goes over. Take them by pairs
    // and check if a pair of nodes is already connected by a wire.
    // If not, then fill the gap either with the given wire, or a brand new one.
    //
    // By function contract given wire is NEVER discarded.
    for (const auto& node_pair : crossed_node_pairs) {
        auto* existing_wire =
            internal::find_wire(node_pair.first, node_pair.second, a_Wires->begin(), a_Wires->end());

        // No gap, continue search
        if (existing_wire != nullptr) {
            continue;
        }

        // There is a gap between nodes…

        if (!has_been_used) {
            // … the given wire hasn't been used yet. Fill the gap with it.
            wire->Port1 = node_pair.first;
            wire->Port1->connect(wire);
            wire->Port2 = node_pair.second;
            wire->Port2->connect(wire);
            wire->x1 = wire->Port1->x();
            wire->y1 = wire->Port1->y();
            wire->x2 = wire->Port2->x();
            wire->y2 = wire->Port2->y();
            a_Wires->append(wire);
            has_been_used = true;
            has_changes = true;
        } else {
            // … the given wire has been already used to fill another gap.
            // Fill this gap with a brand new wire.
	        auto* w = new Wire(node_pair.first->x(), node_pair.first->y(),
			                   node_pair.second->x(), node_pair.second->y());
            w->Port1 = node_pair.first;
            w->Port1->connect(w);
            w->Port2 = node_pair.second;
            w->Port2->connect(w);
            a_Wires->append(w);
            has_changes = true;
        }
    }

    // If after iterating over all node pairs, the given wire still hasn't been
    // used, it means that the wire goes over one or several existing wires, i.e.
    // there is already a connection between the points the wire was intended
    // to connect.
    //
    // By function contract the given wire is never discarded, and to fulfil this
    // requirement  the wire between last pair of nodes is replaced with the given
    // wire.
    if (!has_been_used) {
        has_changes = false;  // indicate no changes

        auto last_pair = crossed_node_pairs.back();
        auto* existing_wire =
            internal::find_wire(last_pair.first, last_pair.second, a_Wires->begin(), a_Wires->end());

        if (wire->Label == nullptr) {
            wire->Label = existing_wire->Label;
        } else {
            delete existing_wire->Label;
            existing_wire->Label = nullptr;
        }

        // Detach last wire
        existing_wire->Port1->disconnect(existing_wire);
        existing_wire->Port1 = nullptr;
        existing_wire->Port2->disconnect(existing_wire);
        existing_wire->Port2 = nullptr;

        // And delete it
        internal::removeFromPtrList(existing_wire, a_Wires);
        delete existing_wire;

        // Put the given wire in place of deleted one
        wire->Port1 = last_pair.first;
        wire->Port1->connect(wire);
        wire->Port2 = last_pair.second;
        wire->Port2->connect(wire);

        // Update given wire dimensions
        wire->x1 = wire->Port1->x();
        wire->y1 = wire->Port1->y();
        wire->x2 = wire->Port2->x();
        wire->y2 = wire->Port2->y();

        a_Wires->append(wire);
    }


    // This value (which will be returned) indicates two things:
    // 1. If there was a change in schematic (i.e. the wire doesn't go all over
    //    existing wires)
    // 2. Number of connections of the node at which wire ends
    const std::pair<bool,Node*> result{has_changes, crossed_node_pairs.back().second};

    // Wire had no label, we're done here
    if (wire_label == nullptr) {
        return result;
    }

    // The last step is to put the label at its place
    for (const auto& node_pair : crossed_node_pairs) {
        // At first we check if label root has the same coordinates
        // as one of the nodes over which the wire goes over

        // First node of a pair
        if (wire_label->cx == node_pair.first->x() &&
            wire_label->cy == node_pair.first->y())
             {
                delete node_pair.first->Label;
                node_pair.first->Label = wire_label;
                wire_label             = nullptr;
                break;
             }

        // Second node of a pair
        if (wire_label->cx == node_pair.second->x() &&
            wire_label->cy == node_pair.second->y()) {
                delete node_pair.second->Label;
                node_pair.first->Label = wire_label;
                wire_label             = nullptr;
                break;
             }

        // If we've got here, then label root doesn't have same coordinates
        // as one of the nodes. Maybe it lies on the wire between the nodes

        auto* a_wire =
            internal::find_wire(node_pair.first, node_pair.second, a_Wires->begin(), a_Wires->end());

        if (qucs_s::geom::is_between(QPoint{wire_label->cx, wire_label->cy},
                                     a_wire->Port1, a_wire->Port2)) {
            delete a_wire->Label;
            a_wire->Label = wire_label;
            wire_label    = nullptr;
            break;
         }
    }

    // safety net
    assert(wire_label == nullptr);
    return result;
 }

namespace internal {

class ChangesPainter : public qucs_s::SchematicMutator {
    Schematic* sch;
public:
    ChangesPainter(Schematic* s) : sch{s} {}
    void deleteWire(Wire* w) override {
        // draw a little cross indicating wire removal
        auto c = w->center();
        sch->PostPaintEvent(_Line, c.x() - 5, c.y() - 5, c.x() + 5, c.y() + 5);
        sch->PostPaintEvent(_Line, c.x() + 5, c.y() - 5, c.x() - 5, c.y() + 5);
    }

    void connectPorts(qucs_s::GenericPort* a, qucs_s::GenericPort* b) override {
        sch->showEphemeralWire(a->center(), b->center());
    }

    void connectPortWithNode(qucs_s::GenericPort* port, Node* node) override {
        sch->showEphemeralWire(port->center(), node->center());
    }

    void putLabel(WireLabel* label, const QPoint& p) override {}
    void moveNode(Node* node, const QPoint& p) override {}
};


class ActualMutator : public qucs_s::SchematicMutator {
    Schematic* sch;
public:
    ActualMutator(Schematic* s) : sch{s} {}
    void deleteWire(Wire* w) override { sch->deleteWire(w, false); }

    void connectPorts(qucs_s::GenericPort* a, qucs_s::GenericPort* b) override {
        Node* old_node;
        old_node = a->replaceNodeWith(sch->provideNode(a->center().x(), a->center().y()));
        old_node = b->replaceNodeWith(sch->provideNode(b->center().x(), b->center().y()));
        sch->dumbConnectWithWire(a->center(), b->center());
    }

    void connectPortWithNode(qucs_s::GenericPort* port, Node* node) override {
        Node* old_node;
        old_node = port->replaceNodeWith(sch->provideNode(port->center().x(), port->center().y()));
        sch->dumbConnectWithWire(port->center(), node->center());
    }

    void putLabel(WireLabel* label, const QPoint& p) override {
        for (auto* node : *sch->a_Nodes) {
            if (node->center() == p) {
                label->pOwner->Label = nullptr;
                delete node->Label;
                node->Label = label;
                node->Label->moveRootTo(p.x(), p.y());
                return;
            }
        }

    }

    void moveNode(Node* node, const QPoint& p) override {
        node->moveCenterTo(p);
    }
};

template<typename NodeContainer>
std::vector<std::vector<Node*>> sameloc_nodes(NodeContainer* nodes) {
    std::set<Node*> processed;
    std::vector<std::vector<Node*>> ret;

    for (auto* n1 : *nodes) {
        if (processed.contains(n1)) continue;

        std::vector<Node*> s;
        for (auto* n2 : *nodes) {
            if (n1 != n2 && n1->center() == n2->center()) {
                s.push_back(n2);
                processed.insert(n2);
            }
        }
        if (!s.empty()) {
            s.push_back(n1);
            ret.push_back(s);
        }
    }
    return ret;
}
}

void Schematic::displayMutations() {
    qucs_s::Healer healer{a_Components, a_Wires};
    internal::ChangesPainter p{this};

    for (auto& mutation : healer.planHealing()) {
        mutation.execute(&p);
    }
}

void Schematic::heal(qucs_s::wire::Planner::PlanType planType) {
    assert(invariants::allComponentsAreConsistent(a_Components));
    assert(invariants::allWiresAreConsistent(a_Wires));
    assert(invariants::noOrphanNodes(a_Nodes));

    // Fix "same location nodes" anomalies

    for (auto sameloc_node_group : internal::sameloc_nodes(a_Nodes)) {
        auto recipient = sameloc_node_group.front();

        for (auto donor = (sameloc_node_group.begin() + 1); donor != sameloc_node_group.end(); donor++) {
            internal::merge(*donor, recipient);
            internal::removeFromPtrList(*donor, a_Nodes);
            delete *donor;
        }
    }


    // Fix "zero-length wires" anomalies

    {
        std::vector<Wire*> zerolen_wires;
        for (auto* w : *a_Wires) {
            assert(w->Port1 != nullptr);
            assert(w->Port2 != nullptr);
            if (w->Port1 == w->Port2) {
                zerolen_wires.push_back(w);
            }
        }
        for (auto* w : zerolen_wires) {
            internal::merge(w->Port1, w->Port2);

            if (w->Label != nullptr && w->Port2->Label == nullptr) {
                w->Port2->Label = w->Label;
                w->Label = nullptr;
            }

            deleteWire(w);
        }
    }

    assert(invariants::allComponentsAreConsistent(a_Components));
    assert(invariants::allWiresAreConsistent(a_Wires));
    assert(invariants::noOrphanNodes(a_Nodes));
    assert(invariants::noSamePlaceNodes(a_Nodes));
    assert(invariants::noZeroLenWires(a_Wires));


    // Fix "node above wire" anomalies

    {
        for (auto* n : *a_Nodes) {
            for (auto wit = a_Wires->begin(); wit != a_Wires->end(); wit++) {
                auto w = *wit;
                if (qucs_s::geom::is_between(n, w->Port1, w->Port2)) {
                    splitWire(w, n);
                    wit = a_Wires->begin();
                }
            }
        }
    }


    // Fix "duplicate wires" anomalies

    {
        std::unordered_map<qucs_s::UnorderedPair<Node*,Node*>,Wire*> unique_wires;
        std::vector<Wire*> wire_duplicates;
        for (auto* wire : *a_Wires) {
            qucs_s::UnorderedPair<Node*,Node*> p{wire->Port1, wire->Port2};

            if (unique_wires.contains(p)) {
                if (unique_wires.at(p)->Label == nullptr) {
                    unique_wires.at(p)->Label = wire->Label;
                    wire->Label = nullptr;
                }
                wire_duplicates.push_back(wire);
            } else {
                unique_wires.emplace(p, wire);
            }
        }

        for (auto* wire : wire_duplicates) {
            deleteWire(wire);
        }
    }

    assert(invariants::allComponentsAreConsistent(a_Components));
    assert(invariants::allWiresAreConsistent(a_Wires));
    assert(invariants::noOrphanNodes(a_Nodes));
    assert(invariants::noSamePlaceNodes(a_Nodes));
    assert(invariants::noZeroLenWires(a_Wires));
    assert(invariants::noNodesOnWires(a_Nodes, a_Wires));
    assert(invariants::noDuplicateWires(a_Wires));


    // Fix geometric anomalies

    auto old_plan = a_wirePlanner.setType(planType);
    qucs_s::Healer healer{a_Components, a_Wires};
    internal::ActualMutator mut{this};
    for (auto& mutation : healer.planHealing()) {
        mutation.execute(&mut);
    }
    a_wirePlanner.setType(old_plan);


    // Fix "node above wire" anomalies

    {
        for (auto* n : *a_Nodes) {
            for (auto wit = a_Wires->begin(); wit != a_Wires->end(); wit++) {
                auto w = *wit;
                if (qucs_s::geom::is_between(n, w->Port1, w->Port2)) {
                    splitWire(w, n);
                    wit = a_Wires->begin();
                }
            }
        }
    }


    // Fix "same location nodes" anomalies

    for (auto sameloc_node_group : internal::sameloc_nodes(a_Nodes)) {
        auto recipient = sameloc_node_group.front();

        for (auto donor = (sameloc_node_group.begin() + 1); donor != sameloc_node_group.end(); donor++) {
            internal::merge(*donor, recipient);
            internal::removeFromPtrList(*donor, a_Nodes);
            delete *donor;
        }
    }


    // Fix "duplicate wires" anomalies

    {
        std::unordered_map<qucs_s::UnorderedPair<Node*,Node*>,Wire*> unique_wires;
        std::vector<Wire*> wire_duplicates;
        for (auto* wire : *a_Wires) {
            qucs_s::UnorderedPair<Node*,Node*> p{wire->Port1, wire->Port2};

            if (unique_wires.contains(p)) {
                if (unique_wires.at(p)->Label == nullptr) {
                    unique_wires.at(p)->Label = wire->Label;
                    wire->Label = nullptr;
                }
                wire_duplicates.push_back(wire);
            } else {
                unique_wires.emplace(p, wire);
            }
        }

        for (auto* wire : wire_duplicates) {
            deleteWire(wire);
        }
    }

    {
        std::vector<Node*> orphans;
        for (auto* node : *a_Nodes) {
            if (node->conn_count() == 0) orphans.push_back(node);
        }
        for (auto* node : orphans) {
            internal::removeFromPtrList(node, a_Nodes);
            delete node;
        }
    }


    // Remove wires between ports of the same component
    {
        std::set<Wire*> shorts;
        for (auto* wire : *a_Wires) {
            std::set<Component*> port_1_comps;
            for (auto* connectable : *wire->Port1) {
                if (auto* comp = dynamic_cast<Component*>(connectable)) {
                    port_1_comps.insert(comp);
                }
            }

            if (port_1_comps.empty()) continue;

            std::set<Component*> shorted;
            for (auto* connectable : *wire->Port2) {
                if (auto* comp = dynamic_cast<Component*>(connectable)) {
                    if (port_1_comps.contains(comp)) {
                        shorted.insert(comp);
                    }
                }
            }

            for (auto* comp : shorted) {
                if (comp->boundingRectNoProperties().contains(wire->center())) {
                    shorts.insert(wire);
                }
            }
        }

        for (auto* wire : shorts) {
            deleteWire(wire);
        }
    }

    optimizeWires();
    //
    // Baked
    //

    assert(invariants::allComponentsAreConsistent(a_Components));
    assert(invariants::allWiresAreConsistent(a_Wires));
    assert(invariants::noOrphanNodes(a_Nodes));
    assert(invariants::noSamePlaceNodes(a_Nodes));
    assert(invariants::noZeroLenWires(a_Wires));
    assert(invariants::noNodesOnWires(a_Nodes, a_Wires));
    assert(invariants::noDuplicateWires(a_Wires));
}

void Schematic::dumbConnectWithWire(const QPoint& a, const QPoint& b) noexcept {
    auto points = a_wirePlanner.plan(a, b);

    // Take points by pairs
    for (std::size_t i = 1; i < points.size(); i++) {
        auto m = points[i-1];
        auto n = points[i];

        auto* wire = new Wire(m.x(), m.y(), n.x(), n.y());
        wire->Port1 = provideNode(m.x(), m.y());
        wire->Port1->connect(wire);
        wire->Port2 = provideNode(n.x(), n.y());
        wire->Port2->connect(wire);
        a_Wires->append(wire);
    }
}
// vim:ts=8:sw=2:noet
