<Qucs Schematic 0.0.24>
<Properties>
  <View=0,-134,3426,953,1,0,0>
  <Grid=10,10,1>
  <DataSet=BJT_dc_curves.dat>
  <DataDisplay=BJT_dc_curves.dpl>
  <OpenDisplay=0>
  <Script=BJT_dc_curves.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Title>
  <FrameText1=Drawn By:>
  <FrameText2=Date:>
  <FrameText3=Revision:>
</Properties>
<Symbol>
</Symbol>
<Components>
  <GND * 1 180 200 0 0 0 0>
  <NPN_SPICE Q1 1 180 150 -21 94 0 0 "mod2N2222A" 1 ".MODEL mod2N2222A NPN (IS=3.0611E-14 NF=1.00124 BF=220 IKF=0.52 VAF=104 ISE=7.5E-15 NE=1.41 " 1 "+ NR=1.005 BR=4 IKR=0.24 VAR=28 ISC=1.06525E-11 NC=1.3728 RB=0.13 RE=0.22" 1 "+ RC=0.12 CJC=9.12E-12 MJC=0.3508 VJC=0.4089 CJE=27.01E-12 TF=0.325E-9 TR=100E-9)" 1 "" 0>
  <GND * 1 340 200 0 0 0 0>
  <Vdc V1 1 340 150 24 -18 0 1 "1 V" 1>
  <IProbe Pr1 1 260 100 -11 16 1 2>
  <.DC DC1 1 70 400 0 43 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.SW SW1 1 80 480 0 71 0 0 "DC1" 1 "lin" 1 "V1" 1 "0" 1 "10" 1 "101" 1 "false" 0>
  <.SW SW2 1 220 480 0 71 0 0 "SW1" 1 "lin" 1 "I1" 1 "20u" 1 "200u" 1 "5" 1 "false" 0>
  <Idc I1 1 100 190 -72 -22 1 1 "1 mA" 1>
  <GND * 1 100 240 0 0 0 0>
</Components>
<Wires>
  <180 180 180 200 "" 0 0 0 "">
  <340 180 340 200 "" 0 0 0 "">
  <180 100 180 120 "" 0 0 0 "">
  <180 100 230 100 "" 0 0 0 "">
  <340 100 340 120 "" 0 0 0 "">
  <290 100 340 100 "" 0 0 0 "">
  <100 150 150 150 "" 0 0 0 "">
  <100 150 100 160 "" 0 0 0 "">
  <100 220 100 240 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 500 757 439 375 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 "" "" "">
	<"ngspice/i(pr1)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
</Paintings>
