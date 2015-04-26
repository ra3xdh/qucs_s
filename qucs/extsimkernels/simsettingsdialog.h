/***************************************************************************
                            simsettingsdialog.h
                             ----------------
    begin                : Tue Apr 21 2015
    copyright            : (C) 2015 by Vadim Kuznetsov
    email                : ra3xdh@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef SIMSETTINGSDIALOG_H
#define SIMSETTINGSDIALOG_H

#include <QtGui>

class SimSettingsDialog : public QDialog
{
    Q_OBJECT

private:
    QLabel *lblXyce;
    QLabel *lblNgspice;
    QLabel *lblXycePar;
    QLabel *lblNprocs;

    QLineEdit *edtNgspice;
    QLineEdit *edtXyce;
    QLineEdit *edtXycePar;
    QSpinBox  *spbNprocs;

    QPushButton *btnOK;
    QPushButton *btnCancel;

    QPushButton *btnSetNgspice;
    QPushButton *btnSetXyce;
    QPushButton *btnSetXycePar;

public:
    explicit SimSettingsDialog(QWidget *parent = 0);
    
    
private slots:
    void slotApply();
    void slotSetNgspice();
    void slotSetXyce();
    void slotSetXycePar();
    
};

#endif // SIMSETTINGSDIALOG_H
