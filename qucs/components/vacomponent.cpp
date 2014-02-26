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

vacomponent::vacomponent(QString filename)
{
  //qDebug() << "vacomponent built from: " << filename;

  QString data = getData(filename);

  // TODO check if JSON is error free?!
  // TODO Need to destroy enginte??
  QScriptEngine engine;
  QScriptValue vadata = engine.evaluate("(" + data + ")");

  Description = getString(vadata, "description");

  QScriptValue entries = vadata.property("property");

  QScriptValueIterator it(entries);

  // Props.append;
  while (it.hasNext()) {
    it.next();
    //qDebug() << "prop" << it.value().toString();

    QScriptValue entry = it.value();

    // skip length named iterate
    if (it.name().compare("length")) {
      //qDebug() << it.name() << ": " << it.value().toString();
      QString name = getString(entry, "name");
      QString value = getString(entry, "value");
      QString display = getString(entry, "display");
      QString desc = getString(entry, "desc");
      // QString unit = getString(entry, "unit");

      // TODO append units to description
      bool show;
      if (!display.compare("false"))
        show = false;
      else
        show = true;

      Props.append (new Property (name, value, show, desc));
    }
  }

  createSymbol(filename);

  Model = getString(vadata, "Model");
  Name  = getString(vadata, "SymName");

  //qDebug() << "Model, Name: " << Model << Name;

  tx = x1+100;
  ty = y1+20;
}

// used by mouseactions to drop new items into the schematic
Component *vacomponent::newOne(QString filename)
{
  vacomponent * p = new vacomponent(filename);
  p->Props.getFirst()->Value = Props.getFirst()->Value;
  p->recreate(0);
  return p;

}

/*
 *
 */
Element *vacomponent::info(QString &Name, char *&BitmapFile,
                           bool getNewOne, QString filename)
{
  // get variables out of file
  QString data = getData(filename);

  QScriptEngine engine;
  QScriptValue vadata = engine.evaluate("(" + data + ")");

  Name  = getDouble(vadata, "Name");

  // TODO let user change this, only used to populate the dock
  // use clemens stuff to render out of symbol paintings?
  BitmapFile = (char *) "VAimage";

  if(getNewOne) return new vacomponent(filename);
  return 0;
}


/*
 * vacomponent creates its own symbol
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
//      colorfill = getString(entry, "colorfill");
//      stylefill = getString(entry, "stylefill");

      // TODO pass pen/brush to paint filled areas
      Rects.append (new Area (x, y, w, h,
                        QPen (QColor (color), thick, Qt::SolidLine)));
    }

    /* TODO ellipse
    if  () {


    }
    */

    if (!type.compare("ellipsearc")) {
      qDebug() << "ellipsearc?" << type;
      x = getDouble(entry, "x");
      y = getDouble(entry, "y");
      w = getDouble(entry, "w");
      h = getDouble(entry, "h");
      angle = getDouble(entry, "angle");
      arclen = getDouble(entry, "arclen");
      color = getString(entry, "color");
      thick = getDouble(entry, "thick");
//      style = getString(entry, "style");
//      colorfill = getString(entry, "colorfill");
//      stylefill = getString(entry, "stylefill");

      // TODO pass pen/brush to paint filled areas
      Arcs.append (new Arc (x, y, w, h, angle, arclen,
                       QPen (QColor (color), thick, Qt::SolidLine)));
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
      qDebug() << "arrow?" << type;
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

  //qDebug() << "box" << x1 << y1 << x2 << y2;
}


// Move this elsewhere?
QString getData(QString filename)
{
  // Try to open the JSON file
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "Bad JSON file: " << filename;
    // TODO handle the error
    return QString("Bad JSON");
  }

  // Stream-in the file
  QTextStream in(&file);

  // Put into a string
  QString data = (QString) in.readAll();
  // close
  file.close();

  return data;
}

//
double getDouble(QScriptValue data, QString prop){
  return data.property(prop).toString().toDouble();
}

//
QString getString(QScriptValue data, QString prop){
  return data.property(prop).toString();
}


