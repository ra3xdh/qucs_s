/***************************************************************************
                              coaxialline.cpp
                             -----------------
    begin                : Sun Jan 29 2006
    copyright            : (C) 2006 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "coaxialline.h"
#include "extsimkernels/spicecompat.h"
#include "node.h"

CoaxialLine::CoaxialLine()
{
    Description = QObject::tr("coaxial transmission line");
    Simulator = spicecompat::simAll;

    Arcs.append(new qucs::Arc(-20, -9, 8, 18, 0, 16 * 360, QPen(Qt::darkBlue, 2)));
    Arcs.append(new qucs::Arc(11, -9, 8, 18, 16 * 270, 16 * 180, QPen(Qt::darkBlue, 2)));

    Lines.append(new qucs::Line(-30, 0, -16, 0, QPen(Qt::darkBlue, 2)));
    Lines.append(new qucs::Line(19, 0, 30, 0, QPen(Qt::darkBlue, 2)));
    Lines.append(new qucs::Line(-16, -9, 16, -9, QPen(Qt::darkBlue, 2)));
    Lines.append(new qucs::Line(-16, 9, 16, 9, QPen(Qt::darkBlue, 2)));

    Ports.append(new Port(-30, 0));
    Ports.append(new Port(30, 0));

    x1 = -30;
    y1 = -12;
    x2 = 30;
    y2 = 12;

    tx = x1 + 4;
    ty = y2 + 4;
    Model = "COAX";
    Name = "Line";
    SpiceModel = "X";

    Props.append(new Property("er", "2.29", true,
        QObject::tr("relative permittivity of dielectric")));
    Props.append(new Property("rho", "0.022e-6", false,
        QObject::tr("specific resistance of conductor")));
    Props.append(new Property("mur", "1", false,
        QObject::tr("relative permeability of conductor")));
    Props.append(new Property("D", "2.95 mm", false,
        QObject::tr("inner diameter of shield")));
    Props.append(new Property("d", "0.9 mm", false,
        QObject::tr("diameter of inner conductor")));
    Props.append(new Property("L", "1500 mm", true,
        QObject::tr("mechanical length of the line")));
    Props.append(new Property("tand", "4e-4", false,
        QObject::tr("loss tangent")));
    Props.append(new Property("Temp", "26.85", false,
        QObject::tr("simulation temperature in degree Celsius")));
}

CoaxialLine::~CoaxialLine()
{
}

Component* CoaxialLine::newOne()
{
    return new CoaxialLine();
}

Element* CoaxialLine::info(QString& Name, char*& BitmapFile, bool getNewOne)
{
    Name = QObject::tr("Coaxial Line");
    BitmapFile = (char*)"coaxial";

    if (getNewOne)
        return new CoaxialLine();
    return 0;
}

QString CoaxialLine::spice_netlist(spicecompat::SpiceDialect dialect)
{
    Q_UNUSED(dialect);
    QString s;

    QString p1 = spicecompat::normalize_node_name(Ports.at(0)->Connection->Name);
    QString p2 = spicecompat::normalize_node_name(Ports.at(1)->Connection->Name);

    QString D = spicecompat::normalize_value(getProperty("D")->Value);
    QString d = spicecompat::normalize_value(getProperty("d")->Value);
    QString er = spicecompat::normalize_value(getProperty("er")->Value);
    QString mur = spicecompat::normalize_value(getProperty("mur")->Value);
    QString L = spicecompat::normalize_value(getProperty("L")->Value);
    QString rho = spicecompat::normalize_value(getProperty("rho")->Value);
    QString tand = spicecompat::normalize_value(getProperty("tand")->Value);

    s = QString("X%1 %2 0 %3 0 COAX De=%4 Di=%5 L=%6 er=%7 mur=%8 rho=%9 tand=%10\n")
            .arg(Name)
            .arg(p1)
            .arg(p2)
            .arg(D)
            .arg(d)
            .arg(L)
            .arg(er)
            .arg(mur)
            .arg(rho)
            .arg(tand);

    return s;
}

QString CoaxialLine::getSpiceLibrary()
{
    QString f = spicecompat::getSpiceLibPath("coax.cir");
    QString s = QString(".INCLUDE \"%1\"\n").arg(f);
    return s;
}
