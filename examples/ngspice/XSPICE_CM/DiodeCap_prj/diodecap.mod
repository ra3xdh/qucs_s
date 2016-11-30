/*
      diodecap cm model.                    

     7 March 2016    Mike Brinson

    This file contains the XSPICE C code for an experimental semiconductor 
    diode model (based on the standard SPICE 2g6 and 3f5 implementation). 
    This model is  used as a test bench for constructing compact 
    device models with the Qucs-0.0.19-S automatic XSPICE CodeModel compiler 
    system.
     
    This is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2, or (at your option)
    any later version.
*/
#define  Xlimexp(x) (x < 80.0) ? exp(x) : exp(80)*(1.0-(x-80.0))
#define  Xpow(x,y)  exp(y*log(x))
#include <math.h>                                                 

void cm_diodecap(ARGS)
{
    Complex_t ac_gain;
    static double  P1, P3, P4, P5, P6, PTNOM, PTEMP, PCJ0, PVJ, PIBV; 
    static double PIS, PAREA, PXTI,  PEG,  PN, PTT, PFC, PBV;
    static double Is_temp, Vt_temp, Vd0, cap, cdep;
    static double derive, F2, F3, PM, Vj_temp, Cj0_temp, BVeff;
    double Id, Vd, Tr,Eg_temp, Eg_tnom;
    double A = 7.02e-4;
    double B = 1108; 
    double exp80 = 5.5406334e34;
    double GMIN = 1e-12;

/* Diode model initialization code - this is only called once at the start
   of a simulation.
*/          
             
     if(INIT) {
        Vd0 = 0.0;
       
        PEG = PARAM(eg);
        PIS = PARAM(is);
        PN  = PARAM(n);
        PAREA = PARAM(area);     
        PXTI  = PARAM(xti);
        PTT   = PARAM(tt);
        PCJ0  = PARAM(cj0);
        PVJ   = PARAM(vj);
        PFC   = PARAM(fc);
        PM    = PARAM(m);
        PBV   = PARAM(bv);
        PIBV  = PARAM(ibv);
        
        cap = 1e-16;
        derive = 1e-20;
        Id = 0.0;  

        F2 = exp( (1+PM)*log(1-PFC) );
        F3 = 1-PFC*(1+PM); 
        P3 = -5*PN;


     }
 
/* Code common to all forms of simulation.  */
       
   PTNOM = PARAM(tnom)+273.15;
   PTEMP = TEMPERATURE+273.15;    
   Vt_temp = 8.65387195e-5*PTEMP;
   P1 = 1/(PN*Vt_temp);
   Tr = PTEMP/PTNOM;    
   Is_temp = PAREA*PIS*Xpow(PXTI/PN, Tr)*exp((-PEG*P1)*(1.0-PTEMP/PTNOM)); 
   P3 = -5*PN;
   P4 = Is_temp*exp80;
   P5 = PAREA*P1*P4; 
   P6 = PAREA*Is_temp;

   Eg_temp = PEG-A*PTEMP*PTEMP/(B+PTEMP);
   Eg_tnom = PEG-A*PTNOM*PTNOM/(B+PTNOM);
   Vj_temp = Tr*PVJ-(2*Vt_temp)*Xpow(1.5,Tr)-(Tr*Eg_tnom-Eg_temp);
   Cj0_temp = PCJ0*(1+PM*(400e-6*(PTEMP-PTNOM)-(Vj_temp-PVJ)/PVJ)); 

   if( PIBV < Is_temp) {
         PIBV = (Is_temp*PBV)/Vt_temp;
         BVeff = PBV;
   }
   else {
      BVeff = PBV -Vt_temp*log(PIBV/Is_temp);
   }

/* DC simulation code - called at each DC analysis iteration during the solution
   of non-linear circuit equations.  Note: Diffusion (cdiff) and depletion (cdep) 
   capacitor values are also estimated and stored in static double variables for 
   possible future use in AC small signal simulation.  
*/

   if (ANALYSIS == DC) {  
             Vd = INPUT(diode);

             if ( Vd >= P3*Vt_temp) { /* Forward bias model code */
                    Id = P6*(Xlimexp(P1*Vd)-1.0) + GMIN*Vd;
                    if ( P1*Vd <= 80.0) {
                        derive = P6*P1*exp(P1*Vd)+GMIN;
                    }
                    else {
                        derive = P5+GMIN;
                    }
             }


          if ( (-PBV < Vd) && (Vd < P3*Vt_temp)) {   /* Reverse bias model code */        
                     Id = -P6+GMIN*Vd;
                     derive = -Is_temp/Vd;
          }

         if (Vd < -PBV) {  /* Reverse breakdowm zener like model code */
                      
                      Id = -P6*(exp(-(BVeff+Vd)/Vt_temp)-1+BVeff/Vt_temp);
                      derive = (Is_temp/Vt_temp)*exp(-(BVeff+Vd)/Vt_temp);
         } 

         OUTPUT(diode) = Id;
         PARTIAL(diode, diode) = derive;
               
         if ( Vd < PFC*PVJ ) {  /* Capacitance cap = cdep+cdiff model code */
             cdep = Cj0_temp/exp(PM*log(1.0 - (Vd/Vj_temp)) );
         }
         else { 
               cdep = (Cj0_temp/F2)*(F3+(PM*Vd/Vj_temp)); 
         }

         cap = PTT*Id/Vt_temp + cdep;
   }

/* Transient simulation code - called at each time step during a transient analysis
   sequence.
*/

   if (ANALYSIS == TRANSIENT) {
           Vd = INPUT(diode);

             if ( Vd >= P3*Vt_temp) { /*  Forward bias model code */
                    Id = P6*(Xlimexp(P1*Vd)-1.0) + GMIN*Vd;
                    if ( P1*Vd <= 80.0) {
                        derive = P6*P1*exp(P1*Vd)+GMIN;
                    }
                    else {
                        derive = P5+GMIN;
                    }
             }


          if ( (-PBV < Vd) && (Vd < P3*Vt_temp)) {   /* Reverse bias model code */        
                     Id = -P6+GMIN*Vd;
                     derive = -Is_temp/Vd;
          }

         if (Vd < -PBV) {  /* Reverse breakdowm zener like model code */
                      
                      Id = -P6*(exp(-(BVeff+Vd)/Vt_temp)-1+BVeff/Vt_temp);
                      derive = (Is_temp/Vt_temp)*exp(-(BVeff+Vd)/Vt_temp);
         } 


          if ( Vd < PFC*PVJ ) {    /* Capacitance cap = cdep+cdiff model code */
             cdep = PCJ0/exp(PM*log(1.0 - (Vd/PVJ)) );
          }
          else { 
               cdep = (PCJ0/F2)*(F3+(PM*Vd/PVJ)); 
          }

        cap = PTT*Id/Vt_temp + cdep;


         if (Vd < -PBV) {  /* Reverse breakdowm zener like model code */
                      
                      Id = -Is_temp*(exp(-(BVeff+Vd)/Vt_temp)-1+BVeff/Vt_temp);
                      OUTPUT(diode) = Id;
                      derive = (Is_temp/Vt_temp)*exp(-(BVeff+Vd)/Vt_temp);
                      PARTIAL(diode, diode) = derive;
         } 

/*      Diode output current at terminal gd0+  = f(Vd,Id) = Id(DC) + d/dt(Qd),
        where cap = dQd/dVd = dQd/dt*dt/dVd  or dQd/dt = cap*dVd/dt = Id(cap).
*/

        if (TIME > 0.0) {
                cap = PTT*Id/Vt_temp;
                OUTPUT(diode) = Id+cap*(Vd0-Vd)/(T(0)-T(1));
        }
        else {
                OUTPUT(diode) = Id;
        }
  cm_analog_auto_partial();  /* Auto calculate derivative dId/dVd  */
  Vd0 = Vd;
   } 
 
/* AC simulation code - called at each analysis frequency.
   Non-linear components are linearized at their bias point to yield 
   the real and imaginary components of the model AC_GAIN: in this
   diode model the AC output current is given by I(f) = Y(f)*V(f),
   where f is the analysis frequency in Hz. Hence, ac_gain.real = gd
   and ac_gain.imag = j*2*pi*f*cap.
*/  
   if (ANALYSIS == AC) {
        ac_gain.real = derive;
        ac_gain.imag = RAD_FREQ*cap;
        AC_GAIN(diode, diode) = ac_gain;
        
    } 
        
}
