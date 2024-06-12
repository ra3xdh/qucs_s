/***************************************************************************
                                symbolwidget.h
                               ----------------
    begin                : Sat May 29 2005
    copyright            : (C) 2005 by Michael Margraf
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

#ifndef SYMBOLWIDGET_H
#define SYMBOLWIDGET_H

#include <QWidget>
#include <QSize>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QString>
#include <QList>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QMessageBox>

#include "element.h"

/*!
 * \file symbolwidget.h
 * \brief Definition of the SymbolWidget class.
 */

class QPaintEvent;
class QSizePolicy;

class SymbolWidget : public QWidget  {
   Q_OBJECT
public:
  SymbolWidget(QWidget *parent = 0);
 ~SymbolWidget();

  QString theModel();
  int setSymbol( QString&, const QString&, const QString&);
  int loadSymFile(const QString &file);
  void enableDragNDrop();
  void disableDragNDrop();
  bool dragNDropEnabled() { return dragNDrop; }
  void enableShowPinNumbers() { showPinNumbers = true; }
  void disableShowPinNumbers() { showPinNumbers = false; }
  bool showPinNumbersEnabled() { return showPinNumbers; }
  // component properties
  int Text_x, Text_y;
  QString Prefix, LibraryPath, ComponentName;
  QString ModelString, VerilogModelString, VHDLModelString,
          SpiceString;

protected:
  void mouseMoveEvent(QMouseEvent*);

private:
  int createStandardSymbol(const QString&, const QString&);

  void  paintEvent(QPaintEvent*);

  int  analyseLine(const QString&);

  static bool getPen  (const QString&, QPen&, int);
  bool getBrush(const QString&, QBrush&, int);

  bool dragNDrop;
  bool showPinNumbers;
  QString PaintText;
  QString DragNDropText;
  QString Warning;
  int TextWidth, DragNDropWidth, TextHeight;
  int cx, cy, x1, x2, y1, y2;
  QList<qucs::Line *> Lines;
  QList<qucs::Arc *> Arcs;
  QList<qucs::Rect *> Rects;
  QList<qucs::Ellips *> Ellipses;
  QList<Text *>  Texts;
};

#endif // SYMBOLWIDGET_H
