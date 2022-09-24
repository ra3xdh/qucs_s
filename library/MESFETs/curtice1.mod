/*
      curtice cm model.                    

     10 March 2016    Mike Brinson

    This file contains the XSPICE ifs specification for an experimental Curtice 
    Level 1 MESFET model. This model is  used as a test bench for constructing 
    compact device models with the Qucs-S automatic XSPICE CodeModel compiler 
    system.
     
    This is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2, or (at your option)
    any later version.
*/

#define  Xpow(x,y)  (exp(y*log(x+1e-20)))
#define  Xsech(y)   (1.0/(cosh(y)+1e-20))
#include <math.h>                                                 

void cm_curtice1(ARGS)
{
    Complex_t  ac_gain2, ac_gain3;
    static double  PBETA, PALPHA, PLAMBDA, PVTO, PRF, PVBI, PAREA;
    static double gdisi, gm, PIS, GMIN;
    static double Vndisi, Vngisi, Idisi, Igisi, ggisi; 
    double x,x1,x2, factor;


          
             
     if(INIT) {
       
        PBETA   = PARAM(beta);
        PLAMBDA = PARAM(lambda);
        PALPHA  = PARAM(alpha);
        PVTO    = PARAM(vto);
        PRF     = PARAM(rf);
        PVBI    = PARAM(vbi);
        PIS     = PARAM(is);
        PAREA   = PARAM(area);
 
        Idisi   = 0.0;
        gdisi   = 1e-20; 

        GMIN   = 1e-9;


     }
                                  

   if (ANALYSIS != AC) {

             Vndisi = INPUT(ndisi);       
             Vngisi = INPUT(ngisi);        
             
             if ( (Vngisi-PVTO) > 0.0) {
                 x      = Vndisi*PALPHA;
                 x1     = 1.0+Vndisi*PLAMBDA;
                 x2     = Vngisi-PVTO;
                 factor = PBETA*Xpow(x2, 2)*x1;
                 Idisi  = factor*tanh(x);
                 gdisi  = factor*PLAMBDA*tanh(x) +PBETA*PALPHA*x1*Xsech(x)*Xsech(x);
                 gm     = 2*PBETA*x2*x1*tanh(x);

             }
             else {
                 Idisi = 0.0;
                 gdisi = 1e-20;
                 gm = 1e-20;
                
             }

            if ( Vngisi > PVBI) {
                 Igisi  = Vngisi/(PRF+1e-10);
                 ggisi  = PRF;
            }
            else {
                 Igisi = -PAREA*PIS+GMIN*Vngisi;
                 ggisi =  GMIN;
            }

             OUTPUT(ndisi)         = Idisi;
             PARTIAL(ndisi, ndisi) = gdisi; 
             PARTIAL(ndisi, ngisi) = gm;
             OUTPUT(ngisi)         = Igisi;
             PARTIAL(ngisi, ngisi) = ggisi; 
   
   }

  else {
         
       ac_gain2.real = gdisi;
       ac_gain2.imag = 0.0;
       AC_GAIN(ndisi, ndisi) = ac_gain2;
      

       ac_gain3.real = gm;
       ac_gain3.imag = 0.0;
       AC_GAIN(ndisi, ngisi) = ac_gain3; 

    
  } 




}

