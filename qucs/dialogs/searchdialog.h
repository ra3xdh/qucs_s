/***************************************************************************
                               searchdialog.h
                              ----------------
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

#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>

class QucsApp;
class QLineEdit;
class QCheckBox;
class QGroupBox;
class QPushButton;


class SearchDialog : public QDialog  {
Q_OBJECT
public:
  SearchDialog(QucsApp*);
 ~SearchDialog();

  void initSearch(bool replace=false);
  void searchText(bool, int);

private slots:
  void slotSearch();

private:
  QucsApp *App;
  QLineEdit *SearchEdit, *ReplaceEdit;
  QCheckBox *PositionBox, *CaseBox, *WordBox, *BackwardBox, *AskBox;
  QGroupBox *ReplaceGroup;
};

#endif
