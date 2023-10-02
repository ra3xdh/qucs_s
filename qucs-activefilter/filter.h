/***************************************************************************
                                 filter.h
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

#ifndef FILTER_H
#define FILTER_H

#include <QtCore>
#include <cmath>
#include <complex>

static const double pi = 3.1415926535897932384626433832795029;  /* pi   */

struct RC_elements {
    int   N;
    double  R1;
    double  R2;
    double  R3;
    double  R4;
    double  R5;
    double  R6;
    double  C1;
    double  C2;
};

struct FilterParam {
    double  Ap; // Band pass atten.
    double  As; // Band stop atten.
    double  Fc; // Cutoff frequency
    double  Fs; // Stopband freq.
    double  Rp; // Passband ripple
    double  Kv; // Passband gain
    double  Fl; // Lower freq.
    double  Fu; // Upper freq.
    double  TW; // Band width
    double  Q; // Quality factor
    int order;
};

class Filter
{

public:
    enum FType {HighPass, LowPass, BandPass, BandStop, NoFilter};
    enum FilterFunc {Butterworth, Chebyshev, Cauer, Bessel, InvChebyshev, Legendre, NoFunc, User};

private:
    void cauerOrderEstim();
    void reformPolesZeros();

protected:
    QVector< std::complex<float> > Poles;
    QVector< std::complex<float> > Zeros;
    QVector<long double> vec_B; // Transfer function numenator
    QVector<long double> vec_A; // and denominator
    QVector<RC_elements> Sections;

    Filter::FType ftype;
    Filter::FilterFunc ffunc;
    int order;
    double  Fc,Kv,Fs,Ap,As,Rp,Fl,Fu,TW,Q,BW,F0;
    int Nr,Nc,Nopamp; // total number of R,C, opamp

    int Nr1,Nc1,Nop1; // number of R,C, opamp per stage

    bool calcButterworth();
    bool calcChebyshev();
    bool calcInvChebyshev();
    bool calcCauer();
    bool calcBessel();
    bool calcLegendre();
    bool calcUserTrFunc();
    bool checkRCL(); // Checks RCL values. Are one of them NaN or not?

    void createFirstOrderComponentsHPF(QString &s,RC_elements stage, int dx);
    void createFirstOrderComponentsLPF(QString &s,RC_elements stage, int dx);
    void createFirstOrderWires(QString &s, int dx, int y);
    double  autoscaleCapacitor(double C, QString &suffix);
    virtual void calcHighPass();
    virtual void calcLowPass();
    virtual void calcBandPass();
    virtual void calcBandStop();
    virtual void createHighPassSchematic(QString &s);
    virtual void createLowPassSchematic(QString &s);
    virtual void createBandPassSchematic(QString &s);
    virtual void createBandStopSchematic(QString &s);

public:


    Filter(Filter::FilterFunc ffunc_, Filter::FType type_, FilterParam par);
    virtual ~Filter();

    void calcFirstOrder();

    void createPartList(QStringList &lst);
    void createPolesZerosList(QStringList &lst);

    virtual void createSchematic(QString &s);

    virtual bool calcFilter();

    void set_TrFunc(QVector<long double> a, QVector<long double> b);

};

#endif // FILTER_H
