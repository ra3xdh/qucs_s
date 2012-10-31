/***************************************************************************
                                labeldialog.h
                               ---------------
    begin                : Thu Dec 09 2004
    copyright            : (C) 2004 by Michael Margraf
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

#ifndef LABELDIALOG_H
#define LABELDIALOG_H

#include <qdialog.h>
#include <qregexp.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <QLabel>

class QLabel;
class QLineEdit;
class QPushButton;
class Q3GridLayout;
class QRegExpValidator;
class WireLabel;


class LabelDialog : public QDialog  {
Q_OBJECT
public:
  LabelDialog(WireLabel*, QWidget *parent=0);
 ~LabelDialog();

  QLineEdit  *NodeName, *InitValue;

private slots:
  void slotExtend();
  void slotOk();
  void slotCancel();

private:
  QPushButton *ButtonOk, *ButtonCancel, *ButtonMore;
  Q3GridLayout *gbox;
  QRegExpValidator *Validator1, *Validator2;
  QRegExp      Expr1, Expr2;
  QLabel      *Label2;

  WireLabel *pLabel;
};

#endif
