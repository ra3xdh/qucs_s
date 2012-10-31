/***************************************************************************
                               optionsdialog.h
                             -------------------
    begin                : Sun Apr 03 2005
    copyright            : (C) 2005 by Stefan Jahn
    email                : stefan@lkcc.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <qdialog.h>
//Added by qt3to4:
#include <Q3VBoxLayout>

class Q3VBoxLayout;
class QComboBox;

/**
  *@author Stefan Jahn
  */

class OptionsDialog : public QDialog  {
   Q_OBJECT
public:
  OptionsDialog(QWidget *parent = 0);
 ~OptionsDialog();

private slots:
  void slotClose();
  void slotSave();

private:
  Q3VBoxLayout *vLayout;
  QComboBox *units[4];
};

#endif
