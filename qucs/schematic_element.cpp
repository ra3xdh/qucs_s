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
#include <stdlib.h>
#include <limits.h>

#include "portsymbol.h"
#include "schematic.h"

#include <set>

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

/* *******************************************************************
   *****                                                         *****
   *****              Actions handling the nodes                 *****
   *****                                                         *****
   ******************************************************************* */

// Inserts a port into the schematic and connects it to another node if
// the coordinates are identical. The node is returned.
Node* Schematic::insertNode(int x, int y, Element *e)
{
    Node *pn;
    // check if new node lies upon existing node
    for(pn = a_Nodes->first(); pn != 0; pn = a_Nodes->next())  // check every node
        if(pn->cx == x) if(pn->cy == y)
            {
                pn->connect(e);
                break;
            }

    if(pn == 0)   // create new node, if no existing one lies at this position
    {
        pn = new Node(x, y);
        a_Nodes->append(pn);
        pn->connect(e);  // connect schematic node to component node
    }
    else return pn;   // return, if node is not new

    // check if the new node lies upon an existing wire
    for(Wire *pw = a_Wires->first(); pw != 0; pw = a_Wires->next())
    {
        if(pw->x1 == x)
        {
            if(pw->y1 > y) continue;
            if(pw->y2 < y) continue;
        }
        else if(pw->y1 == y)
        {
            if(pw->x1 > x) continue;
            if(pw->x2 < x) continue;
        }
        else continue;

        // split the wire into two wires
        splitWire(pw, pn);
        return pn;
    }

    return pn;
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

// Inserts a port into the schematic and connects it to another node if the
// coordinates are identical. If 0 is returned, no new wire is inserted.
// If 2 is returned, the wire line ended.
int Schematic::insertWireNode1(Wire *w)
{
    Node *pn;
    // check if new node lies upon an existing node
    for(pn = a_Nodes->first(); pn != 0; pn = a_Nodes->next()) // check every node
        if(pn->cx == w->x1) if(pn->cy == w->y1) break;

    if(pn != 0)
    {
        pn->connect(w);
        w->Port1 = pn;
        return 2;   // node is not new
    }



    // check if the new node lies upon an existing wire
    for(Wire *ptr2 = a_Wires->first(); ptr2 != 0; ptr2 = a_Wires->next())
    {
        if(ptr2->x1 == w->x1)
        {
            if(ptr2->y1 > w->y1) continue;
            if(ptr2->y2 < w->y1) continue;

            if(ptr2->isHorizontal() == w->isHorizontal())   // ptr2-wire is vertical
            {
                if(ptr2->y2 >= w->y2)
                {
                    delete w;    // new wire lies within an existing wire
                    return 0;
                }
                else
                {
                    // one part of the wire lies within an existing wire
                    // the other part not
                    if(ptr2->Port2->conn_count() == 1)
                    {
                        w->y1 = ptr2->y1;
                        w->Port1 = ptr2->Port1;
                        if(ptr2->Label)
                        {
                            w->Label = ptr2->Label;
                            w->Label->pOwner = w;
                        }
                        ptr2->Port1->disconnect(ptr2);  // two -> one wire
                        ptr2->Port1->connect(w);
                        a_Nodes->removeRef(ptr2->Port2);
                        a_Wires->removeRef(ptr2);
                        return 2;
                    }
                    else
                    {
                        w->y1 = ptr2->y2;
                        w->Port1 = ptr2->Port2;
                        ptr2->Port2->connect(w);   // shorten new wire
                        return 2;
                    }
                }
            }
        }
        else if(ptr2->y1 == w->y1)
        {
            if(ptr2->x1 > w->x1) continue;
            if(ptr2->x2 < w->x1) continue;

            if(ptr2->isHorizontal() == w->isHorizontal())   // ptr2-wire is horizontal
            {
                if(ptr2->x2 >= w->x2)
                {
                    delete w;   // new wire lies within an existing wire
                    return 0;
                }
                else
                {
                    // one part of the wire lies within an existing wire
                    // the other part not
                    if(ptr2->Port2->conn_count() == 1)
                    {
                        w->x1 = ptr2->x1;
                        w->Port1 = ptr2->Port1;
                        if(ptr2->Label)
                        {
                            w->Label = ptr2->Label;
                            w->Label->pOwner = w;
                        }
                        ptr2->Port1->disconnect(ptr2); // two -> one wire
                        ptr2->Port1->connect(w);
                        a_Nodes->removeRef(ptr2->Port2);
                        a_Wires->removeRef(ptr2);
                        return 2;
                    }
                    else
                    {
                        w->x1 = ptr2->x2;
                        w->Port1 = ptr2->Port2;
                        ptr2->Port2->connect(w);   // shorten new wire
                        return 2;
                    }
                }
            }
        }
        else continue;

        pn = new Node(w->x1, w->y1);   // create new node
        a_Nodes->append(pn);
        pn->connect(w);  // connect schematic node to the new wire
        w->Port1 = pn;

        // split the wire into two wires
        splitWire(ptr2, pn);
        return 2;
    }

    pn = new Node(w->x1, w->y1);   // create new node
    a_Nodes->append(pn);
    pn->connect(w);  // connect schematic node to the new wire
    w->Port1 = pn;
    return 1;
}

// ---------------------------------------------------
// if possible, connect two horizontal wires to one
bool Schematic::connectHWires1(Wire *w)
{
    Wire *pw;
    Node *n = w->Port1;

    for (auto* connected : *n) {
        if (connected == w || connected->Type != isWire) {
            continue;
        }

        pw = dynamic_cast<Wire*>(connected);

        if (!pw->isHorizontal()) {
            continue;
        }

        if(pw->x1 < w->x1)
        {
            if(n->conn_count() != 2) continue;
            if(pw->Label)
            {
                w->Label = pw->Label;
                w->Label->pOwner = w;
            }
            else if(n->Label)
            {
                w->Label = n->Label;
                w->Label->pOwner = w;
                w->Label->Type = isHWireLabel;
            }
            w->x1 = pw->x1;
            w->Port1 = pw->Port1;      // new wire lengthens an existing one
            a_Nodes->removeRef(n);
            w->Port1->disconnect(pw);
            w->Port1->connect(w);
            a_Wires->removeRef(pw);
            return true;
        }
        if(pw->x2 >= w->x2)    // new wire lies within an existing one ?
        {
            w->Port1->disconnect(w); // second node not yet made
            delete w;
            return false;
        }
        if(pw->Port2->conn_count() < 2)
        {
            // existing wire lies within the new one
            if(pw->Label)
            {
                w->Label = pw->Label;
                w->Label->pOwner = w;
            }
            pw->Port1->disconnect(pw);
            a_Nodes->removeRef(pw->Port2);
            a_Wires->removeRef(pw);
            return true;
        }
        w->x1 = pw->x2;    // shorten new wire according to an existing one
        w->Port1->disconnect(w);
        w->Port1 = pw->Port2;
        w->Port1->connect(w);
        return true;
    }

    return true;
}

// ---------------------------------------------------
// if possible, connect two vertical wires to one
bool Schematic::connectVWires1(Wire *w)
{
    Wire *pw;
    Node *n = w->Port1;

    for (auto* connected : *n) {
        if (connected == w || connected->Type != isWire) {
            continue;
        }

        pw = dynamic_cast<Wire*>(connected);

        if (pw->isHorizontal()) {
            continue;
        }

        if(pw->y1 < w->y1)
        {
            if(n->conn_count() != 2) continue;
            if(pw->Label)
            {
                w->Label = pw->Label;
                w->Label->pOwner = w;
            }
            else if(n->Label)
            {
                w->Label = n->Label;
                w->Label->pOwner = w;
                w->Label->Type = isVWireLabel;
            }
            w->y1 = pw->y1;
            w->Port1 = pw->Port1;         // new wire lengthens an existing one
            a_Nodes->removeRef(n);
            w->Port1->disconnect(pw);
            w->Port1->connect(w);
            a_Wires->removeRef(pw);
            return true;
        }
        if(pw->y2 >= w->y2)    // new wire lies complete within an existing one ?
        {
            w->Port1->disconnect(w); // second node not yet made
            delete w;
            return false;
        }
        if(pw->Port2->conn_count() < 2)
        {
            // existing wire lies within the new one
            if(pw->Label)
            {
                w->Label = pw->Label;
                w->Label->pOwner = w;
            }
            pw->Port1->disconnect(pw);
            a_Nodes->removeRef(pw->Port2);
            a_Wires->removeRef(pw);
            return true;
        }
        w->y1 = pw->y2;    // shorten new wire according to an existing one
        w->Port1->disconnect(w);
        w->Port1 = pw->Port2;
        w->Port1->connect(w);
        return true;
    }

    return true;
}

// ---------------------------------------------------
// Inserts a port into the schematic and connects it to another node if the
// coordinates are identical. If 0 is returned, no new wire is inserted.
// If 2 is returned, the wire line ended.
int Schematic::insertWireNode2(Wire *w)
{
    Node *pn;
    // check if new node lies upon an existing node
    for(pn = a_Nodes->first(); pn != 0; pn = a_Nodes->next())  // check every node
        if(pn->cx == w->x2) if(pn->cy == w->y2) break;

    if(pn != 0)
    {
        pn->connect(w);
        w->Port2 = pn;
        return 2;   // node is not new
    }



    // check if the new node lies upon an existing wire
    for(Wire *ptr2 = a_Wires->first(); ptr2 != 0; ptr2 = a_Wires->next())
    {
        if(ptr2->x1 == w->x2)
        {
            if(ptr2->y1 > w->y2) continue;
            if(ptr2->y2 < w->y2) continue;

            // (if new wire lies within an existing wire, was already check before)
            if(ptr2->isHorizontal() == w->isHorizontal())   // ptr2-wire is vertical
            {
                // one part of the wire lies within an existing wire
                // the other part not
                if(ptr2->Port1->conn_count() == 1)
                {
                    if(ptr2->Label)
                    {
                        w->Label = ptr2->Label;
                        w->Label->pOwner = w;
                    }
                    w->y2 = ptr2->y2;
                    w->Port2 = ptr2->Port2;
                    ptr2->Port2->disconnect(ptr2);  // two -> one wire
                    ptr2->Port2->connect(w);
                    a_Nodes->removeRef(ptr2->Port1);
                    a_Wires->removeRef(ptr2);
                    return 2;
                }
                else
                {
                    w->y2 = ptr2->y1;
                    w->Port2 = ptr2->Port1;
                    ptr2->Port1->connect(w);   // shorten new wire
                    return 2;
                }
            }
        }
        else if(ptr2->y1 == w->y2)
        {
            if(ptr2->x1 > w->x2) continue;
            if(ptr2->x2 < w->x2) continue;

            // (if new wire lies within an existing wire, was already check before)
            if(ptr2->isHorizontal() == w->isHorizontal())   // ptr2-wire is horizontal
            {
                // one part of the wire lies within an existing wire
                // the other part not
                if(ptr2->Port1->conn_count() == 1)
                {
                    if(ptr2->Label)
                    {
                        w->Label = ptr2->Label;
                        w->Label->pOwner = w;
                    }
                    w->x2 = ptr2->x2;
                    w->Port2 = ptr2->Port2;
                    ptr2->Port2->disconnect(ptr2);  // two -> one wire
                    ptr2->Port2->connect(w);
                    a_Nodes->removeRef(ptr2->Port1);
                    a_Wires->removeRef(ptr2);
                    return 2;
                }
                else
                {
                    w->x2 = ptr2->x1;
                    w->Port2 = ptr2->Port1;
                    ptr2->Port1->connect(w);   // shorten new wire
                    return 2;
                }
            }
        }
        else continue;

        pn = new Node(w->x2, w->y2);   // create new node
        a_Nodes->append(pn);
        pn->connect(w);  // connect schematic node to the new wire
        w->Port2 = pn;

        // split the wire into two wires
        splitWire(ptr2, pn);
        return 2;
    }

    pn = new Node(w->x2, w->y2);   // create new node
    a_Nodes->append(pn);
    pn->connect(w);  // connect schematic node to the new wire
    w->Port2 = pn;
    return 1;
}

// ---------------------------------------------------
// if possible, connect two horizontal wires to one
bool Schematic::connectHWires2(Wire *w)
{
    Wire *pw;
    Node *n = w->Port2;

    for (auto* connected : *n) {
        if (connected == w || connected->Type != isWire) {
            continue;
        }

        pw = dynamic_cast<Wire*>(connected);

        if (!pw->isHorizontal()) {
            continue;
        }

        if(pw->x2 > w->x2)
        {
            if(n->conn_count() != 2) continue;
            if(pw->Label)
            {
                w->Label = pw->Label;
                w->Label->pOwner = w;
            }
            w->x2 = pw->x2;
            w->Port2 = pw->Port2;      // new wire lengthens an existing one
            a_Nodes->removeRef(n);
            w->Port2->disconnect(pw);
            w->Port2->connect(w);
            a_Wires->removeRef(pw);
            return true;
        }
        // (if new wire lies complete within an existing one, was already
        // checked before)

        if(pw->Port1->conn_count() < 2)
        {
            // existing wire lies within the new one
            if(pw->Label)
            {
                w->Label = pw->Label;
                w->Label->pOwner = w;
            }
            pw->Port2->disconnect(pw);
            a_Nodes->removeRef(pw->Port1);
            a_Wires->removeRef(pw);
            return true;
        }
        w->x2 = pw->x1;    // shorten new wire according to an existing one
        w->Port2->disconnect(w);
        w->Port2 = pw->Port1;
        w->Port2->connect(w);
        return true;
    }

    return true;
}

// ---------------------------------------------------
// if possible, connect two vertical wires to one
bool Schematic::connectVWires2(Wire *w)
{
    Wire *pw;
    Node *n = w->Port2;

    for (auto* connected : *n) {
        if (connected == w || connected->Type != isWire) {
            continue;
        }

        pw = dynamic_cast<Wire*>(connected);

        if (pw->isHorizontal()) {
            continue;
        }

        if(pw->y2 > w->y2)
        {
            if(n->conn_count() != 2) continue;
            if(pw->Label)
            {
                w->Label = pw->Label;
                w->Label->pOwner = w;
            }
            w->y2 = pw->y2;
            w->Port2 = pw->Port2;     // new wire lengthens an existing one
            a_Nodes->removeRef(n);
            w->Port2->disconnect(pw);
            w->Port2->connect(w);
            a_Wires->removeRef(pw);
            return true;
        }
        // (if new wire lies complete within an existing one, was already
        // checked before)

        if(pw->Port1->conn_count() < 2)
        {
            // existing wire lies within the new one
            if(pw->Label)
            {
                w->Label = pw->Label;
                w->Label->pOwner = w;
            }
            pw->Port2->disconnect(pw);
            a_Nodes->removeRef(pw->Port1);
            a_Wires->removeRef(pw);
            return true;
        }
        w->y2 = pw->y1;    // shorten new wire according to an existing one
        w->Port2->disconnect(w);
        w->Port2 = pw->Port1;
        w->Port2->connect(w);
        return true;
    }

    return true;
}

// ---------------------------------------------------
// Inserts a vertical or horizontal wire into the schematic and connects
// the ports that hit together. Returns whether the beginning and ending
// (the ports of the wire) are connected or not.
int Schematic::insertWire(Wire *w)
{
    int  tmp, con = 0;
    bool ok;

    // change coordinates if necessary (port 1 coordinates must be less
    // port 2 coordinates)
    if(w->x1 > w->x2)
    {
        tmp = w->x1;
        w->x1 = w->x2;
        w->x2 = tmp;
    }
    else if(w->y1 > w->y2)
    {
        tmp = w->y1;
        w->y1 = w->y2;
        w->y2 = tmp;
    }
    else con = 0x100;



    tmp = insertWireNode1(w);
    if(tmp == 0) return 3;  // no new wire and no open connection
    if(tmp > 1) con |= 2;   // insert node and remember if it remains open

    if(w->isHorizontal()) ok = connectHWires1(w);
    else ok = connectVWires1(w);
    if(!ok) return 3;




    tmp = insertWireNode2(w);
    if(tmp == 0) return 3;  // no new wire and no open connection
    if(tmp > 1) con |= 1;   // insert node and remember if it remains open

    if(w->isHorizontal()) ok = connectHWires2(w);
    else ok = connectVWires2(w);
    if(!ok) return 3;



    // change node 1 and 2
    if(con > 255) con = ((con >> 1) & 1) | ((con << 1) & 2);

    a_Wires->append(w);    // add wire to the schematic




    int  n1, n2;
    Wire *pw, *nw;
    Node *pn, *pn2;

    // ................................................................
    // Check if the new line covers existing nodes.
    // In order to also check new appearing wires -> use "for"-loop
    for(pw = a_Wires->current(); pw != 0; pw = a_Wires->next())
        for(pn = a_Nodes->first(); pn != 0; )    // check every node
        {
            if(pn->cx == pw->x1)
            {
                if(pn->cy <= pw->y1)
                {
                    pn = a_Nodes->next();
                    continue;
                }
                if(pn->cy >= pw->y2)
                {
                    pn = a_Nodes->next();
                    continue;
                }
            }
            else if(pn->cy == pw->y1)
            {
                if(pn->cx <= pw->x1)
                {
                    pn = a_Nodes->next();
                    continue;
                }
                if(pn->cx >= pw->x2)
                {
                    pn = a_Nodes->next();
                    continue;
                }
            }
            else
            {
                pn = a_Nodes->next();
                continue;
            }

            n1 = 2;
            n2 = 3;
            pn2 = pn;
            // check all connections of the current node
            for(auto* pe : *pn)
            {
                if(pe->Type != isWire) continue;
                nw = (Wire*)pe;
                // wire lies within the new ?
                if(pw->isHorizontal() != nw->isHorizontal()) continue;

                pn  = nw->Port1;
                pn2 = nw->Port2;
                n1  = pn->conn_count();
                n2  = pn2->conn_count();
                if(n1 == 1)
                {
                    a_Nodes->removeRef(pn);     // delete node 1 if open
                    pn2->disconnect(nw);   // remove connection
                    pn = pn2;
                }

                if(n2 == 1)
                {
                    pn->disconnect(nw);   // remove connection
                    a_Nodes->removeRef(pn2);     // delete node 2 if open
                    pn2 = pn;
                }

                if(pn == pn2)
                {
                    if(nw->Label)
                    {
                        pw->Label = nw->Label;
                        pw->Label->pOwner = pw;
                    }
                    a_Wires->removeRef(nw);    // delete wire
                    a_Wires->findRef(pw);      // set back to current wire
                }
                break;
            }
            if(n1 == 1) if(n2 == 1) continue;

            // split wire into two wires
            if((pw->x1 != pn->cx) || (pw->y1 != pn->cy))
            {
                nw = new Wire(pw->x1, pw->y1, pn->cx, pn->cy, pw->Port1, pn);
                pn->connect(nw);
                a_Wires->append(nw);
                a_Wires->findRef(pw);
                pw->Port1->connect(nw);
            }
            pw->Port1->disconnect(pw);
            pw->x1 = pn2->cx;
            pw->y1 = pn2->cy;
            pw->Port1 = pn2;
            pn2->connect(pw);

            pn = a_Nodes->next();
        }

    if (a_Wires->containsRef (w))  // if two wire lines with different labels ...
        oneLabel(w->Port1);       // ... are connected, delete one label
    return con | 0x0200;   // sent also end flag
}

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

// ---------------------------------------------------
// If possible, make one wire out of two wires.
bool Schematic::oneTwoWires(Node *n)
{
    Wire *e3;
    Wire *e1 = (Wire*)n->any();  // two wires -> one wire
    Wire *e2 = (Wire*)n->other_than(e1);

    if(e1->Type == isWire) if(e2->Type == isWire)
            if(e1->isHorizontal() == e2->isHorizontal())
            {
                if(e1->x1 == e2->x2) if(e1->y1 == e2->y2)
                    {
                        e3 = e1;
                        e1 = e2;
                        e2 = e3;    // e1 must have lesser coordinates
                    }
                if(e2->Label)     // take over the node name label ?
                {
                    e1->Label = e2->Label;
                    e1->Label->pOwner = e1;
                }
                else if(n->Label)
                {
                    e1->Label = n->Label;
                    e1->Label->pOwner = e1;
                    if(e1->isHorizontal())
                        e1->Label->Type = isHWireLabel;
                    else
                        e1->Label->Type = isVWireLabel;
                }

                e1->x2 = e2->x2;
                e1->y2 = e2->y2;
                e1->Port2 = e2->Port2;
                a_Nodes->removeRef(n);    // delete node (is auto delete)
                e1->Port2->disconnect(e2);
                e1->Port2->connect(e1);
                a_Wires->removeRef(e2);
                return true;
            }
    return false;
}

// ---------------------------------------------------
// Deletes the wire 'w'.
void Schematic::deleteWire(Wire *w)
{
    if(w->Port1->conn_count() == 1)
    {
        if(w->Port1->Label) delete w->Port1->Label;
        a_Nodes->removeRef(w->Port1);     // delete node 1 if open
    }
    else
    {
        w->Port1->disconnect(w);   // remove connection
        if(w->Port1->conn_count() == 2)
            oneTwoWires(w->Port1);  // two wires -> one wire
    }

    if(w->Port2->conn_count() == 1)
    {
        if(w->Port2->Label) delete w->Port2->Label;
        a_Nodes->removeRef(w->Port2);     // delete node 2 if open
    }
    else
    {
        w->Port2->disconnect(w);   // remove connection
        if(w->Port2->conn_count() == 2)
            oneTwoWires(w->Port2);  // two wires -> one wire
    }

    if(w->Label)
    {
        delete w->Label;
        w->Label = 0;
    }
    a_Wires->removeRef(w);
}

// ---------------------------------------------------
int Schematic::copyWires(int& x1, int& y1, int& x2, int& y2,
                         QList<Element *> *ElementCache)
{
    int count=0;
    Node *pn;
    Wire *pw;
    WireLabel *pl;
    for(pw = a_Wires->first(); pw != 0; )  // find bounds of all selected wires
        if(pw->isSelected)
        {
            if(pw->x1 < x1) x1 = pw->x1;
            if(pw->x2 > x2) x2 = pw->x2;
            if(pw->y1 < y1) y1 = pw->y1;
            if(pw->y2 > y2) y2 = pw->y2;

            count++;
            ElementCache->append(pw);

            // rescue non-selected node labels
            pn = pw->Port1;
            if(pn->Label)
                if(pn->conn_count() < 2)
                {
                    ElementCache->append(pn->Label);

                    // Don't set pn->Label->pOwner=0 , so text position stays unchanged.
                    // But remember its wire.
                    pn->Label->pOwner = (Node*)pw;
                    pn->Label = 0;
                }
            pn = pw->Port2;
            if(pn->Label)
                if(pn->conn_count() < 2)
                {
                    ElementCache->append(pn->Label);

                    // Don't set pn->Label->pOwner=0 , so text position stays unchanged.
                    // But remember its wire.
                    pn->Label->pOwner = (Node*)pw;
                    pn->Label = 0;
                }

            pl = pw->Label;
            pw->Label = 0;
            deleteWire(pw);
            pw->Label = pl;    // restore wire label
            pw = a_Wires->current();
        }
        else pw = a_Wires->next();

    return count;
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

// ---------------------------------------------------
// Moves the marker pointer left/right on the graph.
void Schematic::markerLeftRight(bool left, QList<Element*> *Elements)
{
    bool acted = false;
    for (auto* e : *Elements) {
        if (auto* pm = dynamic_cast<Marker*>(e)) {
            if (pm->moveLeftRight(left))
                acted = true;
            }
    }

    if(acted)  setChanged(true, true, 'm');
}

// ---------------------------------------------------
// Moves the marker pointer up/down on the more-dimensional graph.
void Schematic::markerUpDown(bool up, QList<Element*> *Elements)
{
    bool acted = false;
    for (auto* e : *Elements) {
        if (auto* pm = dynamic_cast<Marker*>(e)) {
            if (pm->moveUpDown(up))
                acted = true;
        }
    }

    if(acted)  setChanged(true, true, 'm');
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
// For moving elements: If the moving element is connected to a not
// moving element, insert two wires. If the connected element is already
// a wire, use this wire. Otherwise create new wire.
void Schematic::newMovingWires(Q3PtrList<Element> *p, Node *pn, int pos) const
{
    Element *pe;

    if(pn->State & 8)  // Were new wires already inserted ?
        return;
    pn->State |= 8;

    for (;;)
    {
        if(pn->State & 16)  // node was already worked on
            break;

        if (pn->conn_count() == 0) {
            return;
        }

        if(pn->conn_count() > 1) {
            break;
        }

        pe = pn->any();

        if(pe->Type != isWire)  // is it connected to exactly one wire ?
            break;

        // .................................................
        long  mask = 1, invMask = 3;
        Wire *pw2=nullptr, *pw = (Wire*)pe;

        Node *pn2 = pw->Port1;
        if(pn2 == pn) pn2 = pw->Port2;

        if(pn2->conn_count() == 2) // two existing wires connected ?
            if((pn2->State & (8+4)) == 0)
            {
                Element *pe2 = pn2->other_than(pe);
                // connected wire connected to exactly one wire ?
                if(pe2->Type == isWire)
                    pw2  = (Wire*)pe2;
            }

        // .................................................
        // reuse one wire
        p->insert(pos, pw);
        pw->Port1->disconnect(pw);   // remove connection 1
        pw->Port1->State |= 16+4;
        pw->Port2->disconnect(pw);   // remove connection 2
        pw->Port2->State |= 16+4;
        a_Wires->take(a_Wires->findRef(pw));

        if(pw->isHorizontal()) mask = 2;

        if(pw2 == nullptr)    // place new wire between component and old wire
        {
            pn = pn2;
            mask ^= 3;
            invMask = 0;
        }

        if(pw->Port1 != pn)
        {
            pw->Port1->State |= mask;
            pw->Port1 = (Node*)(uintptr_t)mask;
            pw->Port2->State |= invMask;
            pw->Port2 = (Node*)(uintptr_t)invMask;  // move port 2 completely
        }
        else
        {
            pw->Port1->State |= invMask;
            pw->Port1 = (Node*)(uintptr_t)invMask;
            pw->Port2->State |= mask;
            pw->Port2 = (Node*)(uintptr_t)mask;
        }

        invMask ^= 3;
        // .................................................
        // create new wire ?
        if(pw2 == nullptr)
        {
            if(pw->Port1 != (Node*)(uintptr_t)mask)
                p->insert(pos,
                          new Wire(pw->x2, pw->y2, pw->x2, pw->y2, (Node*)(uintptr_t)mask, (Node*)(uintptr_t)invMask));
            else
                p->insert(pos,
                          new Wire(pw->x1, pw->y1, pw->x1, pw->y1, (Node*)(uintptr_t)mask, (Node*)(uintptr_t)invMask));
            return;
        }


        // .................................................
        // reuse a second wire
        p->insert(pos, pw2);
        pw2->Port1->disconnect(pw2);   // remove connection 1
        pw2->Port1->State |= 16+4;
        pw2->Port2->disconnect(pw2);   // remove connection 2
        pw2->Port2->State |= 16+4;
        a_Wires->take(a_Wires->findRef(pw2));

        if(pw2->Port1 != pn2)
        {
            pw2->Port1 = (Node*)nullptr;
            pw2->Port2->State |= mask;
            pw2->Port2 = (Node*)(uintptr_t)mask;
        }
        else
        {
            pw2->Port1->State |= mask;
            pw2->Port1 = (Node*)(uintptr_t)mask;
            pw2->Port2 = (Node*)nullptr;
        }
        return;
    }

    // only x2 moving
    p->insert(pos, new Wire(pn->cx, pn->cy, pn->cx, pn->cy, (Node*)0, (Node*)1));
    // x1, x2, y2 moving
    p->insert(pos, new Wire(pn->cx, pn->cy, pn->cx, pn->cy, (Node*)1, (Node*)3));
}

// ---------------------------------------------------
// For moving of elements: Copies all selected elements into the
// list 'p' and deletes them from the document.
// BUG: does not (only) copy, as the name suggests.
//      cannot be used to make copies.
// returns the number of "copied" _Markers_ only
int Schematic::copySelectedElements(Q3PtrList<Element> *p)
{
    int i, count = 0;
    Component *pc;
    Wire      *pw;
    Diagram   *pd;
    Element   *pe;
    Node      *pn;


    // test all components *********************************
    // Insert components before wires in order to prevent short-cut removal.
    for(pc = a_Components->first(); pc != nullptr; )
        if(pc->isSelected)
        {
            p->append(pc);
            count++;

            // delete all port connections
            for (Port *pp : pc->Ports)
            {
                pp->Connection->disconnect((Element*)pc);
                pp->Connection->State = 4;
            }

            a_Components->take();   // take component out of the document
            pc = a_Components->current();
        }
        else pc = a_Components->next();

    // test all wires and wire labels ***********************
    for(pw = a_Wires->first(); pw != nullptr; )
    {
        if(pw->Label) if(pw->Label->isSelected)
                p->append(pw->Label);

        if(pw->isSelected)
        {
            p->append(pw);

            pw->Port1->disconnect(pw);   // remove connection 1
            pw->Port1->State = 4;
            pw->Port2->disconnect(pw);   // remove connection 2
            pw->Port2->State = 4;
            a_Wires->take();
            pw = a_Wires->current();
        }
        else pw = a_Wires->next();
    }

    // ..............................................
    // Inserts wires, if a connection to a not moving element is found.
    // The order of the "for"-loops is important to guarantee a stable
    // operation: components, new wires, old wires
    pc = (Component*)p->first();
    for(i=0; i<count; i++)
    {
        for (Port *pp : pc->Ports)
            newMovingWires(p, pp->Connection, count);

        p->findRef(pc);   // back to the real current pointer
        pc = (Component*)p->next();
    }

    for(pe = (Element*)pc; pe != nullptr; pe = p->next())  // new wires
        if(pe->isSelected)
            break;

    for(pw = (Wire*)pe; pw != nullptr; pw = (Wire*)p->next())
        if(pw->Type == isWire)    // not working on labels
        {
            newMovingWires(p, pw->Port1, count);
            newMovingWires(p, pw->Port2, count);
            p->findRef(pw);   // back to the real current pointer
        }


    // ..............................................
    // delete the unused nodes
    for(pn = a_Nodes->first(); pn!=0; )
    {
        if(pn->State & 8)
            if(pn->conn_count() == 2)
                if(oneTwoWires(pn))    // if possible, connect two wires to one
                {
                    pn = a_Nodes->current();
                    continue;
                }

        if(pn->conn_count() == 0)
        {
            if(pn->Label)
            {
                pn->Label->Type = isMovingLabel;
                if(pn->State & 1)
                {
                    if(!(pn->State & 2)) pn->Label->Type = isHMovingLabel;
                }
                else if(pn->State & 2) pn->Label->Type = isVMovingLabel;
                p->append(pn->Label);    // do not forget the node labels
            }
            a_Nodes->remove();
            pn = a_Nodes->current();
            continue;
        }

        pn->State = 0;
        pn = a_Nodes->next();
    }

    // test all node labels
    // do this last to avoid double copying
    for(pn = a_Nodes->first(); pn != 0; pn = a_Nodes->next())
        if(pn->Label) if(pn->Label->isSelected)
                p->append(pn->Label);


    // test all paintings **********************************
    for(Painting *ppa = a_Paintings->first(); ppa != 0; )
        if(ppa->isSelected)
        {
            p->append(ppa);
            a_Paintings->take();
            ppa = a_Paintings->current();
        }
        else ppa = a_Paintings->next();

    count = 0;  // count markers now
    // test all diagrams **********************************
    for(pd = a_Diagrams->first(); pd != 0; )
        if(pd->isSelected)
        {
            p->append(pd);
            a_Diagrams->take();
            pd = a_Diagrams->current();
        }
        else
        {
            for (Graph *pg : pd->Graphs)
            {
                QMutableListIterator<Marker *> im(pg->Markers);
                Marker *pm;
                while (im.hasNext())
                {
                    pm = im.next();
                    if(pm->isSelected)
                    {
                        count++;
                        p->append(pm);
                    }
                }
            }

            pd = a_Diagrams->next();
        }

    return count;
}

// ---------------------------------------------------
bool Schematic::copyComps2WiresPaints(int& x1, int& y1, int& x2, int& y2,
                                      QList<Element *> *ElementCache)
{
    x1=INT_MAX;
    y1=INT_MAX;
    x2=INT_MIN;
    y2=INT_MIN;
    copyLabels(x1, y1, x2, y2, ElementCache);   // must be first of all !
    copyComponents2(x1, y1, x2, y2, ElementCache);
    copyWires(x1, y1, x2, y2, ElementCache);
    copyPaintings(x1, y1, x2, y2, ElementCache);

    if(y1 == INT_MAX) return false;  // no element selected
    return true;
}

// ---------------------------------------------------
// Used in "aligning()", "distributeHorizontal()", "distributeVertical()".
int Schematic::copyElements(int& x1, int& y1, int& x2, int& y2,
                            QList<Element *> *ElementCache)
{
    int bx1, by1, bx2, by2;
    a_Wires->setAutoDelete(false);
    a_Components->setAutoDelete(false);

    x1=INT_MAX;
    y1=INT_MAX;
    x2=INT_MIN;
    y2=INT_MIN;
    // take components and wires out of list, check their boundings
    int number = copyComponents(x1, y1, x2, y2, ElementCache);
    number += copyWires(x1, y1, x2, y2, ElementCache);

    a_Wires->setAutoDelete(true);
    a_Components->setAutoDelete(true);

    // find upper most selected diagram
    for(Diagram *pd = a_Diagrams->last(); pd != 0; pd = a_Diagrams->prev())
        if(pd->isSelected)
        {
            pd->Bounding(bx1, by1, bx2, by2);
            if(bx1 < x1) x1 = bx1;
            if(bx2 > x2) x2 = bx2;
            if(by1 < y1) y1 = by1;
            if(by2 > y2) y2 = by2;
            ElementCache->append(pd);
            number++;
        }
    // find upper most selected painting
    for(Painting *pp = a_Paintings->last(); pp != 0; pp = a_Paintings->prev())
        if(pp->isSelected)
        {
            auto br = pp->boundingRect();
            x1 = std::min(x1, br.left());
            y1 = std::max(y1, br.left() + br.width());
            x2 = std::min(x2, br.top());
            y2 = std::max(y2, br.top() + br.height());
            ElementCache->append(pp);
            number++;
        }

    return number;
}

// ---------------------------------------------------
// Deletes all selected elements.
bool Schematic::deleteElements()
{
    bool sel = false;
    auto selection = currentSelection();

    for (auto* pc : selection.components) {     // all selected component
            deleteComp(pc);
            sel = true;
    }

    for (auto* l : selection.labels) {
        l->pOwner->Label = nullptr;
        delete l;
    }

    // selection.wires cannot be used for traversing,
    // because each removal of a wire or a component may change
    // wire set. Consider example:
    //
    //     o
    //  w1 |   R1
    //     o-[==]-o
    //  w2 |
    //     o
    //
    // Two wires and a component. If component is removed first,
    // then wires w1 and w2 are merged into one wire. If they
    // both were selected, then selection.wires becomes no longer
    // valid.
    for (auto* pw = a_Wires->first(); pw != nullptr; )
    {
        if (pw->isSelected) {
            deleteWire(pw);
            // Call to deleteWire removes one item from the list
            // and everything shifts one position down.
            pw = a_Wires->current();
            sel = true;
        } else {
            pw = a_Wires->next();
        }
    }

    Diagram *pd = a_Diagrams->first();
    while(pd != 0)      // test all diagrams
        if(pd->isSelected)
        {
            a_Diagrams->remove();
            pd = a_Diagrams->current();
            sel = true;
        }
        else
        {
            bool wasGraphDeleted = false;
            // all graphs of diagram

            QMutableListIterator<Graph *> ig(pd->Graphs);
            Graph *pg;

            while (ig.hasNext())
            {
                pg = ig.next();
                // all markers of diagram
                QMutableListIterator<Marker *> im(pg->Markers);
                Marker *pm;
                while (im.hasNext())
                {
                    pm = im.next();
                    if(pm->isSelected)
                    {
                        im.remove();
                        sel = true;
                    }
                }

                if(pg->isSelected)
                {
                    ig.remove();
                    sel = wasGraphDeleted = true;
                }
            }
            if(wasGraphDeleted)
                pd->recalcGraphData();  // update diagram (resize etc.)

            pd = a_Diagrams->next();
        } //else

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

    if(sel)
    {
        updateAllBoundingRect();   // set new document size
        setChanged(sel, true);
    }
    return sel;
}

// ---------------------------------------------------
/*!
 * \brief Schematic::aligning align selected elements.
 * \param Mode: top, bottom, left, right, center vertical, center horizontal
 * \return True if aligned
 */
bool Schematic::aligning(int Mode)
{
    int x1, y1, x2, y2;
    int bx1, by1, bx2, by2, *bx=0, *by=0, *ax=0, *ay=0;
    QList<Element *> ElementCache;
    int count = copyElements(x1, y1, x2, y2, &ElementCache);
    if(count < 1) return false;


    ax = ay = &x2;  // = 0
    switch(Mode)
    {
    case 0:  // align top
        bx = &x1;
        by = &by1;
        y2 = 1;
        break;
    case 1:  // align bottom
        bx = &x1;
        y1 = y2;
        by = &by2;
        y2 = 1;
        break;
    case 2:  // align left
        by = &y1;
        bx = &bx1;
        y2 = 1;
        break;
    case 3:  // align right
        by = &y1;
        x1 = x2;
        bx = &bx2;
        y2 = 1;
        break;
    case 4:  // center horizontally
        x1 = (x2+x1) / 2;
        by = &x2;  // = 0
        ax = &bx1;
        bx = &bx2;
        y1 = 0;
        y2 = 2;
        break;
    case 5:  // center vertically
        y1 = (y2+y1) / 2;
        bx = &x2;  // = 0
        ay = &by1;
        by = &by2;
        x1 = 0;
        y2 = 2;
        break;
    }
    x2 = 0;

    Wire      *pw;
    Component *pc;
    Element   *pe;
    // re-insert elements
    // Go backwards in order to insert node labels before its component.
    QListIterator<Element *> elementCacheIter(ElementCache);
    elementCacheIter.toBack();
    while (elementCacheIter.hasPrevious()) {
        pe = elementCacheIter.previous();
        switch(pe->Type)
        {
        case isComponent:
        case isAnalogComponent:
        case isDigitalComponent:
            pc = (Component*)pe;
            pc->Bounding(bx1, by1, bx2, by2);
            pc->setCenter(x1-((*bx)+(*ax))/y2, y1-((*by)+(*ay))/y2, true);
            insertRawComponent(pc);
            break;

        case isWire:
            pw = (Wire*)pe;
            bx1 = pw->x1;
            by1 = pw->y1;
            bx2 = pw->x2;
            by2 = pw->y2;
            pw->setCenter(x1-((*bx)+(*ax))/y2, y1-((*by)+(*ay))/y2, true);
//        if(pw->Label) {  }
            insertWire(pw);
            break;

        case isDiagram:
            // Should the axis label be counted for ? I guess everyone
            // has a different opinion.
//        ((Diagram*)pe)->Bounding(bx1, by1, bx2, by2);

            // Take size without axis label.
            bx1 = ((Diagram*)pe)->cx;
            by2 = ((Diagram*)pe)->cy;
            bx2 = bx1 + ((Diagram*)pe)->x2;
            by1 = by2 - ((Diagram*)pe)->y2;
            ((Diagram*)pe)->setCenter(x1-((*bx)+(*ax))/y2, y1-((*by)+(*ay))/y2, true);
            break;

        case isPainting: {
            auto br = ((Painting*)pe)->boundingRect();
            bx1 = br.left();
            by1 = br.top();
            bx2 = br.left() + br.width();
            by2 = br.top() + br.height();
            ((Painting*)pe)->setCenter(x1-((*bx)+(*ax))/y2, y1-((*by)+(*ay))/y2, true);
            break;
        }
        case isNodeLabel:
            if(((Element*)(((WireLabel*)pe)->pOwner))->Type & isComponent)
            {
                pc = (Component*)(((WireLabel*)pe)->pOwner);
                pc->Bounding(bx1, by1, bx2, by2);
            }
            else
            {
                pw = (Wire*)(((WireLabel*)pe)->pOwner);
                bx1 = pw->x1;
                by1 = pw->y1;
                bx2 = pw->x2;
                by2 = pw->y2;
            }
            ((WireLabel*)pe)->cx += x1-((*bx)+(*ax))/y2;
            ((WireLabel*)pe)->cy += y1-((*by)+(*ay))/y2;
            insertNodeLabel((WireLabel*)pe);
            break;

        default:
            ;
        }
    }

    ElementCache.clear();
    if(count < 2) return false;

    setChanged(true, true);
    return true;
}

/*!
 * \brief Schematic::distributeHorizontal sort selection horizontally
 * \return True if sorted
 */
bool Schematic::distributeHorizontal()
{
    int x1, y1, x2, y2;
    int bx1, by1, bx2, by2;
    QList<Element *> ElementCache;
    int count = copyElements(x1, y1, x2, y2, &ElementCache);
    if(count < 1) return false;

    Element *pe;
    WireLabel *pl;
    // Node labels are not counted for, so put them to the end.
    /*  for(pe = ElementCache.last(); pe != 0; pe = ElementCache.prev())
        if(pe->Type == isNodeLabel) {
          ElementCache.append(pe);
          ElementCache.removeRef(pe);
        }*/

    // using bubble sort to get elements x ordered
    QListIterator<Element *> elementCacheIter(ElementCache);
    if(count > 1)
        for(int i = count-1; i>0; i--)
        {
            pe = ElementCache.first();
            for(int j=0; j<i; j++)
            {
                pe->getCenter(bx1, by1);
                pe=elementCacheIter.peekNext();
                pe->getCenter(bx2, by2);
                if(bx1 > bx2)    // change two elements ?
                {
                    ElementCache.replace(j+1, elementCacheIter.peekPrevious());
                    ElementCache.replace(j, pe);
                    pe = elementCacheIter.next();
                }
            }
        }

    ElementCache.last()->getCenter(x2, y2);
    ElementCache.first()->getCenter(x1, y1);
    Wire *pw;
    int x = x2;
    int dx=0;
    if(count > 1) dx = (x2-x1)/(count-1);
    // re-insert elements and put them at right position
    // Go backwards in order to insert node labels before its component.
    elementCacheIter.toBack();
    while (elementCacheIter.hasPrevious())
    {
        pe = elementCacheIter.previous();
        switch(pe->Type)
        {
        case isComponent:
        case isAnalogComponent:
        case isDigitalComponent:
            pe->cx = x;
            insertRawComponent((Component*)pe);
            break;

        case isWire:
            pw = (Wire*)pe;
            if(pw->isHorizontal())
            {
                x1 = pw->x2 - pw->x1;
                pw->x1 = x - (x1 >> 1);
                pw->x2 = pw->x1 + x1;
            }
            else  pw->x1 = pw->x2 = x;
//        if(pw->Label) {	}
            insertWire(pw);
            break;

        case isDiagram:
            pe->cx = x - (pe->x2 >> 1);
            break;

        case isPainting:
            pe->getCenter(bx1, by1);
            pe->setCenter(x, by1, false);
            break;

        case isNodeLabel:
            pl = (WireLabel*)pe;
            if(((Element*)(pl->pOwner))->Type & isComponent)
                pe->cx += x - ((Component*)(pl->pOwner))->cx;
            else
            {
                pw = (Wire*)(pl->pOwner);
                if(pw->isHorizontal())
                {
                    x1 = pw->x2 - pw->x1;
                    pe->cx += x - (x1 >> 1) - pw->x1;
                }
                else  pe->cx += x - pw->x1;
            }
            insertNodeLabel(pl);
            x += dx;
            break;

        default:
            ;
        }
        x -= dx;
    }

    ElementCache.clear();
    if(count < 2) return false;

    setChanged(true, true);
    return true;
}

/*!
 * \brief Schematic::distributeVertical sort selection vertically.
 * \return True if sorted
 */
bool Schematic::distributeVertical()
{
    int x1, y1, x2, y2;
    int bx1, by1, bx2, by2;
    QList<Element *> ElementCache;
    int count = copyElements(x1, y1, x2, y2, &ElementCache);
    if(count < 1) return false;

    // using bubble sort to get elements y ordered
    QListIterator<Element *> elementCacheIter(ElementCache);
    Element *pe;
    if(count > 1)
        for(int i = count-1; i>0; i--)
        {
            pe = ElementCache.first();
            for(int j=0; j<i; j++)
            {
                pe->getCenter(bx1, by1);
                pe = elementCacheIter.peekNext();
                pe->getCenter(bx2, by2);
                if(by1 > by2)    // change two elements ?
                {
                    ElementCache.replace(j+1, elementCacheIter.peekPrevious());
                    ElementCache.replace(j, pe);
                    pe = elementCacheIter.next();
                }
            }
        }

    ElementCache.last()->getCenter(x2, y2);
    ElementCache.first()->getCenter(x1, y1);
    Wire *pw;
    int y  = y2;
    int dy=0;
    if(count > 1) dy = (y2-y1)/(count-1);
    // re-insert elements and put them at right position
    // Go backwards in order to insert node labels before its component.
    elementCacheIter.toBack();
    while (elementCacheIter.hasPrevious())
    {
        pe = elementCacheIter.previous();
        switch(pe->Type)
        {
        case isComponent:
        case isAnalogComponent:
        case isDigitalComponent:
            pe->cy = y;
            insertRawComponent((Component*)pe);
            break;

        case isWire:
            pw = (Wire*)pe;
            if(pw->isHorizontal())  pw->y1 = pw->y2 = y;
            else
            {
                y1 = pw->y2 - pw->y1;
                pw->y1 = y - (y1 >> 1);
                pw->y2 = pe->y1 + y1;
            }
//        if(pw->Label) {	}
            insertWire(pw);
            break;

        case isDiagram:
            pe->cy = y + (pe->y2 >> 1);
            break;

        case isPainting:
            pe->getCenter(bx1, by1);
            pe->setCenter(bx1, y, false);
            break;

        case isNodeLabel:
            if(((Element*)(((WireLabel*)pe)->pOwner))->Type & isComponent)
                pe->cy += y - ((Component*)(((WireLabel*)pe)->pOwner))->cy;
            else
            {
                pw = (Wire*)(((WireLabel*)pe)->pOwner);
                if(!pw->isHorizontal())
                {
                    y1 = pw->y2 - pw->y1;
                    pe->cy += y - (y1 >> 1) - pw->y1;
                }
                else  pe->cy += y - pw->y1;
            }
            insertNodeLabel((WireLabel*)pe);
            y += dy;
            break;

        default:
            ;
        }
        y -= dy;
    }

    ElementCache.clear();
    if(count < 2) return false;

    setChanged(true, true);
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
    for (Port *pp : component->Ports)
        pp->Connection = insertNode(pp->x+component->cx, pp->y+component->cy, component);

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
// Sets the component ports anew. Used after rotate, mirror etc.
void Schematic::setCompPorts(Component *pc)
{
    WireLabel *pl;
    Q3PtrList<WireLabel> LabelCache;

    for (Port *pp : pc->Ports)
    {
        pp->Connection->disconnect((Element*)pc);// delete connections
        switch(pp->Connection->conn_count())
        {
        case 0:
            pl = pp->Connection->Label;
            if(pl)
            {
                LabelCache.append(pl);
                pl->cx = pp->x + pc->cx;
                pl->cy = pp->y + pc->cy;
            }
            a_Nodes->removeRef(pp->Connection);
            break;
        case 2:
            oneTwoWires(pp->Connection); // try to connect two wires to one
        default:
            ;
        }
    }

    // Re-connect component node to schematic node. This must be done completely
    // after the first loop in order to avoid problems with node labels.
    for (Port *pp : pc->Ports)
        pp->Connection = insertNode(pp->x+pc->cx, pp->y+pc->cy, pc);

    for(pl = LabelCache.first(); pl != 0; pl = LabelCache.next())
        insertNodeLabel(pl);
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

// ---------------------------------------------------
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

// ---------------------------------------------------
// Deletes the component 'c'.
void Schematic::deleteComp(Component *c)
{
    // delete all port connections
    for (Port *pn : c->Ports)
        switch(pn->Connection->conn_count())
        {
        case 1  :
            delete pn->Connection->Label;
            a_Nodes->removeRef(pn->Connection);  // delete open nodes
            pn->Connection = 0;		  //  (auto-delete)
            break;
        case 3  :
            pn->Connection->disconnect(c);// delete connection
            oneTwoWires(pn->Connection);  // two wires -> one wire
            break;
        default :
            pn->Connection->disconnect(c);// remove connection
            break;
        }
    emit signalComponentDeleted(c);
    a_Components->removeRef(c);   // delete component
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

// ---------------------------------------------------
int Schematic::copyComponents(int& x1, int& y1, int& x2, int& y2,
                              QList<Element *> *ElementCache)
{
    int bx1, by1, bx2, by2, count=0;
    // find bounds of all selected components
    for (auto* pc : currentSelection().components)
    {
            pc->Bounding(bx1, by1, bx2, by2);  // is needed because of "distribute
            if(bx1 < x1) x1 = bx1;             // uniformly"
            if(bx2 > x2) x2 = bx2;
            if(by1 < y1) y1 = by1;
            if(by2 > y2) y2 = by2;

            count++;
            ElementCache->append(pc);

            // rescue non-selected node labels
            for (Port *pp : pc->Ports)
                if(pp->Connection->Label)
                    if(pp->Connection->conn_count() < 2)
                    {
                        ElementCache->append(pp->Connection->Label);

                        // Don't set pp->Connection->Label->pOwner=0,
                        // so text position stays unchanged, but
                        // remember component for align/distribute.
                        pp->Connection->Label->pOwner = (Node*)pc;

                        pp->Connection->Label = 0;
                    }

            deleteComp(pc);
    }
    return count;
}

// ---------------------------------------------------
void Schematic::copyComponents2(int& x1, int& y1, int& x2, int& y2,
                                QList<Element *> *ElementCache)
{
    // find bounds of all selected components
    for (auto* pc : currentSelection().components)
    {
            // is better for unsymmetrical components
            if(pc->cx < x1)  x1 = pc->cx;
            if(pc->cx > x2)  x2 = pc->cx;
            if(pc->cy < y1)  y1 = pc->cy;
            if(pc->cy > y2)  y2 = pc->cy;

            ElementCache->append(pc);

            // rescue non-selected node labels
            for (Port *pp : pc->Ports)
                if(pp->Connection->Label)
                    if(pp->Connection->conn_count() < 2)
                    {
                        ElementCache->append(pp->Connection->Label);
                        pp->Connection->Label = 0;
                        // Don't set pp->Connection->Label->pOwner=0,
                        // so text position stays unchanged.
                    }

            deleteComp(pc);
    }
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

// ---------------------------------------------------
// Inserts a node label.
void Schematic::insertNodeLabel(WireLabel *pl)
{
    if(placeNodeLabel(pl) != -1)
        return;

    // Go on, if label don't lie on existing node.

    Wire *pw = selectedWire(pl->cx, pl->cy);
    if(pw)    // lies label on existing wire ?
    {
        if(getWireLabel(pw->Port1) == 0)  // wire not yet labeled ?
            pw->setName(pl->Name, pl->initValue, 0, pl->cx, pl->cy);

        delete pl;
        return;
    }


    Node *pn = new Node(pl->cx, pl->cy);
    a_Nodes->append(pn);

    pn->Label = pl;
    pl->Type  = isNodeLabel;
    pl->pOwner = pn;
}

// ---------------------------------------------------
void Schematic::copyLabels(int& x1, int& y1, int& x2, int& y2,
                           QList<Element *> *ElementCache)
{
    // find bounds of all selected labels
    for (auto* pl : currentSelection().labels)
    {
                if(pl->x1 < x1) x1 = pl->x1;
                if(pl->y1-pl->y2 < y1) y1 = pl->y1-pl->y2;
                if(pl->x1+pl->x2 > x2) x2 = pl->x1+pl->x2;
                if(pl->y1 > y2) y2 = pl->y1;
                ElementCache->append(pl);
    }
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

// ---------------------------------------------------
void Schematic::copyPaintings(int& x1, int& y1, int& x2, int& y2,
                              QList<Element *> *ElementCache)
{
    // find boundings of all selected paintings
    for (auto* pp : currentSelection().paintings)
        {
            auto br = pp->boundingRect();
            x1 = std::min(x1, br.left());
            y1 = std::max(y1, br.left() + br.width());
            x2 = std::min(x2, br.top());
            y2 = std::max(y2, br.top() + br.height());

            ElementCache->append(pp);
            a_Paintings->take(a_Paintings->find(pp));
        }
}

std::pair<bool,Node*> Schematic::connectWithWire(const QPoint& a, const QPoint& b) noexcept {
    auto points = a_wirePlanner.plan(a, b);

    int resultFlags = 0;
    // Take points by pairs
    for (std::size_t i = 1; i < points.size(); i++) {
        auto m = points[i-1];
        auto n = points[i];
        resultFlags = resultFlags | insertWire(new Wire(m.x(), m.y(), n.x(), n.y()));
    }

    const auto lastPoint = points.back();
    for (auto* node : *a_Nodes) {
        if (node->cx == lastPoint.x() && node->cy == lastPoint.y()) {
            return {resultFlags, node};
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
    return {resultFlags, nullptr};
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

// vim:ts=8:sw=2:noet
