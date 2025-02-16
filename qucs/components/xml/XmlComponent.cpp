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


XmlComponent::XmlComponent(
        const QString& name,
        const QString& schematicId,
        const QString& description,
        const QString& defaultModel,
        const QString& spiceModel,
        const QList<Parameter>& parameters) :
    a_name(name),
    a_schematicId(schematicId),
    a_description(description),
    a_defaultModel(defaultModel),
    a_spiceModel(spiceModel),
    a_parameters(parameters)
{
}

Component* XmlComponent::newOne()
{
    return new XmlComponent(
            a_name,
            a_schematicId,
            a_description,
            a_defaultModel,
            a_spiceModel,
            a_parameters);
}

Element* XmlComponent::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
    /*Name = QObject::tr("MiM C");
    BitmapFile = const_cast<char*>("C_SPICE");

    if (getNewOne)
    {
        return newOne();
    }*/

    return nullptr;
}

QString XmlComponent::netlist()
{
    return QString("");
}

QString XmlComponent::spice_netlist(spicecompat::SpiceDialect dialect /* = spicecompat::SPICEDefault */)
{
    Q_UNUSED(dialect);

    //QString ltr = getProperty("Letter")->Value;
    QString s /*= spicecompat::check_refdes(Name, ltr)*/;
    /*for (Port *p1 : Ports)
    {
        QString name = p1->Connection->Name;
        if (name == "gnd")
        {
            name = "0";
        }

        s += " " + name + " ";   // node names
    }

    QString model = getProperty("model")->Value;
    QString W = getProperty("W")->Value;
    QString L = getProperty("L")->Value;

    if (model.length()  > 0)
    {
        s += QString("%1").arg(model);
    }
    if (W.length() > 0)
    {
        s += QString(" W=%1").arg(W);
    }
    if (L.length() > 0)
    {
        s += QString(" L=%1").arg(L);
    }
    s += "\n";*/

    return s;
}

QString XmlComponent::cdl_netlist()
{
    return spice_netlist(spicecompat::CDL);
}
