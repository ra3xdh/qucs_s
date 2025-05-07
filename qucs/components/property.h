#ifndef COMPONENTS_PROPERTY_H
#define COMPONENTS_PROPERTY_H

#include <QRect>
#include <QString>
#include "../extsimkernels/spicecompat.h"

class QPainter;

struct Property {
  enum class Type { File, Equation, Value };

  Property() = delete;
  Property(const QString& propertyName,
           const QString& propertyValue,
           bool isVisibleByDefault = false,
           const QString& description = "",
           Type propertyType = Type::Value,
           spicecompat::Simulator supportedSims = spicecompat::Simulator::simAll)
      : Name(propertyName)
      , Value(propertyValue)
      , display(isVisibleByDefault)
      , Description(description)
      , type(propertyType)
      , simulators(supportedSims)
      {};

  QString Name;
  QString Value;
  bool    display;   // show on schematic or not ?
  QString Description;
  Type type;
  spicecompat::Simulator simulators;
  QRect boundingRect() const { return br; };
  void paint(int x, int y, QPainter* p);
private:
  QRect br;
};

#endif