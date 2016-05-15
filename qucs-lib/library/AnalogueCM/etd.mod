/*
     etd cm model.                    

    2 April 2016    Mike Brinson

        
    This is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2, or (at your option)
    any later version.
*/


#include <math.h>                                                

void cm_etd(ARGS)
{
    Complex_t  ac_gain1;
    static double  PVP, PIP, PVV, PIV, PA2;
    static double PIS, T2, con1, con2, con3, VT;
    double ith, ix, it, dith, dix, ditu; 
    static double  vtd, itd, diff; 
    double P_K, P_Q; 
             
     if (INIT) {
        PVP    = PARAM(vp); 
        PIP    = PARAM(ip);
        PVV    = PARAM(vv);
        PIV    = PARAM(iv);
        PA2    = PARAM(a2);
        PIS    = PARAM(is);
 
/*      Constants                     */  
            
        P_K    = 1.3806503e-23 ;     /* Boltzmann's constant in J/K */
        P_Q    = 1.602176462e-19;    /* Charge of an electron in C   */
        T2     = PARAM(temp)+237.15;
        VT     = P_K*T2/P_Q;         /* Thermal voltage at Temp in volts */
        con1   = PIV*PA2;
        con2   = PIS/VT;
        con3   = PIP/PVP;



     }

   if (ANALYSIS != AC) { 
         vtd  = INPUT(ntd);
         ith  = PIS*(exp( vtd/VT) -1.0);
         ix   = PIV*exp(PA2*(vtd-PVV));
         it   = PIP*(vtd/PVP)*exp(1-vtd/PVP);
         itd  = ith+ix+it;
         dith = con2*exp(vtd/VT);
         dix  = con1*exp(PA2*(vtd-PVV));
         ditu = con3*(1-vtd/PVP)*exp(1-vtd/PVP);
         diff = dith+dix+ditu;
         
         OUTPUT(ntd) = itd;
         PARTIAL(ntd, ntd) = diff; 

   }

  else {
       ac_gain1.real = diff;
       ac_gain1.imag = 0.0;
       AC_GAIN(ntd, ntd) = ac_gain1;
      
     }
              
}
