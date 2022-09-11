/***************************************************************************
                            transferfuncdialog.h
                              ----------------
    begin                : Wed Apr 10 2014
    copyright            : (C) 2014 by Vadim Kuznetsov
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

#ifndef TRANSFERFUNCDIALOG_H
#define TRANSFERFUNCDIALOG_H

#include <QtGui>
#include <QtWidgets>

class TransferFuncDialog : public QDialog
{
    Q_OBJECT

private:
    QLabel *lblA;
    QLabel *lblB;
    QLabel *imgTrfuncEq;
    QTableWidget *tblA;
    QTableWidget *tblB;

    QPushButton *btnAccept;
    QPushButton *btnCancel;

    QHBoxLayout *top;
    QVBoxLayout *top1;
    QVBoxLayout *low1;
    QVBoxLayout *low2;
    QHBoxLayout *low3;

public:
    explicit TransferFuncDialog(QVector<long double> &a, QVector<long double> &b, QWidget *parent = 0);

    void getCoeffs(QVector<long double> &a, QVector<long double> &b);
    
signals:
    
private slots:
    void slotCheckCoeffs();

public slots:
    
};

#endif // TRANSFERFUNCDIALOG_H
