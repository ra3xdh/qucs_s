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

#ifndef WIRE_H
#define WIRE_H

#include <QGraphicsItem>
#include <QPainter>
#include <math.h>

#include "symbol.h"

class Symbol;

///
/// @class Wire
/// @brief Wire connection between symbols
///
class Wire : public QGraphicsItem {
public:
  ///
  /// @brief Construct wire with source and destination
  /// @param sourceNode Source symbol
  /// @param port_num_source Source port number
  /// @param destNode Destination symbol
  /// @param port_num_dest Destination port number
  ///
  Wire(Symbol* sourceNode, int port_num_source, Symbol* destNode,
       int port_num_dest);

  ///
  /// @brief Default constructor
  ///
  Wire(): arrowSize(10) {
    setAcceptedMouseButtons(Qt::NoButton);
  }

  ///
  /// @brief Class destructor
  ///
  ~Wire() {}

  ///
  /// @brief Set wire source
  /// @param sourceNode Source symbol
  /// @param port Source port number
  ///
  void setSource(Symbol* destNode, int port){
    source          = destNode;
    port_num_source = port;
  }

  ///
  /// @brief Set wire destination
  /// @param destNode Destination symbol
  /// @param port Destination port number
  ///
  void setDestination(Symbol* destNode, int port){
    dest          = destNode;
    port_num_dest = port;
  }

  ///
  /// @brief Set wire color
  /// @param Color Wire color
  ///
  void setColor(QColor Color) {
    WireColor = Color;
  }

  ///
  /// @brief Paint wire on scene
  ///
  void paintWire(){
    source->addWire(this);
    dest->addWire(this);
    adjust();
  }

  ///
  /// @brief Get source symbol
  /// @return Source symbol pointer
  ///
  Symbol* sourceNode() const {
    return source;
  }

  ///
  /// @brief Get destination symbol
  /// @return Destination symbol pointer
  ///
  Symbol* destNode() const{
    return dest;
  }

  ///
  /// @brief Adjust wire geometry based on endpoint positions
  ///
  void adjust();

  enum { Type = UserType + 2 };
  int type() const { return Type; }

protected:
  ///
  /// @brief Get wire bounding rectangle
  /// @return Bounding rectangle
  ///
  QRectF boundingRect() const;

  ///
  /// @brief Paint wire
  /// @param painter QPainter instance
  /// @param option Style options
  /// @param widget Target widget
  ///
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget);

private:
  Symbol *source, *dest;       ///< Source and destination symbols
  QColor WireColor;            ///< Wire display color
  QPointF sourcePoint;         ///< Source endpoint in local coordinates
  QPointF destPoint;           ///< Destination endpoint in local coordinates
  qreal arrowSize;             ///< Arrow size for directional indicators
  int port_num_source;         ///< Source port number
  int port_num_dest;           ///< Destination port number
};

#endif
