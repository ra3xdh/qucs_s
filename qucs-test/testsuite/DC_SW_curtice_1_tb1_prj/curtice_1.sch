<Qucs Schematic 0.0.18>
<Properties>
  <View=22,-52,861,656,1.10593,0,0>
  <Grid=10,10,1>
  <DataSet=curtice_1.dat>
  <DataDisplay=curtice_1.dpl>
  <OpenDisplay=1>
  <Script=curtice_1.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
  <.PortSym -30 0 2 0>
  <Line -30 0 20 0 #000080 2 1>
  <Line -10 -20 0 40 #000080 2 1>
  <Line 0 40 0 -30 #000080 2 1>
  <Line 0 -10 0 -30 #000080 2 1>
  <.ID 20 -46 Curtice "1=RG=1m=external gate resistance=" "1=RD=1m=external drain resistance=" "1=RS=1m=external source resistance=" "1=VBR=10e10=reverse GD resp. GS breakdown voltage=" "1=LG=1e-15=external gate inductance=" "1=LD=1e-15=external drain inductance=" "1=LS=1e-15=external source inductance=" "1=IS=10f=diode saturation current=" "1=N=1=diode emission coefficient=" "1=XTI=0=diode saturation current temperature coefficient=" "1=EG=1.11=diode energy gap=" "1=Beta=3m=transconductance parameter=" "1=Lambda=40m=channel length modulation parameter for quadr. model=" "1=VT0=-6=quadr. model gate threshold voltage=" "1=Temp=27=simulation temperature=" "1=Alpha=0.8=coefficient of Vds in tanh function for quadr. model=" "1=CDS=300f=interelectrode drain-source bias-independent capacitance=" "1=TAU=10p=internal time delay from drain to source=" "1=RIN=1m=series res. to CGS=" "1=CGS=300f=interelectrode gate-source cap.=" "1=CGD=300f=interelectrode gate-drain cap.=" "1=Tnom=27=temperature at which model parameter were extracted=">
  <.PortSym 0 -40 3 0>
  <.PortSym 0 40 1 0>
  <Line -10 -10 10 0 #000080 2 1>
  <Line -10 10 10 0 #000080 2 1>
  <Line -5 35 10 -10 #000080 2 1>
</Symbol>
<Components>
  <R Rs 1 280 410 -26 15 0 0 "RS" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <L Ls 1 200 410 -26 10 0 0 "LS" 1 "" 0>
  <Port Source 1 90 410 -23 12 0 0 "1" 1 "analog" 0>
  <R Rin 1 240 320 -26 15 0 0 "RIN" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <EDD D2 1 310 180 -57 -113 1 2 "explicit" 0 "1" 0 "0" 1 "CGD*V1" 1>
  <Port Drain 1 90 180 -23 -69 1 0 "3" 1 "analog" 0>
  <R Rd 1 220 180 -39 -85 1 0 "RD" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <L Ld 1 140 180 -26 10 0 0 "LD" 1 "" 0>
  <R Rg 1 360 130 15 -26 0 1 "RG" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <L Lg 1 360 60 10 -26 0 1 "LG" 1 "" 0>
  <Port Gate 1 360 30 -85 -42 0 3 "2" 1 "analog" 0>
  <Eqn Eqn1 1 490 20 -32 16 0 0 "Vt=kB/q*TK" 1 "GMIN=1e-12" 1 "TK=Temp+273.15" 1 "TnK=Tnom+273.15" 1 "yes" 0>
  <Eqn Eqn2 1 490 240 -32 16 0 0 "TR=TK/TnK" 1 "IsT=IS*exp(XTI/N*ln(TR) - EG/N/Vt*(1-TR))" 1 "yes" 0>
  <EDD D1 1 300 270 -238 221 0 3 "explicit" 0 "4" 0 "V1<-VBR+50*Vt ? -IsT*(1+exp(-(VBR+V1)/Vt)) + GMIN*V1 : 0" 1 "0" 1 "V1>=-VBR+50*Vt && V1<-5*Vt ? -IsT+GMIN*V1 : V1>=-5*Vt ? IsT*(exp(V1/(N*Vt))-1) + GMIN*V1 : 0" 1 "0" 1 "V1-VT0>0 ? Beta*(V1-VT0)^2*(1+Lambda*V3)*tanh(Alpha*V3) : 0" 1 "CDS*V3 + TAU*I3" 1 "0" 1 "CGS*V4" 1>
</Components>
<Wires>
  <270 300 270 320 "" 0 0 0 "">
  <390 300 390 320 "" 0 0 0 "">
  <270 320 330 320 "" 0 0 0 "">
  <330 320 390 320 "" 0 0 0 "">
  <330 300 330 320 "" 0 0 0 "">
  <330 220 330 240 "" 0 0 0 "">
  <390 220 390 240 "" 0 0 0 "">
  <330 220 360 220 "" 0 0 0 "">
  <210 300 210 320 "" 0 0 0 "">
  <210 220 330 220 "" 0 0 0 "">
  <210 220 210 240 "" 0 0 0 "">
  <330 320 330 410 "" 0 0 0 "">
  <310 410 330 410 "" 0 0 0 "">
  <230 410 250 410 "" 0 0 0 "">
  <90 410 170 410 "" 0 0 0 "">
  <360 220 390 220 "" 0 0 0 "">
  <360 180 360 220 "" 0 0 0 "">
  <340 180 360 180 "" 0 0 0 "">
  <270 180 270 240 "" 0 0 0 "">
  <270 180 280 180 "" 0 0 0 "">
  <250 180 270 180 "" 0 0 0 "">
  <170 180 190 180 "" 0 0 0 "">
  <90 180 110 180 "" 0 0 0 "">
  <360 160 360 180 "" 0 0 0 "">
  <360 90 360 100 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 700 30 13 #000000 0 "Author: Mike Brinson">
</Paintings>
