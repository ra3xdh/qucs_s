/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "XmlComponent.h"
#include "node.h"
#include "misc.h"
#include "extsimkernels/spicecompat.h"

#include <QColor>
#include <QPen>

XmlComponent::XmlComponent(
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
        const QList<Line>& lines) :
    a_schematicId(schematicId),
    a_description(description),
    a_ngspiceNetList(ngspiceNetList),
    a_ngspiceNetListInclude(ngspiceNetListInclude),
    a_cdlNetList(cdlNetList),
    a_cdlNetListInclude(cdlNetListInclude),
    a_parameters(parameters),
    a_portSyms(portSyms),
    a_lines(lines)
{
    Description = QObject::tr(description.toUtf8().constData());
    Model = defaultModel;
    SpiceModel = spiceModel;
    Name = name;

    foreach (const Parameter& param, parameters)
    {
        Props.append(new Property(
            param.a_name,
            param.a_defaultValue,
            param.a_show,
            QObject::tr(param.a_description.toUtf8().constData())));
    }

    createSymbol();
}

Component* XmlComponent::newOne()
{
    return new XmlComponent(
            Name,
            a_schematicId,
            a_description,
            Model,
            SpiceModel,
            a_ngspiceNetList,
            a_ngspiceNetListInclude,
            a_cdlNetList,
            a_cdlNetListInclude,
            a_parameters,
            a_portSyms,
            a_lines);
}

Element* XmlComponent::getInfo(QString& Name, char* &BitmapFile, bool getNewOne)
{
    Name = QObject::tr(a_schematicId.toUtf8().constData());
    BitmapFile = const_cast<char*>(a_schematicId.toUtf8().constData());

    if (getNewOne)
    {
        return newOne();
    }

    return nullptr;
}

void XmlComponent::createSymbol()
{
    foreach (const Line& line, a_lines)
    {
        Lines.append(new qucs::Line(
            line.a_x1,
            line.a_y1,
            line.a_x2,
            line.a_y2,
            QPen(QColor::fromString(line.a_color), line.a_width, static_cast<Qt::PenStyle>(line.a_style))));
    }

    foreach (const PortSym& portSym, a_portSyms)
    {
        Ports.append(new Port(portSym.a_x,  portSym.a_y));
    }

    x1 = -30;
    y1 = -30;
    x2 = 4;
    y2 = 30;
}

QString XmlComponent::netlist()
{
    return QString("");
}

QString XmlComponent::spice_netlist(spicecompat::SpiceDialect dialect /* = spicecompat::SPICEDefault */)
{
    Q_UNUSED(dialect);

    return a_ngspiceNetList;
}

QString XmlComponent::cdl_netlist()
{
    if (!a_cdlNetList.isEmpty())
    {
        return a_cdlNetList;
    }
    else
    {
        return spice_netlist(spicecompat::CDL);
    }
}
