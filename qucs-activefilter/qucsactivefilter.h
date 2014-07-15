/***************************************************************************
                             qucsactivefilter.h
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

#ifndef FILTERSINTEZ_H
#define FILTERSINTEZ_H

#include <QtGui>
#include <QtSvg>
#include <QMainWindow>
#include <complex>
#include "filter.h"

class QucsActiveFilter : public QMainWindow
{
    Q_OBJECT

private:
    QLabel *lblInputData;
    QLabel *lblA1;
    QLabel *lblA2;
    QLabel *lblF1;
    QLabel *lblF2;
    QLabel *lblTyp;
    QLabel *lblKv;
    QLabel *lblRpl1;
    QLabel *lblRpl2;
    QLabel *lblResp;

    QLabel *lblAFR;
    QLabel *lblTopology;
    QLabel *lblOrder;

    QLineEdit *edtA1; // passband attenuation A1
    QLineEdit *edtA2; // stopband attenuation A2
    QLineEdit *edtF1; // passband cutoff frequency F1
    QLineEdit *edtF2; // stopband cutoff frequency F2
    QLineEdit *edtKv;
    QLineEdit *edtPassbRpl;
    QLineEdit *edtStopbRpl;
    QLineEdit *edtOrder;

    QComboBox *cbxFilterFunc;
    QPushButton *btnCalcSchematic;
    QPushButton *btnDefineTransferFunc;

    QLabel *lblResult;
    QTextEdit *txtResult;

    QSvgWidget *imgAFR;
    QLabel *lblSch;

    /*QRadioButton *btnLowPass;
    QRadioButton *btnHighPass;
    QRadioButton *btnBandPass;
    QRadioButton *btnBandStop;*/

    QComboBox *cbxFilterType;
    QComboBox *cbxResponse;

    QPushButton *btnElements;
    //QPushButton *btnPassive;

    QHBoxLayout *top;
    QVBoxLayout *left;
    QVBoxLayout *center;
    QVBoxLayout *right;
    QVBoxLayout *top1;
    
    QSvgWidget *sch_pic;

    QWidget *zenter;

    void errorMessage(QString s);

    QVector< std::complex<float> > Poles;

    float Fc;
    int Nfil;

    QVector<long double> coeffB;
    QVector<long double> coeffA;

    Filter::FType ftyp;

private slots:

    void slotUpdateSchematic();
    void slotUpdateResponse();
    void slotCalcSchematic();
    void slotSwitchParameters();
    void slotDefineTransferFunc();

public:
    QucsActiveFilter(QWidget *parent = 0);
    ~QucsActiveFilter();
};

#endif // FILTERSINTEZ_H
