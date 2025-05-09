<Qucs Schematic 0.0.24>
<Properties>
  <View=0,-126,1403,869,1,0,0>
  <Grid=10,10,1>
  <DataSet=diode_dblswp_qucs.dat>
  <DataDisplay=diode_dblswp_qucs.dpl>
  <OpenDisplay=0>
  <Script=diode_dblswp_qucs.m>
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
  <Vdc V1 1 60 170 -49 -21 0 1 "2" 1>
  <GND * 1 60 220 0 0 0 0>
  <IProbe Pr1 1 110 120 -12 -53 0 0>
  <DIODE_SPICE D1 1 340 170 20 -18 1 3 "1N4007" 1 "" 0 "" 0 "" 0 "" 0>
  <GND * 1 340 220 0 0 0 0>
  <.DC DC1 1 30 400 0 44 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <.SW SW1 1 200 400 0 73 0 0 "DC1" 1 "lin" 1 "V1" 1 "0" 1 "2" 1 "200" 1 "false" 0>
  <R R1 1 230 120 -16 13 0 0 "0.1" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <.SW SW2 1 340 400 0 73 0 0 "SW1" 1 "lin" 1 "R1" 1 "0.01" 1 "0.5" 1 "10" 1 "false" 0>
  <SpiceModel SpiceModel1 1 60 320 -27 16 0 0 ".model 1N4007 D(IS=7.02767n RS=0.0341512 N=1.80803 EG=1.05743 XTI=5 BV=1000 IBV=5e-08 CJO=1e-11 VJ=0.7 M=0.5 FC=0.5 TT=1e-07)" 1 "" 0 "" 0 "" 0 "Line_5=" 0>
</Components>
<Wires>
  <60 200 60 220 "" 0 0 0 "">
  <60 120 60 140 "" 0 0 0 "">
  <60 120 80 120 "" 0 0 0 "">
  <340 200 340 220 "" 0 0 0 "">
  <340 120 340 140 "" 0 0 0 "">
  <260 120 340 120 "" 0 0 0 "">
  <140 120 200 120 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 577 270 354 294 3 #c0c0c0 1 00 1 0 0.5 2 1 -1.07296 2 12 1 -1 0.5 1 315 0 225 "" "" "">
	<"ngspice/i(pr1)" #0000ff 0 3 0 0 0>
  </Rect>
  <Rect3D 1014 296 366 327 3 #c0c0c0 1 00 1 0 0.2 2 1 0 0.02 0.109 1 -0.548307 1 6.33137 315 0 225 "" "" "">
	<"ngspice/i(pr1)" #ff0000 0 3 0 0 1>
  </Rect3D>
</Diagrams>
<Paintings>
</Paintings>
