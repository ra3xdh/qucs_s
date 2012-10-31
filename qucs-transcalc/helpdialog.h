/***************************************************************************
                                helpdialog.h
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

#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <qdialog.h>
//Added by qt3to4:
#include <Q3VBoxLayout>

class Q3TextEdit;
class Q3VBoxLayout;

/**
  *@author Stefan Jahn
  */

class HelpDialog : public QDialog  {
   Q_OBJECT
public:
  HelpDialog(QWidget *parent = 0);
 ~HelpDialog();

private slots:
  void slotClose();

private:
  Q3VBoxLayout *vLayout;
  Q3TextEdit *Text;
};

#endif
