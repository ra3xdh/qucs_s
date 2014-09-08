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
    Nop1 = 3;
    Nc1 = 2;
}

void SchCauer::calcLowPass()
{
    float R1,R2,R3,R4,R5,C1,C2;
    float Wc = 2*M_PI*Fc;
    float Nst = order/2 + order%2;
    float Kv1 = pow(Kv,1.0/Nst);
    //qDebug()<<Kv1;

    for (int k=1; k <= order/2; k++) {

        float re = Poles.at(k-1).real();
        float im = Poles.at(k-1).imag();
        float B = -2.0*re;
        float C = re*re + im*im;
        im = Zeros.at(k-1).imag();
        float A = im*im;

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
        curr_sec.C1 = C1;
        curr_sec.C2 = C2;
        Sections.append(curr_sec);

    }

    this->calcFirstOrder();
}


void SchCauer::calcHighPass()
{
    float R1,R2,R3,R4,R5,C1,C2;
    float Wc = 2*M_PI*Fc;
    float Nst = order/2 + order%2;
    float Kv1 = pow(Kv,1.0/Nst);
    //qDebug()<<Kv1;

    for (int k=1; k <= order/2; k++) {

        float re = Poles.at(k-1).real();
        float im = Poles.at(k-1).imag();
        float B = -2.0*re;
        float C = re*re + im*im;
        im = Zeros.at(k-1).imag();
        float A = im*im;

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
        curr_sec.C1 = C1;
        curr_sec.C2 = C2;
        Sections.append(curr_sec);

    }

    this->calcFirstOrder();
}

void SchCauer::calcBandPass()
{
    float R1,R2,R3,R4,R5,R6,R7,C1,C2;
    float W0 = 2*M_PI*F0;
    float Kv1 = pow(Kv,1.0/order);
    int cnt = 1;

    for (int k=1; k <= order/2; k++) {

        float re = Poles.at(k-1).real();
        float im = Poles.at(k-1).imag();
        float B = -2.0*re;
        float C = re*re + im*im;
        im = Zeros.at(k-1).imag();
        float A = im*im;
        float mu = 2.0;

        float A1 = 1+(A+sqrt(A*A+4*A*Q*Q))/(2*Q*Q);
        float H = C + 4.0*Q*Q;
        float E = (1.0/B)*sqrt(0.5*(H+sqrt(H*H-(4.0*B*B*Q*Q))));
        float F = (B*E)/Q;
        float D = 0.5*(F+sqrt(F*F-4.0));

        C1 = 10.0/F0;
        C2 = C1;

        R1 = ((mu*D)/(Kv1*A1*E*W0*C1))*sqrt(A/C);
        R2 = E/(D*W0*C2);
        R3 = mu/(D*E*W0*C1);
        R5 = R3;
        R4 = (Kv1/mu)*sqrt(C/A)*R5;
        R6 = mu*R2/(mu-1);
        R7 = mu*R2;

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
        R7 = mu*R2;

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

}

void SchCauer::createBandStopSchematic(QString &s)
{

}

void SchCauer::createGenericSchematic(QString &s)
{
    RC_elements stage;
    int dx = 0;
    int N2ord = order/2; // number of 2-nd order stages
    int N1stOrd = order%2; // number of 1-st order stages

    s += "<Qucs Schematic ";
    s += PACKAGE_VERSION;
    s += ">\n";
    s += "<Components>\n";
    s += "<Vac V1 1 80 290 18 -26 0 1 \"1 V\" 0 \"1 kHz\" 0 \"0\" 0 \"0\" 0>\n";
    s += "<.DC DC1 1 40 510 0 61 0 0 \"26.85\" 0 \"0.001\" 0 \"1 pA\" 0 \"1 uV\" 0 \"no\" 0 \"150\" 0 \"no\" 0 \"none\" 0 \"CroutLU\" 0>\n";
    s += "<Eqn Eqn1 1 640 520 -30 14 0 0 \"K=dB(out.v/in.v)\" 1 \"yes\" 0>\n";
    s += "<.AC AC1 1 320 510 0 61 0 0 \"lin\" 1 \"1 Hz\" 1 \"10 kHz\" 1 \"5001\" 1 \"no\" 0>\n";
    s += "<GND * 1 80 320 0 0 0 0>\n";

    for (int i=1; i<=N2ord; i++) {
        stage = Sections.at(i-1);
        QString suffix1, suffix2;
        float C1 = autoscaleCapacitor(stage.C1,suffix1);
        float C2 = autoscaleCapacitor(stage.C2,suffix2);
        s += QString("<OpAmp OP%1 1 %2 240 -26 -70 1 0 \"1e6\" 1 \"15 V\" 0>\n").arg(1+(i-1)*Nop1).arg(270+dx);
        s += QString("<OpAmp OP%1 1 %2 400 -26 -70 1 0 \"1e6\" 1 \"15 V\" 0>\n").arg(2+(i-1)*Nop1).arg(300+dx);
        s += QString("<OpAmp OP%1 1 %2 260 -26 42 0 0 \"1e6\" 1 \"15 V\" 0>\n").arg(3+(i-1)*Nop1).arg(560+dx);
        s += QString("<C C%1 1 %2 150 -26 17 0 0 \"%3%4\" 1 \"\" 0 \"neutral\" 0>\n").arg(1+(i-1)*Nc1).arg(330+dx).arg(C1,0,'f',3).arg(suffix1);
        s += QString("<C C%1 1 %2 400 -26 17 0 0 \"%3%4\" 1 \"\" 0 \"neutral\" 0>\n").arg(2+(i-1)*Nc1).arg(450+dx).arg(C2,0,'f',3).arg(suffix2);
        s += QString("<R R%1 1 %2 220 -26 15 0 0 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(1+(i-1)*Nr1).arg(180+dx).arg(stage.R1,0,'f',3);
        s += QString("<R R%1 1 %2 240 -26 15 0 0 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(2+(i-1)*Nr1).arg(410+dx).arg(stage.R2,0,'f',3);
        s += QString("<R R%1 1 %2 110 -26 15 0 0 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(3+(i-1)*Nr1).arg(440+dx).arg(stage.R3,0,'f',3);
        s += QString("<R R%1 1 %2 320 -26 15 0 0 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(4+(i-1)*Nr1).arg(360+dx).arg(stage.R4,0,'f',3);
        s += QString("<R R%1 1 %2 380 -26 15 0 0 \"%3k\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"european\" 0>\n").arg(5+(i-1)*Nr1).arg(190+dx).arg(stage.R5,0,'f',3);
        s += QString("<GND * 1 %1 290 0 0 0 0>\n").arg(240+dx);
        s += QString("<GND * 1 %1 440 0 0 0 0>\n").arg(250+dx);

        dx += 580;
    }

    if (N1stOrd!=0) {
        if (ftype==Filter::LowPass) {
            createFirstOrderComponentsLPF(s,Sections.last(),dx+80);
        } else if (ftype==Filter::HighPass) {
            createFirstOrderComponentsHPF(s,Sections.last(),dx+80);
        }

    }

    s += "</Components>\n";
    s += "<Wires>\n";
    dx = 0;
    s += "<80 220 140 220 \"in\" 120 170 0 \"\">\n";
    s += "<80 220 80 260 \"\" 0 0 0 \"\">\n";

    for (int i=1; i<=N2ord; i++) {
        if (i!=1) {
            s += QString("<%1 260 %2 260 \"\" 0 0 0 \"\">\n").arg(dx+30).arg(140+dx);
        }
        s += QString("<%1 370 %2 370 \"\" 0 0 0 \"\">\n").arg(500+dx).arg(610+dx);
        s += QString("<%1 220 %2 220 \"\" 0 0 0 \"\">\n").arg(210+dx).arg(220+dx);
        s += QString("<%1 280 %2 370 \"\" 0 0 0 \"\">\n").arg(500+dx).arg(500+dx);
        s += QString("<%1 280 %2 280 \"\" 0 0 0 \"\">\n").arg(500+dx).arg(530+dx);
        s += QString("<%1 260 %2 260 \"\" 0 0 0 \"\">\n").arg(600+dx).arg(610+dx);
        s += QString("<%1 260 %2 370 \"\" 0 0 0 \"\">\n").arg(610+dx).arg(610+dx);
        s += QString("<%1 240 %2 240 \"\" 0 0 0 \"\">\n").arg(440+dx).arg(480+dx);
        s += QString("<%1 400 %2 400 \"\" 0 0 0 \"\">\n").arg(340+dx).arg(420+dx);
        s += QString("<%1 380 %2 380 \"\" 0 0 0 \"\">\n").arg(220+dx).arg(260+dx);
        s += QString("<%1 380 %2 380 \"\" 0 0 0 \"\">\n").arg(260+dx).arg(270+dx);
        s += QString("<%1 150 %2 220 \"\" 0 0 0 \"\">\n").arg(220+dx).arg(220+dx);
        s += QString("<%1 150 %2 150 \"\" 0 0 0 \"\">\n").arg(220+dx).arg(300+dx);
        s += QString("<%1 240 %2 240 \"\" 0 0 0 \"\">\n").arg(310+dx).arg(380+dx);
        s += QString("<%1 260 %2 290 \"\" 0 0 0 \"\">\n").arg(240+dx).arg(240+dx);
        s += QString("<%1 220 %2 220 \"\" 0 0 0 \"\">\n").arg(220+dx).arg(240+dx);
        s += QString("<%1 420 %2 420 \"\" 0 0 0 \"\">\n").arg(250+dx).arg(270+dx);
        s += QString("<%1 420 %2 440 \"\" 0 0 0 \"\">\n").arg(250+dx).arg(250+dx);
        if ((2*i)==order) {
            s += QString("<%1 110 %2 260 \"out\" %3 160 101 \"\">\n").arg(610+dx).arg(610+dx).arg(540+dx);
        } else {
            s += QString("<%1 110 %2 260 \"\" 0 0 0 \"\">\n").arg(610+dx).arg(610+dx);
        }
        s += QString("<%1 110 %2 110 \"\" 0 0 0 \"\">\n").arg(470+dx).arg(610+dx);
        s += QString("<%1 110 %2 150 \"\" 0 0 0 \"\">\n").arg(220+dx).arg(220+dx);
        s += QString("<%1 110 %2 110 \"\" 0 0 0 \"\">\n").arg(220+dx).arg(410+dx);
        s += QString("<%1 220 %2 220 \"\" 0 0 0 \"\">\n").arg(140+dx).arg(150+dx);
        s += QString("<%1 220 %2 260 \"\" 0 0 0 \"\">\n").arg(140+dx).arg(140+dx);
        s += QString("<%1 260 %2 380 \"\" 0 0 0 \"\">\n").arg(140+dx).arg(140+dx);
        s += QString("<%1 380 %2 380 \"\" 0 0 0 \"\">\n").arg(140+dx).arg(160+dx);
        s += QString("<%1 240 %2 240 \"\" 0 0 0 \"\">\n").arg(480+dx).arg(530+dx);
        s += QString("<%1 240 %2 400 \"\" 0 0 0 \"\">\n").arg(480+dx).arg(480+dx);
        s += QString("<%1 320 %2 400 \"\" 0 0 0 \"\">\n").arg(420+dx).arg(420+dx);
        s += QString("<%1 320 %2 320 \"\" 0 0 0 \"\">\n").arg(390+dx).arg(420+dx);
        s += QString("<%1 320 %2 380 \"\" 0 0 0 \"\">\n").arg(260+dx).arg(260+dx);
        s += QString("<%1 320 %2 320 \"\" 0 0 0 \"\">\n").arg(260+dx).arg(330+dx);
        s += QString("<%1 150 %2 150 \"\" 0 0 0 \"\">\n").arg(360+dx).arg(380+dx);
        s += QString("<%1 150 %2 240 \"\" 0 0 0 \"\">\n").arg(380+dx).arg(380+dx);

        dx +=580;
    }

    if (N1stOrd!=0) {
            createFirstOrderWires(s,dx+80,260);
    }

    s += "</Wires>\n";
}
