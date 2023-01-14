/***************************************************************************
                           transferfuncdialog.cpp
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


#include "transferfuncdialog.h"

TransferFuncDialog::TransferFuncDialog(QVector<long double> &a, QVector<long double> &b, QWidget *parent) :
    QDialog(parent)
{
    this->setWindowTitle(tr("Define filter transfer function"));

    imgTrfuncEq = new QLabel;
    imgTrfuncEq->setPixmap(QPixmap(":images/bitmaps/trfunc.png"));
    //imgTrfuncEq->setScaledContents(true);



    lblB = new QLabel(tr("Numerator b[i]="));
    lblA = new QLabel(tr("Denominator a[i]="));

    QStringList indexes;
    for (int i=0;i<50;i++) {
        indexes<<QString::number(i);
    }

    tblA = new QTableWidget;
    QStringList head1;
    head1<<tr("a[i]");
    tblA->setColumnCount(head1.count());
    tblA->setRowCount(50);
    tblA->setHorizontalHeaderLabels(head1);
    tblA->setVerticalHeaderLabels(indexes);
    tblA->setFixedWidth(150);

    if (!a.isEmpty()) {
        int i = a.count()-1;

        for (auto& num : a) {
            QTableWidgetItem *it = new QTableWidgetItem(QString::number((double)num));
            tblA->setItem(i,0,it);
            i--;
        }
    }


    tblB = new QTableWidget;
    QStringList head2;
    head2<<tr("b[i]");
    tblB->setColumnCount(head2.count());
    tblB->setRowCount(50);
    tblB->setHorizontalHeaderLabels(head2);
    tblB->setVerticalHeaderLabels(indexes);
    tblB->setFixedWidth(150);

    if (!b.isEmpty()) {
        int i = b.count()-1;

        for (auto& num :b) {
            QTableWidgetItem *it = new QTableWidgetItem(QString::number((double)num));
            tblB->setItem(i,0,it);
            i--;
        }
    }

    btnAccept = new QPushButton(tr("Accept"));
    connect(btnAccept,SIGNAL(clicked()),this,SLOT(accept()));
    connect(btnAccept,SIGNAL(clicked()),this,SLOT(slotCheckCoeffs()));
    btnCancel = new QPushButton(tr("Cancel"));
    connect(btnCancel,SIGNAL(clicked()),this,SLOT(reject()));

    low1 = new QVBoxLayout;
    low1->addWidget(lblB);
    low1->addWidget(tblB);

    low2 = new QVBoxLayout;
    low2->addWidget(lblA);
    low2->addWidget(tblA);

    low3 = new QHBoxLayout;
    low3->addWidget(btnAccept);
    low3->addWidget(btnCancel);

    top = new QHBoxLayout;
    top->addLayout(low1);
    top->addLayout(low2);

    top1 = new QVBoxLayout;
    top1->addWidget(imgTrfuncEq);
    top1->addLayout(top);
    top1->addLayout(low3);


    this->setLayout(top1);

}

void TransferFuncDialog::getCoeffs(QVector<long double> &a, QVector<long double> &b)
{
    a.clear();
    b.clear();

   // bool a0 = false;

    for (int i=tblA->rowCount()-1;i>=0;i--) {
        QTableWidgetItem *itm = tblA->item(i,0);
        if (itm!=0) {

            //if (!a0) a0 = true;

            QString str = itm->text();
            //if ((str.isEmpty())) break;
            bool ok;
            long double n = (long double) str.toDouble(&ok);
            if (ok) a.append(n);
        }
        //else if (a0) a.append(0.0);
    }


    //bool b0 = true;

    for (int i=tblB->rowCount()-1;i>=0;i--) {
        QTableWidgetItem *itm = tblB->item(i,0);
        if (itm!=0) {

            //if (!b0) b0 = true;

            QString str = itm->text();
            //if ((str.isEmpty())) break;
            bool ok;
            long double n = (long double) str.toDouble(&ok);
            if (ok) b.append(n);
        }
        //else if (b0) b.append(0.0);
    }
}

void TransferFuncDialog::slotCheckCoeffs()
{

}
