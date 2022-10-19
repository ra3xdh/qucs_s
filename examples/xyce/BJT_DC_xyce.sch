<Qucs Schematic 0.0.21>
<Properties>
  <View=-260,-80,1125,802,1,0,0>
  <Grid=10,10,1>
  <DataSet=BJT_DC_xyce.dat>
  <DataDisplay=BJT_DC_xyce.dpl>
  <OpenDisplay=0>
  <Script=BJT_DC_xyce.m>
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
  <GND * 1 270 320 0 0 0 0>
  <GND * 1 420 320 0 0 0 0>
  <GND * 1 110 370 0 0 0 0>
  <IProbe Pr1 1 340 200 -12 -56 1 2>
  <.DC DC1 1 120 560 0 43 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <NPN_SPICE Q1 1 270 290 -134 120 0 0 "mod2N2222A" 1 ".MODEL mod2N2222A NPN (IS=3.0611E-14 NF=1.00124 BF=220 IKF=0.52 VAF=104 ISE=7.5E-15 NE=1.41 " 1 "+ NR=1.005 BR=4 IKR=0.24 VAR=28 ISC=1.06525E-11 NC=1.3728 RB=0.13 RE=0.22" 1 "+ RC=0.12 CJC=9.12E-12 MJC=0.3508 VJC=0.4089 CJE=27.01E-12 TF=0.325E-9 TR=100E-9)" 1 "" 0>
  <S4Q_V V1 1 420 290 19 -24 0 1 "DC 12" 1 "" 0 "" 0 "" 0 "" 0>
  <S4Q_V V2 1 110 340 -102 -20 0 1 "DC 0.75" 1 "" 0 "" 0 "" 0 "" 0>
  <IProbe Pr2 1 180 290 -16 -60 0 0>
  <.SW SW1 1 290 560 0 71 0 0 "DC1" 1 "lin" 1 "mod2N2222A:BF" 1 "200" 1 "250" 1 "5" 1 "true" 1>
</Components>
<Wires>
  <270 200 270 260 "" 0 0 0 "">
  <270 200 310 200 "" 0 0 0 "">
  <420 200 420 260 "" 0 0 0 "">
  <370 200 420 200 "" 0 0 0 "">
  <210 290 240 290 "" 0 0 0 "">
  <110 290 150 290 "" 0 0 0 "">
  <110 290 110 310 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 670 342 386 242 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 "" "" "">
	<"xyce/I(PR1)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 30 0 12 #000000 0 "Figure 5.7 Effects of BJT BF parameter scan on DC collector current: XYCE simualtion.">
  <Text 30 30 12 #000000 0 "It shows how to sweep model parameters with  XYCE Spice. \nSimulate it with XYCE and obtain Ic versus Beta of BJT.\n\nNOTE: This Example could be run only with XYCE">
</Paintings>
