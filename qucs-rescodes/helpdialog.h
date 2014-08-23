/***************************************************************************
                                helpdialog.h
                             -------------------
    begin                : Fri Mar 04 2005
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

#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <qdialog.h>

class QTextEdit;
class QVBoxLayout;

/**
  *@author Michael Margraf
  */

class HelpDialog : public QDialog  {
   Q_OBJECT
public:
  HelpDialog(QWidget *parent = 0);
 ~HelpDialog();

private slots:
  void slotClose();

private:
  QVBoxLayout *vLayout;
  QTextEdit *Text;
};

#endif
