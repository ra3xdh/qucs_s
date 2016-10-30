/*
      RFind inductance cm model.                    

     12 Oct 2016    Mike Brinson
     
    This is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2, or (at your option)
    any later version.
*/
                                                
#include <math.h>

void cm_RFind (ARGS)
{
    Complex_t      ac_gain;
    static double  PLIND, PIL1, PIL2, PTC1, PTC2, PTNOMC, PLIC, PRS, PCP, PRP;
    static double  iind, iind_dc, iind_old, vind, vind_old, icp, icp_old, irp;
    static double  tnomk, tempk, lvalue, iind_dc_old;
           double  delta, p1;    


    if(INIT) {        
        PLIND   = PARAM(L);
        PIL1    = PARAM(il1);
        PIL2    = PARAM(il2);
        PTC1    = PARAM(tc1);    
        PTC2    = PARAM(tc2); 
        PTNOMC  = PARAM(tnom);
        PLIC    = PARAM(lic);
        PRS     = PARAM(Rs);
        PCP     = PARAM(Cp); 
        PRP     = PARAM(Rp);

        tnomk = PTNOMC + 273.15;
        tempk = TEMPERATURE + 273.15;
        iind_dc_old      = PLIC;
        icp       = 0.0;
        icp_old   = 0.0;
    }




    if(ANALYSIS == DC) {
        lvalue = PLIND*(1+PIL1*iind_dc_old+PIL2*iind_dc_old*iind_dc_old)*
                 (1+PTC1*(tempk-tnomk)+PTC2*(tempk-tnomk)*(tempk-tnomk)); 
        vind = INPUT(nind);
        iind_dc = vind/(PRS+1e-7) + vind/(PRP+1e-7);                 
        OUTPUT(nind) = iind_dc;
        PARTIAL(nind, nind) = 1/(PRS+1e-7) + 1/(PRP+1e-7);

        iind_dc_old = iind_dc;
                  


   }
        
   if(ANALYSIS == AC) {
        p1 =(PRS*PRS+RAD_FREQ*RAD_FREQ*lvalue*lvalue);
        ac_gain.real = PRS/p1+1/PRP;
        ac_gain.imag= RAD_FREQ*PCP - (lvalue * RAD_FREQ)/p1;
        AC_GAIN(nind,nind) = ac_gain;
   }

   if(ANALYSIS == TRANSIENT) {
         vind = INPUT(nind);           
         delta = TIME - T(1); 
         p1 = lvalue/delta;           
         iind = (iind_old*p1 + vind)/(PRS+p1);
         icp = icp_old+PCP*(vind - vind_old)/delta;
         irp = vind/PRP;
         OUTPUT(nind) = iind + icp + irp;
         PARTIAL(nind, nind) = 1/(PRS+p1) + PCP/delta + 1/PRP;
         vind_old = vind;
         iind_old = iind;
         icp_old  = icp;
          
   }

}

