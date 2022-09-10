* BSIMSOI4.0 example modelcard
.Model N1 NMOS Level= 10 

+MOBMOD  = 2             SHMOD    =  1           CAPMOD   = 2
+SOIMOD  = 0		 IGBMOD   =  1		 IGCMOD   = 1
+RDSMOD  = 0

+TNOM    = 25            TOX      = 3.e-9     	 TSI      = 41e-9
+TBOX	 = 100e-9	 TOXM 	  = 3.e-9	 VTH0	  = 0.29
+NCH 	 = 4e17		 NSUB	  = 1e16
+RBODY   = 0		 RBSH	 = 0

+LINT    = 0             LLN      = 1            LW       = 0
+LWN     = 0             LWL      = 0       
+WINT    = 0             WLN      = 1            WW       = 0
+WWN     = 1             WWL      = 0            

+CDSC    = 1.3           CDSCB    = 0.004        CDSCD    = 0.1
+CIT     = 0.00007       NFACTOR  = .9           XJ       = 5E-08          
+VSAT    = 86000         AT       = 28000             
+A0      = 1.8           AGS      = -0.175       A1       = 0
+A2      = 0.99          KETA     = 0.25         KETAS    = 0.23
+NGATE    = 0
+K1      = .59           KT1      = -.28         KT1L     = 8e-9
+KT2     = -.0646        K2       = 0            K3       = -3.5
+K3B     = 0             W0       = 0
+LPE0     = 6.3E-8       DVT0     = 200          DVT1     = 3.72
+DVT2    = 0.19          DVT0W    = 0            DVT1W    = 0
+DVT2W   = 0   		 DROUT    = 3.617        DSUB     = 1.12

+UA      = 8E-11         UA1      = 3.37e-10     UB       = 2.2e-18
+UB1     = -3.12e-18     UC       = -5e-10       UC1      = -6.1e-10
+U0      = 268           UTE      = -1.6

+VOFF    = -.13          DELTA    = 0.01	 PRT	  = 10
+RDSW    = 175           PRWG     = 0            PRWB     = -0.0176
+ETA0    = 0.31          ETAB     = -.1605       PCLM     = 1.8804
+PDIBLC1 = 20            PDIBLC2  = 0            PDIBLCB  = -0.05
+PVAG    = -0.07
+WR      = 1             DWG      = 0            DWB      = 0 
+B0      = 0             B1       = 10

+NDIODE  = 1.00          NRECF0  = 2.20         NRECR0  = 7.7
+NDIODED = 1.00          NRECF0D  = 2.20         NRECR0D  = 7.7
+NTUN   = 25            ISDIF    = 2e-6         ISREC    = 13
+NTUND   = 25            IDDIF    = 2e-6         IDREC    = 13
+ISTUN   = 1e-5          VREC0   = 2.8          VTUN0   = 1      
+IDTUN   = 1e-5          VREC0D   = 2.8          VTUN0D   = 1      
+IDBJT   = 1e-5		 AHLID    = 1.5e-10
+ISBJT   = 4e-3          LBJT0    = 8e-8         VABJT    = -0.4
+AELY    = 5.5e7         AHLI    = 1.5e-10      NBJT     = .6
+FBJTII  = 0.06          LN       = .00001    
+NTRECF  = 0.60          NTRECR   = 1.2          XDIF     = 1.15
+XBJT    = 1.08          XREC     = 0.95         XTUN     = 0
+ALPHA0  = 1E-08         BETA0    = 0            BETA1    = .028   
+BETA2   = .067          TII      = -0.7         SII0     = 3.4  
+SII1    = .8            SII2     = .08          SIID     = 0.08
+ESATII  = 1.7e6         VDSATII0 = 0.55         LII      = 3e-9
+AGIDL   = 0     

+RTH0    = .01           CTH0     = 1.46e-5

+CLC     = 1e-7          CLE      = 0.6          XPART    = 0.0
+DWC     = -0.0217e-6    DLC      = 0.0120e-6    DELVT    = -0.131
+CGSO    = 1.44e-10      CGDO     = 1.44e-10     CGSL     = 1.634e-10
+CF      = 0             CGDL     = 1.634e-10    CKAPPA   = 2
+CJSWG  = 3.79e-10      MJSWG   = 0.675        PBSWG   = 1.004
+CJSWGD  = 3.79e-10      MJSWGD   = 0.675        PBSWGD   = 1.004
+TCJSWG  = 6.5e-4        NDIF     = -1	         TT       = 2.4e-10
+DLBG     = 0.105e-6     LDIF0    = 5e-3
+KB1     = 1             CSDMIN   = 0

+TOXQM	 = 1.045E-9	 TOXREF	  = 1.67e-9	 NTOX 	  = 1
+EBG	 = 1.6		 VEVB	  = 0.075	 ALPHAGB1 = 0.465
+BETAGB1 = 0.07		 VGB1	  = 40		 VGB2 	  = 24
+VECB	 = 0.018	 ALPHAGB2 = 0.425	 BETAGB2  = 0.055

+AIGC	 = 0.043  	 BIGC 	  = 0.0054	 CIGC 	  = 0.0075 
+AIGSD 	 = 0.043	 BIGSD	  = 0.0054	 CIGSD    = 0.0075 
+DLCIG   = 2e-8		 NIGC     = 1	   	 PIGCD    = 1.0
+POXEDGE = 1

+FNOIMOD = 1		 TNOIMOD  = 1     	 TNOIA	  = 1
+TNOIB 	 = 2.5		 RNOIA    = 0.577 	 RNOIB 	  = 0.37
+NTNOI 	 = 1.0	         EM 	  = 41000000	 AF	  = 1
+EF    	 = 1		 KF       = 1		 NOIF	  = 2.0

+SAREF   = 5E-6          SBREF    = 5E-6         WLOD     = 2E-6
+KU0     = -4E-6	 KVSAT    = 0.2          KVTH0    = 0.0
+TKU0    = 0.0           LLODKU0  = 1.1		 WLODKU0  = 1.1
+LLODVTH = 1.0  	 WLODVTH  = 1.0 	 LKU0     = 1E-6  
+WKU0	 = 1E-6  	 PKU0     = 0.0  	 LKVTH0   = 1.1E-6
+WKVTH0  = 1.1E-6 	 PKVTH0   = 0.0  	 STK2     = 0.0
+LODK2   = 1.0           STETA0   = 0.0  	 LODETA0  = 1.0		

+RGATEMOD= 0		 RSHG	  = 1.1	  	 XRCRG1	  = 1
+XRCRG2  = 1

+RBODYMOD= 0		 RBDB	  = 10		 RBSB	  = 50
