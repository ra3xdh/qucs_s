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

///
/// @brief Abstract base class for schematic symbols
///
class Symbol : public QGraphicsObject {
public:

  ///
  /// @brief Class constructor
  ///
  Symbol(){}

  ///
  /// @brief Add wire connection to symbol
  /// @note No implementation provided as this is an abstract base class.
  /// The implementation will be overriden in the classes inheriting Symbol
  ///
  virtual void addWire(Wire* Wire)    = 0;

  ///
  /// @brief Get symbol bounding rectangle
  /// @note No implementation provided as this is an abstract base class.
  /// The implementation will be overriden in the classes inheriting Symbol
  ///
  virtual QRectF boundingRect() const = 0;

  ///
  /// @brief Get symbol selection shape
  /// @note No implementation provided as this is an abstract base class.
  /// The implementation will be overriden in the classes inheriting Symbol
  ///
  virtual QPainterPath shape() const  = 0;

  ///
  /// @brief Paint symbol on scene
  /// @note No implementation provided as this is an abstract base class.
  /// The implementation will be overriden in the classes inheriting Symbol
  ///
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                     QWidget* widget) = 0;

  ///
  /// @brief Get port location in symbol coordinates
  /// @note No implementation provided as this is an abstract base class.
  /// The implementation will be overriden in the classes inheriting Symbol
  ///
  virtual QPoint getPortLocation(int) = 0;

protected:

  ///
  /// @brief Handle item change events
  /// @note No implementation provided as this is an abstract base class.
  /// The implementation will be overriden in the classes inheriting Symbol
  ///
  virtual QVariant itemChange(GraphicsItemChange change,
                              const QVariant& value)              = 0;

  ///
  /// @brief Handle mouse press events
  /// @note No implementation provided as this is an abstract base class.
  /// The implementation will be overriden in the classes inheriting Symbol
  ///
  virtual void mousePressEvent(QGraphicsSceneMouseEvent* event)   = 0;

  ///
  /// @brief Handle mouse release events
  /// @note No implementation provided as this is an abstract base class.
  /// The implementation will be overriden in the classes inheriting Symbol
  ///
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) = 0;
};

#endif
