<Qucs Schematic 0.0.21>
<Properties>
  <View=-219,-42,1311,818,0.981191,59,0>
  <Grid=10,10,1>
  <DataSet=Current_Source_Generator.dat>
  <DataDisplay=Current_Source_Generator.dpl>
  <OpenDisplay=0>
  <Script=Current_Source_Generator.m>
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
  <Irect I1 1 120 140 -88 -63 0 1 "2 mA" 1 "1 ms" 1 "1 ms" 1 "1 ns" 1 "1 ns" 1 "0 ns" 1>
  <C C1 1 260 140 19 -20 0 1 "100 nF" 1 "0" 0 "neutral" 0>
  <GND *3 5 160 380 0 0 0 0>
  <R R2 1 160 310 18 -16 0 1 "10k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <Irect I3 1 80 310 -88 -63 0 1 "1 mA" 1 "1 ns" 1 "1 ns" 1 "1 ms" 1 "1 ms" 1 "0 ns" 1>
  <Idc I2 1 160 140 23 -17 0 3 "1 mA" 1>
  <.TR TR1 1 70 540 0 72 0 0 "lin" 1 "0" 1 "10 ms" 1 "101" 1 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <R RL1 1 400 140 19 -19 0 1 "10k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <GND *1 5 400 220 0 0 0 0>
</Components>
<Wires>
  <260 80 260 110 "" 0 0 0 "">
  <160 200 260 200 "" 0 0 0 "">
  <260 170 260 200 "" 0 0 0 "">
  <260 200 400 200 "" 0 0 0 "">
  <120 200 160 200 "" 0 0 0 "">
  <120 170 120 200 "" 0 0 0 "">
  <120 80 160 80 "" 0 0 0 "">
  <120 80 120 110 "" 0 0 0 "">
  <160 170 160 200 "" 0 0 0 "">
  <80 340 80 360 "" 0 0 0 "">
  <80 360 160 360 "" 0 0 0 "">
  <160 340 160 360 "" 0 0 0 "">
  <160 360 160 380 "" 0 0 0 "">
  <80 260 80 280 "" 0 0 0 "">
  <80 260 160 260 "" 0 0 0 "">
  <160 260 160 280 "" 0 0 0 "">
  <160 80 260 80 "" 0 0 0 "">
  <160 80 160 110 "" 0 0 0 "">
  <260 80 400 80 "" 0 0 0 "">
  <400 80 400 110 "" 0 0 0 "">
  <400 170 400 200 "" 0 0 0 "">
  <400 200 400 220 "" 0 0 0 "">
  <160 260 160 260 "out2" 170 230 0 "">
  <400 80 400 80 "out1" 410 50 0 "">
</Wires>
<Diagrams>
  <Rect 620 712 600 312 3 #c0c0c0 1 00 1 0 0.0005 0.004 1 -1 2 11 1 -1 0.5 1 315 0 225 "time, s" "" "">
	<"ngspice/tran.v(out2)" #ff0000 0 3 0 0 0>
  </Rect>
  <Rect 620 352 600 312 3 #c0c0c0 1 00 1 0 0.0005 0.004 1 -12 2 6 1 -1 2 11 315 0 225 "time, s" "" "">
	<"ngspice/tran.v(out1)" #0000ff 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 20 0 14 #000000 0 "Triangle Wave Generator">
  <Rectangle 20 60 340 160 #aa5500 0 2 #c0c0c0 1 0>
  <Text 70 440 14 #000000 0 "For Qucs-S:\nTriangle Wave Generator via rectangular voltage source\nTH does-not include Tr and TL does-not include Tf">
</Paintings>
