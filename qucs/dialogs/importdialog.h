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

#include <QDialog>
#include <QProcess>
#include <QGridLayout>
#include <QLabel>

class QPlainTextEdit;
class QLineEdit;
class QGridLayout;
class QPushButton;
class QComboBox;
class QLabel;


class ImportDialog : public QDialog  {
   Q_OBJECT

private:
    QString lastImportDir;

public:
  ImportDialog(QWidget*);
 ~ImportDialog();

private slots:
  void slotDisplayMsg();
  void slotDisplayErr();
  void slotProcessEnded(int status);
  void slotImport();
  void slotAbort();
  void slotBrowse();
  void slotSaveBrowse();
  void slotType(int index);
  void slotValidateInput();
  void slotValidateOutput();

private:
  void startSimulator();
  bool getDataVarsFromDatafile(const QString &filename);

private:
  QGridLayout *all;

  QLabel *OutputLabel, *LibLabel;
  QProcess Process;
  QPlainTextEdit *MsgText;
  QLineEdit *ImportEdit, *OutputEdit, *LibName;
  QPushButton *ImportButt, *CancelButt, *AbortButt;
  QComboBox *OutType, *InType, *OutputData;

public:
  void setImportDir(const QString &dir) { lastImportDir = dir; };

};

#endif
