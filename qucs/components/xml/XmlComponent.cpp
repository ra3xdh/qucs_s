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

#include <main.h>

#include <QColor>
#include <QBrush>
#include <QPen>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStringList>
#include <QFontMetrics>
#include <QFont>
#include <QRect>
#include <QTransform>

#include <limits>
#include <algorithm>
#include <list>
#include <cmath>

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
        const QList<Arc>& arcs,
        const QList<Rectangle>& rectangles,
        const QList<Arrow>& arrows,
        const QList<Text>& texts) :
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
    a_arcs(arcs),
    a_rectangles(rectangles),
    a_arrows(arrows),
    a_texts(texts)
{
    Description = QObject::tr(description.toUtf8().constData());
    Model = defaultModel;
    SpiceModel = spiceModel;
    Name = schematicId;
    a_deviceName = name;

    removeMultipleSpaces(a_ngspiceNetList);
    removeMultipleSpaces(a_cdlNetList);
    removeMultipleSpaces(a_qucsatorNetList);

    std::list<Property*> properties;

    foreach (const Parameter& param, parameters)
    {
        bool insertParam(true);
        if (!param.a_condition.isEmpty())
        {
            insertParam = false;
            const QStringList condition(param.a_condition.split("=", Qt::SkipEmptyParts));

            if (condition.size() == 2)
            {
                if (condition[0] == "name")
                {
                    QStringList names(condition[1].split(",", Qt::SkipEmptyParts));
                    foreach (const QString& name, names)
                    {
                        if (name == a_name)
                        {
                            properties.erase(
                                    std::remove_if(
                                        properties.begin(),
                                        properties.end(),
                                        [param](Property* property) { return property->Name == param.a_name; }),
                                    properties.end());

                            insertParam = true;
                            break;
                        }
                    }
                }
            }
        }

        if (insertParam)
        {
            properties.push_back(
                    new Property(
                        param.a_name,
                        param.a_equation.isEmpty() ? param.a_defaultValue : param.a_equation,
                        param.a_unit,
                        param.a_show,
                        QObject::tr(param.a_description.toUtf8().constData()),
                        param.a_equation.isEmpty() ? Property::Type::Value : Property::Type::Equation));
        }
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
    Props.assign(properties.begin(), properties.end());
#else
    QList<Property*>* props(&Props);
    std::for_each(properties.begin(), properties.end(), [props](Property*& prop) { props->push_back(prop); });
#endif

    // Append hidden property to persist variant identity through save/load.
    // Empty Description ensures name-based serialization ("_xml_device_name=value"
    // in the schematic file), which is robust against XML parameter list changes.
    Props.append(
        new Property("_xml_device_name", a_name, "", false, "", Property::Type::Value));

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
            a_arcs,
            a_rectangles,
            a_arrows,
            a_texts);
}

void XmlComponent::rebuildParameters()
{
    // Save current property values (except hidden _xml_device_name)
    // to preserve user's edits across parameter list rebuild
    QHash<QString, QString> savedValues;
    for (Property* prop : Props) {
        if (prop->Name != "_xml_device_name") {
            savedValues[prop->Name] = prop->Value;
        }
    }

    // Clear existing properties
    Props.clear();

    // Rebuild from a_parameters (the original XML definition), respecting condition="name=..."
    std::list<Property*> properties;
    foreach (const Parameter& param, a_parameters)
    {
        bool insertParam(true);
        if (!param.a_condition.isEmpty())
        {
            insertParam = false;
            const QStringList condition(param.a_condition.split("=", Qt::SkipEmptyParts));

            if (condition.size() == 2 && condition[0] == "name")
            {
                QStringList names(condition[1].split(",", Qt::SkipEmptyParts));
                foreach (const QString& name, names)
                {
                    if (name == a_name)
                    {
                        // Remove any previously-added parameter with same name
                        // to allow name-specific override of default parameters
                        properties.erase(
                            std::remove_if(
                                    properties.begin(),
                                    properties.end(),
                                    [param](Property* property) { return property->Name == param.a_name; }),
                            properties.end());
                        insertParam = true;
                        break;
                    }
                }
            }
        }

        if (insertParam)
        {
            QString defaultValue = param.a_equation.isEmpty() ? param.a_defaultValue : param.a_equation;

            properties.push_back(
                new Property(
                    param.a_name,
                    defaultValue,
                    param.a_unit,
                    param.a_show,
                    QObject::tr(param.a_description.toUtf8().constData()),
                    param.a_equation.isEmpty() ? Property::Type::Value : Property::Type::Equation));
        }
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
    Props.assign(properties.begin(), properties.end());
#else
    QList<Property*>* props(&Props);
    std::for_each(properties.begin(), properties.end(), [props](Property*& prop) { props->push_back(prop); });
#endif

    // Restore saved property values to preserve user's edits after rebuild
    for (Property* prop : Props) {
        if (savedValues.contains(prop->Name)) {
            prop->Value = savedValues[prop->Name];
        }
    }

    // Re-add the hidden property with current a_name value
    Props.append(
        new Property("_xml_device_name", a_name, "", false, "", Property::Type::Value));
}

void XmlComponent::recreate()
{
    // Search for hidden variant name property
    Property* nameProp = nullptr;
    for (Property* prop : Props) {
        if (prop->Name == "_xml_device_name") {
            nameProp = prop;
            break;
        }
    }

    // If hidden property exists and has a different value than the current a_name,
    // restore the variant identity and rebuild the component
    if (nameProp && !nameProp->Value.isEmpty() && nameProp->Value != a_name) {
        a_name = nameProp->Value;
        a_deviceName = a_name;

        // Rebuild properties with the correct variant's conditional parameters
        rebuildParameters();

        // Rebuild symbol with the correct variant-specific geometry
        createSymbol();
    }
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
    // Clear all geometry lists to avoid doubling when called again (e.g. via recreate())
    Lines.clear();
    Arcs.clear();
    Rects.clear();
    Texts.clear();
    Ports.clear();

    x1 = std::numeric_limits<int>::max();
    y1 = std::numeric_limits<int>::max();
    x2 = std::numeric_limits<int>::min();
    y2 = std::numeric_limits<int>::min();

    foreach (const Line& line, a_lines)
    {
        if (!evaluateConditionInclude(line.a_condition))
        {
            continue;
        }

        Lines.append(new qucs::Line(
            line.a_x1,
            line.a_y1,
            line.a_x2,
            line.a_y2,
            QPen(misc::ColorFromString(line.a_color), line.a_width, static_cast<Qt::PenStyle>(line.a_style))));

        x1 = line.a_x1 < x1 ? line.a_x1 : x1;
        y1 = line.a_y1 < y1 ? line.a_y1 : y1;
        y1 = line.a_y2 < y1 ? line.a_y2 : y1;
        x2 = line.a_x2 > x2 ? line.a_x2 : x2;
        y2 = line.a_y2 > y2 ? line.a_y2 : y2;
        y2 = line.a_y1 > y2 ? line.a_y1 : y2;
    }

    foreach (const Arc& arc, a_arcs)
    {
        if (!evaluateConditionInclude(arc.a_condition))
        {
            continue;
        }

        Arcs.append(new qucs::Arc(
            arc.a_x,
            arc.a_y,
            arc.a_arcWidth,
            arc.a_height,
            arc.a_angle,
            arc.a_len,
            QPen(misc::ColorFromString(arc.a_color), arc.a_width, static_cast<Qt::PenStyle>(arc.a_style))));
    }

    foreach (const Rectangle& rect, a_rectangles)
    {
        if (!evaluateConditionInclude(rect.a_condition))
        {
            continue;
        }

        const QPen pen(
                misc::ColorFromString(rect.a_color),
                rect.a_lineWidth,
                static_cast<Qt::PenStyle>(rect.a_style));
        const QBrush brush(
                rect.a_filled ? QBrush(
                    misc::ColorFromString(rect.a_fillColor), static_cast<Qt::BrushStyle>(rect.a_fillStyle)) :
                QBrush(Qt::NoBrush));

        Rects.append(new qucs::Rect(rect.a_x, rect.a_y, rect.a_width, rect.a_height, pen, brush));

        x1 = rect.a_x < x1 ? rect.a_x : x1;
        y1 = rect.a_y < y1 ? rect.a_y : y1;
        x2 = rect.a_x + rect.a_width > x2 ? rect.a_x + rect.a_width : x2;
        y2 = rect.a_y + rect.a_height > y2 ? rect.a_y + rect.a_height : y2;
    }

    foreach (const Arrow& arrow, a_arrows)
    {
        if (!evaluateConditionInclude(arrow.a_condition))
        {
            continue;
        }

        const QPen pen(
                misc::ColorFromString(arrow.a_color),
                arrow.a_width,
                static_cast<Qt::PenStyle>(arrow.a_style));

        Lines.append(new qucs::Line(arrow.a_x1, arrow.a_y1, arrow.a_x2, arrow.a_y2, pen));

        const double arrowAngle(
                atan2(
                    static_cast<double>(arrow.a_y2 - arrow.a_y1),
                    static_cast<double>(arrow.a_x2 - arrow.a_x1)));
        const double headAngle(atan2(arrow.a_headWidth, arrow.a_headHeight));
        const double headWingLength(hypot(arrow.a_headWidth, arrow.a_headHeight));

        const QPoint leftWing(
                arrow.a_x2 - static_cast<int>(headWingLength * cos(headAngle + arrowAngle)),
                arrow.a_y2 - static_cast<int>(headWingLength * sin(headAngle + arrowAngle)));
        const QPoint rightWing(
                arrow.a_x2 - static_cast<int>(headWingLength * cos(arrowAngle - headAngle)),
                arrow.a_y2 - static_cast<int>(headWingLength * sin(arrowAngle - headAngle)));

        Lines.append(new qucs::Line(arrow.a_x2, arrow.a_y2, leftWing.x(), leftWing.y(), pen));
        Lines.append(new qucs::Line(arrow.a_x2, arrow.a_y2, rightWing.x(), rightWing.y(), pen));

        x1 = std::min({x1, arrow.a_x1, arrow.a_x2, leftWing.x(), rightWing.x()});
        y1 = std::min({y1, arrow.a_y1, arrow.a_y2, leftWing.y(), rightWing.y()});
        x2 = std::max({x2, arrow.a_x1, arrow.a_x2, leftWing.x(), rightWing.x()});
        y2 = std::max({y2, arrow.a_y1, arrow.a_y2, leftWing.y(), rightWing.y()});
    }

    foreach (const Text& textDef, a_texts)
    {
        if (!evaluateConditionInclude(textDef.a_condition))
        {
            continue;
        }

        ::Text* text(
                new ::Text(
                    textDef.a_x,
                    textDef.a_y,
                    textDef.a_text,
                    misc::ColorFromString(textDef.a_color),
                    textDef.a_size,
                    textDef.a_cos,
                    textDef.a_sin));
        Texts.append(text);

        QFont font(QucsSettings.font);
        font.setPixelSize(textDef.a_size);
        QFontMetrics metrics(font);

        const int textWidth(metrics.horizontalAdvance(textDef.a_text));

        QRect orig(0, 0, textWidth, metrics.height());
        QTransform trans(QTransform().rotate(text->angle()));
        QRect mapped(trans.mapRect(orig));
        mapped.translate(textDef.a_x, textDef.a_y);

        x1 = mapped.left() < x1 ? mapped.left() : x1;
        y1 = mapped.top() < y1 ? mapped.top() : y1;
        x2 = mapped.right() > x2 ? mapped.right() : x2;
        y2 = mapped.bottom() > y2 ? mapped.bottom() : y2;
    }

    foreach (const PortSym& portSym, a_portSyms)
    {
        if (!evaluateConditionInclude(portSym.a_condition))
        {
            continue;
        }

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

    // Dialect including/excluding conditions
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


    // Parameter value conditions
    const QString conditionNonEmpty(QString::fromUtf8("nonempty"));
    QString conditionalTypePattern(
            QString::fromUtf8("(\\w+)='(%1)'").arg(conditionNonEmpty));
    QRegularExpression conditionalPattern(
            QString::fromUtf8("\\{\\{([^\\}]+)(\\}*?)\\}\\}::(%1)").arg(conditionalTypePattern));
    Q_ASSERT(conditionalPattern.isValid());

    int from(0);
    int idx;
    QRegularExpressionMatch conditionalMatch;

    while ((idx = netList.indexOf(conditionalPattern, from, &conditionalMatch)) != -1)
    {
        const QString paramName(conditionalMatch.captured(conditionalMatch.lastCapturedIndex()-1));
        const QString condition(conditionalMatch.captured(conditionalMatch.lastCapturedIndex()));
        bool remove(true);

        // we have 5 subexpressions
        Q_ASSERT(conditionalMatch.lastCapturedIndex() == 5);

        if (condition == conditionNonEmpty)
        {
            foreach (const Property* property, Props)
            {
                if (property->Name == paramName && !(getValue(*property).isEmpty()))
                {
                    remove = false;
                    break;
                }
            }

            if (remove)
            {
                netList.remove(idx, conditionalMatch.capturedLength());
            }
        }

        if (!remove)
        {
            // remove conditional statements around parameter
            const QString replacement(QString::fromUtf8("%1%2").
                    arg(conditionalMatch.captured(1)).
                    arg(conditionalMatch.captured(2)));
            netList.replace(idx, conditionalMatch.capturedLength(), replacement);

            from = idx + replacement.size();
        }
    }


    // Parameter/Parameter value conditions
    const QString conditionNotEqual(QString::fromUtf8("nequal"));
    conditionalTypePattern = QString::fromUtf8("(\\w+)='(%1)'").arg(conditionNotEqual);
    conditionalPattern.setPattern(
            QString::fromUtf8("\\{\\{([^\\}]+)(\\}*?)\\}\\}::(%1)").arg(conditionalTypePattern));
    Q_ASSERT(conditionalPattern.isValid());

    from = 0;
    while ((idx = netList.indexOf(conditionalPattern, from, &conditionalMatch)) != -1)
    {
        const QString parameterName(
                conditionalMatch.captured(1).remove(QRegularExpression(QString::fromUtf8("[\\{\\}]"))));
        const QString refParameterName(conditionalMatch.captured(conditionalMatch.lastCapturedIndex()-1));
        const QString condition(conditionalMatch.captured(conditionalMatch.lastCapturedIndex()));

        bool remove(false);
        QString parameterValue;
        if (condition == conditionNotEqual)
        {
            QString refParameterValue(refParameterName == "schematic_id" ? a_schematicId : "");
            foreach (const Property* property, Props)
            {
                if (property->Name == refParameterName)
                {
                    refParameterValue = getValue(*property);
                }
                if (property->Name == parameterName)
                {
                    parameterValue = getValue(*property);
                }
            }

            if (refParameterValue == parameterValue)
            {
                netList.remove(idx, conditionalMatch.capturedLength());
                remove = true;
            }
        }

        if (!remove)
        {
            // remove conditional statements around parameter
            const QString replacement(QString::fromUtf8("%1%2").
                    arg(conditionalMatch.captured(1)).
                    arg(conditionalMatch.captured(2)));
            netList.replace(idx, conditionalMatch.capturedLength(), replacement);

            from = idx + replacement.size();
        }
    }


    QString partCounter(Name);
    partCounter.replace(a_schematicId, "");

    netList.replace("{PartCounter}", partCounter);

    QString nets;
    foreach (const Port* port, Ports)
    {
        nets += QString::fromUtf8("%1%2").arg(nets.isEmpty() ? "" : " ").arg(port->Connection->getName());
    }

    nets.replace(" gnd ", " 0 ");
    netList.replace("{nets}", nets);

    foreach (const Property* property, Props)
    {
        const QString propertyValue(getValue(*property) + property->unit);
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

    return netlist(netList, netListInclude, dialect);
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

void XmlComponent::removeMultipleSpaces(QString& netlist)
{
    QRegularExpression multipleSpacePattern(QString::fromUtf8("\\s\\s\\s*"));
    Q_ASSERT(multipleSpacePattern.isValid());

    netlist.replace(multipleSpacePattern, " ");
}

bool XmlComponent::evaluateConditionInclude(const QString& elementCondition) const
{
    if (elementCondition.isEmpty())
    {
        return true; // no condition => include
    }

    const QStringList conditionParts(elementCondition.split("=", Qt::SkipEmptyParts));
    if (conditionParts.size() != 2)
    {
        // Unexpected syntax: to be safe, include element (or you could choose to exclude).
        return true;
    }

    const QString paramName = conditionParts[0].trimmed();
    const QString paramValue = conditionParts[1].trimmed();

    // Default behavior in createSymbol() for lines is to skip unless a matching property is found
    // => here return true only when a match is found.

    // special-case "name" to compare component name and allow comma-separated list
    if (paramName == "name")
    {
        const QStringList allowedNames = paramValue.split(',', Qt::SkipEmptyParts);
        for (const QString& name : allowedNames)
        {
            if (name.trimmed() == a_name)
            {
                return true;
            }
        }
        return false;
    }

    // search matching property and compare its value
    for (const Property* property : Props)
    {
        if (property->Name == paramName)
        {
            const QString value = getValue(*property).trimmed();
            const QStringList allowedValues = paramValue.split(',', Qt::SkipEmptyParts);
            for (const QString& allowedValue : allowedValues)
            {
                if (allowedValue.trimmed() == value)
                {
                    return true;
                }
            }
            // property exists but value doesn't match => do not include
            return false;
        }
    }

    // parameter referenced in condition not found; follow the more conservative behavior:
    // treat as not matched => exclude element
    return false;
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
