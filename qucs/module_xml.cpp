/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "module.h"
#include "Component.hxx"
#include "xml/XmlComponent.h"
#include "main.h"

#include <QString>
#include <QStringList>
#include <QList>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QSharedPointer>

#include <iostream>
#include <sstream>


void Module::registerXmlComponent(const QString& category, const QSharedPointer<XmlComponent>& xmlComp)
{
    // put into category and the component hash
    if ((xmlComp->Simulator & QucsSettings.DefaultSimulator) == QucsSettings.DefaultSimulator)
    {
        Module* mod(new Module());
        mod->a_xmlComp = xmlComp;
        mod->a_category = category;

        mod->a_icon = new QPixmap(128, 128);
        xmlComp->paintIcon(mod->a_icon);

        intoCategory(mod);
        if (!s_modules.contains(xmlComp->Model))
        {
            s_modules.insert(xmlComp->Model, mod);
        }
    }
}

void Module::registerXmlComponents(const QString& componentPath)
{
    QDir componentDir(componentPath);

    if (!componentDir.exists())
    {
        std::cerr << "XML component path '" << componentPath.toUtf8().constData() << "' don't exists" << std::endl;
        return;
    }

    QStringList components(componentDir.entryList({"*.xml"}, QDir::Files));

    foreach (const QString& component, components)
    {
        QFile componentFile(componentPath + QDir::separator() + component);

        std::cout << "Component xml: " << component.toUtf8().constData() << std::endl;

        if (!componentFile.open(QIODevice::ReadOnly))
        {
            std::cerr
                << "Could not open '"
                << componentPath.toUtf8().constData()
                << QString(QDir::separator()).toUtf8().constData()
                << component.toUtf8().constData()
                << "'"
                << std::endl;
            break;
        }

        QTextStream componentStream(&componentFile);
        QStringList componentContent;

        while (true)
        {
            QString line = componentStream.readLine();

            if (line.isNull())
            {
                break;
            }
            else
            {
                QRegularExpression pattern(QString::fromUtf8("<File>(.*)</File>"));
                QRegularExpressionMatch match(pattern.match(line));

                if (match.hasMatch())
                {
                    QString path(match.captured(1));
                    path.replace("{QUCS_S_COMPONENTS_LIBRARY}", componentPath);

                    QFile includeFile(path);

                    if (!includeFile.open(QIODevice::ReadOnly))
                    {
                        std::cerr
                            << "Could not open '"
                            << path.toUtf8().constData()
                            << "'"
                            << std::endl;
                    }
                    else
                    {
                        QTextStream includeFileStream(&includeFile);
                        QString content(includeFileStream.readAll());

                        line.replace(pattern, content);
                    }
                }

                componentContent.append(line);
            }
        }

        std::istringstream stream(componentContent.join("\n").toUtf8().toStdString());

        try
        {
            ::xml_schema::properties properties;
            properties.no_namespace_schema_location(
                    QString(componentPath + QDir::separator() + "Component.xsd").toStdString());

            std::unique_ptr<component::xml::Component> component(
                    component::xml::Component_(stream, 0, properties));

            QList<XmlComponent::Parameter> parameters;

            auto compParameters(component->Parameters().Parameter());
            for (auto it(compParameters.begin()); it != compParameters.end(); ++it)
            {
                XmlComponent::Parameter parameter(
                        QString::fromUtf8(it->name().get()),
                        QString::fromUtf8(it->unit().get()),
                        QString::fromUtf8(it->default_value().get()),
                        static_cast<bool>(it->show().get()),
                        QString::fromUtf8(it->Description()).trimmed()
                );

                parameters << parameter;
            }

            QList<XmlComponent::PortSym> portSyms;

            auto compPortSyms(component->Symbols().Symbol().PortSym());
            for (auto it(compPortSyms.begin()); it != compPortSyms.end(); ++it)
            {
                XmlComponent::PortSym portSym(
                        it->x().get(),
                        it->y().get(),
                        it->type().get(),
                        it->angle().get()
                );

                portSyms << portSym;
            }

            QList<XmlComponent::Line> lines;

            auto compLines(component->Symbols().Symbol().Line());
            for (auto it(compLines.begin()); it != compLines.end(); ++it)
            {
                XmlComponent::Line line(
                        it->x1().get(),
                        it->y1().get(),
                        it->x2().get(),
                        it->y2().get(),
                        QString::fromUtf8(it->color().get()),
                        static_cast<uint32_t>(it->width().get()),
                        it->style().get()
                );

                lines << line;
            }

            QString nspiceNetlist;
            QString nspiceNetlistInclude;
            QString cdlNetlist;
            QString cdlNetlistInclude;

            if (component->Netlists().NgspiceNetlist().present())
            {
                nspiceNetlist = QString::fromUtf8(component->Netlists().NgspiceNetlist().get().value().get());

                if (component->Netlists().NgspiceNetlist().get().Include().present())
                {
                    nspiceNetlistInclude =
                        QString::fromUtf8(
                                component->Netlists().NgspiceNetlist().get().Include().get().value().get());
                }
            }

            if (component->Netlists().CDLNetlist().present())
            {
                cdlNetlist = QString::fromUtf8(component->Netlists().CDLNetlist().get().value().get());

                if (component->Netlists().CDLNetlist().get().Include().present())
                {
                    cdlNetlistInclude =
                        QString::fromUtf8(
                                component->Netlists().CDLNetlist().get().Include().get().value().get());
                }
            }

            QSharedPointer<XmlComponent> xmlComponent(new XmlComponent(
                    QString::fromUtf8(component->name().get()),
                    QString::fromUtf8(component->schematic_id().get()),
                    QString::fromUtf8(component->Description()).trimmed(),
                    QString::fromUtf8(component->Models().DefaultModel().value().get()),
                    QString::fromUtf8(component->Models().SpiceModel().value().get()),
                    nspiceNetlist,
                    nspiceNetlistInclude,
                    cdlNetlist,
                    cdlNetlistInclude,
                    parameters,
                    portSyms,
                    lines
            ));

            registerXmlComponent(
                    QString::fromUtf8(component->library().get()), xmlComponent);

#if 0
            std::cout << "Component-library: " << component->library().get() << std::endl;
            std::cout << "Component-name: " << component->name().get() << std::endl;
            std::cout << "Schematic-id: " << component->schematic_id().get() << std::endl;
            std::cout << "Description: " << QString::fromUtf8(component->Description()).trimmed().toStdString() << std::endl;
            std::cout << "Default model: " << component->Models().DefaultModel().value().get() << std::endl;
            std::cout << "Spice model: " << component->Models().SpiceModel().value().get() << std::endl;

            if (component->Netlists().NgspiceNetlist().present())
            {
                std::cout
                    << "Ngspice netlist: "
                    << component->Netlists().NgspiceNetlist().get().value().get() << std::endl;

                if (component->Netlists().NgspiceNetlist().get().Include().present())
                {
                    std::cout
                        << "        include: "
                        << component->Netlists().NgspiceNetlist().get().Include().get().value().get()
                        << std::endl;
                }
            }

            if (component->Netlists().CDLNetlist().present())
            {
                std::cout
                    << "CDL netlist: "
                    << component->Netlists().CDLNetlist().get().value().get() << std::endl;

                if (component->Netlists().CDLNetlist().get().Include().present())
                {
                    std::cout
                        << "        include: "
                        << component->Netlists().CDLNetlist().get().Include().get().value().get()
                        << std::endl;
                }
            }

            auto _lines(component->Symbols().Symbol().Line());
            for (auto it(_lines.begin()); it != _lines.end(); ++it)
            {
                std::cout
                    << "Line x1=" << static_cast<int>(it->x1().get())
                    << ", y1=" << static_cast<int>(it->y1().get())
                    << ", x2=" << static_cast<int>(it->x2().get())
                    << ", y2=" << static_cast<int>(it->y2().get())
                    << ", color=" << it->color().get()
                    << ", width=" << static_cast<int>(it->width().get())
                    << ", style=" << static_cast<int>(it->style().get())
                    << std::endl;
            }

            auto _portSyms(component->Symbols().Symbol().PortSym());
            for (auto it(_portSyms.begin()); it != _portSyms.end(); ++it)
            {
                std::cout
                    << "PortSym x=" << static_cast<int>(it->x().get())
                    << ", y=" << static_cast<int>(it->y().get())
                    << ", type=" << static_cast<int>(it->type().get())
                    << ", angle=" << static_cast<int>(it->angle().get())
                    << std::endl;
            }

            for (auto it(compParameters.begin()); it != compParameters.end(); ++it)
            {
                std::cout
                    << "Parameter name=" << it->name().get()
                    << ", unit=" << it->unit().get()
                    << ", default-value=" << it->default_value().get()
                    << ", show=" << static_cast<bool>(it->show().get())
                    << ", description=" << QString::fromUtf8(it->Description()).trimmed().toStdString()
                    << std::endl;
            }
#endif

        }
        catch (const xml_schema::exception& exc)
        {
            std::cerr << componentFile.fileName().toUtf8().constData() << ": " << exc << std::endl;
        }
    }
}

