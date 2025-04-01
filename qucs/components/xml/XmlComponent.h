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

#include <QString>

class XmlComponent: public Component
{
public:
    struct Parameter
    {
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

        QString a_name;
        QString a_unit;
        QString a_defaultValue;
        bool a_show;
        QString a_description;
    };

    struct PortSym
    {
        PortSym(
                int x,
                int y,
                int type,
                int angle) :
            a_x(x),
            a_y(y),
            a_type(type),
            a_angle(angle)
        {}

        int a_x;
        int a_y;
        int a_type;
        int a_angle;
    };

    struct Line
    {
        Line(
                int x1,
                int y1,
                int x2,
                int y2,
                const QString& color,
                uint32_t width,
                int style) :
            a_x1(x1),
            a_y1(y1),
            a_x2(x2),
            a_y2(y2),
            a_color(color),
            a_width(width),
            a_style(style)
        {}

        int a_x1;
        int a_y1;
        int a_x2;
        int a_y2;
        QString a_color;
        uint32_t a_width;
        int a_style;
    };

    XmlComponent(
            const QString& name,
            const QString& schematicId,
            const QString& description,
            const QString& defaultModel,
            const QString& spiceModel,
            const QString& ngspiceNetList,
            const QString& ngspiceNetListInclude,
            const QString& cdlNetList,
            const QString& cdlNetListInclude,
            const QList<Parameter>& parameters,
            const QList<PortSym>& portSyms,
            const QList<Line>& lines);
    ~XmlComponent() {}

    Component* newOne();
    Element* getInfo(QString&, char* &, bool getNewOne);

protected:
    virtual void createSymbol();
    virtual QString netlist();
    virtual QString spice_netlist(spicecompat::SpiceDialect dialect = spicecompat::SPICEDefault);
    virtual QString cdl_netlist();

private:
    QString a_schematicId;
    QString a_description;
    QString a_ngspiceNetList;
    QString a_ngspiceNetListInclude;
    QString a_cdlNetList;
    QString a_cdlNetListInclude;
    QList<Parameter> a_parameters;
    QList<PortSym> a_portSyms;
    QList<Line> a_lines;
};

#endif // XML_COMPONENT_H
