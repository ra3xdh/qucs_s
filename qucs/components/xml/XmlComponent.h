/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef XML_COMPONENT_H
#define XML_COMPONENT_H

#include "../component.h"

class XmlComponent: public Component
{
public:
    class Parameter
    {
    public:
        Parameter(
                const QString& name,
                const QString& unit,
                const QString& defaultValue,
                bool show,
                const QString& description) :
            a_name(name),
            a_unit(unit),
            a_defaultValue(defaultValue),
            a_show(show),
            a_description(description)
        {}

    private:
        QString a_name;
        QString a_unit;
        QString a_defaultValue;
        bool a_show;
        QString a_description;
    };

    XmlComponent(
            const QString& name,
            const QString& schematicId,
            const QString& description,
            const QString& defaultModel,
            const QString& spiceModel,
            const QList<Parameter>& parameters);
    ~XmlComponent() {}

    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne = false);

protected:
    QString netlist();
    QString spice_netlist(spicecompat::SpiceDialect dialect = spicecompat::SPICEDefault);
    virtual QString cdl_netlist();

private:
    QString a_name;
    QString a_schematicId;
    QString a_description;
    QString a_defaultModel;
    QString a_spiceModel;
    QList<Parameter> a_parameters;
};

#endif // XML_COMPONENT_H
