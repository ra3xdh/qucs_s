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
#include <QtWidgets>

class SimSettingsDialog : public QDialog
{
    Q_OBJECT

private:
    QLabel *a_lblXyce;
    QLabel *a_lblNgspice;
    QLabel *a_lblSpiceOpus;
    QLabel *a_lblQucsator;
    QLabel *a_lblNgspiceSimParam;
    QLabel *a_lblXyceSimParam;
    QLabel *a_lblSpopusSimParam;
    QLabel *a_lblCompatMode;

    QComboBox *a_cbxCompatMode;

    QLineEdit *a_edtNgspice;
    QLineEdit *a_edtSpiceOpus;
    QLineEdit *a_edtXyce;
    QLineEdit *a_edtQucsator;
    QLineEdit *a_edtNgspiceSimParam;
    QLineEdit *a_edtXyceSimParam;
    QLineEdit *a_edtSpopusSimParam;

    QPushButton *a_btnOK;
    QPushButton *a_btnCancel;

    QPushButton *a_btnSetNgspice;
    QPushButton *a_btnSetSpOpus;
    QPushButton *a_btnSetXyce;
    QPushButton *a_btnSetQucsator;

public:
    explicit SimSettingsDialog(QWidget *parent = 0);


private slots:
    void slotApply();
    void slotCancel();
    void slotSetNgspice();
    void slotSetXyce();
    void slotSetXycePar();
    void slotSetSpiceOpus();
    void slotSetQucsator();

};

#endif // SIMSETTINGSDIALOG_H
