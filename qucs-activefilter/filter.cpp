/***************************************************************************
                                filter.cpp
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "filter.h"
#include "qf_poly.h"
#include "bessel.h"
#include "legendre.h"

static const int MaxOrder = 50;

Filter::Filter(Filter::FilterFunc ffunc_, Filter::FType type_, FilterParam par)
{
    ffunc = ffunc_;
    ftype = type_;

    if ((ftype==Filter::HighPass)||(ftype==Filter::LowPass)) {
        Fc = par.Fc;
        Fs = par.Fs;        
        Ap = par.Ap;
    } else {
        Fl = par.Fl;
        Fu = par.Fu;
        TW = par.TW;
        BW = fabs(Fu -Fl);
        F0 = sqrt(Fu*Fl);
        if ((ftype==Filter::BandPass)||
            (ftype==Filter::BandStop)) { // BandPass
            Fc=BW;          // cutoff freq. of LPF prototype
            double  Fs1 = Fu + TW;
            double  Fs1lp = fabsf(Fs1 - (F0*F0)/Fs1);    // stopband freq. of LPF prototype
            double  Fs2 = Fl - TW;
            double  Fs2lp = fabsf(Fs2 - (F0*F0)/Fs2);
            Fs = std::min(Fs1lp,Fs2lp);
        }
        Ap = 3.0;        
        qDebug()<<Fc<<Fs;
        Q = F0/fabs(Fu-Fl);
    }

    Rp = par.Rp;
    As = par.As;
    Kv = par.Kv;
    if (ffunc==Filter::Bessel||
        ffunc==Filter::Legendre) {
        order = par.order;
    }
}

Filter::~Filter()
{

}

void Filter::createSchematic(QString &s)
{
    switch (ftype) {
    case Filter::HighPass : createHighPassSchematic(s);
        break;
    case Filter::LowPass : createLowPassSchematic(s);
        break;
    case Filter::BandPass : createBandPassSchematic(s);
        break;
    case Filter::BandStop : createBandStopSchematic(s);
        break;
    default: break;
    }

}

void Filter::createHighPassSchematic(QString &s)
{
    s = "<Qucs Schematic ";
    s += PACKAGE_VERSION;
    s += ">\n";
}

void Filter::createLowPassSchematic(QString &s)
{
    s = "<Qucs Schematic ";
    s += PACKAGE_VERSION;
    s += ">\n";
}

void Filter::createBandPassSchematic(QString &s)
{
    s = "<Qucs Schematic " PACKAGE_VERSION ">\n";
}

void Filter::createBandStopSchematic(QString &s)
{
    s = "<Qucs Schematic " PACKAGE_VERSION ">\n";
}

bool Filter::calcFilter()
{
    Sections.clear();
    Poles.clear();
    Zeros.clear();
    bool res = false;

    switch (ffunc) {
    case Filter::Chebyshev : res = calcChebyshev();
        break;
    case Filter::Butterworth : res = calcButterworth();
        break;
    case Filter::Cauer : res = calcCauer();
        break;
    case Filter::InvChebyshev : res = calcInvChebyshev();
        break;
    case Filter::Bessel : res = calcBessel();
        break;
    case Filter::Legendre : res = calcLegendre();
        break;
    case Filter::User : res = calcUserTrFunc();
        break;
    default :
        return false;
        break;
    }

    if (!res) return false;

    if (Poles.isEmpty()) {
        return false;
    }

    if (((ffunc==Filter::Cauer)||
         (ffunc==Filter::InvChebyshev))
            &&(Zeros.isEmpty())) {
        return false;
    }

    switch (ftype) {
    case Filter::LowPass : calcLowPass();
        break;
    case Filter::HighPass : calcHighPass();
        break;
    case Filter::BandPass : calcBandPass();
        break;
    case Filter::BandStop : calcBandStop();
        break;
    default: return false;
        break;
    }

    res = checkRCL();

    Nr = Nr1*(order/2);
    Nc = Nc1*(order/2);
    Nopamp = Nop1*order/2;
    return res;
}


void Filter::calcHighPass()
{

}

void Filter::calcLowPass()
{

}

void Filter::calcBandPass()
{

}

void Filter::calcBandStop()
{

}

void Filter::calcFirstOrder()
{
    if (order%2 != 0) {

        double  R1, R2,R3;

        int k = order/2 + 1;
        double  Wc = 2*pi*Fc;
        double  re = Poles.at(k-1).real();
        //float im = Poles.at(k-1).imag();
        //float C = re*re + im*im;
        double  C = -re;
        double  C1 = 10/Fc;

        if (ftype==Filter::HighPass) {
            R1 = 1.0*C/(Wc*C1);
        } else {
            R1 = 1.0/(Wc*C*C1);
        }

        //qDebug()<<C;

        if (Kv != 1.0) {
            R2 = Kv*R1/(Kv - 1);
            R3 = Kv*R1;
        } else {
            R2 = 1.0;
            R3 = 0;
        }
        RC_elements curr_stage;
        curr_stage.N = k;
        curr_stage.R1 = 1000*R1;
        curr_stage.R2 = 1000*R2;
        curr_stage.R3 = 1000*R3;
        curr_stage.R4 = 0;
        curr_stage.C1 = C1;
        curr_stage.C2 = 0;
        Sections.append(curr_stage);

    }

}

void Filter::createPartList(QStringList &lst) {
    lst << QObject::tr("Part list");
    lst << QString("%1%2%3%4%5%6%7%8%9").arg("Stage#",6)
           .arg("C1",12).arg("C2",12).arg("R1(kOhm)",10).arg("R2(kOhm)",10).arg("R3(kOhm)",10).arg("R4(kOhm)",10)
           .arg("R5(kOhm)",10).arg("R6(kOhm)",10);

    for (const auto &stage: Sections) {
        QString suff1, suff2;
        double C1 = autoscaleCapacitor(stage.C1, suff1);
        double C2 = autoscaleCapacitor(stage.C2, suff2);

        lst << QString("%1%2%3%4%5%6%7%8%9%10%11")
                .arg(stage.N, 6)
                .arg(C1, 10, 'f', 3)
                .arg(suff1)
                .arg(C2, 10, 'f', 3)
                .arg(suff2)
                .arg(stage.R1, 10, 'f', 3)
                .arg(stage.R2, 10, 'f', 3)
                .arg(stage.R3, 10, 'f', 3)
                .arg(stage.R4, 10, 'f', 3)
                .arg(stage.R5, 10, 'f', 3)
                .arg(stage.R6, 10, 'f', 3);
    }
}

void Filter::createPolesZerosList(QStringList &lst) {
    lst << QString(QObject::tr("Filter order = %1")).arg(order);
    if (!Zeros.isEmpty()) {
        lst << "" << QObject::tr("Zeros list Pk=Re+j*Im");
        for (std::complex<float> zero: Zeros) {
            lst << QString::number(zero.real()) + " + j*" + QString::number(zero.imag());
        }
    }

    if ((ftype == Filter::BandPass) || (ftype == Filter::BandStop)) {
        lst << "" << QObject::tr("LPF prototype poles list Pk=Re+j*Im");
    } else {
        lst << "" << QObject::tr("Poles list Pk=Re+j*Im");
    }
    for (std::complex<float> pole: Poles) {
        lst << QString::number(pole.real()) + " + j*" + QString::number(pole.imag());
    }
    lst << "";
}

void Filter::createFirstOrderComponentsHPF(QString &s,RC_elements stage,int dx)
{
    QString suf;
    double  C1 = autoscaleCapacitor(stage.C1,suf);
    s += QString("<OpAmp OP%1 1 %2 160 -26 42 0 0 \"1e6\" 1 \"15 V\" 0>\n").arg(Nopamp+1).arg(270+dx);
    s += QString("<GND * 1 %1 270 0 0 0 0>\n").arg(170+dx);
    s += QString("<GND * 1 %1 370 0 0 0 0>\n").arg(220+dx);
    s += QString("<R R%1 1 %2 340 15 -26 0 1 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(Nr+2).arg(220+dx).arg(stage.R2,0,'f',3);
    s += QString("<R R%1 1 %2 240 -75 -26 1 1 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(Nr+1).arg(170+dx).arg(stage.R1,0,'f',3);
    s += QString("<R R%1 1 %2 260 -26 15 1 2 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(Nr+3).arg(310+dx).arg(stage.R3,0,'f',3);
    s += QString("<C C%1 1 %2 190 -26 -45 1 0 \"%3%4\" 1 \"\" 0 \"neutral\" 0>\n").arg(Nc+1).arg(100+dx).arg(C1,0,'f',3).arg(suf);
}


void Filter::createFirstOrderComponentsLPF(QString &s,RC_elements stage,int dx)
{
    QString suf;
    double  C1 = autoscaleCapacitor(stage.C1,suf);
    s += QString("<OpAmp OP%1 1 %2 160 -26 42 0 0 \"1e6\" 1 \"15 V\" 0>\n").arg(Nopamp+1).arg(270+dx);
    s += QString("<GND * 1 %1 270 0 0 0 0>\n").arg(170+dx);
    s += QString("<GND * 1 %1 370 0 0 0 0>\n").arg(220+dx);
    s += QString("<R R%1 1 %2 340 15 -26 0 1 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(Nr+2).arg(220+dx).arg(stage.R2,0,'f',3);
    s += QString("<R R%1 1 %2 190 -75 -52 1 0 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(Nr+1).arg(100+dx).arg(stage.R1,0,'f',3);
    s += QString("<R R%1 1 %2 260 -26 15 1 2 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(Nr+3).arg(310+dx).arg(stage.R3,0,'f',3);
    s += QString("<C C%1 1 %2 240 26 -45 1 1 \"%3%4\" 1 \"\" 0 \"neutral\" 0>\n").arg(Nc+1).arg(170+dx).arg(C1,0,'f',3).arg(suf);
}

void Filter::createFirstOrderWires(QString &s, int dx, int y)
{
    s += QString("<%1 190 %2 190 \"\" 0 0 0 \"\">\n").arg(dx-20).arg(70+dx);
    s += QString("<%1 190 %2 %3 \"\" 0 0 0 \"\">\n").arg(dx-20).arg(dx-20).arg(y);
    s += QString("<%1 %2 %3 %4 \"\" 0 0 0 \"\">\n").arg(dx-20).arg(y).arg(dx-50).arg(y);

    s += QString("<%1 190 %2 190 \"\" 0 0 0 \"\">\n").arg(130+dx).arg(170+dx);
    s += QString("<%1 160 %2 160 \"out\" %3 130 39 \"\">\n").arg(310+dx).arg(360+dx).arg(380+dx);
    s += QString("<%1 260 %2 260 \"\" 0 0 0 \"\">\n").arg(340+dx).arg(360+dx);
    s += QString("<%1 160 %2 260 \"\" 0 0 0 \"\">\n").arg(360+dx).arg(360+dx);
    s += QString("<%1 190 %2 210 \"\" 0 0 0 \"\">\n").arg(170+dx).arg(170+dx);
    s += QString("<%1 140 %2 190 \"\" 0 0 0 \"\">\n").arg(170+dx).arg(170+dx);
    s += QString("<%1 140 %2 140 \"\" 0 0 0 \"\">\n").arg(170+dx).arg(240+dx);
    s += QString("<%1 180 %2 260 \"\" 0 0 0 \"\">\n").arg(220+dx).arg(220+dx);
    s += QString("<%1 180 %2 180 \"\" 0 0 0 \"\">\n").arg(220+dx).arg(240+dx);
    s += QString("<%1 260 %2 260 \"\" 0 0 0 \"\">\n").arg(220+dx).arg(280+dx);
    s += QString("<%1 260 %2 310 \"\" 0 0 0 \"\">\n").arg(220+dx).arg(220+dx);
}


double Filter::autoscaleCapacitor(double C, QString &suffix)
{
    double  C1 = C*1e-6;

    if (C1>=1e-7) {
        suffix = "uF";
        C1 *= 1e6;
    }

    if ((C1<1e-7)&&(C1>=1e-8)) {
        suffix = "nF";
        C1 *= 1e9;
    }

    if (C1<1e-8) {
        suffix = "pF";
        C1 *= 1e12;
    }
    return C1;
}

bool Filter::checkRCL()
{
    for (auto &sec : Sections) {
        if (std::isnan(sec.R1)||
                std::isnan(sec.R2)||
                std::isnan(sec.R3)||
                std::isnan(sec.R4)||
                std::isnan(sec.C1)||
                std::isnan(sec.C2)) {
            return false;
        }
    }
    return true;
}

bool Filter::calcChebyshev()
{
    double  kf = std::max(Fs/Fc,Fc/Fs);
    double  eps=sqrt(pow(10,0.1*Rp)-1);

    double  N1 = acosh(sqrt((pow(10,0.1*As)-1)/(eps*eps)))/acosh(kf);
    int N = ceil(N1);

    if (N>MaxOrder) {
        Poles.clear();
        Zeros.clear();
        return false;
    }

    if (ftype==Filter::BandStop) {
        if (N%2!=0) N++; // only even order Cauer.
    }

    double  a = sinh((asinh(1/eps))/N);
    double  b = cosh((asinh(1/eps))/N);

    Poles.clear();
    Zeros.clear();

    for (int k=1;k<=N;k++) {
            double  re = -1*a*sin(pi*(2*k-1)/(2*N));
            double  im = b*cos(pi*(2*k-1)/(2*N));
            std::complex<float> pol(re,im);
            Poles.append(pol);
    }

    order = Poles.count();
    return true;
}

bool Filter::calcButterworth()
{
    double  kf = std::min(Fc/Fs,Fs/Fc);

    double  C1=(pow(10,(0.1*Ap))-1)/(pow(10,(0.1*As))-1);
    double  J2=log10(C1)/(2*log10(kf));
    int N2 = round(J2+1);

    if (ftype==Filter::BandStop) {
        if (N2%2!=0) N2++; // only even order Cauer.
    }

    Poles.clear();
    Zeros.clear();

    if (N2>MaxOrder) return false;

    for (int k=1;k<=N2;k++) {
        double  re =-1*sin(pi*(2*k-1)/(2*N2));
        double  im =cos(pi*(2*k-1)/(2*N2));
        std::complex<float> pol(re,im);
        Poles.append(pol);
    }

    order = Poles.count();

    return true;
}

bool Filter::calcInvChebyshev() // Chebyshev Type-II filter
{
    Poles.clear();
    Zeros.clear();

    double  kf = std::max(Fs/Fc,Fc/Fs);

    order = ceil(acosh(sqrt(pow(10.0,0.1*As)-1.0))/acosh(kf));

    if ((ftype==Filter::BandPass)||(ftype==Filter::BandStop)) {
        if (order%2!=0) order++; // only even order Cauer.
    }

    if (order>MaxOrder) return false;

    double  eps = 1.0/(sqrt(pow(10.0,0.1*As)-1.0));
    double  a = sinh((asinh(1.0/eps))/(order));
    double  b = cosh((asinh(1.0/eps))/(order));

    for (int k=1;k<=order;k++) {
        double  im = 1.0/(cos(((2*k-1)*pi)/(2*order)));
        Zeros.append(std::complex<float>(0,im));
    }

    for (int k=1;k<=order;k++) {
        double  re = -1*a*sin(pi*(2*k-1)/(2*order));
        double  im = b*cos(pi*(2*k-1)/(2*order));
        std::complex<float> invpol(re,im); // inverse pole
        std::complex<float> pol;
        pol = std::complex<float>(1.0,0) / invpol; // pole
        Poles.append(pol);
    }

    return true;
}

void Filter::cauerOrderEstim() // from Digital Filter Design Handbook page 102
{
    double  k = std::min(Fc/Fs,Fs/Fc);
    double  kk = sqrt(sqrt(1.0-k*k));
    double  u = 0.5*(1.0-kk)/(1.0+kk);
    double  q = 150.0*pow(u,13) + 2.0*pow(u,9) + 2.0*pow(u,5) + u;
    double  dd = (pow(10.0,As/10.0)-1.0)/(pow(10.0,Rp/10.0)-1.0);
    order = ceil(log10(16.0*dd)/log10(1.0/q));

    if ((ftype==Filter::BandPass)||(ftype==Filter::BandStop)) {
        if (order%2!=0) order++; // only even order Cauer.
    }
}

bool Filter::calcCauer() // from Digital Filter Designer's handbook p.103
{
    double  P0;
    //float H0;
    double  mu;
    double  aa[50],bb[50],cc[50];

    cauerOrderEstim();
    if (order>MaxOrder) {
        Poles.clear();
        Zeros.clear();
        return false;
    }

    double  k = Fc/Fs;
    double  kk = sqrt(sqrt(1.0-k*k));
    double  u = 0.5*(1.0-kk)/(1.0+kk);
    double  q = 150.0*pow(u,13) + 2.0*pow(u,9) + 2.0*pow(u,5) + u;
    double  numer = pow(10.0,Rp/20.0)+1.0;
    double  vv = log(numer/(pow(10.0,Rp/20.0)-1.0))/(2.0*order);
    double  sum = 0.0;
    for (int m=0;m<5;m++) {
        double  term = pow(-1.0,m);
        term = term*pow(q,m*(m+1));
        term = term*sinh((2*m+1)*vv);
        sum = sum +term;
    }
    numer = 2.0*sum*sqrt(sqrt(q));

    sum=0.0;
    for (int m=1;m<5;m++) {
        double  term = pow(-1.0,m);
        term = term*pow(q,m*m);
        term = term*cosh(2.0*m*vv);
        sum += term;
    }
    double  denom = 1.0+2.0*sum;
    P0 = fabs(numer/denom);
    double  ww = 1.0+k*P0*P0;
    ww = sqrt(ww*(1.0+P0*P0/k));
    int r = (order-(order%2))/2;
    //float numSecs = r;

    for (int i=1;i<=r;i++) {
        if ((order%2)!=0) {
            mu = i;
        } else {
            mu = i-0.5;
        }
        sum = 0.0;
        for(int m=0;m<5;m++) {
            double  term = pow(-1.0,m)*pow(q,m*(m+1));
            term = term*sin((2*m+1)*pi*mu/order);
            sum += term;
        }
        numer = 2.0*sum*sqrt(sqrt(q));

        sum = 0.0;
        for(int m=1;m<5;m++) {
            double  term = pow(-1.0,m)*pow(q,m*m);
            term = term*cos(2.0*m*pi*mu/order);
            sum += term;
        }
        denom = 1.0+2.0*sum;
        double  xx = numer/denom;
        double  yy = 1.0 - k*xx*xx;
        yy = sqrt(yy*(1.0-(xx*xx/k)));
        aa[i-1] = 1.0/(xx*xx);
        denom = 1.0 + pow(P0*xx,2);
        bb[i-1] = 2.0*P0*yy/denom;
        denom = pow(denom,2);
        numer = pow(P0*yy,2)+pow(xx*ww,2);
        cc[i-1] = numer/denom;
    }

    if (order%2!=0) {
        cc[order-1] = P0; // first order section
        bb[order-1] = 0;
    }

    Zeros.clear();
    Poles.clear();
    for (int i=0;i<r;i++) {
        double  im = sqrt(aa[i]);
        Zeros.append(std::complex<float>(0,im));
        double  re = -0.5*bb[i];
        im = 0.5*sqrt(-1.0*bb[i]*bb[i]+4*cc[i]);
        Poles.append(std::complex<float>(re,im));
    }

    if (order%2!=0) {
        Poles.append(std::complex<float>(-cc[order-1],0.0));
    }

    for (int i=r-1;i>=0;i--) {
        double  im = sqrt(aa[i]);
        Zeros.append(std::complex<float>(0,-im));
        double  re = -0.5*bb[i];
        im = 0.5*sqrt(-1.0*bb[i]*bb[i]+4*cc[i]);
        Poles.append(std::complex<float>(re,-im));
    }

    return true;
}

bool Filter::calcBessel()
{
    Poles.clear();
    Zeros.clear();

    if (order<=0) return false;

    for (int i=0;i<order;i++) {
        Poles.append(std::complex<float>(BesselPoles[order-1][2*i],BesselPoles[order-1][2*i+1]));
    }

    reformPolesZeros();
    return true;
}

bool Filter::calcUserTrFunc()
{
    if ((!vec_A.isEmpty())&&(!vec_B.isEmpty())) {
        long double *a = vec_A.data();
        long double *b = vec_B.data();

        int a_order = vec_A.count() - 1;
        int b_order = vec_B.count() - 1;

        order = std::max(a_order,b_order);
        if (order>MaxOrder) return false;

        qf_poly Numenator(b_order,b);
        qf_poly Denomenator(a_order,a);

        Numenator.to_roots();
        Denomenator.to_roots();

        Numenator.disp_c();
        Denomenator.disp_c();

        Numenator.roots_to_complex(Zeros);
        Denomenator.roots_to_complex(Poles);

        reformPolesZeros();
        return true;
    }

    return false;

}

void Filter::reformPolesZeros()
{
    int Np = Poles.count();
    int Nz = Zeros.count();

    for (int i=0;i<Np/2;i++) {
        if ((i%2)!=0) {
            std::complex<float> tmp;
            tmp = Poles[i];
            Poles[i]=Poles[Np-1-i];
            Poles[Np-1-i]=tmp;
        }
    }

    for (int i=0;i<Nz/2;i++) {
        if ((i%2)!=0) {
            std::complex<float> tmp;
            tmp = Poles[i];
            Poles[i]=Poles[Nz-1-i];
            Poles[Nz-1-i]=tmp;
        }
    }
}

void Filter::set_TrFunc(QVector<long double> a, QVector<long double> b)
{
    vec_A = a;
    vec_B = b;
}

bool Filter::calcLegendre()
{
    Poles.clear();
    Zeros.clear();

    if (order<=0) return false;

    for (int i=0;i<order;i++) {
        Poles.append(std::complex<float>(LegendrePoles[order-1][2*i],LegendrePoles[order-1][2*i+1]));
    }

    reformPolesZeros();
    return true;
}


