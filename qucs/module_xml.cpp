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
#include <QChar>
#include <QStringList>
#include <QList>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QSharedPointer>
#include <QDirIterator>

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

void Module::unregisterXmlComponents()
{
    Category::unregisterXmlModules();

    QHash<QString, Module*>::iterator i(s_modules.begin());
    while (i != s_modules.end())
    {
        if (i.value() == nullptr || i.value()->a_xmlComp)
        {
            delete i.value();
            i = s_modules.erase(i);
        }
        else
        {
            ++i;
        }
    }
}

void Module::registerXmlComponents()
{
    registerXmlComponents(QucsSettings.ComponentDir);

    foreach (const QString& path, qucsXmlCompPathList)
    {
        registerXmlComponents(path);
    }

    const QString pdkRoot(qgetenv("PDK_ROOT"));
    const QString pdk(qgetenv("PDK"));

    QString pdkPath;

    if (!pdkRoot.isEmpty())
    {
        pdkPath = QString::fromUtf8("%1%2%3").
           arg(pdkRoot).
           arg(!pdk.isEmpty() ? QString(QDir::separator()) : "").
           arg(!pdk.isEmpty() ? pdk : "");

        QDir pdkDir(pdkPath);

        if (pdkDir.exists())
        {
            QRegularExpression pattern(QString::fromUtf8("qucs.*symbols"));
            QDirIterator it(
                    pdkPath,
                    {"*qucs*", "*symbols*"},
                    QDir::Dirs | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);

            while (it.hasNext())
            {
                QString dir(it.next());
                QRegularExpressionMatch match(pattern.match(dir));

                if (match.hasMatch())
                {
                    registerXmlComponents(dir);
                }
            }
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
        QString logComponent(QString::fromUtf8("Component xml: %1%2%3").arg(componentPath).arg(QDir::separator()).arg(component));

        std::cout << logComponent.toUtf8().constData() << std::endl;

        if (!componentFile.open(QIODevice::ReadOnly))
        {
            std::cerr
                << "Could not open '"
                << logComponent.toUtf8().constData()
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
                    QStringList pathsToTry = {componentPath, QucsSettings.ComponentDir};
                    bool found(false);
                    QFile includeFile;

                    foreach (const QString& pathToTry, pathsToTry)
                    {
                        QString path(match.captured(1));
                        path.replace("{QUCS_S_COMPONENTS_LIBRARY}", pathToTry);

                        includeFile.setFileName(path);

                        if (includeFile.open(QIODevice::ReadOnly))
                        {
                            found = true;
                            break;
                        }
                    }

                    if (found)
                    {
                        QTextStream includeFileStream(&includeFile);
                        QString content(includeFileStream.readAll());

                        line.replace(pattern, content);
                    }
                    else
                    {
                        QString path(match.captured(1));
                        path.replace("{QUCS_S_COMPONENTS_LIBRARY}", componentPath);

                        std::cerr
                            << "Could not open '"
                            << path.toUtf8().constData()
                            << "'"
                            << std::endl;
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
                    QString(QucsSettings.ComponentDir + QDir::separator() + "Component.xsd").toStdString());

            std::unique_ptr<component::xml::Component> component(
                    component::xml::Component_(stream, 0, properties));

            QList<XmlComponent::Parameter> parameters;

            auto compParameters(component->Parameters().Parameter());
            for (auto it(compParameters.begin()); it != compParameters.end(); ++it)
            {
                XmlComponent::Parameter parameter(
                        QString::fromUtf8(it->name().get()),
                        QString::fromUtf8(it->unit().get()),
                        QString::fromUtf8(it->default_value()),
                        QString::fromUtf8(it->equation()),
                        static_cast<bool>(it->show().get()),
                        QString::fromUtf8(it->Description()).trimmed(),
                        QString::fromUtf8(it->condition())
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
                        it->angle().get(),
                        QString::fromUtf8(it->condition())
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
                        QString::fromUtf8(it->color()),
                        static_cast<uint32_t>(it->width().get()),
                        it->style().get(),
                        QString::fromUtf8(it->condition())
                );

                lines << line;
            }

            QList<XmlComponent::Arc> arcs;

            auto compArcs(component->Symbols().Symbol().Arc());
            for (auto it(compArcs.begin()); it != compArcs.end(); ++it)
            {
                XmlComponent::Arc arc(
                        it->x().get(),
                        it->y().get(),
                        it->arcWidth().get(),
                        it->height().get(),
                        it->angle().get(),
                        it->len().get(),
                        QString::fromUtf8(it->color()),
                        static_cast<uint32_t>(it->width()),
                        it->style(),
                        QString::fromUtf8(it->condition())
                );

                arcs << arc;
            }

            QList<XmlComponent::Rectangle> rectangles;

            auto compRectangles(component->Symbols().Symbol().Rectangle());
            for (auto it(compRectangles.begin()); it != compRectangles.end(); ++it)
            {
                XmlComponent::Rectangle rectangle(
                        it->x().get(),
                        it->y().get(),
                        it->width().get(),
                        it->height().get(),
                        QString::fromUtf8(it->color()),
                        static_cast<uint32_t>(it->lineWidth()),
                        it->style(),
                        QString::fromUtf8(it->fillColor()),
                        it->fillStyle(),
                        static_cast<bool>(it->filled()),
                        QString::fromUtf8(it->condition())
                );

                rectangles << rectangle;
            }

            QList<XmlComponent::Arrow> arrows;

            auto compArrows(component->Symbols().Symbol().Arrow());
            for (auto it(compArrows.begin()); it != compArrows.end(); ++it)
            {
                XmlComponent::Arrow arrow(
                        it->x1().get(),
                        it->y1().get(),
                        it->x2().get(),
                        it->y2().get(),
                        QString::fromUtf8(it->color()),
                        static_cast<uint32_t>(it->width()),
                        it->style(),
                        it->headHeight(),
                        it->headWidth(),
                        it->headStyle(),
                        QString::fromUtf8(it->condition())
                );

                arrows << arrow;
            }

            QList<XmlComponent::Text> texts;

            auto compTexts(component->Symbols().Symbol().Text());
            for (auto it(compTexts.begin()); it != compTexts.end(); ++it)
            {
                XmlComponent::Text text(
                        it->x().get(),
                        it->y().get(),
                        QString::fromUtf8(it->text().get()),
                        QString::fromUtf8(it->color()),
                        it->size(),
                        it->cos(),
                        it->sin(),
                        QString::fromUtf8(it->condition())
                );

                texts << text;
            }

            QString nspiceNetlist;
            QString nspiceNetlistInclude;
            QString cdlNetlist;
            QString cdlNetlistInclude;
            QString xyceNetlist;
            QString xyceNetlistInclude;
            QString qucsatorNetlist;
            QString qucsatorNetlistInclude;

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

            if (component->Netlists().XyceNetlist().present())
            {
                xyceNetlist = QString::fromUtf8(component->Netlists().XyceNetlist().get().value().get());

                if (component->Netlists().XyceNetlist().get().Include().present())
                {
                    xyceNetlistInclude =
                        QString::fromUtf8(
                                component->Netlists().XyceNetlist().get().Include().get().value().get());
                }
            }

            if (component->Netlists().QucsatorNetlist().present())
            {
                qucsatorNetlist = QString::fromUtf8(component->Netlists().QucsatorNetlist().get().value().get());

                if (component->Netlists().QucsatorNetlist().get().Include().present())
                {
                    qucsatorNetlistInclude =
                        QString::fromUtf8(
                                component->Netlists().QucsatorNetlist().get().Include().get().value().get());
                }
            }

            const QStringList names(QString::fromUtf8(component->names().get()).split(",", Qt::SkipEmptyParts));

            foreach (const QString& name, names)
            {
                QSharedPointer<XmlComponent> xmlComponent(new XmlComponent(
                        name,
                        QString::fromUtf8(component->schematic_id().get()),
                        QString::fromUtf8(component->Description()).trimmed(),
                        QString::fromUtf8(component->Models().DefaultModel().value().get()),
                        QString::fromUtf8(component->Models().SpiceModel().value().get()),
                        nspiceNetlist,
                        nspiceNetlistInclude,
                        cdlNetlist,
                        cdlNetlistInclude,
                        xyceNetlist,
                        xyceNetlistInclude,
                        qucsatorNetlist,
                        qucsatorNetlistInclude,
                        parameters,
                        portSyms,
                        lines,
                        arcs,
                        rectangles,
                        arrows,
                        texts
                ));

                registerXmlComponent(
                        QString::fromUtf8(component->library().get()), xmlComponent);

#if 0
                std::cout << "  Component-library: " << component->library().get() << std::endl;
                std::cout << "  Component-name: " << name.toStdString() << std::endl;
                std::cout << "  Schematic-id: " << component->schematic_id().get() << std::endl;
                std::cout << "  Description: " << QString::fromUtf8(component->Description()).trimmed().toStdString() << std::endl;
                std::cout << "  Default model: " << component->Models().DefaultModel().value().get() << std::endl;
                std::cout << "  Spice model: " << component->Models().SpiceModel().value().get() << std::endl;

                if (component->Netlists().NgspiceNetlist().present())
                {
                    std::cout
                        << "  Ngspice netlist: "
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
                        << "  CDL netlist: "
                        << component->Netlists().CDLNetlist().get().value().get() << std::endl;


                    if (component->Netlists().CDLNetlist().get().Include().present())
                    {
                        std::cout
                            << "        include: "
                            << component->Netlists().CDLNetlist().get().Include().get().value().get()
                            << std::endl;
                    }
                }

                if (component->Netlists().QucsatorNetlist().present())
                {
                    std::cout
                        << " Qucsator netlist: "
                        << component->Netlists().QucsatorNetlist().get().value().get() << std::endl;

                    if (component->Netlists().QucsatorNetlist().get().Include().present())
                    {
                        std::cout
                            << "        include: "
                            << component->Netlists().QucsatorNetlist().get().Include().get().value().get()
                            << std::endl;
                    }
                }

                auto _lines(component->Symbols().Symbol().Line());
                for (auto it(_lines.begin()); it != _lines.end(); ++it)
                {
                    std::cout
                        << "  Line x1=" << static_cast<int>(it->x1().get())
                        << ", y1=" << static_cast<int>(it->y1().get())
                        << ", x2=" << static_cast<int>(it->x2().get())
                        << ", y2=" << static_cast<int>(it->y2().get())
                        << ", color=" << it->color()
                        << ", width=" << static_cast<int>(it->width().get())
                        << ", style=" << static_cast<int>(it->style().get())
                        << ", condition=" << it->condition()
                        << std::endl;
                }

                auto _arcs(component->Symbols().Symbol().Arc());
                for (auto it(_arcs.begin()); it != _arcs.end(); ++it)
                {
                    std::cout
                        << "  Arc x=" << static_cast<int>(it->x().get())
                        << ", y=" << static_cast<int>(it->y().get())
                        << ", arcWidth=" << static_cast<int>(it->arcWidth().get())
                        << ", height=" << static_cast<int>(it->height().get())
                        << ", angle=" << static_cast<int>(it->angle().get())
                        << ", len=" << static_cast<int>(it->len().get())
                        << ", color=" << it->color()
                        << ", width=" << static_cast<int>(it->width())
                        << ", style=" << static_cast<int>(it->style())
                        << ", condition=" << it->condition()
                        << std::endl;
                }

                auto rects(component->Symbols().Symbol().Rectangle());
                for (auto it(rects.begin()); it != rects.end(); ++it)
                {
                    std::cout
                        << "  Rectangle x=" << static_cast<int>(it->x().get())
                        << ", y=" << static_cast<int>(it->y().get())
                        << ", width=" << it->width().get()
                        << ", height=" << it->height().get()
                        << ", color=" << it->color()
                        << ", lwidth=" << static_cast<uint32_t>(it->lineWidth())
                        << ", style=" << it->style()
                        << ", fill-color=" << it->fillColor()
                        << ", fill-style=" << it->fillStyle()
                        << ", filled=" << static_cast<bool>(it->filled())
                        << ", condition=" << it->condition()
                        << std::endl;
                }

                auto arrows(component->Symbols().Symbol().Arrow());
                for (auto it(arrows.begin()); it != arrows.end(); ++it)
                {
                    std::cout
                        << "  Arrow x1=" << static_cast<int>(it->x1().get())
                        << ", y1=" << static_cast<int>(it->y1().get())
                        << ", x2=" << static_cast<int>(it->x2().get())
                        << ", y2=" << static_cast<int>(it->y2().get())
                        << ", color=" << it->color()
                        << ", width=" << static_cast<uint32_t>(it->width())
                        << ", style=" << it->style()
                        << ", head-height=" << it->headHeight()
                        << ", head-width=" << it->headWidth()
                        << ", head-style=" << it->headStyle()
                        << ", condition=" << it->condition()
                        << std::endl;
                }

                auto texts(component->Symbols().Symbol().Text());
                for (auto it(texts.begin()); it != texts.end(); ++it)
                {
                    std::cout
                        << "  Text x=" << static_cast<int>(it->x().get())
                        << ", y:" << static_cast<int>(it->y().get())
                        << ", text:" << it->text().get()
                        << ", color:" << it->color()
                        << ", size:" << it->size()
                        << ", cos:" << it->cos()
                        << ", sin:" << it->sin()
                        << ", condition:" << it->condition()
                        << std::endl;
                }

                auto _portSyms(component->Symbols().Symbol().PortSym());
                for (auto it(_portSyms.begin()); it != _portSyms.end(); ++it)
                {
                    std::cout
                        << "  PortSym x=" << static_cast<int>(it->x().get())
                        << ", y=" << static_cast<int>(it->y().get())
                        << ", type=" << static_cast<int>(it->type().get())
                        << ", angle=" << static_cast<int>(it->angle().get())
                        << std::endl;
                }

                for (auto it(compParameters.begin()); it != compParameters.end(); ++it)
                {
                    std::cout
                        << "  Parameter name=" << it->name().get()
                        << ", unit=" << it->unit().get()
                        << ", default-value=" << it->default_value()
                        << ", equation=" << it->equation()
                        << ", show=" << static_cast<bool>(it->show().get())
                        << ", description=" << QString::fromUtf8(it->Description()).trimmed().toStdString()
                        << ", condition=" << QString::fromUtf8(it->condition()).toStdString()
                        << std::endl;
                }
#endif
            }

        }
        catch (const xml_schema::exception& exc)
        {
            std::cerr << componentFile.fileName().toUtf8().constData() << ": " << exc << std::endl;
        }
    }
}

