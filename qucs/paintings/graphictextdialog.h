/***************************************************************************
                            graphictextdialog.h
                           ---------------------
    begin                : Wed Nov 26 2003
    copyright            : (C) 2003 by Michael Margraf
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

#ifndef GRAPHICTEXTDIALOG_H
#define GRAPHICTEXTDIALOG_H

#include <QDialog>
#include <QVBoxLayout>

class QLineEdit;
class QTextEdit;
class QPushButton;
class QVBoxLayout;
class QIntValidator;


class GraphicTextDialog : public QDialog  {
Q_OBJECT
public:
  GraphicTextDialog(QWidget *parent=0, const char *name=0);
 ~GraphicTextDialog();

private slots:
  void slotSetColor();
  void slotOkButton();

public:
  QLineEdit   *TextSize, *Angle;
  QPushButton *ColorButt;
  QTextEdit   *text;

private:
  QVBoxLayout *vert;
  QIntValidator *val50, *val360;
};

#endif
