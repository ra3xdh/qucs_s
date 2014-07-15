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
#include <math.h>
#include <complex>

struct RC_elements {
    int   N;
    float R1;
    float R2;
    float R3;
    float R4;
    float R5;
    float C1;
    float C2;
};

struct FilterParam {
    float Ap;
    float As;
    float Fc;
    float Fs;
    float Rp;
    float Kv;
    int order;
};

class Filter
{

public:
    enum FType {HighPass, LowPass, BandPass, BandStop, NoFilter};
    enum FilterFunc {Butterworth, Chebyshev, Cauer, Bessel, InvChebyshev, NoFunc, User};

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
    float Fc,Kv,Fs,Ap,As,Rp;
    int Nr,Nc,Nopamp; // total number of R,C, opamp

    int Nr1,Nc1,Nop1; // number of R,C, opamp per stage

    void calcButterworth();
    void calcChebyshev();
    void calcInvChebyshev();
    void calcCauer();
    void calcBessel();
    void calcUserTrFunc();
    bool checkRCL(); // Checks RCL values. Are one of them NaN or not?

    void createFirstOrderComponentsHPF(QString &s,RC_elements stage, int dx);
    void createFirstOrderComponentsLPF(QString &s,RC_elements stage, int dx);
    void createFirstOrderWires(QString &s, int dx, int y);
    float autoscaleCapacitor(float C, QString &suffix);
    virtual void calcHighPass();
    virtual void calcLowPass();
    virtual void createHighPassSchematic(QString &s);
    virtual void createLowPassSchematic(QString &s);

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
