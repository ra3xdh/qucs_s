/***************************************************************************
                                symbol.h
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
#ifndef SYMBOL_H
#define SYMBOL_H

#include <QGraphicsItem>
#include <QList>
#include <QMessageBox>

class Wire;
class GraphWidget;
QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

class Symbol : public QGraphicsObject {
public:
  Symbol(){};

  virtual void addWire(Wire *Wire) = 0;
  virtual QRectF boundingRect() const = 0;
  virtual QPainterPath shape() const = 0;
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget) = 0;
  virtual QPoint getPortLocation(int) = 0;

protected:
  virtual QVariant itemChange(GraphicsItemChange change,
                              const QVariant &value) = 0;
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) = 0;
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) = 0;
};

#endif
