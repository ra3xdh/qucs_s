/***************************************************************************
                              sweepdialog.h
                             ---------------
    begin                : Sat Aug 13 2005
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

#ifndef SWEEPDIALOG_H
#define SWEEPDIALOG_H

#include <qdialog.h>
#include <qregexp.h>
#include <q3ptrlist.h>
#include <qspinbox.h>
//Added by qt3to4:
#include <Q3GridLayout>

#include "node.h"

class Graph;
class Schematic;
class Q3GridLayout;


class mySpinBox : public QSpinBox {
   Q_OBJECT
public:
  mySpinBox(int, int, int, double*, QWidget*);

protected:
  QString mapValueToText(int);

private:
  double *Values;
};



class SweepDialog : public QDialog {
   Q_OBJECT
public:
  SweepDialog(Schematic*);
 ~SweepDialog();

private slots:
  void slotNewValue(int);

private:
  Graph* setBiasPoints();

  Q3GridLayout *all;   // the mother of all widgets
  Q3PtrList<mySpinBox> BoxList;

  Graph *pGraph;
  Schematic *Doc;
  Q3PtrList<Node> NodeList;
  Q3PtrList<double> ValueList;
};

#endif
