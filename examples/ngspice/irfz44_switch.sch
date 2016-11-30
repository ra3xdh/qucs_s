<Qucs Schematic 0.0.19>
<Properties>
  <View=0,-137,1087,721,1,0,120>
  <Grid=10,10,1>
  <DataSet=irfz44_switch.dat>
  <DataDisplay=irfz44_switch.dpl>
  <OpenDisplay=1>
  <Script=irfz44_test.m>
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
  <GND * 1 100 310 0 0 0 0>
  <Vrect V2 1 100 280 -96 -26 1 1 "12 V" 1 "0.2 ms" 1 "0.2 ms" 1 "1 ns" 0 "1 ns" 0 "0 ns" 0>
  <R R2 1 150 310 15 -26 0 1 "47k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <GND * 1 200 260 0 0 0 0>
  <GND * 1 150 340 0 0 0 0>
  <.TR TR1 1 250 280 0 71 0 0 "lin" 1 "0" 1 "1 ms" 1 "1000" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <Lib T1 1 200 230 8 -26 0 0 "MOSFETs" 1 "IRFZ44N" 1>
  <R R1 1 230 30 -26 15 0 0 "50 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <L L1 1 290 30 -26 -50 1 0 "1mH" 1 "" 0>
  <C C1 1 240 -60 -26 -57 1 0 "1000 pF" 1 "" 0 "neutral" 0>
  <Vdc V1 1 360 60 18 -26 0 1 "12V" 1>
  <GND * 1 360 90 0 0 0 0>
  <IProbe Pr1 1 200 130 -41 -26 0 3>
</Components>
<Wires>
  <100 230 100 250 "" 0 0 0 "">
  <100 230 150 230 "gate" 113 160 37 "">
  <150 230 170 230 "" 0 0 0 "">
  <150 230 150 280 "" 0 0 0 "">
  <200 30 200 100 "" 0 0 0 "">
  <200 -60 200 30 "" 0 0 0 "">
  <200 -60 210 -60 "" 0 0 0 "">
  <320 30 360 30 "" 0 0 0 "">
  <270 -60 360 -60 "" 0 0 0 "">
  <360 -60 360 30 "" 0 0 0 "">
  <200 160 200 200 "" 0 0 0 "">
  <200 200 200 200 "drain" 270 160 0 "">
</Wires>
<Diagrams>
  <Rect 525 184 500 257 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 "" "" "">
	<"ngspice/tran.v(drain)" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(gate)" #ff0000 0 3 0 0 0>
  </Rect>
  <Rect 532 623 487 348 3 #c0c0c0 1 00 1 -1 0.2 1 1 -1 0.5 1 1 -1 0.5 1 315 0 225 "" "" "">
	<"xyce/tran.V(DRAIN)" #0000ff 0 3 0 0 0>
	<"xyce/tran.V(GATE)" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 40 470 12 #000000 0 "This example shows how realistic switching circuits \ncould be simulated with Ngspice and Xyce. This \napproach can be used for simulation of power electronics \nin time domain. Also this illustrates usage of library \ncomponents with Ngspice and Xyce.\n\nNOTE: This Example could be run only with Ngspice/Xyce">
</Paintings>
