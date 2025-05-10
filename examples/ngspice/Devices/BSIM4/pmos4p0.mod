*BSIMSOI4.0 example modelcard
.Model P1 PMOS Level= 10

+MOBMOD  = 2             SHMOD    = 1           CAPMOD   = 2
+SOIMOD  = 0		 IGBMOD	  = 1 		IGCMOD	 = 1
+RDSMOD  = 0

+TNOM    = 25            TOX      = 3e-9        TOXM 	 = 3e-9
+TBOX	 = 4e-7		 TSI 	  = 41e-9	VTH0     = -0.15
+NCH	 = 1.5e18	 NSUB	  = 1e16
+RBODY   = 0	 	 RBSH	  = 0

+LINT    = 0             LLN      = 1            LW       = 0
+LWN     = 0             LWL      = 0       
+WINT    = 0             WLN      = 1            WW       = 0
+WWN     = 1             WWL      = 0            

+CDSC    = .1835         CDSCB    = 0.073        CDSCD    = .1376
+CIT     = .00000918     NFACTOR  = .8258        XJ       = 5E-08          
+VSAT    = 101362        AT       = 6264             
+A0      = 1.5           AGS      = 1.0          A1       = 0.145
+A2      = 0.7           KETA     = 0.1          KETAS    = 0.07
+NGATE    = 0
+K1      = .52           KT1      = -.18         KT1L     = 6e-9
+KT2     = -.02          K2       = 0            K3       = -1
+K3B     = 0             W0       = 0
+LPE0    = 1.75E-07      DVT0     = 130          DVT1     = 2.4325  
+DVT2    = 0.07          DVT0W    = 0            DVT1W    = 0
+DVT2W   = 0   
+DROUT   = 3.09          DSUB     = 2.004                    

+UA      = 3E-10         UA1      = 4.36E-10     UB       = 1.3067E-18
+UB1     = -1.188E-18    UC       = -6.5395E-10  UC1      = 1.537E-10   
+U0      = .0097         UTE      = -.75     
+VOFF    = -.15          DELTA    = .04   
+RDSW    = 340           PRWG     = 0            PRWB     = 0
+PRT     = 30
+ETA0    = 5             ETAB     = -.04         PCLM     = .634584     
+PDIBLC1 = 10            PDIBLC2  = .0172        PDIBLCB  = 1.12E-08          
+PVAG    = -.15          
+WR      = 1             DWG      = 0            DWB      = 0 
+B0      = 0             B1       = 0

+NDIODE = 1.10          NRECF0  = 1.6          NRECR0  = 7.5
+NDIODED = 1.10          NRECF0D  = 1.6          NRECR0D  = 7.5
+NTUN   = 2.05          ISDIF    = 1.5e-4       ISREC    = 1.8e-2
+NTUND   = 2.05          IDDIF    = 1.5e-4       IDREC    = 1.8e-2
+ISTUN   = 1.7e-5        VREC0   = 1.6          VTUN0   = 1.40
+IDTUN   = 1.7e-5        VREC0D   = 1.6          VTUN0D   = 1.40
+ISBJT   = .13           LBJT0    = 1e-8         VABJT    = -0.9
+IDBJT	 = .13		 AHLID	  = 0.7E-09
+AELY    = 2E+07         AHLI    = 0.7E-09      NBJT     = .9      
+FBJTII  = 0             LN       = .00001    
+NTRECF  = .05           NTRECR   = 0.03         XDIF     = 1.00
+XBJT    = 1.15          XREC     = 0.85         XTUN     = 0.0
+ALPHA0  = 1E-08         BETA0    = 0            BETA1    = .034   
+BETA2   = .065          TII      = -.5          SII0     = 3  
+SII1    = .7            SII2     = .08          SIID     = -.4
+ESATII  = 3200000       LII      = 0            VDSATII0 = 0.74
+AGIDL   = 0

+RTH0    = .04		 CTH0     = 1.46e-5

+CLC     = 1e-7          CLE      = 0.6          XPART    = 0
+DWC     = -0.029e-6     DLC      = 0.021e-6     DELVT    = -0.0686
+CGSO    = 0.38e-10      CGDO     = 0.38e-10     CKAPPA   = 4
+CGSL    = 2.3e-10       CGDL     = 2.3e-10      CF       = 0
+CJSWG  = 4.04e-10      MJSWG   = 0.61275      PBSWG   = 1.0363
+CJSWGD  = 4.04e-10      MJSWGD   = 0.61275      PBSWGD   = 1.0363
+CJSWG =0.0 CJSWGD = 0.0
+TCJSWG  = 1.11e-3       TT       = 2.0e-13      NDIF     = -1
+LDIF0   = 1             KB1      = 1            CSDMIN   = 1e-20

+TOXQM   = 1.045E-9      TOXREF   = 1.67e-9      NTOX     = 1
+EBG     = 1.6           VEVB     = 0.075        ALPHAGB1 = 0.465
+BETAGB1 = 0.07          VGB1     = 40           VGB2     = 24
+VECB    = 0.018         ALPHAGB2 = 0.425        BETAGB2  = 0.055

+AIGC    = 0.043         BIGC     = 0.0054       CIGC     = 0.0075
+AIGSD   = 0.043         BIGSD    = 0.0054       CIGSD    = 0.0075
+DLCIG   = 2e-8          NIGC     = 1            PIGCD    = 1.0
+POXEDGE = 1

+SAREF   = 5E-6          SBREF    = 5E-6         WLOD     = 2E-6
+KU0     = 4E-6         KVSAT    = 0.2           KVTH0    = 0.0
+TKU0    = 0.0           LLODKU0  = 1.1          WLODKU0  = 1.1
+LLODVTH = 1.0           WLODVTH  = 1.0          LKU0     = 1E-6
+WKU0    = 1E-6          PKU0     = 0.0          LKVTH0   = 1.1E-6
+WKVTH0  = 1.1E-6        PKVTH0   = 0.0          STK2     = 0.0
+LODK2   = 1.0           STETA0   = 0.0          LODETA0 = 1.0

+RGATEMOD= 0             RSHG     = 2.1          XRCRG1   = 12
+XRCRG2  = 1

+RBODYMOD= 0             RBDB     = 10          RBSB     = 50 
