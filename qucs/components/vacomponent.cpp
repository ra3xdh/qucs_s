/***************************************************************************
                            vacomponent.cpp
                            ---------------
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


#include "vacomponent.h"

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "node.h"


/*!
 * \file vacomponent.cpp
 * \brief Implementation of the vacomponent class.
 */
vacomponent::vacomponent(QJsonObject json)
{
   parseJson(json);
}


/*!
 * \brief vacomponent::vacomponent
 * \param filename File (JSON) containing the symbol paintins and properties.
 */
vacomponent::vacomponent(QString filename)
{
    //QString data = getData(filename);
    QJsonObject json;
    try {
        json = getJsonObject(filename);
    } catch (const std::runtime_error& ex) {
        return;
    }
    parseJson(json);
}

void vacomponent::parseJson(QJsonObject json)
{
    /// \todo check if JSON is error free
    /// \todo Need to destroy engine?

    Description = getString(json, "description");

    QJsonArray propArray = json["property"].toArray();

    for (int propIndex = 0; propIndex < propArray.size(); ++propIndex) {
        QJsonObject property = propArray[propIndex].toObject();
        //if (it.name().compare("length")) {
        QString name = getString(property, "name");
        QString value = getString(property, "value");
        QString display = getString(property, "display");
        QString desc = getString(property, "desc");
        // QString unit = getString(entry, "unit");

        /// \todo append units to description

        bool show;
        if (!display.compare("false"))
            show = false;
        else
            show = true;

        /// \todo what if there are no properties?
        Props.append (new Property (name, value, show, desc));
    }

    createSymbol(json);

    Model = getString(json, "Model");
    Name  = getString(json, "SymName");
    SpiceModel = "N";

    /// TODO adjust location of text
    tx = x1+100;
    ty = y1+20;
}

/*!
 * \brief vacomponent::newOne is used to mouse drop new items into the schematic.
 * \param filename File (JSON) containing the symbol paintins and properties.
 * \return \a Component based on the \p filename
 *  Used by mouseactions to drop new items into the schematic.
 */
Component *vacomponent::newOne(QString filename)
{
  vacomponent * p = new vacomponent(filename);
  if (Props.count())
      p->Props.getFirst()->Value = Props.getFirst()->Value;
  p->recreate(0);
  return p;

}

/*!
 * \brief vacomponent::info is used to either get information or create objects.
 * \param Name Model name, returned by reference
 * \param BitmapFile Bitmap file for the dock, returned by reference
 * \param getNewOne if set return new object based on JSON file
 * \param filename File (JSON) containing the symbol paintins and properties.
 * \return Null or a new \a Element if \p getNewOne is true
 * Used to get \p Name and \p BitmapFile.
 * It can also create new objects from symbol file.
 */
Element *vacomponent::info(QString &Name, QString &BitmapFile,
                           bool getNewOne, QString filename)
{
    // get variables out of file
    QJsonObject json;
    try {
        json = getJsonObject(filename);
    } catch (const std::runtime_error& ex) {
        return 0;
    }

    Name  = getString(json, "Model");

    /// Default BitmapFile is [modulename]
    /// The BitmapFile JSON entry can be modified in \see LoadDialog::slotChangeIcon()
    BitmapFile  = getString(json, "BitmapFile");

    if(getNewOne) return new vacomponent(json);
    return 0;
}

/*!
 * \brief vacomponent::createSymbol Constructor call this to create the symbol.
 * \param filename File (JSON) containing the symbol paintins and properties.
 * It reads the JSON file and parses the symbol paintings. Data is appended to
 * to the appropriate lists, Lines, Rects, Ellips, ...
 */
void vacomponent::createSymbol(QJsonObject json)
{
    // map string to Qt Pen/Brush
    // any better to convert QString into Qt::BrushStyle ?
    QMap<QString, Qt::BrushStyle> brushMap;
    QMap<QString, Qt::PenStyle> penMap;

    brushMap.insert("Qt::NoBrush",          Qt::NoBrush);
    brushMap.insert("Qt::SolidPattern",     Qt::SolidPattern);
    brushMap.insert("Qt::Dense1Pattern",    Qt::Dense1Pattern);
    brushMap.insert("Qt::Dense2Pattern",    Qt::Dense2Pattern);
    brushMap.insert("Qt::Dense3Pattern",    Qt::Dense3Pattern);
    brushMap.insert("Qt::Dense4Pattern",    Qt::Dense4Pattern);
    brushMap.insert("Qt::Dense5Pattern",    Qt::Dense5Pattern);
    brushMap.insert("Qt::Dense6Pattern",    Qt::Dense6Pattern);
    brushMap.insert("Qt::Dense7Pattern",    Qt::Dense7Pattern);
    brushMap.insert("Qt::HorPattern",       Qt::HorPattern);
    brushMap.insert("Qt::VerPattern",       Qt::VerPattern);
    brushMap.insert("Qt::CrossPattern",     Qt::CrossPattern);
    brushMap.insert("Qt::BDiagPattern",     Qt::BDiagPattern);
    brushMap.insert("Qt::FDiagPattern",     Qt::FDiagPattern);
    brushMap.insert("Qt::DiagCrossPattern", Qt::DiagCrossPattern);

    penMap.insert("Qt::NoPen", Qt::NoPen);
    penMap.insert("Qt::SolidLine", Qt::SolidLine);
    penMap.insert("Qt::DashLine", Qt::DashLine);
    penMap.insert("Qt::DotLine", Qt::DotLine);
    penMap.insert("Qt::DashDotLine", Qt::DashDotLine);
    penMap.insert("Qt::DashDotDotLine", Qt::DashDotDotLine);
    penMap.insert("Qt::CustomDashLine", Qt::CustomDashLine);



    QJsonArray paintArray = json["paintings"].toArray();

    for (int paintIndex = 0; paintIndex < paintArray.size(); ++paintIndex) {
        QJsonObject paint = paintArray[paintIndex].toObject();

        qreal x, x1, x2, y, y1, y2, w, h, thick, angle, arclen;
        qreal size, cos, sin;
        QString color, style    , colorfill, stylefill, s;

        //QScriptValue entry = it.value();
        QString type = getString(paint, "type");

        if (!type.compare("line")) {
            x1 = getDouble(paint, "x1");
            y1 = getDouble(paint, "y1");
            x2 = getDouble(paint, "x2");
            y2 = getDouble(paint, "y2");
            color = getString(paint, "color");
            thick = getDouble(paint, "thick");
            style = getString(paint, "style");

            Lines.append (new qucs::Line (x1, y1, x2, y2,
                                        QPen (QColor (color), thick, penMap.value(style))));
        }

        if (!type.compare("rectangle")) {
            x = getDouble(paint, "x");
            y = getDouble(paint, "y");
            w = getDouble(paint, "w");
            h = getDouble(paint, "h");
            color = getString(paint, "color");
            thick = getDouble(paint, "thick");
            style = getString(paint, "style");
            colorfill = getString(paint, "colorfill");
            stylefill = getString(paint, "stylefill");

            Rects.append (new qucs::Area (x, y, w, h,
                                        QPen (QColor (color), thick, penMap.value(style)),
                                        QBrush(QColor (colorfill), brushMap.value(stylefill))
                                        ));
        }

        if (!type.compare("ellipse")) {
            x = getDouble(paint, "x");
            y = getDouble(paint, "y");
            w = getDouble(paint, "w");
            h = getDouble(paint, "h");
            color = getString(paint, "color");
            thick = getDouble(paint, "thick");
            style = getString(paint, "style");
            colorfill = getString(paint, "colorfill");
            stylefill = getString(paint, "stylefill");

            Ellips.append (new qucs::Area (x, y, w, h,
                                         QPen (QColor (color), thick, penMap.value(style)),
                                         QBrush(QColor (colorfill), brushMap.value(stylefill))
                                         ));
        }

        if (!type.compare("ellipsearc")) {
            x = getDouble(paint, "x");
            y = getDouble(paint, "y");
            w = getDouble(paint, "w");
            h = getDouble(paint, "h");
            angle = getDouble(paint, "angle");
            arclen = getDouble(paint, "arclen");
            color = getString(paint, "color");
            thick = getDouble(paint, "thick");
            style = getString(paint, "style");

            Arcs.append (new qucs::Arc (x, y, w, h, angle, arclen,
                                      QPen (QColor (color), thick, penMap.value(style))));
        }

        if (!type.compare("portsymbol")) {
            x = getDouble(paint, "x");
            y = getDouble(paint, "y");
            Ports.append (new Port (x, y));
        }

        if (!type.compare("graphictext")) {
            x = getDouble(paint, "x");
            y = getDouble(paint, "y");
            s = getString(paint, "s");
            color = getString(paint, "color");
            size = getDouble(paint, "size");
            cos = getDouble(paint, "cos");
            sin = getDouble(paint, "sin");
            Texts.append (new Text (x, y, s,
                                  QColor (color), size, cos, sin));
        }

        if (!type.compare("arrow")) {
            x1 = getDouble(paint, "x1");
            y1 = getDouble(paint, "y1");
            x2 = getDouble(paint, "x2");
            y2 = getDouble(paint, "y2");
            color = getString(paint, "color");
            thick = getDouble(paint, "thick");
            style = getString(paint, "style");
            Lines.append (new qucs::Line (x1, y1, x2, y2,
                                        QPen (QColor (color), thick, penMap.value(style))));
        }
    }

    // bounding box, painted gray if component selected
    x1 = getDouble(json, "x1");
    y1 = getDouble(json, "y1");
    x2 = getDouble(json, "x2");
    y2 = getDouble(json, "y2");
}


QString vacomponent::spice_netlist(bool isXyce)
{
    if (isXyce) return QString("");

    QString s = SpiceModel + Name + " ";
    for(const auto pp: Ports) {
        s += pp->Connection->Name + " ";
    }
    QString tmp_model = QString("mod_%1_%2").arg(Model).arg(Name);
    s += tmp_model + "\n";
    QString par_str;
    for(unsigned int i = 0; i < Props.count(); i++) {
        par_str += QString("%1=%2 ")
                .arg(Props.at(i)->Name)
                .arg(Props.at(i)->Value);
    }
    s += QString(".MODEL %1 %2 %3\n").arg(tmp_model)
                                      .arg(Model)
                                      .arg(par_str);
    return s;
}

// Move this elsewhere?


/*!
 * \brief getJsonObject Reads the JSON file
 * \param filename  File (JSON) containing the symbol paintings and properties.
 * \return the parsed JSON file as a QJsonObject
 */
QJsonObject getJsonObject(QString filename)
{
    // Try to open the JSON file
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(0, QObject::tr("Error"),
                              QObject::tr("Symbol file not found: %1").arg(filename));
        throw std::runtime_error("File not found");
    }

    // Stream-in the file
    QTextStream in(&file);

    // Put into a string
    QString data = (QString) in.readAll();
    // remove trailing comma`s
    data = data.simplified();
    data.remove(" ");
    data.replace(",]","]");
    data.replace(",}","}");
    // close
    file.close();

    QJsonParseError error;

    QJsonDocument jdoc = QJsonDocument::fromJson(data.toUtf8(), &error);


    if(error.error != QJsonParseError::NoError) {
        QMessageBox::critical(0, QObject::tr("Error"),
                              QObject::tr("Symbol file not found: %1").arg(filename));
        throw std::runtime_error("Json parse error");
    }


    QJsonObject object = jdoc.object();

    return object;
}

/*!
 * \brief getDouble Helper to get a property out of a JSON script
 * \param data JSON data as a QJsonObject
 * \param prop JSON property key
 * \return a double corresponding to the JSON value
 */
double getDouble(QJsonObject data, QString prop){
  return data[prop].toDouble();
}

/*!
 * \brief getString Helper to get a property out of a JSON script
 * \param data JSON data as a QScritValue
 * \param prop JSON property key
 * \return a QString corresponding to the JSON value
 */
QString getString(QJsonObject data, QString prop){
  return data[prop].toString();
}
