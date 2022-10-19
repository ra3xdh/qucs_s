<Qucs Schematic 0.0.21>
<Properties>
  <View=-219,-160,1311,898,0.981191,59,0>
  <Grid=10,10,1>
  <DataSet=Triangle_Wave_Generator.dat>
  <DataDisplay=Triangle_Wave_Generator.dpl>
  <OpenDisplay=0>
  <Script=Triangle_Wave_Generator.m>
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
  <GND *1 5 420 220 0 0 0 0>
  <Irect I1 1 120 140 -88 -63 0 1 "2 mA" 1 "1 ms" 1 "1 ms" 1 "1 ns" 1 "1 ns" 1 "0 ns" 1>
  <C C1 1 260 140 19 -20 0 1 "100 nF" 1 "0" 0 "neutral" 0>
  <GND *2 5 160 580 0 0 0 0>
  <R R1 1 160 510 18 -16 0 1 "1G" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <Vrect V1 1 80 510 -90 -70 0 1 "10 V" 1 "1ns" 1 "1ns" 1 "1 ms" 1 "1 ms" 1 "0 ns" 1>
  <.TR TR1 1 40 710 0 72 0 0 "lin" 1 "0" 1 "4 ms" 1 "101" 1 "Trapezoidal" 0 "2" 0 "1 ns" 0 "1e-16" 0 "150" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "26.85" 0 "1e-3" 0 "1e-6" 0 "1" 0 "CroutLU" 0 "no" 0 "yes" 0 "0" 0>
  <GND *3 5 160 380 0 0 0 0>
  <R R2 1 160 310 18 -16 0 1 "10k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
  <Irect I3 1 80 310 -88 -63 0 1 "1 mA" 1 "1 ns" 1 "1 ns" 1 "1 ms" 1 "1 ms" 1 "0 ns" 1>
  <Idc I2 1 160 140 23 -17 0 3 "1 mA" 1>
  <R RL1 1 420 140 19 -19 0 1 "10k" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "US" 0>
</Components>
<Wires>
  <260 80 260 110 "" 0 0 0 "">
  <160 200 260 200 "" 0 0 0 "">
  <260 170 260 200 "" 0 0 0 "">
  <260 80 420 80 "" 0 0 0 "">
  <420 80 420 110 "" 0 0 0 "">
  <260 200 420 200 "" 0 0 0 "">
  <420 170 420 200 "" 0 0 0 "">
  <420 200 420 220 "" 0 0 0 "">
  <120 200 160 200 "" 0 0 0 "">
  <120 170 120 200 "" 0 0 0 "">
  <120 80 160 80 "" 0 0 0 "">
  <120 80 120 110 "" 0 0 0 "">
  <160 170 160 200 "" 0 0 0 "">
  <80 540 80 560 "" 0 0 0 "">
  <80 560 160 560 "" 0 0 0 "">
  <160 540 160 560 "" 0 0 0 "">
  <160 560 160 580 "" 0 0 0 "">
  <80 460 80 480 "" 0 0 0 "">
  <80 460 160 460 "" 0 0 0 "">
  <160 460 160 480 "" 0 0 0 "">
  <80 340 80 360 "" 0 0 0 "">
  <80 360 160 360 "" 0 0 0 "">
  <160 340 160 360 "" 0 0 0 "">
  <160 360 160 380 "" 0 0 0 "">
  <80 260 80 280 "" 0 0 0 "">
  <80 260 160 260 "" 0 0 0 "">
  <160 260 160 280 "" 0 0 0 "">
  <160 80 260 80 "" 0 0 0 "">
  <160 80 160 110 "" 0 0 0 "">
  <420 80 420 80 "out1" 430 50 0 "">
  <160 460 160 460 "out3" 170 430 0 "">
  <160 260 160 260 "out2" 170 230 0 "">
</Wires>
<Diagrams>
  <Rect 620 292 600 312 3 #c0c0c0 1 00 1 0 0.0005 0.004 1 -1e+06 10 -999956 1 -1 0.5 1 315 0 225 "time, s" "" "">
	<"ngspice/tran.v(out1)" #0000ff 0 3 0 0 0>
	<"ngspice/tran.v(out2)" #ff0000 0 3 0 0 1>
  </Rect>
  <Rect 620 652 600 312 3 #c0c0c0 1 00 1 0 0.0005 0.004 1 -1 0.5 1 1 -1 2 11 315 0 225 "time, s" "" "">
	<"ngspice/tran.v(out3)" #ff0000 0 3 0 0 0>
  </Rect>
</Diagrams>
<Paintings>
  <Text 20 0 14 #000000 0 "Triangle Wave Generator via current sources\nRL must be high, ie "perfect" integrator">
  <Rectangle 20 60 340 160 #aa5500 0 2 #c0c0c0 1 0>
  <Text 30 620 14 #000000 0 "For Qucs-S:\nTriangle Wave Generator via rectangular voltage source\nTH does-not include Tr and TL does-not include Tf">
</Paintings>
