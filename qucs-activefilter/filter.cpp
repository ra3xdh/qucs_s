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

Filter::Filter(Filter::FilterFunc ffunc_, Filter::FType type_, FilterParam par)
{
    ffunc = ffunc_;
    ftype = type_;

    if ((ftype==Filter::HighPass)||(ftype==Filter::LowPass)) {
        Fc = par.Fc;
        Fs = par.Fs;
        As = par.As;
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
            float Fs1 = Fu + TW;
            float Fs1lp = fabsf(Fs1 - (F0*F0)/Fs1);    // stopband freq. of LPF prototype
            float Fs2 = Fl - TW;
            float Fs2lp = fabsf(Fs2 - (F0*F0)/Fs2);
            Fs = std::min(Fs1lp,Fs2lp);
        }
        Ap = 3.0;
        As = 20.0;
        qDebug()<<Fc<<Fs;
        Q = F0/fabs(Fu-Fl);
    }

    Rp = par.Rp;
    Kv = par.Kv;
    if (ffunc==Filter::Bessel) {
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
    s = "<Qucs Schematic "PACKAGE_VERSION">\n";
}

void Filter::createBandStopSchematic(QString &s)
{
    s = "<Qucs Schematic "PACKAGE_VERSION">\n";
}

bool Filter::calcFilter()
{
    Sections.clear();
    Poles.clear();
    Zeros.clear();

    switch (ffunc) {
    case Filter::Chebyshev : calcChebyshev();
        break;
    case Filter::Butterworth : calcButterworth();
        break;
    case Filter::Cauer : calcCauer();
        break;
    case Filter::InvChebyshev : calcInvChebyshev();
        break;
    case Filter::Bessel : calcBessel();
        break;
    case Filter::User : calcUserTrFunc();
        break;
    default :
        return false;
        break;
    }

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

    bool res = checkRCL();

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

        float R1, R2,R3;

        int k = order/2 + 1;
        float Wc = 2*M_PI*Fc;
        float re = Poles.at(k-1).real();
        //float im = Poles.at(k-1).imag();
        //float C = re*re + im*im;
        float C = -re;
        float C1 = 10/Fc;

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

void Filter::createPartList(QStringList &lst)
{
    lst<<QObject::tr("Part list");
    if ((ffunc==Filter::InvChebyshev)||(ffunc==Filter::Cauer)) {
        lst<<"Stage# C1(uF) R1(kOhm) R2(kOhm) R3(kOhm) R4(kOhm)  R5(kOhm)";
    } else {
        lst<<"Stage# C1(uF) C2(uF) R1(kOhm) R2(kOhm) R3(kOhm) R4(kOhm)";
    }

    RC_elements stage;

    foreach (stage,Sections) {
        QString suff1,suff2;
        float C1=autoscaleCapacitor(stage.C1,suff1);
        float C2=autoscaleCapacitor(stage.C2,suff2);
        if ((ffunc==Filter::InvChebyshev)||(ffunc==Filter::Cauer)) {
            lst<<QString("%1%2%3%4%5%6%7%8").arg(stage.N,6).arg(C1,10,'f',3).arg(suff1)
                 .arg(stage.R1,10,'f',3).arg(stage.R2,10,'f',3).arg(stage.R3,10,'f',3).arg(stage.R4,10,'f',3).arg(stage.R5,10,'f',3);
        } else {
            lst<<QString("%1%2%3%4%5%6%7%8%9").arg(stage.N,6).arg(C1,10,'f',3).arg(suff1).arg(C2,10,'f',3).arg(suff2)
                 .arg(stage.R1,10,'f',3).arg(stage.R2,10,'f',3).arg(stage.R3,10,'f',3).arg(stage.R4,10,'f',3);
        }

    }
}

void Filter::createPolesZerosList(QStringList &lst)
{
    lst<<QString(QObject::tr("Filter order = %1")).arg(order);
    if (!Zeros.isEmpty()) {
        lst<<""<<QObject::tr("Zeros list Pk=Re+j*Im");
        std::complex<float> zero;
        foreach(zero,Zeros) {
                lst<<QString::number(zero.real()) + " + j*" + QString::number(zero.imag());
        }
    }
    lst<<""<<QObject::tr("Poles list Pk=Re+j*Im");
    std::complex<float> pole;
    foreach(pole,Poles) {
            lst<<QString::number(pole.real()) + " + j*" + QString::number(pole.imag());
    }
    lst<<"";
}

void Filter::createFirstOrderComponentsHPF(QString &s,RC_elements stage,int dx)
{
    QString suf;
    float C1 = autoscaleCapacitor(stage.C1,suf);
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
    float C1 = autoscaleCapacitor(stage.C1,suf);
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


float Filter::autoscaleCapacitor(float C, QString &suffix)
{
    float C1 = C*1e-6;

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
    RC_elements sec;
    foreach (sec,Sections) {
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

void Filter::calcChebyshev()
{
    float kf = std::max(Fs/Fc,Fc/Fs);
    float eps=sqrt(pow(10,0.1*Rp)-1);

    float N1 = acosh(sqrt((pow(10,0.1*As)-1)/(eps*eps)))/acosh(kf);
    int N = ceil(N1);

    if (ftype==Filter::BandStop) {
        if (N%2!=0) N++; // only even order Cauer.
    }

    float a = sinh((asinh(1/eps))/N);
    float b = cosh((asinh(1/eps))/N);

    Poles.clear();
    Zeros.clear();

    for (int k=1;k<=N;k++) {
            float re = -1*a*sin(M_PI*(2*k-1)/(2*N));
            float im = b*cos(M_PI*(2*k-1)/(2*N));
            std::complex<float> pol(re,im);
            Poles.append(pol);
    }

    order = Poles.count();
}

void Filter::calcButterworth()
{
    float kf = std::min(Fc/Fs,Fs/Fc);

    float C1=(pow(10,(0.1*Ap))-1)/(pow(10,(0.1*As))-1);
    float J2=log10(C1)/(2*log10(kf));
    int N2 = round(J2+1);

    if (ftype==Filter::BandStop) {
        if (N2%2!=0) N2++; // only even order Cauer.
    }

    Poles.clear();
    Zeros.clear();

    for (int k=1;k<=N2;k++) {
        float re =-1*sin(M_PI*(2*k-1)/(2*N2));
        float im =cos(M_PI*(2*k-1)/(2*N2));
        std::complex<float> pol(re,im);
        Poles.append(pol);
    }

    order = Poles.count();
}

void Filter::calcInvChebyshev() // Chebyshev Type-II filter
{
    Poles.clear();
    Zeros.clear();

    float kf = std::max(Fs/Fc,Fc/Fs);

    order = ceil(acosh(sqrt(pow(10.0,0.1*As)-1.0))/acosh(kf));

    if ((ftype==Filter::BandPass)||(ftype==Filter::BandStop)) {
        if (order%2!=0) order++; // only even order Cauer.
    }

    float eps = 1.0/(sqrt(pow(10.0,0.1*As)-1.0));
    float a = sinh((asinh(1.0/eps))/(order));
    float b = cosh((asinh(1.0/eps))/(order));

    for (int k=1;k<=order;k++) {
        float im = 1.0/(cos(((2*k-1)*M_PI)/(2*order)));
        Zeros.append(std::complex<float>(0,im));
    }

    for (int k=1;k<=order;k++) {
        float re = -1*a*sin(M_PI*(2*k-1)/(2*order));
        float im = b*cos(M_PI*(2*k-1)/(2*order));
        std::complex<float> invpol(re,im); // inverse pole
        std::complex<float> pol;
        pol = std::complex<float>(1.0,0) / invpol; // pole
        Poles.append(pol);
    }

}

void Filter::cauerOrderEstim() // from Digital Filter Design Handbook page 102
{
    float k = std::min(Fc/Fs,Fs/Fc);
    float kk = sqrt(sqrt(1.0-k*k));
    float u = 0.5*(1.0-kk)/(1.0+kk);
    float q = 150.0*pow(u,13) + 2.0*pow(u,9) + 2.0*pow(u,5) + u;
    float dd = (pow(10.0,As/10.0)-1.0)/(pow(10.0,Rp/10.0)-1.0);
    order = ceil(log10(16.0*dd)/log10(1.0/q));

    if ((ftype==Filter::BandPass)||(ftype==Filter::BandStop)) {
        if (order%2!=0) order++; // only even order Cauer.
    }
}

void Filter::calcCauer() // from Digital Filter Designer's handbook p.103
{
    float P0;
    //float H0;
    float mu;
    float aa[50],bb[50],cc[50];

    cauerOrderEstim();
    float k = Fc/Fs;
    float kk = sqrt(sqrt(1.0-k*k));
    float u = 0.5*(1.0-kk)/(1.0+kk);
    float q = 150.0*pow(u,13) + 2.0*pow(u,9) + 2.0*pow(u,5) + u;
    float numer = pow(10.0,Rp/20.0)+1.0;
    float vv = log(numer/(pow(10.0,Rp/20.0)-1.0))/(2.0*order);
    float sum = 0.0;
    for (int m=0;m<5;m++) {
        float term = pow(-1.0,m);
        term = term*pow(q,m*(m+1));
        term = term*sinh((2*m+1)*vv);
        sum = sum +term;
    }
    numer = 2.0*sum*sqrt(sqrt(q));

    sum=0.0;
    for (int m=1;m<5;m++) {
        float term = pow(-1.0,m);
        term = term*pow(q,m*m);
        term = term*cosh(2.0*m*vv);
        sum += term;
    }
    float denom = 1.0+2.0*sum;
    P0 = fabs(numer/denom);
    float ww = 1.0+k*P0*P0;
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
            float term = pow(-1.0,m)*pow(q,m*(m+1));
            term = term*sin((2*m+1)*M_PI*mu/order);
            sum += term;
        }
        numer = 2.0*sum*sqrt(sqrt(q));

        sum = 0.0;
        for(int m=1;m<5;m++) {
            float term = pow(-1.0,m)*pow(q,m*m);
            term = term*cos(2.0*m*M_PI*mu/order);
            sum += term;
        }
        denom = 1.0+2.0*sum;
        float xx = numer/denom;
        float yy = 1.0 - k*xx*xx;
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
        float im = sqrt(aa[i]);
        Zeros.append(std::complex<float>(0,im));
        float re = -0.5*bb[i];
        im = 0.5*sqrt(-1.0*bb[i]*bb[i]+4*cc[i]);
        Poles.append(std::complex<float>(re,im));
    }

    if (order%2!=0) {
        Poles.append(std::complex<float>(-cc[order-1],0.0));
    }

    for (int i=r-1;i>=0;i--) {
        float im = sqrt(aa[i]);
        Zeros.append(std::complex<float>(0,-im));
        float re = -0.5*bb[i];
        im = 0.5*sqrt(-1.0*bb[i]*bb[i]+4*cc[i]);
        Poles.append(std::complex<float>(re,-im));
    }
}

void Filter::calcBessel()
{
    Poles.clear();
    Zeros.clear();

    if (order<=0) return;

    for (int i=0;i<order;i++) {
        Poles.append(std::complex<float>(BesselPoles[order-1][2*i],BesselPoles[order-1][2*i+1]));
    }

    reformPolesZeros();
}

void Filter::calcUserTrFunc()
{
    if ((!vec_A.isEmpty())&&(!vec_B.isEmpty())) {
        long double *a = vec_A.data();
        long double *b = vec_B.data();

        int a_order = vec_A.count() - 1;
        int b_order = vec_B.count() - 1;

        order = std::max(a_order,b_order);

        qf_poly Numenator(b_order,b);
        qf_poly Denomenator(a_order,a);

        Numenator.to_roots();
        Denomenator.to_roots();

        Numenator.disp_c();
        Denomenator.disp_c();

        Numenator.roots_to_complex(Zeros);
        Denomenator.roots_to_complex(Poles);

        reformPolesZeros();
    }

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
