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

  // component properties
  int Text_x, Text_y;
  QString Prefix, LibraryPath, ComponentName;
  QString ModelString, VerilogModelString, VHDLModelString;

protected:
  void mouseMoveEvent(QMouseEvent*);

private:
  int createStandardSymbol(const QString&, const QString&);

  void  paintEvent(QPaintEvent*);

  int  analyseLine(const QString&);

  bool getPen  (const QString&, QPen&, int);
  bool getBrush(const QString&, QBrush&, int);

  QString PaintText;
  QString DragNDropText;
  QString Warning;
  int TextWidth, DragNDropWidth, TextHeight;
  int cx, cy, x1, x2, y1, y2;
  QList<qucs::Line *> Lines;
  QList<qucs::Arc *> Arcs;
  QList<qucs::Area *> Rects, Ellips;
  QList<Text *>  Texts;
};

#endif // SYMBOLWIDGET_H
