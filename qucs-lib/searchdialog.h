/***************************************************************************
                                searchdialog.h
                               ----------------
    begin                : Sat Jun 11 2005
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

#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <qdialog.h>
//Added by qt3to4:
#include <Q3VBoxLayout>

class Q3VBoxLayout;
class QLineEdit;
class QucsLib;


class SearchDialog : public QDialog  {
   Q_OBJECT
public:
  SearchDialog(QucsLib *parent);
 ~SearchDialog();

private slots:
  void slotClose();
  void slotSearch();

private:
  Q3VBoxLayout *all;
  QLineEdit *SearchEdit;
  QucsLib   *ParentDialog;
};

#endif
