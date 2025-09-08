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
#include <QList>

/**
 * The class XmlComponent is the internal representations of XML based qucs-s
 * components. The associated XML Schema can be found under library/components/Component.xsd.
 * Automatic code generation for schema-based read-in and validation of XML components is
 * integrated in build system.
 *
 * Symbol sharing:
 * The information for graphic representation of a component can be outsourced to an external file
 * and be reused by other components too. In that case the Symbol section only refers that file,
 * e.g.:
 *
 * <Symbol id="Standard">
 *    <File>{QUCS_S_COMPONENTS_LIBRARY}/Resistor.sym</File>
 * </Symbol>
 *
 * {QUCS_S_COMPONENTS_LIBRARYa} will be resolve to 'path_of_installed_qucs_s_components/components',
 * e.g. /usr/local/share/qucs-s/components/ under Linux.
 *
 * Parameter:
 * Muparser (https://beltoforion.de/en/muparser) compatible parameter
 * equations are supported for XML components, e.g.
 *
 * <Parameter name="Vrms" unit="V" equation="sqrt(Z/1000.0)*10^(P/20)" show="true">
 *    <Description>RMS Voltage</Description>
 * </Parameter>
 * <Parameter name="Z" unit="Ohm" default_value="50" show="true">
 *    <Description>Port impedance</Description>
 * </Parameter>
 * <Parameter name="P" unit="dBm" default_value="0" show="true">
 *    <Description>(Available) AC power (dBm)</Description>
 * </Parameter>
 *
 * Netlisting:
 * Netlist generation of XML components is template based. There is dedicated template possible
 * for every type of Spice, Xyce, CDL and Qucsator.
 * The following pre-defined netlist placeholder will be supported:
 * - {PartCounter} : will be replaced by the qucs_s internal component counter
 * - {nets} : will be replace be the component-assigned nets
 * Component defined parameters can be referred as {parameter_name}, e.g.
 * <NgspiceNetlist value = 'VP{PartCounter} {nets} dc 0 ac {P} SIN(0 {P} {f}) portnum {NumPorts} z0 {Z}'>
 * An alternative to dedicated netlists is referencing another netlist template and usage of
 * type-conditional statements. Referencing another netlist template is done by the template value
 * {netlist_name}, e.g. <CDLNetlist value = '{NgspiceNetlist}'> </CDLNetlist>. In that case the
 * Ngspice netlist template will be taken as base for CDL netlisting of this component.
 * Type-conditional statements have the form '{{statement}}::condition' (1) or
 * '{{statement}}::!condition' (2) with condition = {CDL|XYCE|SPICE}.
 * (1) will be resolved so that the statement will be used only if the actual netlisting type
 * matches the condition, e.g. {{p={p}}}::CDL will be resolved to p='value_of_parameter_p' if a
 * CDL netlist shall be generated, otherwise to an empty string.
 * (2) will be resolved so that the statement will be used only if the actual netlisting type
 * don't matches the condition, e.g. {{p={p}}}::!CDL will be resolved to p='value_of_parameter_p'
 * if a netlist of other type then CDL shall be generated, otherwise to an empty string.
 */

class XmlComponent: public Component
{
public:
    struct Parameter
    {
        Parameter(
                const QString& name,
                const QString& unit,
                const QString& defaultValue,
                const QString& equation,
                bool show,
                const QString& description,
                const QString& condition) :
            a_name(name),
            a_unit(unit),
            a_defaultValue(defaultValue),
            a_equation(equation),
            a_show(show),
            a_description(description),
            a_condition(condition)
        {}

        QString a_name;
        QString a_unit;
        QString a_defaultValue;
        QString a_equation;
        bool a_show;
        QString a_description;
        QString a_condition;
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
                int style,
                const QString& condition) :
            a_x1(x1),
            a_y1(y1),
            a_x2(x2),
            a_y2(y2),
            a_color(color),
            a_width(width),
            a_style(style),
            a_condition(condition)
        {}

        int a_x1;
        int a_y1;
        int a_x2;
        int a_y2;
        QString a_color;
        uint32_t a_width;
        int a_style;
        QString a_condition;
    };

    struct Arc
    {
        Arc(
                int x,
                int y,
                int arcWidth,
                int height,
                int angle,
                int len,
                const QString& color,
                uint32_t width,
                int style) :
            a_x(x),
            a_y(y),
            a_arcWidth(arcWidth),
            a_height(height),
            a_angle(angle),
            a_len(len),
            a_color(color),
            a_width(width),
            a_style(style)
        {}

        int a_x;
        int a_y;
        int a_arcWidth;
        int a_height;
        int a_angle;
        int a_len;
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
            const QString& xyceNetList,
            const QString& xyceNetListInclude,
            const QString& qucsatorNetList,
            const QString& qucsatorNetListInclude,
            const QList<Parameter>& parameters,
            const QList<PortSym>& portSyms,
            const QList<Line>& lines,
            const QList<Arc>& arcs);
    ~XmlComponent() {}

    Component* newOne();
    Element* getInfo(QString&, char* &, bool getNewOne);

protected:
    virtual void createSymbol();
    virtual QString netlist();
    virtual QString spice_netlist(spicecompat::SpiceDialect dialect = spicecompat::SPICEDefault);
    virtual QString cdl_netlist();
    void removeMultipleSpaces(QString& netlist);

private:
    QString netlist(
            const QString& netListTemplate,
            const QString netListInclude,
            spicecompat::SpiceDialect dialect) const;
    void resolveNetListInclude(const QString& key, QString& netListInclude) const;

    QString a_name;
    QString a_schematicId;
    QString a_description;
    QString a_ngspiceNetList;
    QString a_ngspiceNetListInclude;
    QString a_cdlNetList;
    QString a_cdlNetListInclude;
    QString a_xyceNetList;
    QString a_xyceNetListInclude;
    QString a_qucsatorNetList;
    QString a_qucsatorNetListInclude;
    QList<Parameter> a_parameters;
    QList<PortSym> a_portSyms;
    QList<Line> a_lines;
    QList<Arc> a_arcs;
};

#endif // XML_COMPONENT_H
