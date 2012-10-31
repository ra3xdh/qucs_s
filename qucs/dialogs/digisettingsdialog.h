/***************************************************************************
                             digisettingsdialog.h
                            ----------------------
    begin                : Sat Apr 01 2006
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

#ifndef DIGISETTINGSDIALOG_H
#define DIGISETTINGSDIALOG_H

#include <qdialog.h>
#include <qregexp.h>
//Added by qt3to4:
#include <QLabel>

class TextDoc;
class QLineEdit;
class QPushButton;
class QRegExpValidator;
class QLabel;
class QRadioButton;


class DigiSettingsDialog : public QDialog  {
Q_OBJECT
public:
  DigiSettingsDialog(TextDoc*);
 ~DigiSettingsDialog();

  QString SimTime;
  QLineEdit *TimeEdit, *LibEdit, *NameEdit;
  QLabel *TimeLabel, *LibLabel, *NameLabel;
  QRadioButton *simRadio, *comRadio;

private slots:
  void slotOk();
  void slotChangeMode(int);

private:
  TextDoc *Doc;
  QRegExp Expr;
  QRegExpValidator *Validator;
};

#endif
