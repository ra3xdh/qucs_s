/***************************************************************************
                               module.h
                              ----------
    begin                : Thu Nov 5 2009
    copyright            : (C) 2009 by Stefan Jahn
    email                : stefan@lkcc.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MODULE_H
#define MODULE_H

#include "component.h"

#include <QList>
#include <QHash>
#include <QMap>
#include <QSharedPointer>

class QPixmap;
class XmlComponent;

// function typedefs for circuits and analyses
typedef Element * (* pInfoFunc) (QString&, char * &, bool);
typedef Element * (* pInfoVAFunc) (QString&, QString&, bool, QString);
typedef Component * (* pCreatorFunc) ();

class Module
{
public:
    Module ();
    ~Module ();

    Element* getInfo(QString& name, char* &bitmapFile, bool getNewOne);
    bool hasInfo() const { return a_info != nullptr || a_xmlComp; }

    static void registerModule (QString, pInfoFunc);
    static void registerComponent (QString, pInfoFunc);
    static void registerXmlComponent(const QString& category, const QSharedPointer<XmlComponent>& xmlComp);
    static void intoCategory (Module *);
    static Component* getComponent(const QString& model);
    static void registerDynamicComponents(void);

public:
    static QHash<QString, Module*> s_modules;
    static QMap<QString, QString> s_vaComponents;

public:
    static void registerModules (void);
    static void registerXmlComponents(const QString& componentPath);
    static void unregisterModules (void);

public:
    pInfoVAFunc a_infoVA;
    QString a_category;
    QPixmap* a_icon;

private:
    pInfoFunc a_info;
    QSharedPointer<XmlComponent> a_xmlComp;
};

class Category
{
public:
    Category ();
    Category (QString);
    ~Category ();

public:
    static QList<Category *> Categories;

public:
    static QStringList getCategories (void);
    static QList<Module *> getModules (QString);
    static int getModulesNr (QString);

public:
    QString Name;
    QList<Module *> Content;
};

#endif /* __MODULE_H__ */
