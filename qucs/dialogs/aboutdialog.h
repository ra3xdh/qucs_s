/*
 * aboutdialog.h - customary about dialog showing various info
 *
 * Copyright (C) 2015, Qucs team (see AUTHORS file)
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

#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <random>
#include <array>

#include <QDialog>
#include <QVBoxLayout>

class QString;
class QTextBrowser;

class AboutDialog : public QDialog  {
   Q_OBJECT

public:
  AboutDialog(QWidget *parent = 0);

protected:
  bool eventFilter(QObject *obj, QEvent *ev);

private:
  void setAuthorsText(void);
  void setTrText(void);

  std::array<QString, 4> qucs_sDevs;
  std::array<QString, 9> currAuths; // current Qucs authors
  std::array<QString, 12> prevDevs; // previous Qucs developers
  std::array<QString, 18> trAuths; // Qucs translators

  QVBoxLayout *all;
  QTextBrowser *authorsBrowser; // to show authors
  QTextBrowser *trBrowser; // to show translators

  int prevTab; // previously selected tab

  std::mt19937 rng {std::random_device{}()}; // random number generator

public slots:
  void currentChangedSlot(int index); // tab selection changed

};

#endif
