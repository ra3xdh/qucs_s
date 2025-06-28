<Qucs Schematic 25.1.2>
<Properties>
  <View=113,-77,1150,532,1.5156,0,0>
  <Grid=10,10,1>
  <DataSet=core_test_BH.dat>
  <DataDisplay=core_test_BH.dpl>
  <OpenDisplay=0>
  <Script=core_test_BH.m>
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
  <GND * 1 170 230 0 0 0 0>
  <GND * 1 460 210 0 0 0 0>
  <.TR TR1 1 180 350 0 70 0 0 "lin" 1 "0" 1 "1500u" 1 "500" 0 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <R R1 1 460 180 15 -26 0 1 "1e6" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <IProbe Pr1 1 200 130 -26 16 0 0>
  <Iac I1 1 170 200 20 -26 0 1 "1500" 1 "1 kHz" 1 "0" 0 "0" 0 "0" 0 "0" 0>
  <CORE CORE1 1 320 130 -40 35 0 0 "100" 1 "100" 1 "0.05" 1 "5e5" 1 "1e-4" 1 "1.0" 1 "1.0" 1 "0.0" 1 "generic" 1 "1.0" 0 "1.0" 0 "1.0" 0 "1.0" 0 "1.0" 0 "1.0" 0 "true" 0>
</Components>
<Wires>
  <170 130 170 170 "" 0 0 0 "">
  <460 130 460 150 "" 0 0 0 "">
  <360 130 460 130 "out" 400 160 33 "">
  <230 130 280 130 "" 0 0 0 "">
</Wires>
<Diagrams>
  <Rect 651 421 455 458 3 #c0c0c0 1 00 1 -10000 1000 10000 1 -2.0233 0.2 2.02331 1 -1 0.1 1 315 0 225 1 0 0 "" "" "">
	<"ngspice/tran.v(out)@tran.i(pr1)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 140 -50 10 #000000 0 "This tesbench schematic could be used\n for Jiles-Atheton magnetic core parameters extraction\n\n1. The magnitude of I1 source equals filed strength H in core in A/m\n2. The v(out) voltage is magnetic flux density B, in T\n3. The MS parameter is related to sturation flux density Bm as MS=Bm*1e6\n4. A,K,C parameters shoudl be tuned to fit the BH-curve and hysteresis loop \n    to match the datasheet data">
</Paintings>
