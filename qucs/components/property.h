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

  class Builder {
    QString m_name;
    QString m_value;
    QString m_description               = "";
    bool m_display                      = true;
    Property::Type m_type               = Property::Type::Value;
    spicecompat::Simulator m_simulators = spicecompat::simAll;

  public:
    // Shotcut for name(name).value(value).desc(desc).buildNew()
    Property* property(const QString& name, const QString& value, const QString& desc)
    {
      this->name(name).value(value).desc(desc);
      return buildNew();
    }

    Builder& name(const QString& name)
    {
      m_name = name;
      return *this;
    }

    Builder& value(const QString& value)
    {
      m_value = value;
      return *this;
    }

    Builder& hidden()
    {
      m_display = false;
      return *this;
    }
    Builder& visible()
    {
      m_display = true;
      return *this;
    }
    Builder& desc(const QString& d)
    {
      m_description = d;
      return *this;
    }
    Builder& type(Property::Type t) {
      m_type = t;
      return *this;
    }
    Builder& simulator(spicecompat::Simulator s) {
      m_simulators = s;
      return *this;
    }
    Property* buildNew() {
      return new Property(m_name, m_value, m_display, m_description, m_type,
                          m_simulators);
    }
  };

private:
  QRect br;
};

#endif