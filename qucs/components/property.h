#ifndef COMPONENTS_PROPERTY_H
#define COMPONENTS_PROPERTY_H

#include <QRect>
#include <QString>

class QPainter;

struct Property {
  Property() = delete;
  Property(const QString& propertyName,
           const QString& propertyValue,
           bool isVisibleByDefault = false,
           const QString& description = "")
      : Name(propertyName)
      , Value(propertyValue)
      , display(isVisibleByDefault)
      , Description(description)
      {};

  QString Name;
  QString Value;
  bool    display;   // show on schematic or not ?
  QString Description;
  QRect boundingRect() const { return br; };
  void paint(int x, int y, QPainter* p);
private:
  QRect br;
};

#endif