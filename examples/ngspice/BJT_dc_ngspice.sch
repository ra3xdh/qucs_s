<Qucs Schematic 0.0.24>
<Properties>
  <View=0,-10,1691,905,1,0,240>
  <Grid=10,10,1>
  <DataSet=BJT_dc_ngspice.dat>
  <DataDisplay=BJT_dc_ngspice.dpl>
  <OpenDisplay=0>
  <Script=BJT_dc_ngspice.m>
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
  <GND * 1 200 330 0 0 0 0>
  <GND * 1 100 370 0 0 0 0>
  <.DC DC1 1 90 560 0 45 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <NPN_SPICE Q1 1 200 290 -134 120 0 0 "mod2N2222A" 1 ".MODEL mod2N2222A NPN (IS=3.0611E-14 NF=1.00124 BF=220 IKF=0.52 VAF=104 ISE=7.5E-15 NE=1.41 " 1 "+ NR=1.005 BR=4 IKR=0.24 VAR=28 ISC=1.06525E-11 NC=1.3728 RB=0.13 RE=0.22" 1 "+ RC=0.12 CJC=9.12E-12 MJC=0.3508 VJC=0.4089 CJE=27.01E-12 TF=0.325E-9 TR=100E-9)" 1 "" 0>
  <Idc I1 1 100 330 -57 -26 1 1 "5u" 1>
  <.SW SW1 1 300 560 0 75 0 0 "DC1" 1 "lin" 1 "V1" 1 "0" 1 "5" 1 "250" 1 "false" 0>
  <Vdc V1 1 340 270 18 -26 0 1 "5" 1>
  <IProbe Pr1 1 270 220 -12 -58 1 2>
  <GND * 1 340 320 0 0 0 0>
  <.SW SW2 1 460 560 0 75 0 0 "SW1" 1 "lin" 1 "@mod2N2222A[BF]" 1 "20" 1 "200" 1 "5" 1 "false" 1>
</Components>
<Wires>
  <200 320 200 330 "" 0 0 0 "">
  <100 290 170 290 "" 0 0 0 "">
  <100 290 100 300 "" 0 0 0 "">
  <100 360 100 370 "" 0 0 0 "">
  <200 220 200 260 "" 0 0 0 "">
  <200 220 240 220 "" 0 0 0 "">
  <340 220 340 240 "" 0 0 0 "">
  <300 220 340 220 "" 0 0 0 "">
  <340 300 340 320 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 650 392 386 242 3 #c0c0c0 1 00 1 0 1 5 1 -0.000112223 0.0002 0.0012 1 -1 0.5 1 315 0 225 0 0 0 "" "" "">
	<"ngspice/i(pr1)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 20 10 12 #000000 0 "This example shows how to sweep model parameters with Ngspice.\nSimulate it with Ngspice and obtain Ic versus Beta of BJT.\n\nNOTE: This Example could be run only with Ngspice">
</Paintings>
