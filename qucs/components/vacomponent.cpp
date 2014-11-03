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

#include <QtGui>

#include "vacomponent.h"

#include <QString>
#include <QScriptEngine>
#include <QScriptValue>
#include <QScriptValueIterator>


/*!
 * \file vacomponent.cpp
 * \brief Implementation of the vacomponent class.
 */


/*!
 * \brief vacomponent::vacomponent
 * \param filename File (JSON) containing the symbol paintins and properties.
 */
vacomponent::vacomponent(QString filename)
{
  QString data = getData(filename);

  /// \todo check if JSON is error free
  /// \todo Need to destroy engine?
  QScriptEngine engine;
  QScriptValue vadata = engine.evaluate("(" + data + ")");

  Description = getString(vadata, "description");

  QScriptValue entries = vadata.property("property");

  QScriptValueIterator it(entries);

  while (it.hasNext()) {
    it.next();

    QScriptValue entry = it.value();

    // skip length named iterate
    if (it.name().compare("length")) {
      QString name = getString(entry, "name");
      QString value = getString(entry, "value");
      QString display = getString(entry, "display");
      QString desc = getString(entry, "desc");
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
  }

  createSymbol(filename);

  Model = getString(vadata, "Model");
  Name  = getString(vadata, "SymName");

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
  QString data = getData(filename);

  QScriptEngine engine;
  QScriptValue vadata = engine.evaluate("(" + data + ")");

  Name  = getString(vadata, "Model");

  /// Default BitmapFile is [modulename]
  /// The BitmapFile JSON entry can be modified in \see LoadDialog::slotChangeIcon()
  BitmapFile  = getString(vadata, "BitmapFile");

  if(getNewOne) return new vacomponent(filename);
  return 0;
}

/*!
 * \brief vacomponent::createSymbol Constructor call this to create the symbol.
 * \param filename File (JSON) containing the symbol paintins and properties.
 * It reads the JSON file and parses the symbol paintings. Data is appended to
 * to the appropriate lists, Lines, Rects, Ellips, ...
 */
void vacomponent::createSymbol(QString filename)
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


  QString data = getData(filename);

  QScriptEngine engine;
  QScriptValue vadata = engine.evaluate("(" + data + ")");

  // get array of symbol paintigs
  QScriptValue entries = vadata.property("paintings");

  QScriptValueIterator it(entries);
  while (it.hasNext()) {
    it.next();

    qreal x, x1, x2, y, y1, y2, w, h, thick, angle, arclen;
    qreal size, cos, sin;
    QString color, style    , colorfill, stylefill, s;

    QScriptValue entry = it.value();
    QString type = getString(entry, "type");

    if (!type.compare("line")) {
      x1 = getDouble(entry, "x1");
      y1 = getDouble(entry, "y1");
      x2 = getDouble(entry, "x2");
      y2 = getDouble(entry, "y2");
      color = getString(entry, "color");
      thick = getDouble(entry, "thick");
      style = getString(entry, "style");

      Lines.append (new Line (x1, y1, x2, y2,
                        QPen (QColor (color), thick, penMap.value(style))));
    }

    if (!type.compare("rectangle")) {
      x = getDouble(entry, "x");
      y = getDouble(entry, "y");
      w = getDouble(entry, "w");
      h = getDouble(entry, "h");
      color = getString(entry, "color");
      thick = getDouble(entry, "thick");
      style = getString(entry, "style");
      colorfill = getString(entry, "colorfill");
      stylefill = getString(entry, "stylefill");

      Rects.append (new Area (x, y, w, h,
                        QPen (QColor (color), thick, penMap.value(style)),
                        QBrush(QColor (colorfill), brushMap.value(stylefill))
                        ));
    }

    if (!type.compare("ellipse")) {
      x = getDouble(entry, "x");
      y = getDouble(entry, "y");
      w = getDouble(entry, "w");
      h = getDouble(entry, "h");
      color = getString(entry, "color");
      thick = getDouble(entry, "thick");
      style = getString(entry, "style");
      colorfill = getString(entry, "colorfill");
      stylefill = getString(entry, "stylefill");

      Ellips.append (new Area (x, y, w, h,
                         QPen (QColor (color), thick, penMap.value(style)),
                         QBrush(QColor (colorfill), brushMap.value(stylefill))
                         ));
    }

    if (!type.compare("ellipsearc")) {
      x = getDouble(entry, "x");
      y = getDouble(entry, "y");
      w = getDouble(entry, "w");
      h = getDouble(entry, "h");
      angle = getDouble(entry, "angle");
      arclen = getDouble(entry, "arclen");
      color = getString(entry, "color");
      thick = getDouble(entry, "thick");
      style = getString(entry, "style");

      Arcs.append (new Arc (x, y, w, h, angle, arclen,
                       QPen (QColor (color), thick, penMap.value(style))));
    }

    if (!type.compare("portsymbol")) {
      x = getDouble(entry, "x");
      y = getDouble(entry, "y");
      Ports.append (new Port (x, y));
    }

    if (!type.compare("graphictext")) {
      x = getDouble(entry, "x");
      y = getDouble(entry, "y");
      s = getString(entry, "s");
      color = getString(entry, "color");
      size = getDouble(entry, "size");
      cos = getDouble(entry, "cos");
      sin = getDouble(entry, "sin");
      Texts.append (new Text (x, y, s,
                              QColor (color), size, cos, sin));
    }

    if (!type.compare("arrow")) {
      x1 = getDouble(entry, "x1");
      y1 = getDouble(entry, "y1");
      x2 = getDouble(entry, "x2");
      y2 = getDouble(entry, "y2");
      color = getString(entry, "color");
      thick = getDouble(entry, "thick");
      style = getString(entry, "style");
      Lines.append (new Line (x1, y1, x2, y2,
                        QPen (QColor (color), thick, penMap.value(style))));
     }
  }

  // bounding box, painted gray if component selected
  x1 = getDouble(vadata, "x1");
  y1 = getDouble(vadata, "y1");
  x2 = getDouble(vadata, "x2");
  y2 = getDouble(vadata, "y2");
}


// Move this elsewhere?
/*!
 * \brief getData Reads the JSON file
 * \param filename  File (JSON) containing the symbol paintings and properties.
 * \return the JSON file as a QString
 */
QString getData(QString filename)
{
  // Try to open the JSON file
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::critical(0, QObject::tr("Error"),
                          QObject::tr("Symbol file not found: %1").arg(filename));
    return "";
  }

  // Stream-in the file
  QTextStream in(&file);

  // Put into a string
  QString data = (QString) in.readAll();
  // close
  file.close();

  return data;
}

/*!
 * \brief getDouble Helper to get a property out of a JSON script
 * \param data JSON data as a QScritValue
 * \param prop JSON property key
 * \return a double corresponding to the JSON value
 */
double getDouble(QScriptValue data, QString prop){
  return data.property(prop).toString().toDouble();
}

/*!
 * \brief getString Helper to get a property out of a JSON script
 * \param data JSON data as a QScritValue
 * \param prop JSON property key
 * \return a QString corresponding to the JSON value
 */
QString getString(QScriptValue data, QString prop){
  return data.property(prop).toString();
}


