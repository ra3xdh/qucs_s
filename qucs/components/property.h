#ifndef COMPONENTS_PROPERTY_H
#define COMPONENTS_PROPERTY_H

#include <QRect>
#include <QString>

class QPainter;

struct Property {
  Property(const QString& _Name="", const QString& _Value="",
	   bool _display=false, const QString& Desc="")
	 : Name(_Name), Value(_Value), display(_display), Description(Desc) {};
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