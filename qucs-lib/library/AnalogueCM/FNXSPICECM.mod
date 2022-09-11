/*
     FNXSPICECM cm model.                    

    14 September 2017  Mike Brinson

        
    This is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2, or (at your option)
    any later version.
*/


#include <math.h>
#include "xspice_mathfunc.h"

void cm_FNXSPICECM(ARGS)
{
	Complex_t ac_gain00;
	static double Jf,Tox,Ef,Jr,Er,M,L,Scale,Shrink,W,Xl,Scalem,Xw;
	static double Xweff,Xleff,Weff,Leff,Areaeff,Eox;
	static double V1,V1_old;
	double Q0, cQ0;
	double delta_t;

	if(INIT) {
		Jf = PARAM(jf);
		Tox = PARAM(tox);
		Ef = PARAM(ef);
		Jr = PARAM(jr);
		Er = PARAM(er);
		M = PARAM(m);
		L = PARAM(l);
		Scale = PARAM(scale);
		Shrink = PARAM(shrink);
		W = PARAM(w);
		Xl = PARAM(xl);
		Scalem = PARAM(scalem);
		Xw = PARAM(xw);
		Xweff=Xw*Scalem;
		Xleff=Xl*Scalem;
		Weff= Scale*Shrink*W+Xweff;
		Leff= Shrink*L*Scale+Xleff;
		Areaeff=M*Weff*Leff;
		Eox=3.45306e-11;
	}
	if (ANALYSIS != AC) {
	if (TIME == 0) {
		V1_old = V1 = INPUT(nPAnode_nPCathode);
		Q0=0.0;
		cQ0=0.0;
	} else {
		V1 = INPUT(nPAnode_nPCathode);
		delta_t=TIME-T(1);
		Q0 = (Eox/Tox*Areaeff)*(V1-V1_old)/(delta_t+1e-20);
		cQ0 = (Eox/Tox*Areaeff)/(delta_t+1e-20);
		V1_old = V1;
	}
		OUTPUT(nPAnode_nPCathode) = (V1>=0.0)?exp(-Tox*Ef/V1)/(Tox*Tox)*Areaeff*Jf*(V1*V1):-exp(Er/V1*Tox)*Areaeff*Jr*(V1*V1)/(Tox*Tox) + Q0;
		PARTIAL(nPAnode_nPCathode,nPAnode_nPCathode) = (V1>=0.0)?2.0*V1/(Tox*Tox)*Areaeff*Jf*exp(-1.0/V1*Tox*Ef)+1.0/Tox*Areaeff*Jf*exp(-1.0/V1*Tox*Ef)*Ef:
                                                                Er*exp(Er/V1*Tox)/Tox*Areaeff*Jr+-2.0*exp(Er/V1*Tox)*V1/(Tox*Tox)*Areaeff*Jr + cQ0;
	} else {
		ac_gain00.real = (V1>=0.0)?2.0*V1/(Tox*Tox)*Areaeff*Jf*exp(-1.0/V1*Tox*Ef)+1.0/Tox*Areaeff*Jf*exp(-1.0/V1*Tox*Ef)*Ef:
                                          Er*exp(Er/V1*Tox)/Tox*Areaeff*Jr+-2.0*exp(Er/V1*Tox)*V1/(Tox*Tox)*Areaeff*Jr;
		ac_gain00.imag = (Eox/Tox*Areaeff)*RAD_FREQ;
		AC_GAIN(nPAnode_nPCathode,nPAnode_nPCathode) = ac_gain00;
	}
}
