/***************************************************************************
                                schcauer.cpp
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

#include "schcauer.h"

SchCauer::SchCauer(Filter::FilterFunc ffunc_, Filter::FType type_, FilterParam par) :
    Filter(ffunc_, type_, par)
{
    Nr1 = 5;

   if ((ftype==Filter::BandPass)||(ftype==Filter::BandStop)) {
       Nr1 = 7;
   }

    Nop1 = 3;
    Nc1 = 2;
}

void SchCauer::calcLowPass()
{
    double  R1,R2,R3,R4,R5,C1,C2;
    double  Wc = 2*pi*Fc;
    double  Nst = order/2 + order%2;
    double  Kv1 = pow(Kv,1.0/Nst);
    //qDebug()<<Kv1;

    for (int k=1; k <= order/2; k++) {

        double  re = Poles.at(k-1).real();
        double  im = Poles.at(k-1).imag();
        double  B = -2.0*re;
        double  C = re*re + im*im;
        im = Zeros.at(k-1).imag();
        double  A = im*im;

        C1 = 10.0/Fc;
        C2 = C1;
        R5 = 1.0/(Wc*C1);
        R1 = (B*R5)/(Kv1*C);
        R2 = R5/B;
        R3 = (B*R5)/C;
        R4 = (Kv1*C*R5)/A;

        RC_elements curr_sec;
        curr_sec.N = k;
        curr_sec.R1 = 1000*R1;
        curr_sec.R2  = 1000*R2;
        curr_sec.R3 = 1000*R3;
        curr_sec.R4 = 1000*R4;
        curr_sec.R5 = 1000*R5;
        curr_sec.R6 = 0;
        curr_sec.C1 = C1;
        curr_sec.C2 = C2;
        Sections.append(curr_sec);

    }

    this->calcFirstOrder();
}


void SchCauer::calcHighPass()
{
    double  R1,R2,R3,R4,R5,C1,C2;
    double  Wc = 2*pi*Fc;
    double  Nst = order/2 + order%2;
    double  Kv1 = pow(Kv,1.0/Nst);
    //qDebug()<<Kv1;

    for (int k=1; k <= order/2; k++) {

        double  re = Poles.at(k-1).real();
        double  im = Poles.at(k-1).imag();
        double  B = -2.0*re;
        double  C = re*re + im*im;
        im = Zeros.at(k-1).imag();
        double  A = im*im;

        C1 = 10.0/Fc;
        C2 = C1;
        R5 = 1.0/(Wc*C1);
        R1 = (B*A*R5)/(Kv1*C);
        R2 = (C*R5)/B;
        R3 = B*R5;
        R4 = Kv1*R5;

        RC_elements curr_sec;
        curr_sec.N = k;
        curr_sec.R1 = 1000*R1;
        curr_sec.R2  = 1000*R2;
        curr_sec.R3 = 1000*R3;
        curr_sec.R4 = 1000*R4;
        curr_sec.R5 = 1000*R5;
        curr_sec.R6 = 0;
        curr_sec.C1 = C1;
        curr_sec.C2 = C2;
        Sections.append(curr_sec);

    }

    this->calcFirstOrder();
}

void SchCauer::calcBandPass()
{
    double  R1,R2,R3,R4,R5,R6,C1,C2;
    double  W0 = 2*pi*F0;
    double  Kv1 = pow(Kv,1.0/order);
    int cnt = 1;

    for (int k=1; k <= order/2; k++) {

        double  re = Poles.at(k-1).real();
        double  im = Poles.at(k-1).imag();
        double  B = -2.0*re;
        double  C = re*re + im*im;
        im = Zeros.at(k-1).imag();
        double  A = im*im;
        double  mu = 2.0;

        double  A1 = 1+(A+sqrt(A*A+4*A*Q*Q))/(2*Q*Q);
        double  H = C + 4.0*Q*Q;
        double  E = (1.0/B)*sqrt(0.5*(H+sqrt(H*H-(4.0*B*B*Q*Q))));
        double  F = (B*E)/Q;
        double  D = 0.5*(F+sqrt(F*F-4.0));

        C1 = 10.0/F0;
        C2 = C1;

        R1 = ((mu*D)/(Kv1*A1*E*W0*C1))*sqrt(A/C);
        R2 = E/(D*W0*C2);
        R3 = mu/(D*E*W0*C1);
        R5 = R3;
        R4 = (Kv1/mu)*sqrt(C/A)*R5;
        R6 = mu*R2/(mu-1);

        RC_elements current_section;
        current_section.N = cnt;
        current_section.R1 = 1000*R1;
        current_section.R2 = 1000*R2;
        current_section.R3 = 1000*R3;
        current_section.R4 = 1000*R4;
        current_section.R5 = 1000*R5;
        current_section.R6 = 1000*R6;
        current_section.C1 = C1;
        current_section.C2 = C1;
        Sections.append(current_section);

        cnt++;

        R1 = ((mu*A1)/(Kv1*D*E*W0*C1))*sqrt(A/C);
        R2 = (D*E)/(W0*C2);
        R3 = (mu*D)/(E*W0*C1);
        R5 = R3;
        R4 = (Kv1/mu)*sqrt(C/A)*R5;
        R6 = mu*R2/(mu-1);

        current_section.N = cnt;
        current_section.R1 = 1000*R1;
        current_section.R2 = 1000*R2;
        current_section.R3 = 1000*R3;
        current_section.R4 = 1000*R4;
        current_section.R5 = 1000*R5;
        current_section.R6 = 1000*R6;
        current_section.C1 = C1;
        current_section.C2 = C1;
        Sections.append(current_section);

        cnt++;
    }
}

void SchCauer::calcBandStop()
{
    double  R1,R2,R3,R4,R5,R6,C1,C2;
    double  W0 = 2*pi*F0;
    double  Kv1 = pow(Kv,1.0/order);
    int cnt = 1;
    double  A=0,A2;

    for (int k=1; k <= order/2; k++) {

        double  re = Poles.at(k-1).real();
        double  im = Poles.at(k-1).imag();
        double  B = -2.0*re;
        double  C = re*re + im*im;
        if ((ffunc==Filter::Cauer)||
            (ffunc==Filter::InvChebyshev)) {
            im = Zeros.at(k-1).imag();
            A = im*im;
            A2 = 1.0+(1.0/(2.0*A*Q*Q)*(1+sqrt(1.0+4.0*A*Q*Q)));
        } else {
            A2 = 1.0;
        }
        double  mu = 2.0;

        double  H = 1.0+4.0*C*Q*Q;
        double  E1 = (1.0/B)*sqrt(0.5*C*(H+sqrt(H*H-4.0*B*B*Q*Q)));
        double  G = (B*E1)/(Q*C);
        double  D1 = 0.5*(G+sqrt(G*G-4));

        double  alpha = A2;
        double  beta = D1/E1;
        double  gamma = D1*D1;

        C1 = 10.0/F0;
        C2 = C1;
        R1 = (mu*beta)/(Kv1*alpha*W0*C1);
        R2 = 1.0/(beta*W0*C2);
        R3 = (Kv1*alpha*R1)/gamma;
        R5 = 1.0/(W0*C1);
        R4 = Kv1*R5/mu;
        R6 = mu*R2/(mu-1.0);

        RC_elements current_section;
        current_section.N = cnt;
        current_section.R1 = 1000*R1;
        current_section.R2 = 1000*R2;
        current_section.R3 = 1000*R3;
        current_section.R4 = 1000*R4;
        current_section.R5 = 1000*R5;
        current_section.R6 = 1000*R6;
        current_section.C1 = C1;
        current_section.C2 = C1;
        Sections.append(current_section);

        cnt++;

        alpha = 1.0/A2;
        beta = 1.0/(D1*E1);
        gamma = 1.0/(D1*D1);

        C1 = 10.0/F0;
        C2 = C1;
        R1 = (mu*beta)/(Kv1*alpha*W0*C1);
        R2 = 1.0/(beta*W0*C2);
        R3 = (Kv1*alpha*R1)/gamma;
        R5 = 1.0/(W0*C1);
        R4 = Kv1*R5/mu;
        R6 = mu*R2/(mu-1.0);

        current_section.N = cnt;
        current_section.R1 = 1000*R1;
        current_section.R2 = 1000*R2;
        current_section.R3 = 1000*R3;
        current_section.R4 = 1000*R4;
        current_section.R5 = 1000*R5;
        current_section.R6 = 1000*R6;
        current_section.C1 = C1;
        current_section.C2 = C1;
        Sections.append(current_section);

        cnt++;


    }
}

void SchCauer::createLowPassSchematic(QString &s)
{
    createGenericSchematic(s);
}

void SchCauer::createHighPassSchematic(QString &s)
{
    createGenericSchematic(s);
}


void SchCauer::createBandPassSchematic(QString &s)
{
    createGenericSchematic(s);
}

void SchCauer::createBandStopSchematic(QString &s)
{
    createGenericSchematic(s);
}

void SchCauer::createGenericSchematic(QString &s)
{
    RC_elements stage;
    int dx = 0;
    int N2ord, N1stOrd;
    double  Fac;

    if ((ftype==Filter::BandPass)||(ftype==Filter::BandStop)) {
        N2ord = Sections.count();
        N1stOrd = 0;
        Fac = (Fu+1000)/1000.0;
    } else {
        N2ord = order/2; // number of 2-nd order stages
        N1stOrd = order%2; // number of 1-st order stages
        Fac = (10.0*Fc)/1000.0;
    }

    s += "<Qucs Schematic ";
    s += PACKAGE_VERSION;
    s += ">\n";
    s += "<Components>\n";
    s += "<Vac V1 1 80 290 18 -26 0 1 \"1 V\" 0 \"1 kHz\" 0 \"0\" 0 \"0\" 0>\n";
    s += "<.DC DC1 1 40 510 0 61 0 0 \"26.85\" 0 \"0.001\" 0 \"1 pA\" 0 \"1 uV\" 0 \"no\" 0 \"150\" 0 \"no\" 0 \"none\" 0 \"CroutLU\" 0>\n";
    s += "<Eqn Eqn1 1 640 520 -30 14 0 0 \"K=dB(out.v/in.v)\" 1 \"yes\" 0>\n";
    s += QStringLiteral("<.AC AC1 1 320 510 0 61 0 0 \"lin\" 1 \"1 Hz\" 1 \"%1 kHz\" 1 \"1001\" 1 \"no\" 0>\n").arg(Fac);
    s += "<GND * 1 80 320 0 0 0 0>\n";

    for (int i=1; i<=N2ord; i++) {
        stage = Sections.at(i-1);
        QString suffix1, suffix2;
        double  C1 = autoscaleCapacitor(stage.C1,suffix1);
        double  C2 = autoscaleCapacitor(stage.C2,suffix2);
        s += QStringLiteral("<OpAmp OP%1 1 %2 240 -26 -70 1 0 \"1e6\" 1 \"15 V\" 0>\n").arg(1+(i-1)*Nop1).arg(270+dx);
        s += QStringLiteral("<OpAmp OP%1 1 %2 400 -26 -70 1 0 \"1e6\" 1 \"15 V\" 0>\n").arg(2+(i-1)*Nop1).arg(300+dx);
        s += QStringLiteral("<OpAmp OP%1 1 %2 260 -26 42 0 0 \"1e6\" 1 \"15 V\" 0>\n").arg(3+(i-1)*Nop1).arg(560+dx);
        s += QStringLiteral("<C C%1 1 %2 150 -26 17 0 0 \"%3%4\" 1 \"\" 0 \"neutral\" 0>\n").arg(1+(i-1)*Nc1).arg(330+dx).arg(C1,0,'f',3).arg(suffix1);
        s += QStringLiteral("<C C%1 1 %2 400 -26 17 0 0 \"%3%4\" 1 \"\" 0 \"neutral\" 0>\n").arg(2+(i-1)*Nc1).arg(450+dx).arg(C2,0,'f',3).arg(suffix2);
        s += QStringLiteral("<R R%1 1 %2 220 -26 15 0 0 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(1+(i-1)*Nr1).arg(180+dx).arg(stage.R1,0,'f',3);
        s += QStringLiteral("<R R%1 1 %2 240 -26 15 0 0 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(2+(i-1)*Nr1).arg(410+dx).arg(stage.R2,0,'f',3);
        s += QStringLiteral("<R R%1 1 %2 110 -26 15 0 0 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(3+(i-1)*Nr1).arg(440+dx).arg(stage.R3,0,'f',3);
        s += QStringLiteral("<R R%1 1 %2 320 -26 15 0 0 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(4+(i-1)*Nr1).arg(360+dx).arg(stage.R4,0,'f',3);
        s += QStringLiteral("<R R%1 1 %2 380 -26 15 0 0 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(5+(i-1)*Nr1).arg(190+dx).arg(stage.R5,0,'f',3);
        s += QStringLiteral("<GND * 1 %1 290 0 0 0 0>\n").arg(240+dx);
        s += QStringLiteral("<GND * 1 %1 440 0 0 0 0>\n").arg(250+dx);

        if ((ftype==Filter::BandPass)||(ftype==Filter::BandStop)) {
            s += QStringLiteral("<GND * 1 %1 470 0 0 0 0>\n").arg(500+dx);
            s += QStringLiteral("<R R%1 1 %2 370 -26 15 0 0 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(6+(i-1)*Nr1).arg(570+dx).arg(stage.R6,0,'f',3);
            s += QStringLiteral("<R R%1 1 %2 440 15 -26 0 1 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(7+(i+1)*Nr1).arg(500+dx).arg(stage.R6,0,'f',3);
        }

        dx += 580;
    }


    if ((ftype==Filter::LowPass)||(ftype==Filter::HighPass)) {
        if (N1stOrd!=0) {
            if (ftype==Filter::LowPass) {
                createFirstOrderComponentsLPF(s,Sections.last(),dx+80);
            } else if (ftype==Filter::HighPass) {
                createFirstOrderComponentsHPF(s,Sections.last(),dx+80);
            }

        }
    }


    s += "</Components>\n";
    s += "<Wires>\n";
    dx = 0;
    s += "<80 220 140 220 \"in\" 120 170 0 \"\">\n";
    s += "<80 220 80 260 \"\" 0 0 0 \"\">\n";

    for (int i=1; i<=N2ord; i++) {
        if (i!=1) {
            s += QStringLiteral("<%1 260 %2 260 \"\" 0 0 0 \"\">\n").arg(dx+30).arg(140+dx);
        }

        s += QStringLiteral("<%1 220 %2 220 \"\" 0 0 0 \"\">\n").arg(210+dx).arg(220+dx);
        s += QStringLiteral("<%1 280 %2 370 \"\" 0 0 0 \"\">\n").arg(500+dx).arg(500+dx);
        s += QStringLiteral("<%1 280 %2 280 \"\" 0 0 0 \"\">\n").arg(500+dx).arg(530+dx);
        s += QStringLiteral("<%1 260 %2 260 \"\" 0 0 0 \"\">\n").arg(600+dx).arg(610+dx);
        s += QStringLiteral("<%1 260 %2 370 \"\" 0 0 0 \"\">\n").arg(610+dx).arg(610+dx);
        if ((ftype==Filter::BandPass)||(ftype==Filter::BandStop)) {
            s += QStringLiteral("<%1 370 %2 410 \"\" 0 0 0 \"\">\n").arg(500+dx).arg(500+dx);
            s += QStringLiteral("<%1 370 %2 370 \"\" 0 0 0 \"\">\n").arg(600+dx).arg(610+dx);
            s += QStringLiteral("<%1 370 %2 370 \"\" 0 0 0 \"\">\n").arg(500+dx).arg(540+dx);
        } else {
            s += QStringLiteral("<%1 370 %2 370 \"\" 0 0 0 \"\">\n").arg(500+dx).arg(610+dx);
        }
        s += QStringLiteral("<%1 240 %2 240 \"\" 0 0 0 \"\">\n").arg(440+dx).arg(480+dx);
        s += QStringLiteral("<%1 400 %2 400 \"\" 0 0 0 \"\">\n").arg(340+dx).arg(420+dx);
        s += QStringLiteral("<%1 380 %2 380 \"\" 0 0 0 \"\">\n").arg(220+dx).arg(260+dx);
        s += QStringLiteral("<%1 380 %2 380 \"\" 0 0 0 \"\">\n").arg(260+dx).arg(270+dx);
        s += QStringLiteral("<%1 150 %2 220 \"\" 0 0 0 \"\">\n").arg(220+dx).arg(220+dx);
        s += QStringLiteral("<%1 150 %2 150 \"\" 0 0 0 \"\">\n").arg(220+dx).arg(300+dx);
        s += QStringLiteral("<%1 240 %2 240 \"\" 0 0 0 \"\">\n").arg(310+dx).arg(380+dx);
        s += QStringLiteral("<%1 260 %2 290 \"\" 0 0 0 \"\">\n").arg(240+dx).arg(240+dx);
        s += QStringLiteral("<%1 220 %2 220 \"\" 0 0 0 \"\">\n").arg(220+dx).arg(240+dx);
        s += QStringLiteral("<%1 420 %2 420 \"\" 0 0 0 \"\">\n").arg(250+dx).arg(270+dx);
        s += QStringLiteral("<%1 420 %2 440 \"\" 0 0 0 \"\">\n").arg(250+dx).arg(250+dx);

        int n;
        if ((ftype==Filter::BandPass)||(ftype==Filter::BandStop)) {
            n = i;
        } else {
            n = 2*i;
        }
        if (n==order) {
            s += QStringLiteral("<%1 110 %2 260 \"out\" %3 160 101 \"\">\n").arg(610+dx).arg(610+dx).arg(540+dx);
        } else {
            s += QStringLiteral("<%1 110 %2 260 \"\" 0 0 0 \"\">\n").arg(610+dx).arg(610+dx);
        }
        s += QStringLiteral("<%1 110 %2 110 \"\" 0 0 0 \"\">\n").arg(470+dx).arg(610+dx);
        s += QStringLiteral("<%1 110 %2 150 \"\" 0 0 0 \"\">\n").arg(220+dx).arg(220+dx);
        s += QStringLiteral("<%1 110 %2 110 \"\" 0 0 0 \"\">\n").arg(220+dx).arg(410+dx);
        s += QStringLiteral("<%1 220 %2 220 \"\" 0 0 0 \"\">\n").arg(140+dx).arg(150+dx);
        s += QStringLiteral("<%1 220 %2 260 \"\" 0 0 0 \"\">\n").arg(140+dx).arg(140+dx);
        s += QStringLiteral("<%1 260 %2 380 \"\" 0 0 0 \"\">\n").arg(140+dx).arg(140+dx);
        s += QStringLiteral("<%1 380 %2 380 \"\" 0 0 0 \"\">\n").arg(140+dx).arg(160+dx);
        s += QStringLiteral("<%1 240 %2 240 \"\" 0 0 0 \"\">\n").arg(480+dx).arg(530+dx);
        s += QStringLiteral("<%1 240 %2 400 \"\" 0 0 0 \"\">\n").arg(480+dx).arg(480+dx);
        s += QStringLiteral("<%1 320 %2 400 \"\" 0 0 0 \"\">\n").arg(420+dx).arg(420+dx);
        s += QStringLiteral("<%1 320 %2 320 \"\" 0 0 0 \"\">\n").arg(390+dx).arg(420+dx);
        s += QStringLiteral("<%1 320 %2 380 \"\" 0 0 0 \"\">\n").arg(260+dx).arg(260+dx);
        s += QStringLiteral("<%1 320 %2 320 \"\" 0 0 0 \"\">\n").arg(260+dx).arg(330+dx);
        s += QStringLiteral("<%1 150 %2 150 \"\" 0 0 0 \"\">\n").arg(360+dx).arg(380+dx);
        s += QStringLiteral("<%1 150 %2 240 \"\" 0 0 0 \"\">\n").arg(380+dx).arg(380+dx);

        dx +=580;
    }

    if ((ftype==Filter::LowPass)||(ftype==Filter::HighPass)) {
        if (N1stOrd!=0) {
                createFirstOrderWires(s,dx+80,260);
        }
    }

    s += "</Wires>\n";
}
