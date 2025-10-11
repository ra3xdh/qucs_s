/*
 *  Copyright (C) 2019-2025 Andrés Martínez Mera - andresmmera@protonmail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef COMPONENT_H
#define COMPONENT_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QList>
#include <QPainter>
#include <QStyleOption>
#include <cmath>
#include <qdebug.h>

#include "../Misc/general.h"
#include "graphwidget.h"
#include "infoclasses.h"
#include "structures.h"
#include "symbol.h"
#include "wire.h"

class Wire;

class Component : public Symbol {
  Q_OBJECT
public:
  Component(GraphWidget* graphWidget, ComponentType, double,
            QMap<QString, QString>, QString ID);
  Component(GraphWidget* graphWidget, struct ComponentInfo);
  ~Component();
  void addWire(Wire* Wire);
  QList<Wire*> Wires() const;

  enum { Type = UserType + 1 };
  int type() const { return Type; }

  QRectF boundingRect() const;
  QPainterPath shape() const;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget);
  QPoint getPortLocation(int);
  QString getID();
  void setRotation(double);
  void setParameters(QMap<QString, QString>);
  QMap<QString, QString> getParameters();
  void setComponentType(ComponentType);
  ComponentType getComponentType();

protected:
  QVariant itemChange(GraphicsItemChange change, const QVariant& value);

  void mousePressEvent(QGraphicsSceneMouseEvent* event);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);

private:
  QList<Wire*> WireList;
  QPointF newPos;
  GraphWidget* graph;
  ComponentType CompType;
  double Rotation;
  QMap<QString, QString> Value;
  QString ID;

  void RotatePoint(QPoint&);
  void RotatePoint(QPoint&, double);

  // Component symbol code
  void paintCapacitor(QPainter*);
  void paintInductor(QPainter*);
  void paintTransmissionLine(QPainter*);
  void paintResistor(QPainter*);
  void paintComplexImpedance(QPainter*);
  void paintTerm(QPainter*);
  void paintGND(QPainter*);
  void paintOpenStub(QPainter*);
  void paintShortStub(QPainter*);
  void paintCoupledLines(QPainter*);
  void paintCoupler(QPainter*);
  void paintSPAR(QPainter*);

  // Microstrip components
  void paintMicrostripLine(QPainter*);
  void paintMicrostripStep(QPainter*);
  void paintMicrostripOpen(QPainter*);
  void paintMicrostripVia(QPainter*);
  void paintMicrostripCoupledLines(QPainter*);

signals:
  void DoubleClicked(struct ComponentInfo);
};
#endif
