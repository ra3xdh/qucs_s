/***************************************************************************
                          filldialog.h  -  description
                             -------------------
    begin                : Thu May 20 2004
    copyright            : (C) 2003 by Michael Margraf
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

#ifndef FILLDIALOG_H
#define FILLDIALOG_H

#include <qdialog.h>
//Added by qt3to4:
#include <QLabel>
#include <Q3VBoxLayout>

class Q3VBoxLayout;
class QLabel;
class QIntValidator;
class QCheckBox;
class QComboBox;
class QPushButton;
class QLineEdit;

/**
  *@author Michael Margraf
  */

class FillDialog : public QDialog  {
Q_OBJECT
public:
  FillDialog(const QString& _Caption, bool show=true, QWidget *parent=0);
  ~FillDialog();

public slots:
  void slotCheckFilled(bool on);

private slots:
  void slotSetColor();
  void slotSetFillColor();

public:
  QLabel        *FillLabel1, *FillLabel2;
  QCheckBox     *CheckFilled;
  QLineEdit     *LineWidth;
  QPushButton   *ColorButt, *FillColorButt;
  QComboBox     *StyleBox, *FillStyleBox;

  Q3VBoxLayout   *all;
  QIntValidator *val100;
};

#endif
