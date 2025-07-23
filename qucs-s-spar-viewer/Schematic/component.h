/***************************************************************************
                                component.h
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
#ifndef COMPONENT_H
#define COMPONENT_H

#include <QGraphicsItem>
#include <QGraphicsView>
#include <QList>
#include <cmath>

#include "../Schematic/graphwidget.h"
#include "../Schematic/infoclasses.h"
#include "../Schematic/structures.h"
#include "../Schematic/symbol.h"
#include "../Schematic/wire.h"
#include "../Misc/general.h"

class Wire;

class Component : public Symbol {
  Q_OBJECT
public:
  Component(GraphWidget *graphWidget, ComponentType, double,
            std::map<QString, QString>, QString ID);
  Component(GraphWidget *graphWidget, struct ComponentInfo);
  void addWire(Wire *Wire);
  QList<Wire *> Wires() const;

  enum { Type = UserType + 1 };
  int type() const { return Type; }

  QRectF boundingRect() const;
  QPainterPath shape() const;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget);
  QPoint getPortLocation(int);
  QString getID();
  void setRotation(double);
  void setParameters(std::map<QString, QString>);
  std::map<QString, QString> getParameters();
  void setComponentType(ComponentType);
  ComponentType getComponentType();

protected:
  QVariant itemChange(GraphicsItemChange change, const QVariant &value);

  void mousePressEvent(QGraphicsSceneMouseEvent *event);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
  QList<Wire *> WireList;
  QPointF newPos;
  GraphWidget *graph;
  ComponentType CompType;
  double Rotation;
  std::map<QString, QString> Value;
  QString ID;

  void RotatePoint(QPoint &);
  void RotatePoint(QPoint &, double);

  // Component symbol code
  void paintCapacitor(QPainter *);
  void paintInductor(QPainter *);
  void paintTransmissionLine(QPainter *);
  void paintResistor(QPainter *);
  void paintTerm(QPainter *);
  void paintGND(QPainter *);
  void paintOpenStub(QPainter *);
  void paintShortStub(QPainter *);
  void paintCoupledLines(QPainter *);
  void paintCoupler(QPainter *);

signals:
  void DoubleClicked(struct ComponentInfo);
};
#endif
