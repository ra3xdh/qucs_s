/***************************************************************************
                             vacomponent.h
                             -------------
    begin                : Thur Feb 21 2014
    copyright            : (C) 2014 by Guilherme Brondani Torri
    email                : guitorri AT gmail DOT com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef VACOMPONENT_H
#define VACOMPONENT_H

#include "component.h"

#include <QFile>
#include <QTextStream>
#include <QScriptEngine>
#include <QScriptValue>
#include <QScriptValueIterator>

#include <QDebug>

class vacomponent : public Component
{
  public:
    //vacomponent();
    vacomponent(QString filename);
    ~vacomponent() { };
    Component* newOne(QString filename);
    static Element* info(QString&, QString &,
                         bool getNewOne=false, QString filename="");
  protected:
    void createSymbol(QString filename);

};

//
QString getData(QString filename);
double getDouble(QScriptValue data, QString prop);
QString getString(QScriptValue data, QString prop);

#endif /* vacomponent_H */
