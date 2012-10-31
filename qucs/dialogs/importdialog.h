/***************************************************************************
                              importdialog.h
                             ----------------
    begin                : Fri Jun 23 2006
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

#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <qdialog.h>
#include <q3process.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <QLabel>

class Q3TextEdit;
class QLineEdit;
class Q3GridLayout;
class QPushButton;
class QComboBox;
class QLabel;


class ImportDialog : public QDialog  {
   Q_OBJECT
public:
  ImportDialog(QWidget*);
 ~ImportDialog();

private slots:
  void slotDisplayMsg();
  void slotDisplayErr();
  void slotProcessEnded();
  void slotImport();
  void slotAbort();
  void slotBrowse();
  void slotType(int);

private:
  void startSimulator();

public:
  Q3GridLayout *all;

  QLabel *OutputLabel;
  Q3Process Process;
  Q3TextEdit *MsgText;
  QLineEdit *ImportEdit, *OutputEdit, *OutputData;
  QPushButton *ImportButt, *CancelButt, *AbortButt;
  QComboBox *OutType;
};

#endif
