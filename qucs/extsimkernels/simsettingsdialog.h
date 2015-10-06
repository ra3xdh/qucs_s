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
    QLabel *lblSpiceOpus;
    QLabel *lblXycePar;
    QLabel *lblNprocs;
    QLabel *lblWorkdir;

    QLineEdit *edtNgspice;
    QLineEdit *edtSpiceOpus;
    QLineEdit *edtXyce;
    QLineEdit *edtXycePar;
    QSpinBox  *spbNprocs;
    QLineEdit *edtWorkdir;

    QPushButton *btnOK;
    QPushButton *btnCancel;

    QPushButton *btnSetNgspice;
    QPushButton *btnSetSpOpus;
    QPushButton *btnSetXyce;
    QPushButton *btnSetXycePar;
    QPushButton *btnSetWorkdir;

public:
    explicit SimSettingsDialog(QWidget *parent = 0);
    
    
private slots:
    void slotApply();
    void slotSetNgspice();
    void slotSetXyce();
    void slotSetXycePar();
    void slotSetSpiceOpus();
    void slotSetWorkdir();
    
};

#endif // SIMSETTINGSDIALOG_H
