/***************************************************************************
                                SchematicContent.h
                                ----------
    author                :  2019 Andres Martinez-Mera
    email                  :  andresmmera@protonmail.com
 ***************************************************************************/

/***************************************************************************
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 ***************************************************************************/

#ifndef SCHEMATICCONTENT_H
#define SCHEMATICCONTENT_H

#include <QPen>
#include <QGraphicsTextItem>

#include "infoclasses.h"
#include "structures.h"
#include "../Misc/general.h"

class Component;

class SchematicContent {
public:
  SchematicContent();
  struct NetworkInfo
  getLadder(); // Returns a ladder code for using the internal
               // simulator. It's faster than using Qucs, but it is
               // restricted to ladder networks
  QString getQucsNetlist();
  QString getSParameterNetlist();
  void setNetlist(QString);
  QString Name;
  QString Type;

private:
  void assignNetToWiresConnectedToNode(QString, QString);

public:
  // Setter getter functions

  // Components, wires and nodes
  void appendComponent(ComponentInfo);
  void appendWire(WireInfo);
  void appendWire(QString, int, QString, int);
  void appendWire(QString, int, QString, int, QColor);
  void appendNode(NodeInfo);
  void appendText(QGraphicsTextItem * text);


  double getZin();
  double getZout();
  QString getZinString();
  QString getZoutString();

  QList<ComponentInfo> getComponents();
  QList<WireInfo> getWires();
  QList<NodeInfo> getNodes();
  QList<QGraphicsTextItem *> getTexts();

  QMap<ComponentType, int>  NumberComponents; // List for assigning IDs to the filter components
  unsigned int NumberWires;


private:
  QList<ComponentInfo> Comps;
  QList<WireInfo> Wires;
  QList<NodeInfo> Nodes;
  QList<QGraphicsTextItem *> Texts;
  QString Description;
  QString netlist;
};

#endif // SCHEMATICCONTENT_H
