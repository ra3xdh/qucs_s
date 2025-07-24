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
#include <QRegularExpression>

#include <limits>

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
        const QString& xyceNetList,
        const QString& xyceNetListInclude,
        const QString& qucsatorNetList,
        const QString& qucsatorNetListInclude,
        const QList<Parameter>& parameters,
        const QList<PortSym>& portSyms,
        const QList<Line>& lines,
        const QList<Arc>& arcs) :
    a_name(name),
    a_schematicId(schematicId),
    a_description(description),
    a_ngspiceNetList(ngspiceNetList),
    a_ngspiceNetListInclude(ngspiceNetListInclude),
    a_cdlNetList(cdlNetList),
    a_cdlNetListInclude(cdlNetListInclude),
    a_xyceNetList(xyceNetList),
    a_xyceNetListInclude(xyceNetListInclude),
    a_qucsatorNetList(qucsatorNetList),
    a_qucsatorNetListInclude(qucsatorNetListInclude),
    a_parameters(parameters),
    a_portSyms(portSyms),
    a_lines(lines),
    a_arcs(arcs)
{
    Description = QObject::tr(description.toUtf8().constData());
    Model = defaultModel;
    SpiceModel = spiceModel;
    Name = schematicId;

    foreach (const Parameter& param, parameters)
    {
        bool isValue;
        static_cast<void>(param.a_defaultValue.toDouble(&isValue));
        Props.append(new Property(
            param.a_name,
            param.a_defaultValue,
            param.a_unit,
            param.a_show,
            QObject::tr(param.a_description.toUtf8().constData()),
            isValue ? Property::Type::Value : Property::Type::Equation));
    }

    createSymbol();

    tx = x1+4;
    ty = y2+4;
}

Component* XmlComponent::newOne()
{
    return new XmlComponent(
            a_name,
            a_schematicId,
            a_description,
            Model,
            SpiceModel,
            a_ngspiceNetList,
            a_ngspiceNetListInclude,
            a_cdlNetList,
            a_cdlNetListInclude,
            a_xyceNetList,
            a_xyceNetListInclude,
            a_qucsatorNetList,
            a_qucsatorNetListInclude,
            a_parameters,
            a_portSyms,
            a_lines,
            a_arcs);
}

Element* XmlComponent::getInfo(QString& Name, char* &BitmapFile, bool getNewOne)
{
    Name = QObject::tr(a_name.toUtf8().constData());
    BitmapFile = const_cast<char*>(a_schematicId.toUtf8().constData());

    if (getNewOne)
    {
        return newOne();
    }

    return nullptr;
}

void XmlComponent::createSymbol()
{
    x1 = std::numeric_limits<int>::max();
    y1 = std::numeric_limits<int>::max();
    x2 = std::numeric_limits<int>::min();
    y2 = std::numeric_limits<int>::min();

    foreach (const Line& line, a_lines)
    {
        Lines.append(new qucs::Line(
            line.a_x1,
            line.a_y1,
            line.a_x2,
            line.a_y2,
            QPen(misc::ColorFromString(line.a_color), line.a_width, static_cast<Qt::PenStyle>(line.a_style))));

        x1 = line.a_x1 < x1 ? line.a_x1 : x1;
        y1 = line.a_y1 < y1 ? line.a_y1 : y1;
        x2 = line.a_x2 > x2 ? line.a_x2 : x2;
        y2 = line.a_y2 > y2 ? line.a_y2 : y2;
    }

    foreach (const Arc& arc, a_arcs)
    {
        Arcs.append(new qucs::Arc(
            arc.a_x,
            arc.a_y,
            arc.a_arcWidth,
            arc.a_height,
            arc.a_angle,
            arc.a_len,
            QPen(misc::ColorFromString(arc.a_color), arc.a_width, static_cast<Qt::PenStyle>(arc.a_style))));
    }

    foreach (const PortSym& portSym, a_portSyms)
    {
        Ports.append(new Port(portSym.a_x,  portSym.a_y));
    }
}

QString XmlComponent::netlist()
{
    return QString("");
}

// virtual
QString XmlComponent::netlist(
        const QString& netListTemplate,
        const QString netListInclude,
        spicecompat::SpiceDialect dialect) const
{
    QString spiceDialectPattern;
    QString excludingDialectPattern;

    switch (dialect)
    {
        case spicecompat::SPICEDefault:
            spiceDialectPattern = "SPICE|!CDL|!XYCE";
            excludingDialectPattern = QString::fromUtf8("XYCE|CDL|!SPICE");
            break;
        case spicecompat::SPICEXyce:
            spiceDialectPattern = "XYCE|!CDL|!SPICE";
            excludingDialectPattern = QString::fromUtf8("SPICE|CDL|!XYCE");
            break;
        case spicecompat::CDL:
            spiceDialectPattern = "CDL|!XYCE|!SPICE";
            excludingDialectPattern = QString::fromUtf8("SPICE|XYCE|!CDL");
            break;
    }

    QString netList(netListTemplate);

    QRegularExpression includingPattern(
            QString::fromUtf8("\\{\\{([^\\}]+)(\\}*?)\\}\\}::(%1)").arg(spiceDialectPattern));
    Q_ASSERT(includingPattern.isValid());

    netList.replace(includingPattern, QString::fromUtf8("\\1\\2"));

    QRegularExpression excludingPattern(
            QString::fromUtf8("\\{\\{([^\\}]+)(\\}*?)\\}\\}::(%1)").arg(excludingDialectPattern));
    Q_ASSERT(excludingPattern.isValid());

    netList.replace(excludingPattern, QString::fromUtf8(""));

    QString partCounter(Name);
    partCounter.replace(a_schematicId, "");

    netList.replace("{PartCounter}", partCounter);

    QString nets;
    foreach (const Port* port, Ports)
    {
        nets += QString::fromUtf8("%1%2").arg(nets.isEmpty() ? "" : " ").arg(port->Connection->Name);
    }

    nets.replace(" gnd ", " 0 ");
    netList.replace("{nets}", nets);

    foreach (const Property* property, Props)
    {
        const QString propertyValue(getValue(*property));
        netList.replace(QString::fromUtf8("{%1}").arg(property->Name), propertyValue);
    }

    QString resolvedInclude(netListInclude);
    if (!resolvedInclude.isEmpty())
    {
        resolveNetListInclude("PDK_ROOT", resolvedInclude);
    }

    return resolvedInclude + (resolvedInclude.isEmpty() ? "" : "\n") + netList + "\n";
}

QString XmlComponent::spice_netlist(spicecompat::SpiceDialect dialect /* = spicecompat::SPICEDefault */)
{
    QString netList;
    QString netListInclude;

    switch (dialect)
    {
        case spicecompat::SPICEDefault:
        case spicecompat::CDL:
            netList = a_ngspiceNetList;
            netListInclude = a_ngspiceNetListInclude;
            break;
        case spicecompat::SPICEXyce:
            netList = a_xyceNetList;
            netListInclude = a_xyceNetListInclude;
            if (netList == QString::fromUtf8("{NgspiceNetlist}"))
            {
                netList = a_ngspiceNetList;
                if (netListInclude.isEmpty())
                {
                    netList = a_ngspiceNetListInclude;
                }
            }
            break;
    }

    if (netList.isEmpty())
    {
        qWarning()
            << (dialect == spicecompat::SPICEDefault ? "Spice" : "Xyce")
            << " netlist of XML component " << a_name << " is empty";
    }

    return netlist(a_ngspiceNetList, a_ngspiceNetListInclude, dialect);
}

QString XmlComponent::cdl_netlist()
{
    if (a_cdlNetList.isEmpty())
    {
        qWarning() << "CDL netlist of XML component '" << a_name << "' is empty";
    }

    if (a_cdlNetList == QString::fromUtf8("{NgspiceNetlist}"))
    {
        return spice_netlist(spicecompat::CDL);
    }
    else
    {
        return netlist(a_cdlNetList, a_cdlNetListInclude, spicecompat::CDL);
    }
}

void XmlComponent::resolveNetListInclude(const QString& key, QString& netListInclude) const
{
    if (netListInclude.contains(QString::fromUtf8("{%1}").arg(key)))
    {
        const QString value(qgetenv(key.toUtf8().constData()));

        if (value.isEmpty())
        {
            qWarning() << "Environment variable " << key << " of XML component '" << a_name << "' is not defined";
        }
        else
        {
            netListInclude.replace(QString::fromUtf8("{%1}").arg(key), value);
        }
    }
}
