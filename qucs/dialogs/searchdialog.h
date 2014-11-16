/*
 * searchdialog.h - declaration of search dialog
 *
 * Copyright (C) 2006, Michael Margraf, michael.margraf@alumni.tu-berlin.de
 * Copyright (C) 2014, Yodalee, lc85301@gmail.com
 *
 * This file is part of Qucs
 *
 * Qucs is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>

class QucsApp;
class QLineEdit;
class QCheckBox;
class QGroupBox;
class QPushButton;

namespace Ui {
  class SearchDialog;
}

class SearchDialog : public QDialog
{
  Q_OBJECT

public:
  SearchDialog(QucsApp*);
 ~SearchDialog();

  void initSearch(bool replace=false);
  void searchText(bool, int);

private slots:
  void slotSearch();

private:
  Ui::SearchDialog *ui;
  QucsApp *App;
};

#endif
