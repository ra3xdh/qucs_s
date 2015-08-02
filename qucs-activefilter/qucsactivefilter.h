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

struct tQucsSettings {
  int x, y;      // position of main window
  QFont font;
  QString LangDir;
  QString Language;
  bool showConsole;
};

extern struct tQucsSettings QucsSettings;

class QucsActiveFilter : public QMainWindow
{
    Q_OBJECT

private:

    enum lstTopology { topoMFB = 0, topoSallenKey = 1, topoCauer = 2};
    enum lstFiltFunc { funcButterworth =0, funcChebyshev=1, funcInvChebyshev=2,
                       funcCauer = 3, funcBessel = 4, funcUser = 5};
    enum lstFiltType { tLowPass =0, tHiPass=1, tBandPass=2, tBandStop=3};

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

    QGroupBox *gpbCons;
    QAction *viewConsole;
    QPlainTextEdit *txtResult;

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

    QVBoxLayout *top1;
    
    QSvgWidget *sch_pic;

    QWidget *zenter;

    void errorMessage(QString s);

    QVector< std::complex<float> > Poles;

    double  Fc;
    int Nfil;

    QVector<long double> coeffB;
    QVector<long double> coeffA;

    Filter::FType ftyp;

private slots:

    void slotUpdateSchematic();
    void slotUpdateResponse();
    void slotCalcSchematic();
    void slotSwitchParameters();
    void slotSetLabels();
    void slotDefineTransferFunc();
    void slotHelpAbout();
    void slotHelpAboutQt();
    void slotHelpIntro();
    void slotViewConsole(bool toggle);
    void fixSize();

public:
    QucsActiveFilter(QWidget *parent = 0);
    ~QucsActiveFilter();
};

#endif // FILTERSINTEZ_H
