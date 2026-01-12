/// @file component.h
/// @brief Graphical component representation in schematic (definition)
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

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


/// @class Component
/// @brief Graphical component representation in schematic
class Component : public Symbol {
  Q_OBJECT
public:

  /// @brief Component class constructor (individual parameters)
  /// @param graphWidget Parent graph widget
  /// @param type Component type
  /// @param rotation Rotation angle in degrees
  /// @param val Parameter map
  /// @param ID Component identifier
  Component(GraphWidget* graphWidget, ComponentType, double,
            QMap<QString, QString>, QString ID);

  /// @brief Construct component from ComponentInfo struct
  /// @param graphWidget Parent graph widget
  /// @param CI Component information structure
  Component(GraphWidget* graphWidget, struct ComponentInfo);


  /// @brief Class destructor
  ~Component(){}

  /// @brief Add wire connection to component
  /// @param Wire Wire to connect
  void addWire(Wire* Wire);

  /// @brief Get list of connected wires to this component
  /// @return List of wire pointers
  QList<Wire*> Wires() const  { return WireList; }

  enum { Type = UserType + 1 };
  int type() const { return Type; }

  /// @brief Get component bounding rectangle
  /// @return Bounding rectangle for painting
  QRectF boundingRect() const;

  /// @brief Get component selection shape
  /// @return Path defining selectable area
  QPainterPath shape() const;

  /// @brief Paint component on scene
  /// @param painter QPainter instance
  /// @param option Style options
  /// @param widget Target widget
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget);

  /// @brief Get port location in component coordinates
  /// @param port_number Port index
  /// @return Port position
  QPoint getPortLocation(int);

  /// @brief Get component identifier
  /// @return Component ID string
  QString getID() { return ID; }

  /// @brief Set component rotation angle
  /// @param R Rotation in degrees
  void setRotation(double R) { Rotation = R; }

  /// @brief Set component parameters
  /// @param val Parameter map
  /// @note All the parameters are QString based, so they are stored as a QMap which relates two strings, keyed by the parameter name
  void setParameters(QMap<QString, QString> val) { Value = val; }

  /// @brief Get component parameters
  /// @return Parameter map
  QMap<QString, QString> getParameters() { return Value; }

  /// @brief Set component type
  /// @param CT Component type
  void setComponentType(ComponentType CT) { CompType = CT; }

  /// @brief Get component type
  /// @return Component type
  ComponentType getComponentType() { return CompType; }

protected:
  /// @brief Handle item change events
  QVariant itemChange(GraphicsItemChange change, const QVariant& value);

  ///  @brief Handle mouse press events
  void mousePressEvent(QGraphicsSceneMouseEvent* event) {
    update();
    QGraphicsItem::mousePressEvent(event);
  }

  /// @brief Handle mouse release events
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    update();
    QGraphicsItem::mouseReleaseEvent(event);
  }

  /// @brief Handle double click events
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);

private:
  QList<Wire*> WireList;           ///< Connected wires
  QPointF newPos;                  ///< New position during moves
  GraphWidget* graph;              ///< Parent graph widget
  ComponentType CompType;          ///< Component type
  double Rotation;                 ///< Rotation angle in degrees
  QMap<QString, QString> Value;    ///< Component parameters
  QString ID;                      ///< Component identifier

  /// @brief Rotate the port with respect to the object centroid
  /// @param P Position of the port
  void RotatePoint(QPoint&);

  /// @brief Rotate the port with respect to the object centroid
  /// @param P Position of the port
  /// @param angle Angle of rotation
  void RotatePoint(QPoint&, double);

  /// @name Component painting methods
  /// @{
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
  /// @}
signals:
  /// @brief Emitted when component is double-clicked
  /// @param CI Component information
  void DoubleClicked(struct ComponentInfo);
};
#endif
